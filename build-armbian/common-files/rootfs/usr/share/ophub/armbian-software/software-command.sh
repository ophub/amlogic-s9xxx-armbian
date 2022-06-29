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
# Function: Execute software install/update/uninstall command
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
# error_msg                 : Output error message
# check_release             : Check release file
# software_install          : Install package
# software_update           : Update package
# software_remove           : Remove package
# docker_container_remove   : Delete the docker container
# docker_image_remove       : Delete the docker image
# docker_update             : Update docker
# docker_remove             : Remove docker
#
# software_101              : For docker
# software_102              : For portainer:9000(docker)
# software_103              : For yacht:8000(docker)
# software_104              : For transmission:9091/51413(docker)
# software_105              : For qbittorrent:8080/6881(docker)
# software_106              : For nextcloud:8088(docker)
# software_107              : For jellyfin:8096/8920/7359/1900(docker)
# software_108              : For homeassistant:8123(docker)
# software_109              : For kodbox:8081(docker)
#
# software_201              : For desktop
# software_202              : For vlc-media-player(desktop)
# software_203              : For firefox(desktop)
#
# software_303              : For plex-media-server
# software_304              : For emby-server
# software_305              : For openmediavault(OMV-6.x)
#
# init_var                  : Initialize variables
#
#========================== Set default parameters ==========================
#
# Get custom firmware information
software_path="/usr/share/ophub/armbian-software"
software_command="${software_path}/software-command.sh"
ophub_release_file="/etc/ophub-release"
docker_path="/opt/docker"
download_path="/opt/downloads"
docker_puid="1000"
docker_pgid="1000"
docker_tz="Asia/Shanghai"
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

# Delete the docker container
docker_container_remove() {
    local container_name="${1}"
    [[ -n "${container_name}" ]] || error_msg "Docker container name is empty!"

    # Query the container ID based on the image name and delete it
    echo -e "${STEPS} Start removing ${container_name} container..."
    docker stop $(docker ps -aq --filter name=${container_name})
    docker rm $(docker ps -aq --filter name=${container_name})
    echo -e "${SUCCESS} ${container_name} removed successfully."
}

# Delete the docker image
docker_image_remove() {
    local image_name="${1}"
    [[ -n "${image_name}" ]] || error_msg "Docker image name is empty!"

    # Query the image ID based on the image name and delete it
    echo -e "${STEPS} Start removing ${image_name} image..."
    docker image rm $(docker images -q --filter reference=${image_name})
    echo -e "${SUCCESS} ${image_name} removed successfully."
}

# Update docker
docker_update() {
    local image_name="${1}"
    local container_name="${2}"
    [[ -n "${image_name}" && -n "${container_name}" ]] || error_msg "Docker image or container name is empty!"

    echo -e "${STEPS} Start updating the docker image..."
    # Update docker image
    docker pull "${image_name}"
    # Delete old container
    docker_container_remove "${container_name}"
    # Start a new one
    sudo ${software_command} -s ${software_id} -m install
}

# Remove docker
docker_remove() {
    local image_name="${1}"
    local container_name="${2}"
    local install_path="${3}"
    [[ -n "${image_name}" && -n "${container_name}" && -n "${install_path}" ]] || error_msg "Docker image, container or path is empty!"

    echo -e "${STEPS} Start removing docker ${container_name}..."
    # Delete old container
    docker_container_remove "${container_name}"
    # Delete old image
    docker_image_remove "${image_name}"
    # Delete the installation directory
    [[ -d "${install_path}" ]] && rm -rf ${install_path}
    exit 0
}

