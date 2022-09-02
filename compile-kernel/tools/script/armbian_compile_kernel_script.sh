#!/bin/bash
#==================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Armbian for Amlogic TV Boxes
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Run on Armbian, Compile the kernel for Amlogic s9xxx tv box
# Copyright (C) 2021- https://github.com/unifreq
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: armbian-kernel -update && armbian-kernel -d -k 5.10.125
# Command optional parameters please refer to the source code repository
#
#================================= Functions list =================================
#
# error_msg          : Output error message
#
# init_var           : Initialize all variables
# toolchain_check    : Check and install the toolchain
# query_version      : Query the latest kernel version
#
# get_kernel_source  : Get the kernel source code
# headers_install    : Deploy the kernel headers file
# compile_env        : Set up the compile kernel environment
# compile_dtbs       : Compile the dtbs
# compile_kernel     : Compile the kernel
# generate_uinitrd   : Generate initrd.img and uInitrd
# packit_dtbs        : Packit dtbs files
# packit_kernel      : Packit boot, modules and header files
# compile_selection  : Choose to compile dtbs or all kernels
# clean_tmp          : Clear temporary files
#
# loop_recompile     : Loop to compile kernel
#
#========================= Set make environment variables =========================
#
# Related file storage path
make_path="${PWD}"
compile_path="${make_path}/compile-kernel"
kernel_path="${compile_path}/kernel"
config_path="${compile_path}/tools/config"
script_path="${compile_path}/tools/script"
out_kernel="${compile_path}/output"
arch_info="$(arch)"
host_release="$(cat /etc/os-release | grep VERSION_CODENAME | cut -d"=" -f2)"
toolchain_path="/usr/local/toolchain"
#
# Set the default value of the [ -r ] parameter
# When set to [ -r kernel.org ], Kernel download from kernel.org
kernel_org_repo="https://cdn.kernel.org/pub/linux/kernel/v5.x/"
# Set the default for downloading kernel sources from github.com
repo_owner="unifreq"
repo_branch="main"
build_kernel=("5.10.125" "5.15.50")
auto_kernel="true"
custom_name="-ophub"
# Set the kernel compile object, options: dtbs / all
package_list="all"
#
# Compile toolchain download mirror, run on Armbian
dev_repo="https://github.com/ophub/kernel/releases/download/dev"
#
# Clang download from: https://github.com/llvm/llvm-project/releases
clang_file="clang+llvm-14.0.6-aarch64-linux-gnu.tar.xz"
#
# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
WARNING="[\033[93m WARNING \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#==================================================================================

error_msg() {
    echo -e " ${ERROR} ${1}"
    exit 1
}

init_var() {
    cd ${make_path}

    # If it is followed by [ : ], it means that the option requires a parameter value
    get_all_ver="$(getopt "dk:a:n:p:r:" "${@}")"

    while [[ -n "${1}" ]]; do
        case "${1}" in
        -d | --default)
            : ${build_kernel:="${build_kernel}"}
            : ${auto_kernel:="${auto_kernel}"}
            : ${custom_name:="${custom_name}"}
            : ${package_list:="${package_list}"}
            : ${repo_owner:="${repo_owner}"}
            ;;
        -k | --kernel)
            if [[ -n "${2}" ]]; then
                oldIFS=$IFS
                IFS=_
                build_kernel=(${2})
                IFS=$oldIFS
                shift
            else
                error_msg "Invalid -k parameter [ ${2} ]!"
            fi
            ;;
        -a | --autoKernel)
            if [[ -n "${2}" ]]; then
                auto_kernel="${2}"
                shift
            else
                error_msg "Invalid -a parameter [ ${2} ]!"
            fi
            ;;
        -n | --customName)
            if [[ -n "${2}" ]]; then
                custom_name="${2}"
                shift
            else
                error_msg "Invalid -n parameter [ ${2} ]!"
            fi
            ;;
        -p | --PackageList)
            if [[ -n "${2}" ]]; then
                package_list="${2}"
                shift
            else
                error_msg "Invalid -p parameter [ ${2} ]!"
            fi
            ;;
        -r | --repo)
            if [[ -n "${2}" ]]; then
                repo_owner="${2}"
                shift
            else
                error_msg "Invalid -r parameter [ ${2} ]!"
            fi
            ;;
        *)
            error_msg "Invalid option [ ${1} ]!"
            ;;
        esac
        shift
    done

    # Receive the value entered by the [ -r ] parameter
    input_r_value="${repo_owner//https\:\/\/github\.com\//}"
    code_owner="$(echo "${input_r_value}" | awk -F '@' '{print $1}' | awk -F '/' '{print $1}')"
    code_repo="$(echo "${input_r_value}" | awk -F '@' '{print $1}' | awk -F '/' '{print $2}')"
    code_branch="$(echo "${input_r_value}" | awk -F '@' '{print $2}')"
    #
    [[ -n "${code_owner}" ]] || error_msg "The [ -r ] parameter is invalid."
    [[ -n "${code_branch}" ]] || code_branch="${repo_branch}"
}

