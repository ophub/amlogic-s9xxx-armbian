#!/bin/bash
#====================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the compile kernel script for Armbian.
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Run on Armbian, generate uInitrd, make kernel scripts, packit header.
# Copyright (C) 2021- https://github.com/unifreq
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
#======================================= TIPS =======================================
#
# To enhance the kernel compilation speed within the Armbian system,
# the required compilation dependencies, including the gcc toolchain,
# have been pre-installed in the Armbian system.
# https://github.com/ophub/kernel/releases/download/dev/armbian.tar.xz
# Include the following contents:
# ~/compile-kernel/tools/script/armbian-compile-kernel-depends
# https://github.com/ophub/kernel/releases/download/dev/arm-gnu-toolchain-x.tar.xz
#
#================================== Functions list ==================================
#
# error_msg           : Output error message
# check_dependencies  : Install the required dependencies
# generate_uinitrd    : Generate uInitrd
# make_kernel_scripts : Make kernel scripts
# packit_header       : Packit header
#
#========================== Set make environment variables ==========================
#
# Set environment variables
chroot_arch_info="$(arch)"
chroot_kernel_version="${1}"
initramfs_conf="/etc/initramfs-tools/update-initramfs.conf"
compress_initrd_file="/etc/initramfs-tools/initramfs.conf"
header_output_path="/opt/header"
armbian_kernel_path="/opt/linux-kernel"

# Compile toolchain download mirror, run on Armbian
dev_repo="https://github.com/ophub/kernel/releases/download/dev"
# Arm GNU Toolchain source: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
gun_file="arm-gnu-toolchain-14.2.rel1-aarch64-aarch64-none-linux-gnu.tar.xz"
# Set the toolchain path
toolchain_path="/usr/local/toolchain"
# Set the kernel arch
SRC_ARCH="arm64"

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
check_dependencies() {
    trap 'error_msg "Failed to check dependencies."' ERR
    echo -e "${STEPS} Checking dependencies..."
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
    
    # Install the necessary packages
    trap 'error_msg "Failed to install required dependencies."' ERR
    [[ -n "$(dpkg -l | awk '{print $2}' | grep -w "^${necessary_packages}$")" ]] || {
        echo -e "${INFO} Installing required dependencies..."
        apt-get update -y
        apt-get install -y ${necessary_packages}
    }
    trap - ERR
}

# Generate uInitrd
generate_uinitrd() {
    trap 'error_msg "Failed to generate the uInitrd file."' ERR
    cd /boot
    echo -e "${STEPS} Generating uInitrd file..."

    # Enable update_initramfs
    [[ -f "${initramfs_conf}" ]] && sed -i "s|^update_initramfs=.*|update_initramfs=yes|g" ${initramfs_conf}

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${chroot_kernel_version}

    if [[ -f "uInitrd" ]]; then
        echo -e "${SUCCESS} The initrd.img and uInitrd file were successfully generated."
        [[ ! -L "uInitrd" ]] && mv -vf uInitrd uInitrd-${chroot_kernel_version}
        sync && sleep 3
    else
        echo -e "${WARNING} The initrd.img and uInitrd files were not updated."
    fi

    echo -e "${INFO} /boot directory after update: \n$(ls -hl *${chroot_kernel_version})"
    trap - ERR
}

