#!/bin/bash
#==========================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Armbian for Amlogic TV Boxes
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Run on Armbian, generate uinitrd
# Copyright (C) 2021- https://github.com/unifreq
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
#===================== Set make environment variables =====================
#
# Set environment variables
chroot_make_path="${PWD}"
chroot_arch_info="$(arch)"
chroot_kernel_version="${1}"
#
# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
WARNING="[\033[93m WARNING \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#==========================================================================

# Install the required dependencies
chroot_env_init() {
    echo -e "${STEPS} Install the required dependencies..."
    sudo apt-get update -y
    sudo apt-get install -y initramfs-tools
}

# Generate uInitrd
chroot_generate_uinitrd() {
    cd /boot
    echo -e "${STEPS} Generate uInitrd file..."
    #echo -e "${INFO} File status in the /boot directory before the update: \n$(ls -l .) \n"

    cp -f vmlinuz-${chroot_kernel_version} zImage 2>/dev/null && sync

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${chroot_kernel_version} 2>/dev/null

    if [[ -f "uInitrd" ]]; then
        echo -e "${SUCCESS} The initrd.img and uInitrd file is Successfully generated."
        mv -f uInitrd uInitrd-${chroot_kernel_version} 2>/dev/null && sync
    else
        echo -e "${WARNING} The initrd.img and uInitrd file not updated."
    fi

    echo -e "${INFO} File situation in the /boot directory after update: \n$(ls -l *${chroot_kernel_version})"
}

echo -e "${INFO} Current system: [ ${chroot_arch_info} ]"
echo -e "${INFO} Current path: [ ${chroot_make_path} ]"
echo -e "${INFO} Compile the kernel version: [ ${chroot_kernel_version} ]"
#
# If dependencies such as initramfs-tools are missing, please enable the [ chroot_env_init ] method
#chroot_env_init
chroot_generate_uinitrd

sync
