#!/bin/bash
#======================================================================================
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Armbian Rebuild and kernel Recompile script
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Run on Armbian, generate uinitrd
# Copyright (C) 2021- https://github.com/unifreq
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#======================================================================================

#===== Do not modify the following parameter settings, Start =====
# Set environment variables
chroot_make_path=${PWD}
chroot_arch_info=$(arch)
chroot_kernel_version="${1}"

# Set font color
blue_font_prefix="\033[94m"
purple_font_prefix="\033[95m"
green_font_prefix="\033[92m"
yellow_font_prefix="\033[93m"
red_font_prefix="\033[91m"
font_color_suffix="\033[0m"
INFO="[${blue_font_prefix}INFO${font_color_suffix}]"
STEPS="[${purple_font_prefix}STEPS${font_color_suffix}]"
SUCCESS="[${green_font_prefix}SUCCESS${font_color_suffix}]"
WARNING="[${yellow_font_prefix}WARNING${font_color_suffix}]"
ERROR="[${red_font_prefix}ERROR${font_color_suffix}]"
#===== Do not modify the following parameter settings, End =======

echo -e "${INFO} Current system: [ ${chroot_arch_info} ]"
echo -e "${INFO} Current path: [ ${chroot_make_path} ]"
echo -e "${INFO} Compile the kernel version: [ ${chroot_kernel_version} ]"

# Install the required dependencies
chroot_env_init() {
    echo -e "${STEPS} Install the required dependencies..."
    sudo apt-get update -y
    sudo apt-get upgrade -y
    sudo apt-get install -y git fakeroot build-essential initramfs-tools libncurses-dev xz-utils libssl-dev bc flex libelf-dev bison
}

# Generate uInitrd
chroot_generate_uinitrd() {
    cd /boot
    echo -e "${STEPS} Generate uInitrd file..."
    #echo -e "${INFO} File status in the /boot directory before the update: \n$(ls -l .) \n"

    cp -f vmlinuz-${chroot_kernel_version} zImage 2>/dev/null && sync

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${chroot_kernel_version} 2>/dev/null

    if [ -f uInitrd ]; then
        echo -e "${SUCCESS} The initrd.img and uInitrd file is Successfully generated."
        mv -f uInitrd uInitrd-${chroot_kernel_version} 2>/dev/null && sync
    else
        echo -e "${WARNING} The initrd.img and uInitrd file not updated."
    fi

    echo -e "${INFO} File situation in the /boot directory after update: \n$(ls -l *${chroot_kernel_version})"
}

# If dependencies such as initramfs-tools are missing, please enable the [ chroot_env_init ] method
#chroot_env_init
chroot_generate_uinitrd

sync
