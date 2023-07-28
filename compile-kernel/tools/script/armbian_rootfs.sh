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
# Description: Redo the Armbian rootfs file.
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: ./compile-kernel/tools/script/redo_armbian_rootfs.sh -v <VERSION_CODENAME>
#          ./compile-kernel/tools/script/redo_armbian_rootfs.sh -v bookworm
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
    get_all_ver="$(getopt "v:" "${@}")"

    while [[ -n "${1}" ]]; do
        case "${1}" in
        -v | --VERSION_CODENAME)
            if [[ -n "${2}" ]]; then
                version_codename="${2}"
                shift
            else
                error_msg "Invalid -v parameter [ ${2} ]!"
            fi
            ;;
        *)
            error_msg "Invalid option [ ${1} ]!"
            ;;
        esac
        shift
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
    [[ "${?}" == "0" ]] && echo -e "${INFO} 01. Temporary directory creation completed." || error_msg "01. Failed to create directory!"

    # Redo Armbian rootfs
    [[ -n "${rootfs_file}" ]] && {
        sudo tar -mxf ${rootfs_file} -C ${tmp_rootfs}/
        cd ${tmp_rootfs}/
        sudo tar -czf ${rootfs_save_name}.tar.gz *
        sudo mv -f ${rootfs_save_name}.tar.gz ../
        [[ "${?}" == "0" ]] && echo -e "${INFO} 02. Making Armbian rootfs completed." || error_msg "02. Failed to redo rootfs!"
    } || error_msg "02. Failed to find rootfs file!"

    # Rename Armbian image
    [[ -n "${image_file}" ]] && {
        cd ${image_path}/
        mv -f ${image_file} ${image_save_name}
        pigz -qf *.img || gzip -qf *.img
        [[ "${?}" == "0" ]] && echo -e "${INFO} 03. Renaming Armbian image completed." || error_msg "03. Failed to rename the image!"
    } || error_msg "03. Failed to find Armbian image!"

    # Add sha256sum verification files
    cd ${image_path}/
    sudo rm -rf $(ls . | grep -vE ".img.gz|.tar.gz" | xargs) 2>/dev/null
    for file in *; do [[ ! -d "${file}" ]] && sha256sum "${file}" >"${file}.sha"; done
    [[ "${?}" == "0" ]] && echo -e "${INFO} 04. The files in the current directory:\n$(ls -l .)" || error_msg "04. Failed to add sha256sum!"

    # Delete Armbian build source codes and temporary files
    cd ${build_path}/
    sudo rm -rf $(ls . | grep -v "^output$" | xargs)
    [[ "${?}" == "0" ]] && echo -e "${INFO} 05. Armbian source code cleanup completed." || error_msg "05. Failed to clean up!"

    sync && sleep 3
}

echo -e "${STEPS} Start to redo the Armbian rootfs file."

# Initialize variables
init_var "${@}"
# Redo Armbian rootfs
redo_rootfs