toolchain_check() {
    cd ${make_path}
    echo -e "${STEPS} Check the cross-compilation environment ..."

    # Install dependencies
    sudo apt-get -qq update
    sudo apt-get -qq install -y $(cat compile-kernel/tools/script/armbian-compile-kernel-depends)

    if [[ "${host_release}" != "jammy" ]]; then
        [[ -d "${toolchain_path}" ]] || mkdir -p ${toolchain_path}
        # Download clang for Armbian
        if [[ ! -d "${toolchain_path}/${clang_file//.tar.xz/}/bin" ]]; then
            echo -e "${INFO} Download clang [ ${clang_file} ] ..."
            wget -c "${dev_repo}/${clang_file}" -O "${toolchain_path}/${clang_file}" >/dev/null 2>&1 && sync
            tar -xJf ${toolchain_path}/${clang_file} -C ${toolchain_path} && sync
            rm -f ${toolchain_path}/${clang_file} && sync
            [[ -d "${toolchain_path}/${clang_file//.tar.xz/}/bin" ]] || error_msg "The clang is not set!"
        fi
    fi
}

query_version() {
    cd ${make_path}
    # Set empty array
    tmp_arr_kernels=()

    # Query the latest kernel in a loop
    i=1
    for KERNEL_VAR in ${build_kernel[*]}; do
        echo -e "${INFO} (${i}) Auto query the latest kernel version of the same series for [ ${KERNEL_VAR} ]"
        # Identify the kernel mainline
        MAIN_LINE="$(echo ${KERNEL_VAR} | awk -F '.' '{print $1"."$2}')"

        if [[ "${code_owner}" == "kernel.org" ]]; then
            # latest_version="5.10.125"
            latest_version="$(curl -s ${kernel_org_repo} | grep -oE linux-${MAIN_LINE}.[0-9]+.tar.xz | sort -rV | head -n 1 | grep -oE '[1-9].[0-9]{1,3}.[0-9]+')"
            if [[ "${?}" -eq "0" && -n "${latest_version}" ]]; then
                tmp_arr_kernels[${i}]="${latest_version}"
            else
                error_msg "Failed to query the kernel version in [ ${kernel_org_repo} ]"
            fi
            echo -e "${INFO} (${i}) [ ${tmp_arr_kernels[$i]} ] is kernel.org latest kernel. \n"
        else
            if [[ -z "${code_repo}" ]]; then linux_repo="linux-${MAIN_LINE}.y"; else linux_repo="${code_repo}"; fi
            github_kernel_repo="${code_owner}/${linux_repo}/${code_branch}"
            github_kernel_ver="https://raw.githubusercontent.com/${github_kernel_repo}/Makefile"
            # latest_version="125"
            latest_version="$(curl -s ${github_kernel_ver} | grep -oE "SUBLEVEL =.*" | head -n 1 | grep -oE '[0-9]{1,3}')"
            if [[ "${?}" -eq "0" && -n "${latest_version}" ]]; then
                tmp_arr_kernels[${i}]="${MAIN_LINE}.${latest_version}"
            else
                error_msg "Failed to query the kernel version in [ github.com/${github_kernel_repo} ]"
            fi
            echo -e "${INFO} (${i}) [ ${tmp_arr_kernels[$i]} ] is github.com/${github_kernel_repo} latest kernel. \n"
        fi

        let i++
    done

    # Reset the kernel array to the latest kernel version
    unset build_kernel
    build_kernel="${tmp_arr_kernels[*]}"
}

