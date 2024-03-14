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
# Description: Run on Armbian, generate uInitrd.
# Copyright (C) 2021- https://github.com/unifreq
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
#===================== Set make environment variables =====================
#
# Set environment variables
chroot_arch_info="$(arch)"
chroot_kernel_version="${1}"
initramfs_conf="/etc/initramfs-tools/update-initramfs.conf"
compress_initrd_file="/etc/initramfs-tools/initramfs.conf"
#
# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
WARNING="[\033[93m WARNING \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#==========================================================================

# Error message
error_msg() {
    echo -e " ${ERROR} ${1}"
    exit 1
}

# Install the required dependencies
chroot_env_init() {
    echo -e "${STEPS} Start checking dependencies..."
    # Check the compression algorithm
    if [[ -f "${compress_initrd_file}" ]]; then
        compress_settings="$(cat ${compress_initrd_file} | grep -E ^COMPRESS= | awk -F'=' '{print $2}')"
        [[ -n "${compress_settings}" ]] || error_msg "The compression algorithm is not set."
    else
        error_msg "The file ${compress_initrd_file} does not exist."
    fi

    # Set the necessary packages
    case "${compress_settings}" in
        xz | lzma) necessary_packages="xz-utils" ;;
        zstd) necessary_packages="zstd" ;;
        gzip | *) necessary_packages="gzip" ;;
    esac

    # Check the necessary packages
    [[ -n "$(dpkg -l | awk '{print $2}' | grep -w "^${necessary_packages}$")" ]] || {
        echo -e "${STEPS} Install the required dependencies..."
        sudo apt-get update -y -qq
        sudo apt-get install -y -qq ${necessary_packages}
    }
}

# Generate uInitrd
chroot_generate_uinitrd() {
    cd /boot
    echo -e "${STEPS} Generate uInitrd file..."

    # Enable update_initramfs
    [[ -f "${initramfs_conf}" ]] && sed -i "s|^update_initramfs=.*|update_initramfs=yes|g" ${initramfs_conf}

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${chroot_kernel_version}

    if [[ -f "uInitrd" ]]; then
        echo -e "${SUCCESS} The initrd.img and uInitrd file is Successfully generated."
        [[ ! -L "uInitrd" ]] && mv -vf uInitrd uInitrd-${chroot_kernel_version}
        sync && sleep 3
    else
        echo -e "${WARNING} The initrd.img and uInitrd file not updated."
    fi

    echo -e "${INFO} File situation in the /boot directory after update: \n$(ls -l *${chroot_kernel_version})"
}

echo -e "${INFO} Current system: [ ${chroot_arch_info} ]"
echo -e "${INFO} Compile the kernel version: [ ${chroot_kernel_version} ]"

# Check dependencies
chroot_env_init
# Generate uInitrd
chroot_generate_uinitrd
