#!/bin/bash
#==================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Run on Armbian, Compile the kernel.
# Copyright (C) 2021~ https://www.kernel.org
# Copyright (C) 2021~ https://github.com/unifreq
# Copyright (C) 2021~ https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: armbian-kernel
# Command optional parameters please refer to the source code repository
#
#================================= Functions list =================================
#
# error_msg          : Output error message
#
# init_var           : Initialize all variables
# toolchain_check    : Check and install the toolchain
# query_version      : Query the latest kernel version
# apply_patch        : Apply custom kernel patches
# get_kernel_source  : Get the kernel source code
#
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
current_path="${PWD}"
compile_path="${current_path}/compile-kernel"
config_path="${compile_path}/tools/config"
script_path="${compile_path}/tools/script"
kernel_patch_path="${compile_path}/tools/patch"
kernel_path="${compile_path}/kernel"
output_path="${compile_path}/output"
[[ -d "${kernel_path}" ]] || mkdir -p ${kernel_path}
[[ -d "${output_path}" ]] || mkdir -p ${output_path}

# Set the temporary backup path for the current system kernel
tmp_backup_path="/ddbr/tmp"
boot_backup_path="${tmp_backup_path}/boot"
modules_backup_path="${tmp_backup_path}/modules"

# Set the system file path to be used
arch_info="$(uname -m)"
host_release="$(cat /etc/os-release | grep '^VERSION_CODENAME=.*' | cut -d"=" -f2)"
initramfs_conf="/etc/initramfs-tools/update-initramfs.conf"
ophub_release_file="/etc/ophub-release"

# Set the default for downloading kernel sources from github.com
repo_owner="unifreq"
repo_branch="main"
build_kernel=("6.1.y" "6.12.y")
all_kernel=("5.4.y" "5.10.y" "5.15.y" "6.1.y" "6.6.y" "6.12.y")
# Set whether to use the latest kernel, options: [ true / false ]
auto_kernel="true"
# Set whether to apply custom kernel patches, options: [ true / false ]
auto_patch="false"
# Set custom signature for the kernel
custom_name="-ophub"
# Set the kernel compile object, options: [ dtbs / all ]
package_list="all"
# Set the compression format, options: [ gzip / lzma / xz / zstd ]
compress_format="xz"
# Set whether to automatically delete the source code after the kernel is compiled
delete_source="false"
# Set make log silent output (recommended to use 'true' when github runner has insufficient space)
silent_log="false"

