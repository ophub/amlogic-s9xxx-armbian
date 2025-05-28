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
# Description: Make a docker version of Armbian.
# Copyright (C) 2021~ https://github.com/ophub/amlogic-s9xxx-armbian
#
#
# Command: ./compile-kernel/tools/script/docker/armbian_docker.sh
#
#======================================== Functions list ========================================
#
# error_msg         : Output error message
# check_depends     : Check dependencies
# find_armbian      : Find Armbian file (armbian/*rootfs.tar.gz)
# adjust_settings   : Adjust related file settings
# make_dockerimg    : make docker image
#
#================================ Set make environment variables ================================
#
# Set default parameters
current_path="${PWD}"
armbian_path="${current_path}/armbian"
armbian_rootfs_file="*rootfs.tar.gz"
docker_path="${current_path}/compile-kernel/tools/script/docker"
tmp_path="${current_path}/tmp"
out_path="${current_path}/out"
compile_path="${current_path}/compile-kernel"
kernel_script="${compile_path}/tools/script/armbian_compile_kernel_script.sh"

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

check_depends() {
    # Check the necessary dependencies
    is_dpkg="0"
    dpkg_packages=("tar" "gzip")
    i="1"
    for package in ${dpkg_packages[*]}; do
        [[ -n "$(dpkg -l | awk '{print $2}' | grep -w "^${package}$" 2>/dev/null)" ]] || is_dpkg="1"
        let i++
    done

    # Install missing packages
    if [[ "${is_dpkg}" -eq "1" ]]; then
        echo -e "${STEPS} Start installing the necessary dependencies..."
        sudo apt-get update
        sudo apt-get install -y ${dpkg_packages[*]}
        [[ "${?}" -ne "0" ]] && error_msg "Dependency installation failed."
    fi
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

make_dockerimg() {
    cd ${current_path}
    echo -e "${STEPS} Start making docker image..."

    # Make docker image
    echo -e "${INFO} Unpack Armbian."
    rm -rf ${tmp_path} && mkdir -p ${tmp_path}
    tar -xzf ${armbian_path}/${armbian_file_name} -C ${tmp_path}

    cd ${tmp_path}
    # Add script for compile kernel
    cp -af --no-preserve=ownership ${compile_path} opt/
    cp -f --no-preserve=ownership ${kernel_script} opt/recompile

    tar -czf armbian-${version_codename}-rootfs.tar.gz *
    [[ "${?}" -eq "0" ]] || error_msg "Docker image creation failed."

    # Move the docker image to the output directory
    rm -rf ${out_path} && mkdir -p ${out_path}
    mv -f armbian-${version_codename}-rootfs.tar.gz ${out_path}
    [[ "${?}" -eq "0" ]] || error_msg "Docker image move failed."
    echo -e "${INFO} Docker image packaging succeeded."

    cd ${current_path}

    # Add Dockerfile
    cp -f ${docker_path}/Dockerfile ${out_path}
    [[ "${?}" -eq "0" ]] || error_msg "Dockerfile addition failed."
    sed -i "s|armbian-rootfs.tar.gz|armbian-${version_codename}-rootfs.tar.gz|g" ${out_path}/Dockerfile
    [[ "${?}" -eq "0" ]] || error_msg "Dockerfile version codename replacement failed."
    echo -e "${INFO} Dockerfile added successfully."

    # Remove temporary directory
    rm -rf ${tmp_path}

    sync && sleep 3
    echo -e "${INFO} Docker files list: \n$(ls -lh ${out_path})"
    echo -e "${SUCCESS} Docker image successfully created."
}

# Show welcome message
echo -e "${STEPS} Welcome to the Docker Image Maker Tool."
echo -e "${INFO} Make path: [ ${PWD} ]"
#
check_depends
find_armbian
make_dockerimg
#
# All process completed
wait