get_kernel_source() {
    cd ${make_path}
    # kernel_folder > kernel_.tar.xz_file > download_from_kernel.org
    echo -e "${STEPS} Start query and download the kernel."
    [[ -d "${kernel_path}" ]] || mkdir -p ${kernel_path}
    if [[ ! -d "${kernel_path}/${local_kernel_path}" ]]; then
        if [[ "${code_owner}" == "kernel.org" ]]; then
            if [[ -f "${kernel_path}/${local_kernel_path}.tar.xz" ]]; then
                echo -e "${INFO} Unzip local files [ ${local_kernel_path}.tar.xz ]"
                cd ${kernel_path}
                tar -xJf ${local_kernel_path}.tar.xz
                [[ "${?}" -eq "0" ]] || error_msg "[ ${local_kernel_path}.tar.xz ] file decompression failed."
            else
                echo -e "${INFO} [ ${kernel_version} ] Kernel loading from [ ${server_kernel_repo}${local_kernel_path}.tar.xz ]"
                wget -q -P ${kernel_path} ${server_kernel_repo}${local_kernel_path}.tar.xz && sync
                if [[ "${?}" -eq "0" && -s "${kernel_path}/${local_kernel_path}.tar.xz" ]]; then
                    echo -e "${SUCCESS} The kernel file is downloaded successfully."
                    cd ${kernel_path}
                    tar -xJf ${local_kernel_path}.tar.xz && sync
                    [[ -d "${local_kernel_path}" ]] || error_msg "[ ${local_kernel_path}.tar.xz ] file decompression failed."
                else
                    error_msg "Kernel file download failed!"
                fi
            fi
        else
            echo -e "${INFO} Start cloning from [ https://github.com/${server_kernel_repo} -b ${code_branch} ]"
            git clone --depth 1 https://github.com/${server_kernel_repo} -b ${code_branch} ${kernel_path}/${local_kernel_path}
            [[ "${?}" -eq "0" ]] || error_msg "[ https://github.com/${server_kernel_repo} ] Clone failed."
        fi
    elif [[ "${code_owner}" != "kernel.org" ]]; then
        # Get a local kernel version
        local_makefile="${kernel_path}/${local_kernel_path}/Makefile"
        local_makefile_version="$(cat ${local_makefile} | grep -oE "VERSION =.*" | head -n 1 | grep -oE '[0-9]{1,3}')"
        local_makefile_patchlevel="$(cat ${local_makefile} | grep -oE "PATCHLEVEL =.*" | head -n 1 | grep -oE '[0-9]{1,3}')"
        local_makefile_sublevel="$(cat ${local_makefile} | grep -oE "SUBLEVEL =.*" | head -n 1 | grep -oE '[0-9]{1,3}')"

        # Local version and server version comparison
        if [[ "${auto_kernel}" == "true" && "${kernel_sub}" -gt "${local_makefile_sublevel}" ]]; then
            # Pull the latest source code of the server
            cd ${kernel_path}/${local_kernel_path}
            git checkout ${code_branch} && git reset --hard origin/${code_branch} && git pull && sync
            unset kernel_version
            kernel_version="${local_makefile_version}.${local_makefile_patchlevel}.${kernel_sub}"
            echo -e "${INFO} Synchronize the upstream source code, compile the kernel version [ ${kernel_version} ]."
        else
            # Reset to local kernel version number
            unset kernel_version
            kernel_version="${local_makefile_version}.${local_makefile_patchlevel}.${local_makefile_sublevel}"
            echo -e "${INFO} Use local source code, compile the kernel version [ ${kernel_version} ]."
        fi
    fi
    sync
}