# Compile toolchain download mirror, run on Armbian
dev_repo="https://github.com/ophub/kernel/releases/download/dev"
# Arm GNU Toolchain source: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
gun_file="arm-gnu-toolchain-14.2.rel1-aarch64-aarch64-none-linux-gnu.tar.xz"
# Set the toolchain path
toolchain_path="/usr/local/toolchain"
# Set the default cross-compilation toolchain: [ clang / gcc / gcc-14.2, etc. ]
toolchain_name="gcc"

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
    echo -e "${STEPS} Start Initializing Variables..."

    # If it is followed by [ : ], it means that the option requires a parameter value
    get_all_ver="$(getopt "k:a:n:m:p:r:t:c:d:s:" "${@}")"

    while [[ -n "${1}" ]]; do
        case "${1}" in
        -k | --kernel)
            if [[ -n "${2}" ]]; then
                if [[ "${2}" == "all" ]]; then
                    build_kernel=(${all_kernel[@]})
                else
                    oldIFS="${IFS}"
                    IFS="_"
                    build_kernel=(${2})
                    IFS="${oldIFS}"
                fi
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
                custom_name="${2// /}"
                [[ "${custom_name:0:1}" != "-" ]] && custom_name="-${custom_name}"
                shift
            else
                error_msg "Invalid -n parameter [ ${2} ]!"
            fi
            ;;
        -m | --MakePackage)
            if [[ -n "${2}" ]]; then
                package_list="${2}"
                shift
            else
                error_msg "Invalid -m parameter [ ${2} ]!"
            fi
            ;;
        -p | --AutoPatch)
            if [[ -n "${2}" ]]; then
                auto_patch="${2}"
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
        -t | --Toolchain)
            if [[ -n "${2}" ]]; then
                toolchain_name="${2}"
                shift
            else
                error_msg "Invalid -t parameter [ ${2} ]!"
            fi
            ;;
        -c | --Compress)
            if [[ -n "${2}" ]]; then
                compress_format="${2}"
                shift
            else
                error_msg "Invalid -c parameter [ ${2} ]!"
            fi
            ;;
        -d | --DeleteSource)
            if [[ -n "${2}" ]]; then
                delete_source="${2}"
                shift
            else
                error_msg "Invalid -d parameter [ ${2} ]!"
            fi
            ;;
        -s | --SilentLog)
            if [[ -n "${2}" ]]; then
                silent_log="${2}"
                shift
            else
                error_msg "Invalid -s parameter [ ${2} ]!"
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

    # Set the gcc version code
    [[ "${toolchain_name}" =~ ^gcc-[0-9]+.[0-9]+ ]] && {
        gcc_version_code="${toolchain_name#*-}"
        gun_file="arm-gnu-toolchain-${gcc_version_code}.rel1-aarch64-aarch64-none-linux-gnu.tar.xz"
    }

    # Set compilation parameters
    export SRC_ARCH="arm64"
    export LOCALVERSION="${custom_name}"

    # Check release file
    [[ -f "${ophub_release_file}" ]] || error_msg "missing [ ${ophub_release_file} ] file."

    # Get values
    source "${ophub_release_file}"
    PLATFORM="${PLATFORM}"
    FDTFILE="${FDTFILE}"

    # Early devices did not add platform parameters, auto-completion
    [[ -z "${PLATFORM}" && -n "${FDTFILE}" ]] && {
        [[ ${FDTFILE:0:5} == "meson" ]] && PLATFORM="amlogic" || PLATFORM="rockchip"
        echo "PLATFORM='${PLATFORM}'" >>${ophub_release_file}
    }
    echo -e "${INFO} Armbian PLATFORM: [ ${PLATFORM} ]"
}

toolchain_check() {
    cd ${current_path}
    echo -e "${STEPS} Start checking the toolchain for compiling the kernel..."

    # Install dependencies
    sudo apt-get -qq update
    sudo apt-get -qq install -y $(cat compile-kernel/tools/script/armbian-compile-kernel-depends)

    # Set the default path
    path_os_variable="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin"

    # Download the cross-compilation toolchain: [ clang / gcc ]
    [[ -d "/etc/apt/sources.list.d" ]] || mkdir -p /etc/apt/sources.list.d
    if [[ "${toolchain_name}" == "clang" ]]; then
        # Install LLVM
        echo -e "${INFO} Start installing the LLVM toolchain..."
        sudo apt-get -qq install -y lsb-release software-properties-common gnupg
        curl -fsSL https://apt.llvm.org/llvm.sh | sudo bash -s all
        [[ "${?}" -eq "0" ]] || error_msg "LLVM installation failed."

        # Set cross compilation parameters
        export PATH="${path_os_variable}"
        export CROSS_COMPILE="aarch64-linux-gnu-"
        export CC="clang"
        export LD="ld.lld"
        export MFLAGS=" LLVM=1 LLVM_IAS=1 "
    else
        # Download Arm GNU Toolchain
        [[ -d "${toolchain_path}" ]] || mkdir -p ${toolchain_path}
        if [[ ! -d "${toolchain_path}/${gun_file//.tar.xz/}/bin" ]]; then
            echo -e "${INFO} Start downloading the ARM GNU toolchain [ ${gun_file} ]..."

            # Download the ARM GNU toolchain. If it fails, wait 1 minute and try again, try 10 times.
            for i in {1..10}; do
                curl -fsSL "${dev_repo}/${gun_file}" -o "${toolchain_path}/${gun_file}"
                [[ "${?}" -eq "0" ]] && break || sleep 60
            done
            [[ "${?}" -eq "0" ]] || error_msg "GNU toolchain file download failed."

            # Decompress the ARM GNU toolchain
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

        # Add ${PATH} variable
        path_gcc="${toolchain_path}/${gun_file//.tar.xz/}/bin:${path_os_variable}"
        export PATH="${path_gcc}"

        # Set cross compilation parameters
        export CROSS_COMPILE="${toolchain_path}/${gun_file//.tar.xz/}/bin/aarch64-none-linux-gnu-"
        export CC="${CROSS_COMPILE}gcc"
        export LD="${CROSS_COMPILE}ld.bfd"
        export MFLAGS=""
    fi
}

