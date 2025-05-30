#!/bin/bash
#================================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Build Armbian docker image.
# Copyright (C) 2021~ https://github.com/ophub/amlogic-s9xxx-armbian
#
#
# Command: ./compile-kernel/tools/script/docker/build_armbian_docker_image.sh
#
#======================================== Functions list ========================================
#
# error_msg    : Output error message
# find_armbian : Find Armbian file (armbian/*rootfs.tar.gz)
# build_docker : Build Armbian docker image
#
#================================ Set make environment variables ================================
#
# Set default parameters
current_path="${PWD}"
armbian_path="${current_path}/armbian"
armbian_rootfs_file="*rootfs.tar.gz"
docker_path="${current_path}/compile-kernel/tools/script/docker"
out_path="${current_path}/out"

# Set default parameters
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
WARNING="[\033[93m WARNING \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#================================================================================================

error_msg() {
    echo -e "${ERROR} ${1}"
    exit 1
}

find_armbian() {
    cd ${current_path}
    echo -e "${STEPS} Start searching for Armbian file..."

    # Find whether the Armbian file exists
    armbian_file_name="$(ls ${armbian_path}/${armbian_rootfs_file} 2>/dev/null | head -n 1 | awk -F "/" '{print $NF}')"
    if [[ -n "${armbian_file_name}" ]]; then
        version_codename="$(echo "${armbian_file_name}" | awk -F'[_-]' '{print $3}')"
        echo -e "${INFO} Armbian file: [ ${armbian_file_name} ], version codename: [ ${version_codename} ]"
    else
        error_msg "There is no [ ${armbian_rootfs_file} ] file in the [ ${armbian_path} ] directory."
    fi

    # Check whether the Dockerfile exists
    [[ -f "${docker_path}/Dockerfile" ]] || error_msg "Missing Dockerfile."
}

build_docker() {
    cd ${current_path}
    echo -e "${STEPS} Start building Armbian docker image..."

    # Move the docker image to the output directory
    rm -rf ${out_path} && mkdir -p ${out_path}
    mv -f ${armbian_path}/${armbian_file_name} ${out_path}/armbian-${version_codename}-rootfs.tar.gz
    [[ "${?}" -eq "0" ]] || error_msg "Docker image move failed."
    echo -e "${INFO} Docker rootfs file added successfully."

    # Add Dockerfile
    cp -f ${docker_path}/Dockerfile ${out_path}
    [[ "${?}" -eq "0" ]] || error_msg "Dockerfile addition failed."
    sed -i "s|^ADD armbian-.*.tar.gz|ADD armbian-${version_codename}-rootfs.tar.gz|g" ${out_path}/Dockerfile
    [[ "${?}" -eq "0" ]] || error_msg "Dockerfile version codename replacement failed."
    echo -e "${INFO} Dockerfile added successfully."

    # Display the output directory
    sync && sleep 3
    echo -e "${INFO} Docker files list: \n$(ls -lh ${out_path})"
    echo -e "${SUCCESS} Docker image created successfully."
}

# Show welcome message
echo -e "${STEPS} Welcome to the Armbian Docker Image Builder."
echo -e "${INFO} Make path: [ ${PWD} ]"
#
find_armbian
build_docker
#
# All process completed
wait
