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
# Copyright (C) 2021- https://github.com/unifreq
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: armbian-kernel -update && armbian-kernel -k 5.10.125
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
current_path="${PWD}"
compile_path="${current_path}/compile-kernel"
kernel_path="${compile_path}/kernel"
config_path="${compile_path}/tools/config"
script_path="${compile_path}/tools/script"
out_kernel="${compile_path}/output"
ophub_release_file="/etc/ophub-release"
arch_info="$(arch)"
host_release="$(cat /etc/os-release | grep '^VERSION_CODENAME=.*' | cut -d"=" -f2)"

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

# Compile toolchain download mirror, run on Armbian
dev_repo="https://github.com/ophub/kernel/releases/download/dev"
# Arm GNU Toolchain source: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
gun_file="arm-gnu-toolchain-12.2.rel1-aarch64-aarch64-none-elf.tar.xz"
# Set the toolchain path
toolchain_path="/usr/local/toolchain"
# Set the default cross-compilation toolchain: [ gcc / clang ]
toolchain_name="clang"

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
    get_all_ver="$(getopt "k:a:n:p:r:t:" "${@}")"

    while [[ -n "${1}" ]]; do
        case "${1}" in
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
                custom_name="${2// /}"
                [[ ${custom_name:0:1} != "-" ]] && custom_name="-${custom_name}"
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
        -t | --Toolchain)
            if [[ -n "${2}" ]]; then
                toolchain_name="${2}"
                shift
            else
                error_msg "Invalid -t parameter [ ${2} ]!"
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

    # Download the cross-compilation toolchain: [ clang / gcc ]
    [[ -d "/etc/apt/sources.list.d" ]] || mkdir -p /etc/apt/sources.list.d
    if [[ "${toolchain_name}" == "clang" ]]; then
        # Set llvm version
        llvm_version="14"
        echo -e "${INFO} Start installing the [ llvm: ${llvm_version} ] toolchain..."

        # Add apt source for llvm
        llvm_toolchain_list="/etc/apt/sources.list.d/llvm-toolchain.list"
        echo "deb http://apt.llvm.org/${host_release}/ llvm-toolchain-${host_release}-${llvm_version} main" >${llvm_toolchain_list}
        echo "deb-src http://apt.llvm.org/${host_release}/ llvm-toolchain-${host_release}-${llvm_version} main" >>${llvm_toolchain_list}
        [[ -s "${llvm_toolchain_list}" ]] || error_msg "failed to add apt source: [ ${llvm_toolchain_list} ]"
        # Add gpg key for llvm
        llvm_toolchain_asc="/etc/apt/trusted.gpg.d/llvm-toolchain.asc"
        curl -sL "https://apt.llvm.org/llvm-snapshot.gpg.key" -o "${llvm_toolchain_asc}"
        [[ -s "${llvm_toolchain_asc}" ]] || error_msg "failed to add gpg key: [ ${llvm_toolchain_asc} ]"

        # Set up the installation package, refer to the source: https://apt.llvm.org/
        llvm_pkg="\
        clang-${llvm_version} lldb-${llvm_version} lld-${llvm_version} clangd-${llvm_version} clang-tidy-${llvm_version} \
        clang-format-${llvm_version} clang-tools-${llvm_version} llvm-${llvm_version}-dev lld-${llvm_version} lldb-${llvm_version} \
        llvm-${llvm_version}-tools libomp-${llvm_version}-dev libc++-${llvm_version}-dev libc++abi-${llvm_version}-dev \
        libclang-common-${llvm_version}-dev libclang-${llvm_version}-dev libclang-dev libunwind-${llvm_version}-dev \
        "
        # Install llvm
        apt-get -qq install -y ${llvm_pkg}

        # Set cross compilation parameters
        export CROSS_COMPILE="aarch64-linux-gnu-"
        export CC="clang"
        export LD="ld.lld"
        export MFLAGS=" LLVM=1 LLVM_IAS=1 "
    else
        # Download Arm GNU Toolchain
        [[ -d "${toolchain_path}" ]] || mkdir -p ${toolchain_path}
        if [[ ! -d "${toolchain_path}/${gun_file//.tar.xz/}/bin" ]]; then
            echo -e "${INFO} Start downloading the ARM GNU toolchain [ ${gun_file} ]..."
            wget -q "${dev_repo}/${gun_file}" -O "${toolchain_path}/${gun_file}"
            [[ "${?}" -eq "0" ]] || error_msg "GNU toolchain file download failed"
            tar -xJf ${toolchain_path}/${gun_file} -C ${toolchain_path}
            rm -f ${toolchain_path}/${gun_file}
            [[ -d "${toolchain_path}/${gun_file//.tar.xz/}/bin" ]] || error_msg "The gcc is not set!"
        fi

        # Add ${PATH} variable
        path_armbian="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin"
        path_gcc="${toolchain_path}/${gun_file//.tar.xz/}/bin:${path_armbian}"
        export PATH="${path_gcc}"

        # Set cross compilation parameters
        export CROSS_COMPILE="${toolchain_path}/${gun_file//.tar.xz/}/bin/aarch64-none-elf-"
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
    cd ${current_path}
    echo -e "${STEPS} Start downloading the kernel source code..."

    # kernel_folder > kernel_.tar.xz_file > download_from_kernel.org
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
                wget -q -P ${kernel_path} ${server_kernel_repo}${local_kernel_path}.tar.xz
                if [[ "${?}" -eq "0" && -s "${kernel_path}/${local_kernel_path}.tar.xz" ]]; then
                    echo -e "${SUCCESS} The kernel file is downloaded successfully."
                    cd ${kernel_path}
                    tar -xJf ${local_kernel_path}.tar.xz
                    [[ -d "${local_kernel_path}" ]] || error_msg "[ ${local_kernel_path}.tar.xz ] file decompression failed."
                else
                    error_msg "Kernel file download failed!"
                fi
            fi
        else
            echo -e "${INFO} Start cloning from [ https://github.com/${server_kernel_repo} -b ${code_branch} ]"
            git clone -q --single-branch --depth 1 https://github.com/${server_kernel_repo} -b ${code_branch} ${kernel_path}/${local_kernel_path}
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
    cp -f .config ${out_kernel}/header/.config

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
    rm -rf ${out_kernel}/{boot/,dtb/,modules/,header/,${kernel_version}/}
    mkdir -p ${out_kernel}/{boot/,dtb/{allwinner/,amlogic/,rockchip/},modules/,header/,${kernel_version}/}

    cd ${kernel_path}/${local_kernel_path}
    echo -e "${STEPS} Set compilation parameters."

    # Set compilation parameters
    export ARCH="arm64"
    export LOCALVERSION="${custom_name}"

    # Show variable
    echo -e "${INFO} ARCH: [ ${ARCH} ]"
    echo -e "${INFO} LOCALVERSION: [ ${LOCALVERSION} ]"
    echo -e "${INFO} CROSS_COMPILE: [ ${CROSS_COMPILE} ]"
    echo -e "${INFO} CC: [ ${CC} ]"
    echo -e "${INFO} LD: [ ${LD} ]"

    # Set generic make string
    MAKE_SET_STRING=" ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CC=${CC} LD=${LD} ${MFLAGS} LOCALVERSION=${LOCALVERSION} "

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
    cd ${current_path}
    echo -e "${STEPS} Generate uInitrd environment initialization..."

    # Backup current system files for /boot
    echo -e "${INFO} Backup the files in the [ /boot ] directory."
    boot_backup_path="/boot/backup"
    rm -rf ${boot_backup_path} && mkdir -p ${boot_backup_path}
    mv -f /boot/{config-*,initrd.img-*,System.map-*,uInitrd-*,vmlinuz-*,uInitrd,zImage,Image} ${boot_backup_path} 2>/dev/null
    # Copy /boot related files into armbian system
    cp -f ${kernel_path}/${local_kernel_path}/System.map /boot/System.map-${kernel_outname}
    cp -f ${kernel_path}/${local_kernel_path}/.config /boot/config-${kernel_outname}
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/Image /boot/vmlinuz-${kernel_outname}
    [[ "${PLATFORM}" == "amlogic" ]] && cp -f /boot/vmlinuz-${kernel_outname} /boot/zImage
    [[ "${PLATFORM}" == "rockchip" ]] && ln -sf vmlinuz-${kernel_outname} /boot/Image
    #echo -e "${INFO} Kernel copy results in the [ /boot ] directory: \n$(ls -l /boot) \n"

    # Backup current system files for /usr/lib/modules
    echo -e "${INFO} Backup the files in the [ /usr/lib/modules ] directory."
    modules_backup_path="/usr/lib/modules/backup"
    rm -rf ${modules_backup_path} && mkdir -p ${modules_backup_path}
    mv -f /usr/lib/modules/$(uname -r) ${modules_backup_path}
    # Copy modules files
    cp -rf ${out_kernel}/modules/lib/modules/${kernel_outname} /usr/lib/modules
    #echo -e "${INFO} Kernel copy results in the [ /usr/lib/modules ] directory: \n$(ls -l /usr/lib/modules) \n"

    # COMPRESS: [ gzip | bzip2 | lz4 | lzma | lzop | xz | zstd ]
    compress_initrd_file="/etc/initramfs-tools/initramfs.conf"
    sed -i "/^COMPRESS=/d" ${compress_initrd_file}
    echo "COMPRESS=gzip" >>${compress_initrd_file}

    cd /boot
    echo -e "${STEPS} Generate uInitrd file..."

    # Generate uInitrd file directly under armbian system
    update-initramfs -c -k ${kernel_outname} 2>/dev/null

    if [[ -f uInitrd ]]; then
        echo -e "${SUCCESS} The initrd.img and uInitrd file is Successfully generated."
        mv -f uInitrd uInitrd-${kernel_outname} 2>/dev/null
    else
        echo -e "${WARNING} The initrd.img and uInitrd file not updated."
    fi

    echo -e "${INFO} File situation in the /boot directory after update: \n$(ls -l *${kernel_outname})"

    # Restore the files in the [ /boot ] directory
    mv -f *${kernel_outname} ${out_kernel}/boot
    mv -f ${boot_backup_path}/* . && rm -rf ${boot_backup_path}

    # Restore the files in the [ /usr/lib/modules ] directory
    rm -rf /usr/lib/modules/${kernel_outname}
    mv -f ${modules_backup_path}/* /usr/lib/modules && rm -rf ${modules_backup_path}
}

packit_dtbs() {
    # Pack 3 dtbs files
    echo -e "${STEPS} Packing the [ ${kernel_outname} ] dtbs packages..."

    cd ${out_kernel}/dtb/allwinner
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/dts/allwinner/*.dtb . && chmod +x *
    tar -czf dtb-allwinner-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${out_kernel}/${kernel_version}
    echo -e "${SUCCESS} The [ dtb-allwinner-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/dtb/amlogic
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/dts/amlogic/*.dtb . && chmod +x *
    tar -czf dtb-amlogic-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${out_kernel}/${kernel_version}
    echo -e "${SUCCESS} The [ dtb-amlogic-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/dtb/rockchip
    cp -f ${kernel_path}/${local_kernel_path}/arch/arm64/boot/dts/rockchip/*.dtb . && chmod +x *
    tar -czf dtb-rockchip-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${out_kernel}/${kernel_version}
    echo -e "${SUCCESS} The [ dtb-rockchip-${kernel_outname}.tar.gz ] file is packaged."
}

packit_kernel() {
    # Pack 3 kernel files
    echo -e "${STEPS} Packing the [ ${kernel_outname} ] boot, modules and header packages..."

    cd ${out_kernel}/boot
    chmod +x *
    tar -czf boot-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${out_kernel}/${kernel_version}
    echo -e "${SUCCESS} The [ boot-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/modules/lib/modules
    tar -czf modules-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${out_kernel}/${kernel_version}
    echo -e "${SUCCESS} The [ modules-${kernel_outname}.tar.gz ] file is packaged."

    cd ${out_kernel}/header
    tar -czf header-${kernel_outname}.tar.gz *
    mv -f *.tar.gz ${out_kernel}/${kernel_version}
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
    sha256sum * >sha256sums
    echo -e "${SUCCESS} The [ sha256sums ] file has been generated"

    cd ${out_kernel}
    tar -czf ${kernel_version}.tar.gz ${kernel_version} && sync && sleep 3

    echo -e "${INFO} Kernel series files are stored in [ ${out_kernel} ]."
}

clean_tmp() {
    cd ${current_path}
    echo -e "${STEPS} Clear the space..."

    rm -rf ${out_kernel}/{boot/,dtb/,modules/,header/,${kernel_version}/}

    echo -e "${SUCCESS} All processes have been completed."
}

loop_recompile() {
    cd ${current_path}

    j="1"
    for k in ${build_kernel[*]}; do
        # kernel_version, such as [ 5.10.125 ]
        kernel_version="${k}"
        # kernel <VERSION> and <PATCHLEVEL>, such as [ 5.10 ]
        kernel_verpatch="$(echo ${kernel_version} | awk -F '.' '{print $1"."$2}')"
        # kernel <SUBLEVEL>, such as [ 125 ]
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
[[ "${auto_kernel}" == "true" ]] && query_version

# Show compile settings
echo -e "${INFO} Kernel compilation toolchain: [ ${toolchain_name} ]"
echo -e "${INFO} Kernel from: [ ${code_owner} ]"
echo -e "${INFO} Kernel List: [ $(echo ${build_kernel[*]} | xargs) ] \n"
# Show server start information
echo -e "${INFO} Server CPU configuration information: \n$(cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c) \n"
echo -e "${INFO} Server memory usage: \n$(free -h) \n"
echo -e "${INFO} Server space usage before starting to compile: \n$(df -hT ${current_path}) \n"

# Loop to compile the kernel
loop_recompile

# Show server end information
echo -e "${STEPS} Server space usage after compilation: \n$(df -hT ${current_path}) \n"
echo -e "${SUCCESS} All process completed successfully."
# All process completed
wait