query_version() {
    cd ${current_path}
    echo -e "${STEPS} Start querying the latest kernel version..."

    # Set empty array
    tmp_arr_kernels=()

    # Query the latest kernel in a loop
    i=1
    for KERNEL_VAR in ${build_kernel[@]}; do
        echo -e "${INFO} (${i}) Auto query the latest kernel version of the same series for [ ${KERNEL_VAR} ]"
        # Identify the kernel mainline
        MAIN_LINE="$(echo ${KERNEL_VAR} | awk -F '.' '{print $1"."$2}')"

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

        let i++
    done

    # Reset the kernel array to the latest kernel version
    unset build_kernel
    build_kernel="${tmp_arr_kernels[@]}"
}

apply_patch() {
    cd ${current_path}
    echo -e "${STEPS} Start applying custom kernel patches..."

    # Apply the common kernel patches
    if [[ -d "${kernel_patch_path}/common-kernel-patches" ]]; then
        echo -e "${INFO} Copy common kernel patches..."
        rm -f ${kernel_path}/${local_kernel_path}/*.patch
        cp -vf ${kernel_patch_path}/common-kernel-patches/*.patch -t ${kernel_path}/${local_kernel_path}

        cd ${kernel_path}/${local_kernel_path}
        for file in *.patch; do
            echo -e "${INFO} Apply kernel patch file: [ ${file} ]"
            patch -p1 <"${file}"
        done
        rm -f *.patch
    else
        echo -e "${INFO} No common kernel patches, skipping."
    fi

    # Apply the dedicated kernel patches
    if [[ -d "${kernel_patch_path}/${local_kernel_path}" ]]; then
        echo -e "${INFO} Copy [ ${local_kernel_path} ] version dedicated kernel patches..."
        rm -f ${kernel_path}/${local_kernel_path}/*.patch
        cp -vf ${kernel_patch_path}/${local_kernel_path}/*.patch -t ${kernel_path}/${local_kernel_path}

        cd ${kernel_path}/${local_kernel_path}
        for file in *.patch; do
            echo -e "${INFO} Apply kernel patch file: [ ${file} ]"
            patch -p1 <"${file}"
        done
        rm -f *.patch
    else
        echo -e "${INFO} No [ ${local_kernel_path} ] version dedicated kernel patches, skipping."
    fi
}

get_kernel_source() {
    cd ${current_path}
    echo -e "${STEPS} Start downloading the kernel source code..."

    [[ -d "${kernel_path}" ]] || mkdir -p ${kernel_path}

    if [[ ! -d "${kernel_path}/${local_kernel_path}" ]]; then
        echo -e "${INFO} Start cloning from [ https://github.com/${server_kernel_repo} -b ${code_branch} ]"

        # Clone the latest kernel source code. If it fails, wait 1 minute and try again, try 10 times.
        for i in {1..10}; do
            git clone -q --single-branch --depth=1 --branch=${code_branch} https://github.com/${server_kernel_repo} ${kernel_path}/${local_kernel_path}
            [[ "${?}" -eq "0" ]] && break || sleep 60
        done
        [[ "${?}" -eq "0" ]] || error_msg "[ https://github.com/${server_kernel_repo} ] Clone failed."
    else
        # Get a local kernel version
        local_makefile="${kernel_path}/${local_kernel_path}/Makefile"
        local_makefile_version="$(cat ${local_makefile} | grep -oE "VERSION =.*" | head -n 1 | grep -oE '[0-9]{1,3}')"
        local_makefile_patchlevel="$(cat ${local_makefile} | grep -oE "PATCHLEVEL =.*" | head -n 1 | grep -oE '[0-9]{1,3}')"
        local_makefile_sublevel="$(cat ${local_makefile} | grep -oE "SUBLEVEL =.*" | head -n 1 | grep -oE '[0-9]{1,3}')"

        # Local version and server version comparison
        if [[ "${auto_kernel}" == "true" || "${auto_kernel}" == "yes" ]] && [[ "${kernel_sub}" -gt "${local_makefile_sublevel}" ]]; then
            # Pull the latest source code of the server
            cd ${kernel_path}/${local_kernel_path}
            git checkout ${code_branch} && git reset --hard origin/${code_branch} && git pull
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

    # Remove the local version number
    rm -f ${kernel_path}/${local_kernel_path}/localversion

    # Apply custom kernel patches
    [[ "${auto_patch}" == "true" || "${auto_patch}" == "yes" ]] && apply_patch
}

headers_install() {
    cd ${kernel_path}/${local_kernel_path}

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
    tar --exclude '*.orig' -c -f - -C ${kernel_path}/${local_kernel_path} -T ${head_list} | tar -xf - -C ${output_path}/header
    tar --exclude '*.orig' -c -f - -T ${obj_list} | tar -xf - -C ${output_path}/header

    # Copy the necessary files to the specified directory
    cp -af include/config "${output_path}/header/include"
    cp -af include/generated "${output_path}/header/include"
    cp -af arch/${SRC_ARCH}/include/generated "${output_path}/header/arch/${SRC_ARCH}/include"
    cp -af .config Module.symvers ${output_path}/header

    # Delete temporary files
    rm -f ${head_list} ${obj_list}
}

compile_env() {
    cd ${current_path}
    echo -e "${STEPS} Start checking local compilation environments."

    # Get kernel output name
    kernel_outname="${kernel_version}${custom_name}"
    echo -e "${INFO} Compile kernel output name [ ${kernel_outname} ]. \n"

    # Create a temp directory
    rm -rf ${output_path}/{boot/,dtb/,modules/,header/,${kernel_version}/}
    mkdir -p ${output_path}/{boot/,dtb/{allwinner/,amlogic/,rockchip/},modules/,header/,${kernel_version}/}

    cd ${kernel_path}/${local_kernel_path}
    echo -e "${STEPS} Set compilation parameters."

    # Show variable
    echo -e "${INFO} ARCH: [ ${SRC_ARCH} ]"
    echo -e "${INFO} LOCALVERSION: [ ${LOCALVERSION} ]"
    echo -e "${INFO} CROSS_COMPILE: [ ${CROSS_COMPILE} ]"
    echo -e "${INFO} CC: [ ${CC} ]"
    echo -e "${INFO} LD: [ ${LD} ]"

    # Set generic make string
    MAKE_SET_STRING=" ARCH=${SRC_ARCH} CROSS_COMPILE=${CROSS_COMPILE} CC=${CC} LD=${LD} ${MFLAGS} LOCALVERSION=${LOCALVERSION} "

    # Make clean/mrproper
    make ${MAKE_SET_STRING} mrproper

    # Check .config file
    if [[ ! -s ".config" ]]; then
        [[ -s "${config_path}/config-${kernel_verpatch}" ]] || error_msg "Missing [ config-${kernel_verpatch} ] template!"
        echo -e "${INFO} Copy [ ${config_path}/config-${kernel_verpatch} ] to [ .config ]"
        cp -f ${config_path}/config-${kernel_verpatch} .config
    else
        echo -e "${INFO} Use the .config file in the current directory."
    fi
    # Clear kernel signature
    sed -i "s|CONFIG_LOCALVERSION=.*|CONFIG_LOCALVERSION=\"\"|" .config

    # Enable/Disabled Linux Kernel Clang LTO
    [[ "${toolchain_name}" == "clang" ]] && {
        kernel_x="$(echo "${kernel_version}" | cut -d '.' -f1)"
        kernel_y="$(echo "${kernel_version}" | cut -d '.' -f2)"
        if [[ "${kernel_x}" -ge "6" ]] || [[ "${kernel_x}" -eq "5" && "${kernel_y}" -ge "12" ]]; then
            scripts/config -e LTO_CLANG_THIN
        else
            scripts/config -d LTO_CLANG_THIN
        fi
    }

    # Make menuconfig
    #make ${MAKE_SET_STRING} menuconfig

    # Set max process
    PROCESS="$(cat /proc/cpuinfo | grep "processor" | wc -l)"
    [[ -z "${PROCESS}" || "${PROCESS}" -lt "1" ]] && PROCESS="1" && echo "PROCESS: 1"
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

    # Set the make log silent output
    [[ "${silent_log}" == "true" || "${silent_log}" == "yes" ]] && silent_print="-s" || silent_print=""

    # Make kernel
    echo -e "${STEPS} Start compilation kernel [ ${local_kernel_path} ]..."
    make ${silent_print} ${MAKE_SET_STRING} Image modules dtbs -j${PROCESS}
    #make ${MAKE_SET_STRING} bindeb-pkg KDEB_COMPRESS=xz KBUILD_DEBARCH=arm64 -j${PROCESS}
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The kernel is compiled successfully."

    # Install modules
    echo -e "${STEPS} Install modules ..."
    make ${silent_print} ${MAKE_SET_STRING} INSTALL_MOD_PATH=${output_path}/modules modules_install
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The modules is installed successfully."

    # Strip debug information
    STRIP="${CROSS_COMPILE}strip"
    find ${output_path}/modules -name "*.ko" -print0 | xargs -0 ${STRIP} --strip-debug 2>/dev/null
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The modules is stripped successfully."

    # Install headers
    echo -e "${STEPS} Install headers ..."
    headers_install
    [[ "${?}" -eq "0" ]] && echo -e "${SUCCESS} The headers is installed successfully."
}

generate_uinitrd() {
    cd ${current_path}
    echo -e "${STEPS} Generate uInitrd environment initialization..."

    # Backup current system files for /boot
    echo -e "${INFO} Backup the files in the [ ${boot_backup_path} ] directory."
    rm -rf ${boot_backup_path} && mkdir -p ${boot_backup_path}
    mv -f /boot/{config-*,initrd.img-*,System.map-*,vmlinuz-*,uInitrd*,*Image} -t ${boot_backup_path}
    # Copy /boot related files into armbian system
    cp -f ${kernel_path}/${local_kernel_path}/System.map /boot/System.map-${kernel_outname}
    cp -f ${kernel_path}/${local_kernel_path}/.config /boot/config-${kernel_outname}
    cp -f ${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/Image /boot/vmlinuz-${kernel_outname}
    if [[ "${PLATFORM}" == "rockchip" || "${PLATFORM}" == "allwinner" ]]; then
        cp -f /boot/vmlinuz-${kernel_outname} /boot/Image
    else
        cp -f /boot/vmlinuz-${kernel_outname} /boot/zImage
    fi
    #echo -e "${INFO} Kernel copy results in the [ /boot ] directory: \n$(ls -l /boot) \n"

    # Backup current system files for /usr/lib/modules
    echo -e "${INFO} Backup the files in the [ ${modules_backup_path} ] directory."
    rm -rf ${modules_backup_path} && mkdir -p ${modules_backup_path}
    mv -f /usr/lib/modules/$(uname -r) -t ${modules_backup_path}
    # Copy modules files
    cp -rf ${output_path}/modules/lib/modules/${kernel_outname} -t /usr/lib/modules
    #echo -e "${INFO} Kernel copy results in the [ /usr/lib/modules ] directory: \n$(ls -l /usr/lib/modules) \n"

    # COMPRESS: [ gzip | lzma | xz | zstd ]
    echo -e "${INFO} Set the [ ${compress_format} ] compression format for the initrd.img file."
    [[ "${kernel_outname}" =~ ^5.4.[0-9]+ ]] && compress_format="xz"
    compress_initrd_file="/etc/initramfs-tools/initramfs.conf"
    if [[ -f "${compress_initrd_file}" ]]; then
        sed -i "s|^COMPRESS=.*|COMPRESS=${compress_format}|g" ${compress_initrd_file}
        compress_settings="$(cat ${compress_initrd_file} | grep -E ^COMPRESS=)"
        echo -e "${INFO} Set the [ ${compress_settings} ] in the initramfs.conf file."
    else
        error_msg "The [ ${compress_initrd_file} ] file does not exist."
    fi

    cd /boot
    echo -e "${STEPS} Generate uInitrd file..."

    # Enable update_initramfs
    [[ -f "${initramfs_conf}" ]] && sed -i "s|^update_initramfs=.*|update_initramfs=yes|g" ${initramfs_conf}

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${kernel_outname}

    # Disable update_initramfs
    [[ -f "${initramfs_conf}" ]] && sed -i "s|^update_initramfs=.*|update_initramfs=no|g" ${initramfs_conf}

    if [[ -f "uInitrd" ]]; then
        echo -e "${SUCCESS} The initrd.img and uInitrd file is Successfully generated."
        [[ ! -L "uInitrd" ]] && mv -vf uInitrd uInitrd-${kernel_outname}
    else
        echo -e "${WARNING} The initrd.img and uInitrd file not updated."
    fi

    echo -e "${INFO} File situation in the /boot directory after update: \n$(ls -hl *${kernel_outname})"

    # Restore the files in the [ /boot ] directory
    mv -f *${kernel_outname} ${output_path}/boot
    mv -f ${boot_backup_path}/* -t .

    # Restore the files in the [ /usr/lib/modules ] directory
    rm -rf /usr/lib/modules/${kernel_outname}
    mv -f ${modules_backup_path}/* -t /usr/lib/modules

    # Remove temporary backup directory
    sync && sleep 3
    rm -rf ${boot_backup_path} ${modules_backup_path}
}

packit_dtbs() {
    # Pack 3 dtbs files
    echo -e "${STEPS} Packing the [ ${kernel_outname} ] dtbs packages..."

    cd ${output_path}/dtb/allwinner
    cp -f ${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/allwinner/*.dtb . 2>/dev/null
    [[ "${?}" -eq "0" ]] && {
        [[ -d "${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/allwinner/overlay" ]] && {
            mkdir -p overlay
            cp -f ${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/allwinner/overlay/*.dtbo overlay/ 2>/dev/null
        }
        tar -czf dtb-allwinner-${kernel_outname}.tar.gz *
        mv -f *.tar.gz ${output_path}/${kernel_version}
        echo -e "${SUCCESS} The [ dtb-allwinner-${kernel_outname}.tar.gz ] file is packaged."
    }

    cd ${output_path}/dtb/amlogic
    cp -f ${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/amlogic/*.dtb . 2>/dev/null
    [[ "${?}" -eq "0" ]] && {
        [[ -d "${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/amlogic/overlay" ]] && {
            mkdir -p overlay
            cp -f ${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/amlogic/overlay/*.dtbo overlay/ 2>/dev/null
        }
        tar -czf dtb-amlogic-${kernel_outname}.tar.gz *
        mv -f *.tar.gz ${output_path}/${kernel_version}
        echo -e "${SUCCESS} The [ dtb-amlogic-${kernel_outname}.tar.gz ] file is packaged."
    }

    cd ${output_path}/dtb/rockchip
    cp -f ${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/rockchip/*.dtb . 2>/dev/null
    [[ "${?}" -eq "0" ]] && {
        [[ -d "${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/rockchip/overlay" ]] && {
            mkdir -p overlay
            cp -f ${kernel_path}/${local_kernel_path}/arch/${SRC_ARCH}/boot/dts/rockchip/overlay/*.dtbo overlay/ 2>/dev/null
        }
        tar -czf dtb-rockchip-${kernel_outname}.tar.gz *
        mv -f *.tar.gz ${output_path}/${kernel_version}
        echo -e "${SUCCESS} The [ dtb-rockchip-${kernel_outname}.tar.gz ] file is packaged."
    }
}

packit_kernel() {
    # Pack 3 kernel files
    echo -e "${STEPS} Packing the [ ${kernel_outname} ] boot, modules and header packages..."

    cd ${output_path}/boot
    rm -rf dtb-*
    chmod +x *
    tar -czf boot-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${output_path}/${kernel_version}
    echo -e "${SUCCESS} The [ boot-${kernel_outname}.tar.gz ] file is packaged."

    cd ${output_path}/modules/lib/modules
    tar -czf modules-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${output_path}/${kernel_version}
    echo -e "${SUCCESS} The [ modules-${kernel_outname}.tar.gz ] file is packaged."

    cd ${output_path}/header
    tar -czf header-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${output_path}/${kernel_version}
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
    cd ${output_path}/${kernel_version}
    sha256sum * >sha256sums
    echo -e "${SUCCESS} The [ sha256sums ] file has been generated"

    cd ${output_path}
    tar -czf ${kernel_version}.tar.gz ${kernel_version}

    echo -e "${INFO} Kernel series files are stored in [ ${output_path} ]."
    echo -e "${INFO} Current space usage: \n$(df -hT ${output_path}) \n"
}

clean_tmp() {
    cd ${current_path}
    echo -e "${STEPS} Clear the space..."

    sync && sleep 3
    rm -rf ${output_path}/{boot/,dtb/,modules/,header/,${kernel_version}/}
    [[ "${delete_source}" == "true" ]] && rm -rf ${kernel_path}/* 2>/dev/null
    rm -rf ${tmp_backup_path}

    echo -e "${SUCCESS} All processes have been completed."
}

loop_recompile() {
    cd ${current_path}

    j="1"
    for k in ${build_kernel[@]}; do
        # kernel_version, such as [ 6.1.15 ]
        kernel_version="${k}"
        # kernel <VERSION> and <PATCHLEVEL>, such as [ 6.1 ]
        kernel_verpatch="$(echo ${kernel_version} | awk -F '.' '{print $1"."$2}')"
        # kernel <SUBLEVEL>, such as [ 15 ]
        kernel_sub="$(echo ${kernel_version} | awk -F '.' '{print $3}')"

        # The loop variable assignment
        if [[ -z "${code_repo}" ]]; then
            server_kernel_repo="${code_owner}/linux-${kernel_verpatch}.y"
            local_kernel_path="linux-${kernel_verpatch}.y"
        else
            server_kernel_repo="${code_owner}/${code_repo}"
            local_kernel_path="${code_repo}-${code_branch}"
        fi

        # Show server start information
        echo -e "${INFO} Server space usage before starting to compile: \n$(df -hT ${kernel_path}) \n"

        # Check disk space size
        echo -ne "(${j}) Start compiling the kernel [\033[92m ${kernel_version} \033[0m]. "
        now_remaining_space="$(df -Tk ${kernel_path} | tail -n1 | awk '{print $5}' | echo $(($(xargs) / 1024 / 1024)))"
        if [[ "${now_remaining_space}" -le "15" ]]; then
            echo -e "${WARNING} Remaining space is less than 15G, exit the compilation."
            break
        else
            echo "Remaining space is ${now_remaining_space}G."
        fi

        # Execute the following functions in sequence
        get_kernel_source
        compile_env
        compile_selection
        clean_tmp

        let j++
    done
}

# Show welcome message
echo -e "${STEPS} Welcome to compile kernel! \n"
echo -e "${INFO} Server running on Armbian: [ Release: ${host_release} / Host: ${arch_info} ] \n"
# Check script permission, supports running on Armbian system.
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo ./${0} ]"
[[ "${arch_info}" == "aarch64" ]] || error_msg "The script only supports running under Armbian system."

# Initialize variables
init_var "${@}"
# Check and install the toolchain
toolchain_check
# Query the latest kernel version
[[ "${auto_kernel}" == "true" || "${auto_kernel}" == "yes" ]] && query_version

# Show compile settings
echo -e "${INFO} Kernel compilation toolchain: [ ${toolchain_name} ]"
echo -e "${INFO} Kernel from: [ ${code_owner} ]"
echo -e "${INFO} Kernel patch: [ ${auto_patch} ]"
echo -e "${INFO} Kernel arch: [ ${SRC_ARCH} ]"
echo -e "${INFO} Kernel Package: [ ${package_list} ]"
echo -e "${INFO} kernel signature: [ ${custom_name} ]"
echo -e "${INFO} Latest kernel version: [ ${auto_kernel} ]"
echo -e "${INFO} kernel initrd compress: [ ${compress_format} ]"
echo -e "${INFO} Kernel List: [ $(echo ${build_kernel[@]} | xargs) ] \n"

# Loop to compile the kernel
loop_recompile

# Show server end information
echo -e "${STEPS} Server space usage after compilation: \n$(df -hT ${kernel_path}) \n"
echo -e "${SUCCESS} All process completed successfully."
# All process completed
wait
