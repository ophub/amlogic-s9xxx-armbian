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
# Function: Execute docker software install/update/remove command
# Copyright (C) 2021- https://github.com/unifreq/openwrt_packit
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: command-docker.sh -s <software_id> -m <install/update/remove>
# Example: command-docker.sh -s 101 -m install
#          command-docker.sh -s 101 -m update
#          command-docker.sh -s 101 -m remove
#
#============================== Software list ===============================
#
# software_101  : For docker
# software_102  : For portainer:8000/9443
# software_103  : For yacht:8001
# software_104  : For transmission:9091/51413
# software_105  : For qbittorrent:8080/6881
# software_106  : For nextcloud:8088
# software_107  : For jellyfin:8096/8920/7359/1900
# software_108  : For homeassistant:8123
# software_109  : For kodbox:8081
# software_110  : For couchpotato:5050
# software_111  : For sonarr:8989
# software_112  : For radarr:7878
# software_113  : For syncthing:8384
# software_114  : For filebrowser:8002
# software_115  : For heimdall:8003/8004
# software_116  : For node-red:1880
# software_117  : For mosquitto:1883/9001
# software_118  : For openwrt:80
# software_119  : For netdata:19999
# software_120  : For xunlei:2345
# software_121  : For docker-headless:10081/10089
# software_122  : For navidrome:4533
# software_123  : For alist:5244
# software_124  : For qinglong:5700
# software_125  : For chatgpt-next-web:3000
#
#============================================================================

# Execute generic functions
software_path="/usr/share/ophub/armbian-software"
software_command="${software_path}/software-command.sh"
source "${software_command}"

# For docker
software_101() {
    case "${software_manage}" in
    install) armbian-docker install ;;
    update) armbian-docker update ;;
    remove) armbian-docker remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For portainer