headers_install() {
    cd ${kernel_path}/${local_kernel_path}

    # Set headers files list
    head_list="$(mktemp)"
    (
        find . arch/${ARCH} -maxdepth 1 -name Makefile\*
        find include scripts -type f -o -type l
        find arch/${ARCH} -name Kbuild.platforms -o -name Platform
        find $(find arch/${ARCH} -name include -o -name scripts -type d) -type f
    ) >${head_list}

    # Set object files list
    obj_list="$(mktemp)"
    {
        [[ -n "$(grep "^CONFIG_OBJTOOL=y" include/config/auto.conf 2>/dev/null)" ]] && echo "tools/objtool/objtool"
        find arch/${ARCH}/include Module.symvers include scripts -type f
        [[ -n "$(grep "^CONFIG_GCC_PLUGINS=y" include/config/auto.conf 2>/dev/null)" ]] && find scripts/gcc-plugins -name \*.so
    } >${obj_list}

    # Install related files to the specified directory
    tar --exclude '*.orig' -c -f - -C ${kernel_path}/${local_kernel_path} -T ${head_list} | tar -xf - -C ${out_kernel}/header
    tar --exclude '*.orig' -c -f - -T ${obj_list} | tar -xf - -C ${out_kernel}/header

    # copy .config manually to be where it's expected to be
    cp .config ${out_kernel}/header/.config && sync

    # Delete temporary files
    rm -f ${head_list} ${obj_list} 2>/dev/null
}

compile_env() {
    cd ${make_path}
    echo -e "${STEPS} Start checking local compilation environments."

    # Get kernel output name
    kernel_outname="${kernel_version}${custom_name}"
    echo -e "${INFO} Compile kernel output name [ ${kernel_outname} ]. \n"

    # Create a temp directory
    rm -rf ${out_kernel}/{boot/,dtb/,modules/,header/,${kernel_version}/} 2>/dev/null && sync
    mkdir -p ${out_kernel}/{boot/,dtb/{allwinner/,amlogic/,rockchip/},modules/,header/,${kernel_version}/} && sync

    cd ${kernel_path}/${local_kernel_path}
    echo -e "${STEPS} Set compilation parameters."

    # Set compilation parameters
    export ARCH="arm64"
    export LOCALVERSION="${custom_name}"
    if [[ "${host_release}" == "jammy" ]]; then
        export CC="clang"
        export LD="ld.lld"
    else
        export CC="${toolchain_path}/${clang_file//.tar.xz/}/bin/clang"
        export LD="${toolchain_path}/${clang_file//.tar.xz/}/bin/ld.lld"
        #
        # Add $PATH variable
        path_armbian="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
        path_clang="${toolchain_path}/${clang_file//.tar.xz/}/bin:${path_armbian}"
        # Set $PATH variable for ~/.bashrc
        sed -i '/^PATH=/d' ~/.bashrc 2>/dev/null && sync
        echo "PATH=${path_clang}" >>~/.bashrc && sync
        source ~/.bashrc
        # Set $PATH variable for /etc/profile
        sed -i '/^PATH=/d' /etc/profile 2>/dev/null && sync
        echo "PATH=${path_clang}" >>/etc/profile && sync
        source /etc/profile
    fi

    # Show variable
    echo -e "${INFO} ARCH: [ ${ARCH} ]"
    echo -e "${INFO} LOCALVERSION: [ ${LOCALVERSION} ]"
    echo -e "${INFO} CC: [ ${CC} ]"
    echo -e "${INFO} LD: [ ${LD} ]"
    # Set generic make string
    MAKE_SET_STRING=" ARCH=${ARCH} CC=${CC} LD=${LD} LLVM=1 LLVM_IAS=1 LOCALVERSION=${LOCALVERSION} "

    # Make clean/mrproper
    make ${MAKE_SET_STRING} mrproper

    # Make menuconfig
    #make ${MAKE_SET_STRING} menuconfig

    # Check .config file
    if [[ ! -s ".config" ]]; then
        echo -e "${INFO} Copy config file to ${local_kernel_path}"
        [[ -s "${config_path}/config-${kernel_verpatch}" ]] && error_msg "Missing [ config-${kernel_verpatch} ] template!"
        echo -e "${INFO} CONFIG_DEMO: [ ${config_path}/config-${kernel_verpatch} ]"
        cp -f ${config_path}/config-${kernel_verpatch} .config && sync
    else
        echo -e "${INFO} Use the .config file in the current directory."
    fi
    #
    sed -i "s|CONFIG_LOCALVERSION=.*|CONFIG_LOCALVERSION=\"\"|" .config
    sync

    # Enable/Disabled Linux Kernel Clang LTO
    kernel_x="$(echo "${kernel_version}" | cut -d '.' -f1)"
    kernel_y="$(echo "${kernel_version}" | cut -d '.' -f2)"
    if [[ "${kernel_x}" -ge "6" ]] || [[ "${kernel_x}" -eq "5" && "${kernel_y}" -ge "12" ]]; then
        scripts/config -e LTO_CLANG_THIN
    else
        scripts/config -d LTO_CLANG_THIN
    fi

    # Set max process
    PROCESS="$(cat /proc/cpuinfo | grep "processor" | wc -l)"
    [[ -z "${PROCESS}" ]] && PROCESS="1" && echo "PROCESS: 1"
}

