#!/bin/bash
#==========================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Build Armbian rootfs file.
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Optional parameters:
# -v, --VERSION_CODENAME: Set the version codename of Armbian rootfs file, e.g., bookworm, jammy, etc.
# -s, --SSHD_CONFIG:      Enable or disable sshd_config file, default is false.
# -c, --COMMAND_COLORS:   Enable or disable command colors, default is false.
# -k, --COMPILE_KERNEL:   Add armbian-kernel and uInitrd generation script, default none.
#
# Usage: ./compile-kernel/tools/script/docker/build_armbian_rootfs_file.sh -v bookworm -s true -c true -k true
#        ./compile-kernel/tools/script/docker/build_armbian_rootfs_file.sh -v bookworm
#
#===================== Set make environment variables =====================
#
# Set environment variables
current_path="${PWD}"
build_path="${current_path}/build"
image_path="${build_path}/output/images"
cache_path="${build_path}/cache/rootfs"
tmp_rootfs="${image_path}/tmp_rootfs"

#
# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
WARNING="[\033[93m WARNING \033[0m]"
NOTE="[\033[93m NOTE \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#==========================================================================

error_msg() {
    echo -e " ${ERROR} ${1}"
    exit 1
}

init_var() {
    echo -e "${STEPS} Start Initializing Variables..."

    # If it is followed by [ : ], it means that the option requires a parameter value
    local options="v:s:c:k:"
    parsed_args=$(getopt -o "${options}" -- "${@}")
    [[ ${?} -ne 0 ]] && error_msg "Parameter parsing failed."
    eval set -- "${parsed_args}"

    while true; do
        case "${1}" in
        -v | --VERSION_CODENAME)
            if [[ -n "${2}" ]]; then
                version_codename="${2}"
                shift 2
            else
                error_msg "Invalid -v parameter [ ${2} ]!"
            fi
            ;;
        -s | --SSHD_CONFIG)
            if [[ -n "${2}" ]]; then
                sshd_config_enable="${2}"
                shift 2
            else
                error_msg "Invalid -s parameter [ ${2} ]!"
            fi
            ;;
        -c | --COMMAND_COLORS)
            if [[ -n "${2}" ]]; then
                command_colors="${2}"
                shift 2
            else
                error_msg "Invalid -c parameter [ ${2} ]!"
            fi
            ;;
        -k | --COMPILE_KERNEL)
            if [[ -n "${2}" ]]; then
                compile_kernel="${2}"
                shift 2
            else
                error_msg "Invalid -k parameter [ ${2} ]!"
            fi
            ;;
        --)
            shift
            break
            ;;
        *)
            [[ -n "${1}" ]] && error_msg "Invalid option [ ${1} ]!"
            break
            ;;
        esac
    done

    echo -e "${INFO} VERSION CODENAME: [ ${version_codename} ]"
}