# Make kernel scripts
make_kernel_scripts() {
    trap 'error_msg "Failed to generate the kernel scripts."' ERR
    cd ${armbian_kernel_path}
    echo -e "${STEPS} Making kernel scripts..."

    # Download Arm GNU Toolchain
    [[ -d "${toolchain_path}" ]] || mkdir -p ${toolchain_path}
    if [[ ! -d "${toolchain_path}/${gun_file//.tar.xz/}/bin" ]]; then
        echo -e "${INFO} Downloading the ARM GNU toolchain [ ${gun_file} ]..."
        curl -fsSL "${dev_repo}/${gun_file}" -o "${toolchain_path}/${gun_file}"
        [[ "${?}" -eq "0" ]] || error_msg "GNU toolchain file download failed."
        tar -xJf ${toolchain_path}/${gun_file} -C ${toolchain_path}
        rm -f ${toolchain_path}/${gun_file}
        # List and check directory names, and change them all to lowercase
        for dir in $(ls ${toolchain_path}); do
            if [[ -d "${toolchain_path}/${dir}" && "${dir}" != "${dir,,}" ]]; then
                mv -f ${toolchain_path}/${dir} ${toolchain_path}/${dir,,}
            fi
        done
        [[ -d "${toolchain_path}/${gun_file//.tar.xz/}/bin" ]] || error_msg "The gcc is not set!"
    fi

    # Set the default path
    path_os_variable="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin"
    # Add ${PATH} variable
    path_gcc="${toolchain_path}/${gun_file//.tar.xz/}/bin:${path_os_variable}"
    # Set cross compilation parameters
    export PATH="${path_gcc}"
    export CROSS_COMPILE="${toolchain_path}/${gun_file//.tar.xz/}/bin/aarch64-none-linux-gnu-"
    export CC="${CROSS_COMPILE}gcc"
    export LD="${CROSS_COMPILE}ld.bfd"
    export MFLAGS=""

    # Set generic make string
    MAKE_SET_STRING=" ARCH=${SRC_ARCH} CROSS_COMPILE=${CROSS_COMPILE} CC=${CC} LD=${LD} ${MFLAGS} LOCALVERSION=${LOCALVERSION} "

    # Set max process
    PROCESS="$(cat /proc/cpuinfo | grep "processor" | wc -l)"
    [[ -z "${PROCESS}" || "${PROCESS}" -lt "1" ]] && PROCESS="1" && echo "PROCESS: 1"
    echo -e "${INFO} Compiling kernel scripts..."
    make ${MAKE_SET_STRING} modules_prepare -j${PROCESS}
    echo -e "${SUCCESS} The kernel scripts were successfully generated."
    trap - ERR
}

packit_header() {
    trap 'error_msg "Failed to generate the kernel header files."' ERR
    cd ${armbian_kernel_path}
    echo -e "${STEPS} Packit header files..."

    # Set headers files list
    head_list="$(mktemp)"
    (
        find . arch/${SRC_ARCH} -maxdepth 1 -name Makefile\*
        find include scripts -type f -o -type l
        find arch/${SRC_ARCH} -name Kbuild.platforms -o -name Platform
        find $(find arch/${SRC_ARCH} -name include -o -name scripts -type d) -type f
    ) >${head_list}

    # Set object files list
    obj_list="$(mktemp)"
    {
        [[ -n "$(grep "^CONFIG_OBJTOOL=y" include/config/auto.conf 2>/dev/null)" ]] && echo "tools/objtool/objtool"
        find arch/${SRC_ARCH}/include Module.symvers include scripts -type f
        [[ -n "$(grep "^CONFIG_GCC_PLUGINS=y" include/config/auto.conf 2>/dev/null)" ]] && find scripts/gcc-plugins -name \*.so
    } >${obj_list}

    # Install related files to the specified directory
    mkdir -p ${header_output_path}
    tar --exclude '*.orig' -c -f - -C ${armbian_kernel_path} -T ${head_list} | tar -xf - -C ${header_output_path}
    tar --exclude '*.orig' -c -f - -T ${obj_list} | tar -xf - -C ${header_output_path}

    # Delete temporary files
    rm -f ${head_list} ${obj_list}

    # Copy the necessary files to the specified directory
    cp -af include/config "${header_output_path}/include"
    cp -af include/generated "${header_output_path}/include"
    cp -af arch/${SRC_ARCH}/include/generated "${header_output_path}/arch/${SRC_ARCH}/include"
    cp -af .config Module.symvers ${header_output_path}

    # Compress the header files
    cd ${header_output_path}
    tar -czf header-${chroot_kernel_version}.tar.gz *
    echo -e "${SUCCESS} The kernel header files are successfully generated."
    trap - ERR
}

# Show welcome message
echo -e "${INFO} Current system: [ ${chroot_arch_info} ]"
echo -e "${INFO} Compile the kernel version: [ ${chroot_kernel_version} ]"

# Check dependencies
check_dependencies
# Generate uInitrd
generate_uinitrd
# Make kernel scripts
make_kernel_scripts
# Packit header
packit_header

echo -e "${SUCCESS} Chroot task finished, exiting Armbian system..."
exit 0