# For docker
software_101() {
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

# For yacht
software_103() {
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Set basic information
    container_name="yacht"
    image_name="selfhostedpro/yacht:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/selfhostedpro/yacht
        docker volume create ${container_name}
        docker run -d --name ${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8000:8000 \
            -v /var/run/docker.sock:/var/run/docker.sock \
            -v ${install_path}/config:/config \
            --restart unless-stopped \
            ${image_name}

        sudo ufw allow 8000/tcp 2>/dev/null

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address: [ http://ip:8000 ]"
        echo -e "${NOTE} The ${container_name} account: [ username:admin@yacht.local  /  password:pass ]"
        echo -e "${NOTE} The ${container_name} website: [ https://yacht.sh ]"
        echo -e "${NOTE} The ${container_name} template: [ https://raw.githubusercontent.com/SelfhostedPro/selfhosted_templates/yacht/Template/template.json ]"
        echo -e "${SUCCESS} The ${container_name} installed successfully."
        exit 0
        ;;
    update)
        docker_update "${image_name}" "${container_name}"
        ;;
    remove)
        docker_remove "${image_name}" "${container_name}" "${install_path}"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For transmission
software_104() {
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Set basic information
    container_name="transmission"
    image_name="linuxserver/transmission:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    # Generate random account
    random_account="$(cat /proc/sys/kernel/random/uuid)"
    tr_default_user="admin"
    tr_default_pass="${random_account:0:18}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."

        echo -ne "${OPTIONS} Set login username, the default is [ ${tr_default_user} ]: "
        read tr_user
        [[ -z "${tr_user}" ]] && tr_user="${tr_default_user}"
        echo -e "${INFO} Login username: [ ${tr_user} ]"

        echo -ne "${OPTIONS} Set login password, the default is [ ${tr_default_pass} ]: "
        read tr_pass
        [[ -z "${tr_pass}" ]] && tr_pass="${tr_default_pass}"
        echo -e "${INFO} Login password: [ ${tr_pass} ]"

        # Instructions: https://github.com/linuxserver/docker-transmission
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -e TRANSMISSION_WEB_HOME=/transmission-web-control/ \
            -e USER=${tr_user} \
            -e PASS=${tr_pass} \
            -p 9091:9091 \
            -p 51413:51413 \
            -p 51413:51413/udp \
            -v ${install_path}/config:/config \
            -v ${install_path}/watch/folder:/watch \
            -v ${download_path}:/downloads \
            --restart unless-stopped \
            ${image_name}

        # Set the transmission-web-control
        echo -e "${STEPS} Start the installation interface: [ transmission-web-control ]..."
        tr_cn_url="https://github.com/ronggang/transmission-web-control/raw/master/release/install-tr-control-cn.sh"
        bash <(curl -fsSL ${tr_cn_url}) ${install_path}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address: [ http://ip:9091 ]"
        echo -e "${NOTE} The ${container_name} account: [ username:${tr_user}  /  password:${tr_pass} ]"
        echo -e "${SUCCESS} The ${container_name} installed successfully."
        exit 0
        ;;
    update)
        docker_update "${image_name}" "${container_name}"
        ;;
    remove)
        docker_remove "${image_name}" "${container_name}" "${install_path}"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For qbittorrent
software_105() {
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Set basic information
    container_name="qbittorrent"
    image_name="linuxserver/qbittorrent:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/qbittorrent
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -e WEBUI_PORT=8080 \
            -p 8080:8080 \
            -p 6881:6881 \
            -p 6881:6881/udp \
            -v ${install_path}/appdata/config:/config \
            -v ${download_path}:/downloads \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address: [ http://ip:8080 ]"
        echo -e "${NOTE} The ${container_name} account: [ username:admin  /  password:adminadmin ]"
        echo -e "${SUCCESS} The ${container_name} installed successfully."
        exit 0
        ;;
    update)
        docker_update "${image_name}" "${container_name}"
        ;;
    remove)
        docker_remove "${image_name}" "${container_name}" "${install_path}"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For nextcloud
software_106() {
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Set basic information
    container_name="nextcloud"
    image_name="arm64v8/nextcloud:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/arm64v8/nextcloud
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8088:80 \
            -v ${install_path}/nextcloud:/var/www/html \
            -v ${install_path}/apps:/var/www/html/custom_apps \
            -v ${install_path}/config:/var/www/html/config \
            -v ${install_path}/data:/var/www/html/data \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://ip:8088 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update)
        docker_update "${image_name}" "${container_name}"
        ;;
    remove)
        docker_remove "${image_name}" "${container_name}" "${install_path}"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For jellyfin
software_107() {
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Set basic information
    container_name="jellyfin"
    image_name="linuxserver/jellyfin:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/jellyfin
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8096:8096 \
            -p 8920:8920 \
            -p 7359:7359/udp \
            -p 1900:1900/udp \
            -v ${install_path}/library:/config \
            -v ${install_path}/tvseries:/data/tvshows \
            -v ${install_path}/movies:/data/movies \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://ip:8096 / https://ip:8920 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update)
        docker_update "${image_name}" "${container_name}"
        ;;
    remove)
        docker_remove "${image_name}" "${container_name}" "${install_path}"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For homeassistant
software_108() {
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Set basic information
    container_name="homeassistant"
    image_name="linuxserver/homeassistant:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/homeassistant
        docker run -d --name=${container_name} \
            --net=host \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8123:8123 \
            -v ${install_path}/data:/config \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} app [ Home Assistant ]"
        echo -e "${NOTE} The ${container_name} address [ http://ip:8123 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update)
        docker_update "${image_name}" "${container_name}"
        ;;
    remove)
        docker_remove "${image_name}" "${container_name}" "${install_path}"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For kodbox
software_109() {
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    # Set basic information
    container_name="kodbox"
    image_name="kodcloud/kodbox:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/kodcloud/kodbox
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8081:80 \
            -v ${install_path}/data:/var/www/html \
            -v ${install_path}/ssl:/etc/nginx/ssl \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://ip:8081 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update)
        docker_update "${image_name}" "${container_name}"
        ;;
    remove)
        docker_remove "${image_name}" "${container_name}" "${install_path}"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For desktop
software_201() {
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
    update)
        software_update
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
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For vlc-media-player
software_202() {
    echo -e "${INFO} Software Name: [ vlc-media-player ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        software_install "vlc"
        ;;
    update)
        software_update
        ;;
    remove)
        software_remove "vlc"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For firefox