redo_rootfs() {
    echo -e "${STEPS} Start redoing Armbian [ ${version_codename} ] rootfs file..."

    # Searching for Armbian image
    image_file="$(basename $(ls ${image_path}/*.img 2>/dev/null | head -n 1))"
    image_version="$(echo ${image_file} | grep -oE '[2-9][0-9]\.[0-9]{1,2}\.[0-9]{1,2}' | head -n 1)"
    image_kernel="$(echo ${image_file} | grep -oE '[5-9]\.[0-9]{1,2}\.[0-9]{1,3}' | head -n 1)"
    image_save_name="Armbian_${image_version}-trunk_${image_kernel}.img"

    # Searching for rootfs file
    rootfs_file="$(ls ${cache_path}/rootfs-*.tar.zst 2>/dev/null | head -n 1)"
    rootfs_save_name="Armbian_${image_version}-${version_codename}_rootfs"

    # Create temporary directory
    mkdir -p ${tmp_rootfs}
    sudo chown root:root ${tmp_rootfs}
    [[ "${?}" == "0" ]] && echo -e "${INFO} 01. Creating temporary directory completed." || error_msg "01. Failed to create directory!"

    # Redo Armbian rootfs
    if [[ -n "${rootfs_file}" ]]; then
        sudo tar -mxf ${rootfs_file} -C ${tmp_rootfs}/
        [[ "${?}" == "0" ]] && echo -e "${INFO} 02. Unpacking Armbian rootfs completed." || error_msg "02. Failed to unpack rootfs file!"

        cd ${tmp_rootfs}/

        # 03. Set sshd_config
        if [[ -n "${sshd_config_enable}" && "${sshd_config_enable}" =~ ^(true|yes)$ ]]; then
            # SSH access is enabled by default
            ssh_config="etc/ssh/sshd_config"
            [[ -f "${ssh_config}" ]] && {
                sudo sed -i "s|^#*Port .*|Port 22|g" ${ssh_config}
                sudo sed -i "s|^#*PermitRootLogin .*|PermitRootLogin yes|g" ${ssh_config}
                sudo sed -i "s|^#*PasswordAuthentication .*|PasswordAuthentication yes|g" ${ssh_config}
                [[ -d "var/run/sshd" ]] || mkdir -p -m0755 var/run/sshd
                echo -e "${INFO} 03.01 Adjusting sshd_config completed."
            } || error_msg "03.01 Failed to adjust sshd_config!"

            # Set root password to 1234
            [[ -f "etc/shadow" ]] && {
                rootnewpasswd="$(openssl passwd -6 "1234")"
                sudo sed -i "s|^root:\*|root:${rootnewpasswd}|" etc/shadow
                echo -e "${INFO} 03.02 Adjusting the default account completed."
            } || error_msg "03.02 Failed to adjust root password!"
        else
            echo -e "${NOTE} 03. Skipping sshd_config adjustment."
        fi

        # 04. Set command colors
        if [[ -n "${command_colors}" && "${command_colors}" =~ ^(true|yes)$ ]]; then
            # Set terminal colors
            [[ -f "usr/bin/dircolors" ]] && {
                # Set the default color for command
                sudo chmod +x usr/bin/dircolors

                # Add color support for command and set the default prompt
                sudo tee -a root/.bashrc >/dev/null <<'EOF'

# enable color support of ls and also add handy aliases
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
    alias ls='ls --color=auto'
    alias grep='grep --color=auto'
    alias fgrep='fgrep --color=auto'
    alias egrep='egrep --color=auto'
fi

# Color prompt (green username@hostname + blue path)
PS1='\[\e[1;32m\]\u@\h\[\e[0m\]:\[\e[1;34m\]\w\[\e[0m\]\$ '

EOF
                # Add the bashrc file to the root profile
                sudo tee -a root/.profile >/dev/null <<'EOF'

if [ -f "$HOME/.bashrc" ]; then
    . "$HOME/.bashrc"
fi

EOF
                echo -e "${INFO} 04. Adjusting the default color completed."
            } || error_msg "04. Failed to adjust dircolors!"
        else
            echo -e "${NOTE} 04. Skipping sshd_config adjustment."
        fi

        # 05. Add armbian-kernel script
        if [[ -n "${compile_kernel}" && "${compile_kernel}" =~ ^(true|yes)$ ]]; then
            down_script="https://raw.githubusercontent.com/ophub/amlogic-s9xxx-armbian/refs/heads/main/build-armbian/armbian-files/common-files/usr/sbin/armbian-kernel"
            add_script="usr/sbin/armbian-kernel"
            sudo mkdir -p usr/sbin/
            sudo wget -q --show-progress --no-check-certificate -O ${add_script} ${down_script}
            [[ "${?}" == "0" ]] && {
                sudo chmod +x ${add_script}
                sudo chown root:root ${add_script}
                echo -e "${INFO} 05.01. Adding armbian-kernel script completed."
            } || error_msg "05.01. Failed to add armbian-kernel script!"

            # Add uInitrd generation script
            sudo mkdir -p etc/initramfs/post-update.d/
            sudo chown root:root etc/initramfs/post-update.d/
            sudo tee etc/initramfs/post-update.d/99-uboot >/dev/null <<'EOF'
#!/bin/bash -e

tempname="/boot/uInitrd-$1"
echo "update-initramfs: Armbian: Converting to u-boot format: ${tempname}" >&2
mkimage -A arm64 -O linux -T ramdisk -C gzip -n uInitrd -d $2 $tempname

echo "update-initramfs: Armbian: Symlinking ${tempname} to /boot/uInitrd" >&2
ln -sfv $(basename $tempname) /boot/uInitrd || {
        echo "update-initramfs: Symlink failed, moving ${tempname} to /boot/uInitrd" >&2
        mv -v $tempname /boot/uInitrd
}

echo "update-initramfs: Armbian: done." >&2

exit 0

EOF
            sudo chmod +x etc/initramfs/post-update.d/99-uboot
            [[ "${?}" == "0" ]] && echo -e "${INFO} 05.02. Adding uInitrd generation script completed." || error_msg "05.02. Failed to adjust uInitrd!"
        else
            echo -e "${NOTE} 05. Skipping armbian-kernel script addition."
        fi

        # Compress the rootfs file
        sudo tar -czf ${rootfs_save_name}.tar.gz *
        sudo mv -f ${rootfs_save_name}.tar.gz ../
        [[ "${?}" == "0" ]] && echo -e "${INFO} 06. Making Armbian rootfs completed." || error_msg "06. Failed to redo rootfs!"
    else
        error_msg "02. Failed to find rootfs file!"
    fi

    # Rename Armbian image
    if [[ -n "${image_file}" ]]; then
        cd ${image_path}/
        mv -f ${image_file} ${image_save_name}
        pigz -qf *.img || gzip -qf *.img
        [[ "${?}" == "0" ]] && echo -e "${INFO} 07. Renaming Armbian image completed." || error_msg "07. Failed to rename the image!"
    else
        error_msg "07. Failed to find Armbian image!"
    fi

    # Add sha256sum verification files
    cd ${image_path}/
    sudo rm -rf $(ls . | grep -vE ".img.gz|.tar.gz" | xargs) 2>/dev/null
    for file in *; do [[ ! -d "${file}" ]] && sha256sum "${file}" >"${file}.sha"; done
    [[ "${?}" == "0" ]] && echo -e "${INFO} 08. The files in the current directory:\n$(ls -lh .)" || error_msg "08. Failed to add sha256sum!"

    # Delete Armbian build source codes and temporary files
    cd ${build_path}/
    sudo rm -rf $(ls . | grep -v "^output$" | xargs)
    [[ "${?}" == "0" ]] && echo -e "${INFO} 09. Armbian source code cleanup completed." || error_msg "09. Failed to clean up!"

    sync && sleep 3
}

echo -e "${STEPS} Start to redo the Armbian rootfs file."

# Initialize variables
init_var "${@}"
# Redo Armbian rootfs
redo_rootfs