software_102() {
    # Set basic information
    container_name="portainer"
    image_name="portainer/portainer-ce:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."

        # Prompt users whether to use their own SSL certificate
        echo -ne "${OPTIONS} Using your own SSL certificate? (y/N): "
        read use_ssl
        [[ "${use_ssl,,}" =~ ^[y] ]] && use_ssl="yes" || use_ssl="no"
        echo -e "${INFO} Your own SSL Certificate Selection: [ ${use_ssl} ]"

        # Instructions(English): https://hub.docker.com/r/portainer/portainer-ce
        docker volume create ${container_name}_data
        if [[ "${use_ssl}" == "yes" ]]; then
            # https://docs.portainer.io/advanced/ssl
            docker run -d --name ${container_name} \
                --restart always \
                -p 8000:8000 \
                -p 9443:9443 \
                -v /var/run/docker.sock:/var/run/docker.sock \
                -v ${install_path}/portainer_data:/data \
                -v ${install_path}/certs:/certs \
                ${image_name} \
                --sslcert /certs/portainer.crt \
                --sslkey /certs/portainer.key
        else
            docker run -d --name ${container_name} \
                --restart always \
                -p 8000:8000 \
                -p 9443:9443 \
                -v /var/run/docker.sock:/var/run/docker.sock \
                -v ${install_path}/portainer_data:/data \
                ${image_name}
        fi

        sync && sleep 3
        [[ "${use_ssl}" == "yes" ]] && {
            echo -e "${NOTE} Please place your SSL certificate in: [ ${install_path}/certs/portainer.crt & portainer.key]"
        }
        echo -e "${NOTE} The ${container_name} address: [ https://${my_address}:9443 ]"
        echo -e "${SUCCESS} The ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For yacht
software_103() {
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
            -p 8001:8000 \
            -v /var/run/docker.sock:/var/run/docker.sock \
            -v ${install_path}/config:/config \
            --restart unless-stopped \
            ${image_name}

        sudo ufw allow 8001/tcp 2>/dev/null

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address: [ http://${my_address}:8001 ]"
        echo -e "${NOTE} The ${container_name} account: [ username:admin@yacht.local  /  password:pass ]"
        echo -e "${NOTE} The ${container_name} website: [ https://yacht.sh ]"
        echo -e "${NOTE} The ${container_name} template: [ https://raw.githubusercontent.com/SelfhostedPro/selfhosted_templates/yacht/Template/template.json ]"
        echo -e "${SUCCESS} The ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For transmission
software_104() {
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
        #echo -e "${STEPS} Start the installation interface: [ transmission-web-control ]..."
        #tr_cn_url="https://github.com/ronggang/transmission-web-control/raw/master/release/install-tr-control-cn.sh"
        #bash <(curl -fsSL ${tr_cn_url}) ${install_path}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address: [ http://${my_address}:9091 ]"
        echo -e "${NOTE} The ${container_name} account: [ username:${tr_user}  /  password:${tr_pass} ]"
        echo -e "${SUCCESS} The ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For qbittorrent
software_105() {
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
        echo -e "${NOTE} The ${container_name} address: [ http://${my_address}:8080 ]"
        echo -e "${NOTE} View ${container_name} username and password: [ docker logs -f qbittorrent ]"
        echo -e "${SUCCESS} The ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For nextcloud
software_106() {
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
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8088 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For jellyfin
software_107() {
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
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8096  /  https://${my_address}:8920 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For homeassistant
software_108() {
    # Set basic information
    container_name="homeassistant"
    image_name="homeassistant/home-assistant:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/homeassistant/home-assistant
        docker run -d --name=${container_name} \
            --privileged \
            --network=host \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8123:8123 \
            -v ${install_path}/config:/config \
            -v ${install_path}/media:/media \
            -v /run/dbus:/run/dbus:ro \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} app [ Home Assistant ]"
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8123 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For kodbox
software_109() {
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
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8081 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For couchpotato
software_110() {
    # Set basic information
    container_name="couchpotato"
    image_name="linuxserver/couchpotato:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/couchpotato
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 5050:5050 \
            -v ${install_path}/appdata/config:/config \
            -v ${download_path}:/downloads \
            -v ${movie_path}:/movies \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:5050 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For sonarr
software_111() {
    # Set basic information
    container_name="sonarr"
    image_name="linuxserver/sonarr:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/sonarr
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8989:8989 \
            -v ${install_path}/data:/config \
            -v ${tv_path}:/tv \
            -v ${download_path}:/downloads \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8989 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For radarr
software_112() {
    # Set basic information
    container_name="radarr"
    image_name="linuxserver/radarr:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/radarr
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 7878:7878 \
            -v ${install_path}/data:/config \
            -v ${movie_path}:/movies \
            -v ${download_path}:/downloads \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:7878 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For syncthing
software_113() {
    # Set basic information
    container_name="syncthing"
    image_name="linuxserver/syncthing:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/syncthing
        docker run -d --name=${container_name} \
            --hostname=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8384:8384 \
            -p 22000:22000/tcp \
            -p 22000:22000/udp \
            -p 21027:21027/udp \
            -v ${install_path}/appdata/config:/config \
            -v ${install_path}/data1:/data1 \
            -v ${install_path}/data2:/data2 \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8384 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For filebrowser
software_114() {
    # Set basic information
    container_name="filebrowser"
    image_name="filebrowser/filebrowser:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/filebrowser/filebrowser
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8002:80 \
            -v ${install_path}/root:/srv \
            -v ${install_path}/filebrowser.db:/database/filebrowser.db \
            -v ${install_path}/settings.json:/config/settings.json \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8002 ]"
        echo -e "${NOTE} The ${container_name} account: [ username:admin  /  password:admin ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For heimdall
software_115() {
    # Set basic information
    container_name="heimdall"
    image_name="linuxserver/heimdall:arm64v8-latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/linuxserver/heimdall
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 8003:80 \
            -p 8004:443 \
            -v ${install_path}/config:/config \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:8003  /  https://${my_address}:8004 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For node-red
software_116() {
    # Set basic information
    container_name="node-red"
    image_name="nodered/node-red:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://nodered.org/docs/getting-started/docker
        docker run -itd --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 1880:1880 \
            -v node_red_data:/data \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:1880 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For mosquitto
software_117() {
    # Set basic information
    container_name="mosquitto"
    image_name="arm64v8/eclipse-mosquitto:latest"
    install_path="${docker_path}/${container_name}"

    # Create a local persistent directory
    mkdir -p ${install_path}/{config/,data/,log/}
    # Initialize the configuration file
    mosquitto_conf="${install_path}/config/mosquitto.conf"
    sudo cat >${mosquitto_conf} <<EOF
persistence true
persistence_location ${install_path}/data
log_dest file ${install_path}/log/mosquitto.log
EOF

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/arm64v8/eclipse-mosquitto/
        docker run -itd --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 1883:1883 \
            -p 9001:9001 \
            -v ${install_path}/config/mosquitto.conf:/mosquitto/config/mosquitto.conf \
            -v ${install_path}/data \
            -v ${install_path}/log \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name}  address [ http://${my_address}:1883  /  http://${my_address}:9001 ]"
        echo -e "${NOTE} The ${container_name} tutorial [ https://www.mosquitto.org/ ]"
        echo -e "${NOTE} The ${container_name}  MQTT.fx [ https://softblade.de/en/download-2/ ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For openwrt
software_118() {
    # Set basic information
    container_name="openwrt"
    image_name="ophub/openwrt-aarch64:latest"
    install_path="${docker_path}/${container_name}"

    echo -ne "${OPTIONS} Please input the docker image, the default is [ ${image_name} ]: "
    read docker_img
    [[ -n "${docker_img}" ]] && image_name="${docker_img}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."

        # Create a virtual network: macvlan
        [[ -z "${my_macvlan}" ]] && {
            echo -ne "${OPTIONS} Add macvlan network, please input gateway, the default is [ ${my_gateway} ]: "
            read gw
            [[ -n "${gw}" ]] && my_gateway="${gw}"

            my_subnet="${my_gateway%.*}.0"
            if [[ -n "$(ifconfig | grep -oE '^br0:')" ]]; then
                parent_lan="br0"
            elif [[ -n "$(ifconfig | grep -oE '^vmbr0:')" ]]; then
                parent_lan="vmbr0"
            else
                parent_lan="eth0"
            fi

            docker network create -d macvlan --subnet=${my_subnet}/24 --gateway=${my_gateway} -o parent=${parent_lan} macnet
        }

        # Instructions: https://hub.docker.com/r/ophub/openwrt-aarch64
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            --network macnet \
            --privileged \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} enter the OpenWrt system [ docker exec -it openwrt bash ]"
        echo -e "${NOTE} The ${container_name} instructions [ https://hub.docker.com/r/ophub/openwrt-aarch64 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For netdata
software_119() {
    # Set basic information
    container_name="netdata"
    image_name="netdata/netdata:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/netdata/netdata
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 19999:19999 \
            -v ${install_path}/netdataconfig:/etc/netdata \
            -v netdatalib:/var/lib/netdata \
            -v netdatacache:/var/cache/netdata \
            -v /etc/passwd:/host/etc/passwd:ro \
            -v /etc/group:/host/etc/group:ro \
            -v /proc:/host/proc:ro \
            -v /sys:/host/sys:ro \
            -v /etc/os-release:/host/etc/os-release:ro \
            --restart unless-stopped \
            --cap-add SYS_PTRACE \
            --security-opt apparmor=unconfined \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:19999 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For xunlei
software_120() {
    # Set basic information
    container_name="xunlei"
    image_name="cnk3x/xunlei:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/cnk3x/xunlei
        docker run -d --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            --hostname=mynas \
            --net=bridge \
            -p 2345:2345 \
            -v ${install_path}/data:/xunlei/data \
            -v ${install_path}/downloads:/xunlei/downloads \
            --restart unless-stopped \
            --privileged \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:2345 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For docker-headless
software_121() {
    # Set basic information
    container_name="docker-headless"
    image_name="infrastlabs/docker-headless:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/infrastlabs/docker-headless
        docker run -itd --name=${container_name} \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 10081:10081 \
            -p 10089:10089 \
            --shm-size 1g \
            --tmpfs /run \
            --tmpfs /run/lock \
            --tmpfs /tmp \
            --cap-add SYS_BOOT \
            --cap-add SYS_ADMIN \
            -v /sys/fs/cgroup:/sys/fs/cgroup \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} Usage [ https://github.com/infrastlabs/docker-headless ]"
        echo -e "${NOTE} The ${container_name} noVnc [ http://${my_address}:10081 ], PASS [ headless ], ReadOnly [ View123 ]"
        echo -e "${NOTE} The ${container_name} RDP [ ${my_address}:10089 ]"
        echo -e "${NOTE} The ${container_name} SSH [ ssh -p 10022 headless@${my_address} ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For navidrome
software_122() {
    # Set basic information
    container_name="navidrome"
    image_name="deluan/navidrome:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/deluan/navidrome
        docker run -d --name=${container_name} \
            --user $(id -u):$(id -g) \
            -e ND_LOGLEVEL=info \
            -p 4533:4533 \
            -v ${install_path}/music:/music \
            -v ${install_path}/data:/data \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:4533 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For alist
software_123() {
    # Set basic information
    container_name="alist"
    image_name="xhofe/alist:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."

        echo -ne "${OPTIONS} Set your login password: "
        read alist_pass
        [[ -z "${alist_pass}" ]] && alist_pass="$(docker exec -it alist ./alist admin random | grep 'password:' | awk '{print $4}')"

        # Instructions: https://hub.docker.com/r/xhofe/alist
        docker run -d --name=${container_name} \
            -e PUID=0 \
            -e PGID=0 \
            -e UMASK=022 \
            -p 5244:5244 \
            -v ${install_path}/alist:/opt/alist/data \
            --restart=always \
            ${image_name}

        docker exec -it alist ./alist admin set ${alist_pass}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:5244 ]"
        echo -e "${NOTE} Login name: [ admin ], password [ ${alist_pass} ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For qinglong
software_124() {
    # Set basic information
    container_name="qinglong"
    image_name="whyour/qinglong:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."
        # Instructions: https://hub.docker.com/r/whyour/qinglong
        docker run -dit --name=${container_name} \
            -v ${install_path}/ql:/ql/data \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -p 5700:5700 \
            --hostname=qinglong \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:5700 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For chatgpt-next-web
software_125() {
    # Set basic information
    container_name="chatgpt-next-web"
    image_name="yidadaa/chatgpt-next-web:latest"
    install_path="${docker_path}/${container_name}"

    case "${software_manage}" in
    install)
        echo -e "${STEPS} Start installing the docker image: [ ${container_name} ]..."

        # Set your OPENAI_API_KEY
        echo -ne "${OPTIONS} Please input your OPENAI_API_KEY, such as sk-xxxxx: "
        read oak
        [[ -n "${oak}" ]] && your_api_key="${oak}" || error_msg "OPENAI_API_KEY is invalid."

        # Set your login password
        echo -ne "${OPTIONS} Please input your login password: "
        read pw
        [[ -n "${pw}" ]] && your_password="${pw}" || error_msg "PassWord is invalid."

        # Instructions: https://hub.docker.com/r/yidadaa/chatgpt-next-web
        docker run -d --name=${container_name} \
            -p 3000:3000 \
            -e PUID=${docker_puid} \
            -e PGID=${docker_pgid} \
            -e TZ=${docker_tz} \
            -e OPENAI_API_KEY=${your_api_key} \
            -e CODE=${your_password} \
            -e HIDE_USER_API_KEY=1 \
            --restart unless-stopped \
            ${image_name}

        sync && sleep 3
        echo -e "${NOTE} The ${container_name} address [ http://${my_address}:3000 ]"
        echo -e "${SUCCESS} ${container_name} installed successfully."
        exit 0
        ;;
    update) docker_update ;;
    remove) docker_remove ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# Initialize variables
init_var "${@}"
software_${software_id} ${software_manage}