software_203() {
    echo -e "${INFO} Software Name: [ firefox ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        [[ "${VERSION_CODEID}" == "ubuntu" ]] && software_install "firefox"
        [[ "${VERSION_CODEID}" == "debian" ]] && software_install "firefox-esr"
        ;;
    update)
        software_update
        ;;
    remove)
        [[ "${VERSION_CODEID}" == "ubuntu" ]] && software_remove "firefox"
        [[ "${VERSION_CODEID}" == "debian" ]] && software_remove "firefox-esr"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For plex-media-server
software_303() {
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

        # Confirm the service is enabled
        echo -e "${STEPS} Confirm the service is enabled..."
        systemctl is-enabled plexmediaserver.service

        # Configure Plex Media Server: http://ip:32400/web
        sync && sleep 3
        echo -e "${NOTE} The Plex Media Server address: [ http://ip:32400/web ]"
        echo -e "${SUCCESS} The Plex Media Server installation is successful."
        ;;
    update)
        software_update
        ;;
    remove)
        software_remove "plexmediaserver"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For emby-server
software_304() {
    echo -e "${INFO} Software Name: [ emby-server ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        # Software version query api
        software_api="https://api.github.com/repos/MediaBrowser/Emby.Releases/releases"
        # Check the latest version, E.g: 4.7.5.0
        software_latest_version="$(curl -s "${software_api}" | grep "tag_name" | awk -F '"' '{print $4}' | grep -E [.]0$ | tr " " "\n" | sort -rV | head -n 1)"
        # Query download address, E.g: https://github.com/MediaBrowser/Emby.Releases/releases/download/4.7.5.0/emby-server-deb_4.7.5.0_arm64.deb
        software_url="$(curl -s "${software_api}" | grep -oE "https:.*${software_latest_version}.*_arm64.deb")"
        [[ -n "${software_url}" ]] || error_msg "The download address is empty!"
        echo -e "${INFO} Software download from: [ ${software_url} ]"

        # Download software, E.g: /tmp/tmp.xxx/emby-server-deb_4.7.5.0_arm64.deb
        tmp_download="$(mktemp -d)"
        software_filename="${software_url##*/}"
        echo -e "${STEPS} Start downloading Emby Server..."
        wget -q -P ${tmp_download} ${software_url}
        [[ "${?}" -eq "0" && -s "${tmp_download}/${software_filename}" ]] || error_msg "Software download failed!"
        echo -e "${INFO} Software downloaded successfully: $(ls ${tmp_download} -l)"

        # Installing Emby Server
        echo -e "${STEPS} Start installing Emby Server..."
        sudo dpkg -i ${tmp_download}/${software_filename}

        # Enable Emby Server to start automatically on system boot
        echo -e "${STEPS} Start setting up the Emby Server to start automatically at system boot..."
        sudo systemctl daemon-reload
        sudo systemctl start emby-server.service
        sudo systemctl enable emby-server.service

        # Confirm the service is enabled
        echo -e "${STEPS} Confirm the service is enabled..."
        systemctl is-enabled emby-server.service

        # Configure Emby Server: http://ip:8096
        sync && sleep 3
        echo -e "${NOTE} The Emby Server address: [ http://ip:8096 ]"
        echo -e "${SUCCESS} The Emby Server installation is successful."
        ;;
    update)
        software_update
        ;;
    remove)
        software_remove "emby-server"
        ;;
    *)
        error_msg "Invalid input parameter: [ ${@} ]"
        ;;
    esac
}