compile_dtbs() {
    cd ${kernel_path}/${local_kernel_path}

    # Make dtbs
    echo -e "${STEPS} Start compilation dtbs [ ${local_kernel_path} ]..."
    make ${MAKE_SET_STRING} dtbs -j${PROCESS}
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The dtbs is compiled successfully."
}

compile_kernel() {
    cd ${kernel_path}/${local_kernel_path}

    # Make kernel
    echo -e "${STEPS} Start compilation kernel [ ${local_kernel_path} ]..."
    make ${MAKE_SET_STRING} Image modules dtbs -j${PROCESS}
    #make ${MAKE_SET_STRING} bindeb-pkg KDEB_COMPRESS=xz KBUILD_DEBARCH=arm64 -j${PROCESS}
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The kernel is compiled successfully."

    # Install modules
    echo -e "${STEPS} Install modules ..."
    make ${MAKE_SET_STRING} INSTALL_MOD_PATH=${out_kernel}/modules modules_install
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The modules is installed successfully."

    # Install headers
    echo -e "${STEPS} Install headers ..."
    headers_install
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The headers is installed successfully."
}

generate_uinitrd() {
    cd ${make_path}
    echo -e "${STEPS} Generate uInitrd environment initialization..."

    # Backup current system files for /boot
    echo -e "${INFO} Backup the files in the [ /boot ] directory."
    boot_backup_path="/boot/backup"
    rm -rf ${boot_backup_path} && mkdir -p ${boot_backup_path} && sync
    mv -f /boot/{config-*,initrd.img-*,System.map-*,uInitrd-*,vmlinuz-*,uInitrd,zImage} ${boot_backup_path} 2>/dev/null && sync
    # Copy /boot related files into armbian system
    cp -f ${kernel_path}/${local_kernel_path}/System.map /boot/System.map-${kernel_outname}
    cp -f ${kernel_path}/${local_kernel_path}/.config /boot/config-${kernel_outname}
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/Image /boot/vmlinuz-${kernel_outname}
    sync
    #echo -e "${INFO} Kernel copy results in the [ /boot ] directory: \n$(ls -l /boot) \n"

    # Backup current system files for /usr/lib/modules
    echo -e "${INFO} Backup the files in the [ /usr/lib/modules ] directory."
    modules_backup_path="/usr/lib/modules/backup"
    rm -rf ${modules_backup_path} && mkdir -p ${modules_backup_path} && sync
    mv -f /usr/lib/modules/$(uname -r) ${modules_backup_path} && sync
    # Copy modules files
    cp -rf ${out_kernel}/modules/lib/modules/${kernel_outname} /usr/lib/modules
    sync
    #echo -e "${INFO} Kernel copy results in the [ /usr/lib/modules ] directory: \n$(ls -l /usr/lib/modules) \n"

    # COMPRESS: [ gzip | bzip2 | lz4 | lzma | lzop | xz | zstd ]
    compress_initrd_file="/etc/initramfs-tools/initramfs.conf"
    sed -i "/^COMPRESS=/d" ${compress_initrd_file} && sync
    echo "COMPRESS=gzip" >>${compress_initrd_file} && sync

    cd /boot
    echo -e "${STEPS} Generate uInitrd file..."
    #echo -e "${INFO} File status in the /boot directory before the update: \n$(ls -l .) \n"

    cp -f vmlinuz-${kernel_outname} zImage 2>/dev/null && sync

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${kernel_outname} 2>/dev/null

    if [[ -f uInitrd ]]; then
        echo -e "${SUCCESS} The initrd.img and uInitrd file is Successfully generated."
        mv -f uInitrd uInitrd-${kernel_outname} 2>/dev/null && sync
    else
        echo -e "${WARNING} The initrd.img and uInitrd file not updated."
    fi

    echo -e "${INFO} File situation in the /boot directory after update: \n$(ls -l *${kernel_outname})"

    # Restore the files in the [ /boot ] directory
    mv -f *${kernel_outname} ${out_kernel}/boot && sync
    mv -f ${boot_backup_path}/* . && sync && rm -rf ${boot_backup_path}

    # Restore the files in the [ /usr/lib/modules ] directory
    rm -rf /usr/lib/modules/${kernel_outname} 2>/dev/null && sync
    mv ${modules_backup_path}/* /usr/lib/modules && sync && rm -rf ${modules_backup_path}
}

packit_dtbs() {
    # Pack 3 dtbs files
    echo -e "${STEPS} Packing the [ ${kernel_outname} ] dtbs packages..."

    cd ${out_kernel}/dtb/allwinner
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/dts/allwinner/*.dtb . && chmod +x * && sync
    tar -czf dtb-allwinner-${kernel_outname}.tar.gz * && sync
    mv -f *.tar.gz ${out_kernel}/${kernel_version} && sync
    echo -e "${SUCCESS} The [ dtb-allwinner-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/dtb/amlogic
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/dts/amlogic/*.dtb . && chmod +x * && sync
    tar -czf dtb-amlogic-${kernel_outname}.tar.gz * && sync
    mv -f *.tar.gz ${out_kernel}/${kernel_version} && sync
    echo -e "${SUCCESS} The [ dtb-amlogic-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/dtb/rockchip
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/dts/rockchip/*.dtb . && chmod +x * && sync
    tar -czf dtb-rockchip-${kernel_outname}.tar.gz * && sync
    mv -f *.tar.gz ${out_kernel}/${kernel_version} && sync
    echo -e "${SUCCESS} The [ dtb-rockchip-${kernel_outname}.tar.gz ] file is packaged."
}

packit_kernel() {
    # Pack 3 kernel files
    echo -e "${STEPS} Packing the [ ${kernel_outname} ] boot, modules and header packages..."

    cd ${out_kernel}/boot
    chmod +x *
    tar -czf boot-${kernel_outname}.tar.gz * && sync
    mv -f *.tar.gz ${out_kernel}/${kernel_version} && sync
    echo -e "${SUCCESS} The [ boot-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/modules/lib/modules
    tar -czf modules-${kernel_outname}.tar.gz * && sync
    mv -f *.tar.gz ${out_kernel}/${kernel_version} && sync
    echo -e "${SUCCESS} The [ modules-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/header
    tar -czf header-${kernel_outname}.tar.gz * && sync
    mv -f *.tar.gz ${out_kernel}/${kernel_version} && sync
    echo -e "${SUCCESS} The [ header-${kernel_outname}.tar.gz ] file is packaged."
}

compile_selection() {
    # Compile by selection
    if [[ "${package_list}" == "dtbs" ]]; then
        compile_dtbs
        packit_dtbs
    else
        compile_kernel
        generate_uinitrd
        packit_dtbs
        packit_kernel
    fi

    # Add sha256sum integrity verification file
    cd ${out_kernel}/${kernel_version}
    sha256sum * >sha256sums && sync
    echo -e "${SUCCESS} The [ sha256sums ] file has been generated"

    cd ${out_kernel}
    tar -czf ${kernel_version}.tar.gz ${kernel_version} && sync

    echo -e "${INFO} Kernel series files are stored in [ ${out_kernel} ]."
}

clean_tmp() {
    cd ${make_path}
    echo -e "${STEPS} Clear the space..."

    rm -rf ${out_kernel}/{boot/,dtb/,modules/,header/,${kernel_version}/} 2>/dev/null

    sync && sleep 3
    echo -e "${SUCCESS} All processes have been completed."
}

loop_recompile() {
    cd ${make_path}

    j="1"
    for k in ${build_kernel[*]}; do
        # kernel_version, such as [ 5.10.125 ]
        kernel_version="${k}"
        # kernel_verpatch, such as [ 5.10 ]
        kernel_verpatch="$(echo ${kernel_version} | awk -F '.' '{print $1"."$2}')"
        # kernel_sub, such as [ 125 ]
        kernel_sub="$(echo ${kernel_version} | awk -F '.' '{print $3}')"

        # The loop variable assignment
        if [[ "${code_owner}" == "kernel.org" ]]; then
            server_kernel_repo="${kernel_org_repo}"
            local_kernel_path="linux-${kernel_version}"
        elif [[ -z "${code_repo}" ]]; then
            server_kernel_repo="${code_owner}/linux-${kernel_verpatch}.y"
            local_kernel_path="linux-${kernel_verpatch}.y"
        else
            server_kernel_repo="${code_owner}/${code_repo}"
            local_kernel_path="${code_repo}-${code_branch}"
        fi

        # Execute the following functions in sequence
        get_kernel_source
        compile_env
        compile_selection
        clean_tmp

        let j++
    done
}

# Check script permission, supports running on Armbian system.
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo ./${0} ]"
[[ "${arch_info}" == "aarch64" ]] || error_msg "The script only supports running under Armbian system."
# Show welcome and server start information
echo -e "Welcome to compile kernel! \n"
echo -e "Server running on Armbian: [ Release: ${host_release} / Host: ${arch_info} ] \n"
echo -e "Server running path [ ${make_path} ] \n"
echo -e "Server CPU configuration information: \n$(cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c) \n"
echo -e "Server memory usage: \n$(free -h) \n"
echo -e "Server space usage before starting to compile: \n$(df -hT ${make_path}) \n"
#
# Initialize variables, download the kernel source code and check the toolchain
init_var "${@}"
[[ "${auto_kernel}" == "true" ]] && query_version
echo -e "Kernel from: [ ${code_owner} ]"
echo -e "Kernel List: [ $(echo ${build_kernel[*]} | tr "\n" " ") ] \n"
toolchain_check
# Loop to compile the kernel
loop_recompile
#
# Show server end information
echo -e "${INFO} Server space usage after compilation: \n$(df -hT ${make_path}) \n"
# All process completed
wait
