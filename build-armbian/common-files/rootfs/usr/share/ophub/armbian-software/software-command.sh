#!/bin/bash
#============================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Armbian for Amlogic TV Boxes
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Function: Execute software install/update/remove command
# Copyright (C) 2021- https://github.com/unifreq/openwrt_packit
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: software-command -s <software_id> -m <install/update/remove>
# Example: software-command -s 101 -m install
#          software-command -s 101 -m update
#          software-command -s 101 -m remove
#
#============================== Functions list ==============================
#
# error_msg         : Output error message
# check_release     : Check release file
# software_install  : Install package
# software_update   : Update package
# software_remove   : Remove package
# init_var          : Initialize variables
#
# software_101      : For docker
# software_102      : For portainer(docker)
# software_103      : For transmission(docker)
# software_104      : For qbittorrent(docker)
#
# software_201      : For desktop
# software_202      : For vlc-media-player(desktop)
# software_203      : For firefox(desktop)
#
# software_303      : For plex-media-server
#
#========================== Set default parameters ==========================
#
# Get custom firmware information
ophub_release_file="/etc/ophub-release"
docker_path="/opt/docker"
download_path="/opt/downloads"
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
        VERSION_CODENAME="${VERSION_CODEID}"
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
    exit 0
}

software_remove() {
    remove_list="${1}"
    echo -e "${STEPS} Start removing packages: [ ${remove_list} ]..."

    # Update the package
    sudo apt-get update
    [[ -n "${remove_list}" ]] && sudo apt-get remove -y ${remove_list}
    sudo apt-get --purge autoremove -y
    sudo apt-get autoclean -y

    echo -e "${SUCCESS} [ ${remove_list} ] packages removed successfully."
}

