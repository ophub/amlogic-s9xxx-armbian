#!/bin/bash
#============================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Function: Execute software install/update/remove command
# Copyright (C) 2021- https://github.com/unifreq/openwrt_packit
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
#============================== Functions list ==============================
#
# error_msg                 : Output error message
# check_release             : Check release file
# software_install          : Install package
# software_update           : Update package
# software_remove           : Remove package
# docker_container_remove   : Delete the docker container
# docker_image_remove       : Delete the docker image
# docker_update             : Update docker
# docker_remove             : Remove docker
# init_var                  : Initialize variables
#
#========================== Set default parameters ==========================
#
# Get custom firmware information
software_path="/usr/share/ophub/armbian-software"
command_docker="${software_path}/command-docker.sh"
ophub_release_file="/etc/ophub-release"
#
# Docker-related default settings
docker_path="/opt/docker"
download_path="/opt/downloads"
movie_path="/opt/movies"
music_path="/opt/music"
tv_path="/opt/tv"
docker_puid="1000"
docker_pgid="1000"
docker_tz="Asia/Shanghai"
#
# Get current network status
my_network_card="$(cat /proc/net/dev 2>/dev/null | awk '{i++; if(i>2){print $1}}' | sed 's/^[\t]*//g' | sed 's/[:]*$//g' | grep -E 'e' | head -n 1)"
my_ifconfig="$(ifconfig -a 2>/dev/null | grep inet | grep -v 'inet6.*' | grep -v 'inet 172.*' | grep -v 'inet 127.*' | head -n1)"
my_address="$(echo ${my_ifconfig} | awk '{print $2}')"
my_netmask="$(echo ${my_ifconfig} | awk '{print $4}')"
my_broadcast="$(echo ${my_ifconfig} | awk '{print $6}')"
my_gateway="$(route -n 2>/dev/null | awk '($2~/^1/){print $2}' | head -n1)"
my_macvlan="$(docker network ls 2>/dev/null | grep macvlan | awk '($2 == "macnet" && $3 == "macvlan"){print $2,$3}' | head -n1)"
my_mac="$(ip a | grep ether | cut -d ' ' -f 6 | head -n 1)"
my_hostname="$(hostname)"
#
# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
OPTIONS="[\033[93m OPTIONS \033[0m]"
NOTE="[\033[93m NOTE \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#============================================================================

# Show error message
error_msg() {
    echo -e "${ERROR} ${1}"
    exit 1
}

check_release() {
    if [[ -f "${ophub_release_file}" ]]; then
        source "${ophub_release_file}" 2>/dev/null
        VERSION_CODEID="${VERSION_CODEID}"
        VERSION_CODENAME="${VERSION_CODENAME}"
    else
        error_msg "${ophub_release_file} file is missing!"
    fi

    [[ -n "${VERSION_CODEID}" && -n "${VERSION_CODENAME}" ]] || error_msg "${ophub_release_file} value is missing!"
}

software_install() {
    install_list="${1}"
    echo -e "${STEPS} Start installing packages: [ ${install_list} ]..."

    # Install the package
    sudo apt-get update
    [[ -n "${install_list}" ]] && sudo apt-get install -y ${install_list}

    echo -e "${SUCCESS} [ ${install_list} ] packages installed successfully."
}

software_update() {
    echo -e "${STEPS} Start updating packages..."

    # Update the package
    sudo apt-get update
    sudo apt-get upgrade -y
    sudo apt-get dist-upgrade -y
    sudo apt-get --purge autoremove -y
    sudo apt-get autoclean -y

    echo -e "${SUCCESS} Package updated successfully."
}

software_remove() {
    remove_list="${1}"
    echo -e "${STEPS} Start removing packages: [ ${remove_list} ]..."

    # Update the package
    sudo apt-get update
    [[ -n "${remove_list}" ]] && sudo apt-get remove --purge -y ${remove_list}
    sudo apt-get --purge autoremove -y
    sudo apt-get autoclean -y

    echo -e "${SUCCESS} [ ${remove_list} ] packages removed successfully."
}

# Delete the docker container
docker_container_remove() {
    local container_name="${1}"
    [[ -n "${container_name}" ]] || error_msg "Docker container name is empty!"

    # Query the container ID based on the image name and delete it
    echo -e "${STEPS} Start removing container: [ ${container_name} ]..."
    docker stop $(docker ps -aq --filter name=${container_name})
    docker rm -f $(docker ps -aq --filter name=${container_name})
    echo -e "${SUCCESS} ${container_name} removed successfully."
}

# Delete the docker image
docker_image_remove() {
    local image_name="${1}"
    [[ -n "${image_name}" ]] || error_msg "Docker image name is empty!"

    # Query the image ID based on the image name and delete it
    echo -e "${STEPS} Start removing image: [ ${image_name} ]..."
    docker image rm -f $(docker images -q --filter reference=${image_name})
    # Automatic deletion of unused docker images
    docker image prune -f >/dev/null
    echo -e "${SUCCESS} ${image_name} removed successfully."
}

# Update docker
docker_update() {
    [[ -n "${image_name}" && -n "${container_name}" ]] || error_msg "Docker image or container name is empty!"

    echo -e "${STEPS} Start updating the docker: [ ${container_name} ]..."
    # Update docker image
    docker pull "${image_name}"
    # Delete old container
    docker_container_remove "${container_name}"
    # Start a new one
    sudo bash ${command_docker} -s ${software_id} -m install
    # Automatic deletion of unused docker images
    docker image prune -f >/dev/null
}

# Remove docker
docker_remove() {
    [[ -n "${image_name}" && -n "${container_name}" && -n "${install_path}" ]] || error_msg "Docker image, container or path is empty!"

    echo -e "${STEPS} Start removing docker: [ ${container_name} ]..."
    # Delete old container
    docker_container_remove "${container_name}"
    # Delete old image
    docker_image_remove "${image_name}"

    # Delete the installation directory
    echo -ne "${OPTIONS} Delete [ ${install_path} ] directory? (y/n): "
    read del_dir
    [[ "${del_dir,,}" == "y" ]] && [[ -d "${install_path}" ]] && rm -rf ${install_path} 2>/dev/null

    exit 0
}

# Initialize variables
init_var() {
    # If it is followed by [ : ], it means that the option requires a parameter value
    get_all_ver="$(getopt "s:m:" "${@}")"

    # Check the input parameters
    while [[ -n "${1}" ]]; do
        case "${1}" in
        -s | --SoftwareID)
            if [[ -n "${2}" ]]; then
                software_id="${2}"
                shift
            else
                error_msg "Invalid -s parameter [ ${2} ]!"
            fi
            ;;
        -m | --Manage)
            if [[ -n "${2}" ]]; then
                if [[ "${2}" == "install" || "${2}" == "update" || "${2}" == "remove" ]]; then
                    software_manage="${2}"
                else
                    error_msg "Invalid -m parameter [ ${2} ]!"
                fi
                shift
            else
                error_msg "Invalid -m parameter [ ${2} ]!"
            fi
            ;;
        *)
            error_msg "Invalid option [ ${1} ]!"
            ;;
        esac
        shift
    done

    # Get related variables
    check_release
    # Execute the corresponding operation command
    echo -e "${INFO} Software Manage: [ ${software_id} / ${software_manage} ]"
}