# For openmediavault(OMV-6.x)
software_305() {
    echo -e "${INFO} Software Name: [ OpenMediaVault ]"
    echo -e "${INFO} Software ID: [ ${software_id} ]"
    echo -e "${INFO} Software Manage: [ ${software_manage} ]"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start checking the installation environment..."
        # Check script permission
        [[ "$(id -u)" == "0" ]] || error_msg "please run this script as root: [ sudo ${0} -s 104 -m install ]"
        # Check systemd running status
        systemd="$(ps --no-headers -o comm 1)"
        [[ "${systemd}" == "systemd" ]] || error_msg "This system is not running systemd."
        # Check the system operating environment
        [[ -z "$(dpkg -l | grep -wE 'gdm3|sddm|lxdm|xdm|lightdm|slim|wdm')" ]] || error_msg "OpenMediaVault does not support running in desktop environment!"

        # Download software, E.g: /tmp/tmp.xxx/install
        tmp_download="$(mktemp -d)"
        software_url="https://github.com/OpenMediaVault-Plugin-Developers/installScript/raw/master/install"
        software_filename="${software_url##*/}"
        echo -e "${STEPS} Start downloading the OpenMediaVault installation script..."
        wget -q -P ${tmp_download} ${software_url}
        [[ "${?}" -eq "0" && -s "${tmp_download}/${software_filename}" ]] || error_msg "Software download failed!"
        chmod +x ${tmp_download}/${software_filename}
        echo -e "${INFO} Software downloaded successfully: $(ls ${tmp_download} -l)"

        # Install OpenMediaVault and omv-extras extension: https://github.com/OpenMediaVault-Plugin-Developers/installScript
        echo -e "${STEPS} Start installing OpenMediaVault and omv-extras extension..."
        sudo ${tmp_download}/${software_filename} -n

        # Configure OpenMediaVault: http://ip
        sync && sleep 3
        echo -e "${NOTE} The OpenMediaVault address: [ http://ip ]"
        echo -e "${NOTE} The OpenMediaVault account: [ username:admin  /  password:openmediavault ]"
        echo -e "${NOTE} How to use OpenMediaVault: [ https://forum.openmediavault.org/ ]"
        echo -e "${SUCCESS} The OpenMediaVault installation is successful."
        ;;
    update)
        software_update
        ;;
    remove)
        software_remove "openmediavault"
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
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo ${0} ]"
#
# Initialize variables
init_var "${@}"

exit 0