# For docker
software_101() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ docker ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        armbian-docker install
        ;;
    update)
        armbian-docker update
        ;;
    remove)
        armbian-docker remove
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For portainer
software_102() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ portainer ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        echo -ne "${OPTIONS} Select Install Portainer-ce?  No=(n) / LAN ip access=(h) / Domain cert access=(s): "
        read pt
        case "${pt}" in
        h | H | http)
            soft_opt="portainer_lan"
            ;;
        s | S | https)
            soft_opt="portainer_domain"
            ;;
        *)
            echo -e "${INFO} Finish the installation." && exit 0
            ;;
        esac
        armbian-docker ${soft_opt}
        ;;
    update)
        armbian-docker update
        ;;
    remove)
        armbian-docker portainer_remove
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For transmission
software_103() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ transmission ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Generate random account
    random_account="$(cat /proc/sys/kernel/random/uuid)"
    # transmission installation path
    tr_path="${docker_path}/transmission"
    tr_default_user="admin"
    tr_default_pass="${random_account:0:18}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ transmission ]..."

        # Check script permission
        [[ "$(id -u)" == "0" ]] || error_msg "please run this script as root: [ sudo ${0} -s 103 -m install ]"

        echo -ne "${OPTIONS} Set login username, the default is [ ${tr_default_user} ]: "
        read tr_user
        [[ -z "${tr_user}" ]] && tr_user="${tr_default_user}"
        echo -e "${INFO} Login username: [ ${tr_user} ]"

        echo -ne "${OPTIONS} Set login password, the default is [ ${tr_default_pass} ]: "
        read tr_pass
        [[ -z "${tr_pass}" ]] && tr_pass="${tr_default_pass}"
        echo -e "${INFO} Login password: [ ${tr_pass} ]"

        [[ -d "${tr_path}" ]] || mkdir -p ${tr_path}

        # Instructions: https://github.com/linuxserver/docker-transmission
        echo -e "${STEPS} Start pulling the docker image: [ linuxserver/transmission:arm64v8-latest ]..."
        docker run -d --name=transmission \
            -e PUID=1000 \
            -e PGID=1000 \
            -e TZ=Asia/Shanghai \
            -e TRANSMISSION_WEB_HOME=/transmission-web-control/ \
            -e USER=${tr_user} \
            -e PASS=${tr_pass} \
            -p 9091:9091 \
            -p 51413:51413 \
            -p 51413:51413/udp \
            -v ${tr_path}/config:/config \
            -v ${tr_path}/watch/folder:/watch \
            -v ${download_path}:/downloads \
            --restart unless-stopped \
            linuxserver/transmission:arm64v8-latest

        # Set the transmission-web-control
        echo -e "${STEPS} Start the installation interface: [ transmission-web-control ]..."
        tr_cn_url="https://github.com/ronggang/transmission-web-control/raw/master/release/install-tr-control-cn.sh"
        bash <(curl -fsSL ${tr_cn_url}) ${tr_path}

        sync && sleep 3
        echo -e "${NOTE} The transmission address [ http://ip:9091 ]"
        echo -e "${NOTE} The transmission account [ username:${tr_user}  /  password:${tr_pass} ]"
        echo -e "${SUCCESS} Transmission installed successfully."
        exit 0
        ;;
    update)
        # Update transmission docker image
        echo -e "${STEPS} Start updating the transmission docker image..."
        docker pull linuxserver/transmission:arm64v8-latest

        # Update transmission-web-control
        echo -e "${STEPS} Start updating the interface: [ transmission-web-control ]..."
        tr_cn_url="https://github.com/ronggang/transmission-web-control/raw/master/release/install-tr-control-cn.sh"
        bash <(curl -fsSL ${tr_cn_url}) ${tr_path}

        # Restart transmission
        echo -e "${STEPS} Restart the transmission docker container..."
        docker restart $(docker ps -aq --filter name=transmission)
        ;;
    remove)
        # Query the container ID based on the image name and delete it
        echo -e "${INFO} Start removing transmission container..."
        docker stop $(docker ps -aq --filter name=transmission)
        docker rm $(docker ps -aq --filter name=transmission)

        # Query the image ID based on the image name and delete it
        echo -e "${INFO} Start removing transmission image..."
        docker image rm $(docker images -q --filter reference=linuxserver/transmission*:*)

        # Delete the transmission installation directory
        [[ -d "${tr_path}" ]] && rm -rf ${tr_path}

        echo -e "${SUCCESS} transmission removed successfully."
        exit 0
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For qbittorrent
software_104() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ qbittorrent ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # qbittorrent installation path
    qb_path="${docker_path}/qbittorrent"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ qbittorrent ]..."

        # Check script permission
        [[ "$(id -u)" == "0" ]] || error_msg "please run this script as root: [ sudo ${0} -s 104 -m install ]"

        [[ -d "${qb_path}" ]] || mkdir -p ${qb_path}

        # Instructions: https://hub.docker.com/r/linuxserver/qbittorrent
        echo -e "${STEPS} Start pulling the docker image: [ linuxserver/qbittorrent:arm64v8-latest ]..."
        docker run -d --name=qbittorrent \
            -e PUID=1000 \
            -e PGID=1000 \
            -e TZ=Asia/Shanghai \
            -e WEBUI_PORT=8080 \
            -p 8080:8080 \
            -p 6881:6881 \
            -p 6881:6881/udp \
            -v ${qb_path}/appdata/config:/config \
            -v ${download_path}:/downloads \
            --restart unless-stopped \
            linuxserver/qbittorrent:arm64v8-latest

        sync && sleep 3
        echo -e "${NOTE} The qbittorrent address [ http://ip:8080 ]"
        echo -e "${NOTE} The qbittorrent account [ username:admin  /  password:adminadmin ]"
        echo -e "${SUCCESS} qbittorrent installed successfully."
        exit 0
        ;;
    update)
        # Update qbittorrent docker image
        echo -e "${STEPS} Start updating the qbittorrent docker image..."
        docker pull linuxserver/qbittorrent:arm64v8-latest

        # Restart qbittorrent
        echo -e "${STEPS} Restart the qbittorrent docker container..."
        docker restart $(docker ps -aq --filter name=qbittorrent)
        ;;
    remove)
        # Query the container ID based on the image name and delete it
        echo -e "${INFO} Start removing qbittorrent container..."
        docker stop $(docker ps -aq --filter name=qbittorrent)
        docker rm $(docker ps -aq --filter name=qbittorrent)

        # Query the image ID based on the image name and delete it
        echo -e "${INFO} Start removing qbittorrent image..."
        docker image rm $(docker images -q --filter reference=linuxserver/qbittorrent*:*)

        # Delete the qbittorrent installation directory
        [[ -d "${qb_path}" ]] && rm -rf ${qb_path}

        echo -e "${SUCCESS} qbittorrent removed successfully."
        exit 0
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For desktop
software_201() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ desktop ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        if [[ "${VERSION_CODEID}" == "ubuntu" ]]; then
            # Install ubuntu-desktop(gdm3) on Ubuntu (jammy/focal)
            software_install "ubuntu-desktop lightdm"

            sync && sleep 3
            echo -e "${SUCCESS} Desktop installation is successful, restarting..."
            reboot
        elif [[ "${VERSION_CODEID}" == "debian" ]]; then
            # Install Xfce(lightdm) on Debian 11 (bullseye)
            software_install "task-xfce-desktop lightdm"

            sync && sleep 3
            echo -e "${SUCCESS} Desktop installation is successful, restarting..."
            reboot
        else
            error_msg "VERSION_CODEID not supported: [ ${VERSION_CODEID} ]"
        fi
        ;;
    remove)
        if [[ "${VERSION_CODEID}" == "ubuntu" ]]; then
            # Remove ubuntu-desktop(gdm3) on Ubuntu (jammy/focal)
            software_remove "ubuntu-desktop lightdm"

            sync && sleep 3
            echo -e "${SUCCESS} Desktop removed successfully, restarting..."
            reboot
        elif [[ "${VERSION_CODEID}" == "debian" ]]; then
            # Remove Xfce(lightdm) on Debian 11 (bullseye)
            software_remove "task-xfce-desktop lightdm"

            sync && sleep 3
            echo -e "${SUCCESS} Desktop removed successfully, restarting..."
            reboot
        else
            error_msg "VERSION_CODEID not supported: [ ${VERSION_CODEID} ]"
        fi
        ;;
    update)
        software_update
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For vlc-media-player
software_202() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ vlc-media-player ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        software_install "vlc"
        ;;
    remove)
        software_remove "vlc"
        ;;
    update)
        software_update
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For firefox
software_203() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ firefox ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        [[ "${VERSION_CODEID}" == "ubuntu" ]] && software_install "firefox"
        [[ "${VERSION_CODEID}" == "debian" ]] && software_install "firefox-esr"
        ;;
    remove)
        [[ "${VERSION_CODEID}" == "ubuntu" ]] && software_remove "firefox"
        [[ "${VERSION_CODEID}" == "debian" ]] && software_remove "firefox-esr"
        ;;
    update)
        software_update
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For plex-media-server
software_303() {
    echo -e "${STEPS} Start executing the command..."
    echo -e "${INFO} Software Name: [ plex-media-server ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        # Install basic dependencies
        echo -e "${STEPS} Start installing basic dependencies..."
        software_install "wget curl gpg gnupg2 software-properties-common apt-transport-https lsb-release ca-certificates"

        # Add Plex Media Server APT repository
        echo -e "${STEPS} Start adding the Plex Media Server APT repository..."
        echo "deb https://downloads.plex.tv/repo/deb public main" | sudo tee /etc/apt/sources.list.d/plexmediaserver.list

        # Import GPG key
        echo -e "${STEPS} Start importing GPG keys..."
        wget https://downloads.plex.tv/plex-keys/PlexSign.key
        cat PlexSign.key | sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/PlexSigkey.gpg
        rm -f PlexSign.key

        # Installing Plex Media server
        echo -e "${STEPS} Start installing Plex Media server..."
        software_install "plexmediaserver"

        # Ensure to open the port 32400 through the firewall
        echo -e "${STEPS} Set firewall to open port 32400..."
        sudo ufw allow 32400 2>/dev/null

        # Enable Plex server to start automatically on system boot
        echo -e "${STEPS} Start setting up the Plex server to start automatically at system boot..."
        sudo systemctl daemon-reload
        sudo systemctl start plexmediaserver.service
        sudo systemctl enable plexmediaserver.service

        # Check Plex Media Server Status
        echo -e "${STEPS} Check Plex Media Server Status..."
        systemctl status plexmediaserver.service

        # Confirm the service is enabled
        echo -e "${STEPS} Confirm the service is enabled..."
        systemctl is-enabled plexmediaserver.service

        # Configure Plex Media Server: http://<plex-media-server-ip>:32400/web
        sync && sleep 3
        echo -e "${NOTE} Plex Media Server address [ http://ip:32400/web ]"
        echo -e "${SUCCESS} Plex Media Server installation is successful."
        ;;
    remove)
        software_remove "plexmediaserver"
        ;;
    update)
        software_update
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
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
    software_${software_id} ${software_manage}
}

# Check script permission, supports running on Armbian system.
echo -e "${STEPS} Welcome to the software service command center: [ ${0} ]"
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo ${0} ]"
#
# Initialize variables
init_var "${@}"

exit 0
