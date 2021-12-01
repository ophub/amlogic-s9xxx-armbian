#!/bin/bash
#===========================================================================
# Description: compile linux kernel for armbian
# Copyright (C) 2020-2021 https://github.com/unifreq
# Copyright (C) 2020-2021 https://github.com/ophub/amlogic-s9xxx-armbian
#===========================================================================

#===== Do not modify the following parameter settings, Start =====
# Set environment variables
chroot_make_path=${PWD}
chroot_arch_info=$(arch)
chroot_kernel_version="${1}"

# Set font color
blue_font_prefix="\033[34m"
purple_font_prefix="\033[35m"
green_font_prefix="\033[32m"
yellow_font_prefix="\033[33m"
red_font_prefix="\033[31m"
font_color_suffix="\033[0m"
INFO="[${blue_font_prefix}INFO${font_color_suffix}]"
STEPS="[${purple_font_prefix}STEPS${font_color_suffix}]"
SUCCESS="[${green_font_prefix}SUCCESS${font_color_suffix}]"
WARNING="[${yellow_font_prefix}WARNING${font_color_suffix}]"
ERROR="[${red_font_prefix}ERROR${font_color_suffix}]"
#===== Do not modify the following parameter settings, End =======

echo -e "${INFO} Current system: ${chroot_arch_info}"
echo -e "${INFO} Current path: ${chroot_make_path}"
echo -e "${INFO} Kernel version: ${chroot_kernel_version}"

# Environment initialization
chroot_env_init() {
    echo -e "${STEPS} Environment initialization."
    sudo apt-get update -y
    sudo apt-get upgrade -y
    sudo apt-get install -y $(curl -fsSL git.io/armbian-kernel-server)
}

# Generate uInitrd
chroot_generate_uinitrd() {
    cd /boot
    echo -e "${STEPS} Generate uInitrd file."
    #echo -e "${INFO} File status in the /boot directory before the update: \n$(ls -l /boot) \n"

    # Backup the original uInitrd file
    [ -f uInitrd ] && mv -f uInitrd uInitrd.bak 2>/dev/null
    [ ! -f zImage ] && cp -f vmlinuz-${chroot_kernel_version} zImage 2>/dev/null && sync

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${chroot_kernel_version} 2>/dev/null
    #echo -e "${INFO} File situation in the aa directory after update: \n$(ls -l /boot) \n"

    if [ -f uInitrd ]; then
        echo -e "${SUCCESS} The uInitrd file is Successfully generated."
        mv -f uInitrd uInitrd-${chroot_kernel_version} 2>/dev/null && sync
    else
        echo -e "${WARNING} The uInitrd file not updated."
    fi

    # Restore the original uInitrd
    [ -f uInitrd.bak ] && mv -f uInitrd.bak uInitrd 2>/dev/null
}

#chroot_env_init
chroot_generate_uinitrd

sync
exit 0
