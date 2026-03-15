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
# error_msg     : Output error message and exit
# check_docker  : Check and install Docker environment
# find_armbian  : Find Armbian rootfs file (armbian/*rootfs.tar.gz)
# build_docker  : Build the Armbian Docker image
# export_docker : Export offline Docker image
#
#================================ Set make environment variables ================================
#
# Set default parameters
current_path="${PWD}"
armbian_path="${current_path}/armbian"
armbian_rootfs_file="*rootfs.tar.gz"
docker_path="${current_path}/compile-kernel/tools/script/docker"
out_path="${current_path}/out"
# Set Docker image name and tag
[[ -n "${1}" ]] && docker_os="${1}" || docker_os="server"
[[ -n "${2}" ]] && docker_arch="${2}" || docker_arch="arm64"

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

check_docker() {
    echo -e "${STEPS} Checking Docker environment..."

    # Check if docker command exists
    if command -v docker >/dev/null 2>&1; then
        local docker_ver="$(docker --version 2>/dev/null)"
        echo -e "${INFO} Docker is installed: [ ${docker_ver} ]"

        # Check if Docker daemon is running
        if docker info >/dev/null 2>&1; then
            echo -e "${INFO} Docker daemon is running."
        else
            echo -e "${WARNING} Docker daemon is not running, attempting to start..."
            sudo systemctl start docker 2>/dev/null || sudo service docker start 2>/dev/null
            sleep 3
            docker info >/dev/null 2>&1 || error_msg "Failed to start Docker daemon."
            echo -e "${SUCCESS} Docker daemon started successfully."
        fi
        return 0
    fi

    echo -e "${INFO} Docker is not installed, installing..."
    # Install Docker using the official convenience script
    curl -fsSL https://get.docker.com | sudo sh
    sudo usermod -aG docker ${USER}

    # Verify installation
    command -v docker >/dev/null 2>&1 || error_msg "Docker installation failed."

    # Start and enable Docker service
    sudo systemctl start docker 2>/dev/null || sudo service docker start 2>/dev/null
    sudo systemctl enable docker 2>/dev/null
    sleep 3
    docker info >/dev/null 2>&1 || error_msg "Docker installed but daemon failed to start."

    echo -e "${SUCCESS} Docker installed and started successfully: [ $(docker --version) ]"
}

find_armbian() {
    cd ${current_path}
    echo -e "${STEPS} Searching for Armbian rootfs file..."

    # Find whether the Armbian file exists
    armbian_file_name="$(ls ${armbian_path}/${armbian_rootfs_file} 2>/dev/null | head -n 1 | awk -F "/" '{print $NF}')"
    if [[ -n "${armbian_file_name}" ]]; then
        echo -e "${INFO} Armbian docker image file: [ ${armbian_file_name} ]."
    else
        error_msg "There is no [ ${armbian_rootfs_file} ] file in the [ ${armbian_path} ] directory."
    fi

    # Check whether the Dockerfile exists
    [[ -f "${docker_path}/Dockerfile" ]] || error_msg "Missing Dockerfile."
}

build_docker() {
    cd ${current_path}
    echo -e "${STEPS} Building Armbian Docker image..."

    # Move the docker image to the output directory
    rm -rf ${out_path} && mkdir -p ${out_path}
    cp -f ${armbian_path}/${armbian_file_name} ${out_path}/${armbian_file_name}
    [[ "${?}" -eq "0" ]] || error_msg "Docker image move failed."
    echo -e "${INFO} Docker rootfs file added successfully."

    # Add Dockerfile
    cp -f ${docker_path}/Dockerfile ${out_path}
    [[ "${?}" -eq "0" ]] || error_msg "Dockerfile addition failed."
    cp -f ${docker_path}/docker_startup.sh ${out_path}
    [[ "${?}" -eq "0" ]] || error_msg "Custom startup script addition failed."
    sed -i "s|^ADD armbian-.*.tar.gz|ADD ${armbian_file_name}|g" ${out_path}/Dockerfile
    [[ "${?}" -eq "0" ]] || error_msg "Dockerfile version codename replacement failed."
    echo -e "${INFO} Dockerfile added successfully."

    # Display the output directory
    sync && sleep 3
    echo -e "${INFO} Docker files list: \n$(ls -lh ${out_path})"
}

export_docker() {
    cd ${current_path}

    echo -e "${STEPS} Building and exporting offline Docker image..."

    # Set image name and tag (Docker requires lowercase image names)
    local docker_image_name="armbian-docker-${docker_os}-${docker_arch}"
    local full_image="${docker_image_name}:${docker_arch}"

    # Build docker image locally
    cd ${out_path}
    docker build --platform linux/${docker_arch} -t "${full_image}" .
    [[ "${?}" -eq "0" ]] || error_msg "Docker image build failed."
    echo -e "${INFO} Docker image [ ${full_image} ] built successfully."

    # Clean up build artifacts, keep only the exported image
    rm -f ${out_path}/{${armbian_file_name},Dockerfile,docker_startup.sh}

    # Export image to tar.gz file
    local export_file="${out_path}/${docker_image_name}.tar.gz"
    (set -o pipefail && docker save "${full_image}" | gzip >"${export_file}")
    [[ "${?}" -eq "0" ]] || error_msg "Docker image export failed."

    # Display export info
    local file_size="$(ls -lh ${export_file} | awk '{print $5}')"
    echo -e "${SUCCESS} Offline Docker image exported: [ ${export_file} ] (${file_size})"
    echo -e "${INFO} Users can import with: [ docker load -i $(basename ${export_file}) ]"
}

# Show welcome message
echo -e "${STEPS} Welcome to the Armbian Docker Image Builder."
echo -e "${INFO} Current path: [ ${current_path} ]"
#
check_docker
find_armbian
build_docker
export_docker
#
echo -e "${SUCCESS} Docker image files created successfully."
