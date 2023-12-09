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
# Function: Execute software install/update/uninstall script
# Copyright (C) 2021- https://github.com/unifreq/openwrt_packit
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: ${0} <install/update/remove>
# Example: ${0} install
#          ${0} update
#          ${0} remove
#
#============================== Functions list ==============================
#
# error_msg           : Output error message
# software_download   : Software download
# software_install    : Software install
# software_update     : Software update
# software_remove     : Software remove
#
#========================== Set default parameters ==========================
#
# Define the software
software_name="frps"
software_core="/usr/bin/frps"
software_conf="/etc/frp/frps.ini"
software_service="/etc/systemd/system/frps.service"
#
# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
PROMPT="[\033[93m PROMPT \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#============================================================================

# Show error message
error_msg() {
    echo -e "${ERROR} ${1}"
    exit 1
}

software_download() {
    echo -e "${STEPS} Start downloading [ ${software_name} ] software..."

    # Software version query api
    software_api="https://api.github.com/repos/fatedier/frp/releases"
    # Check the latest version, E.g: v0.43.0
    software_latest_version="$(curl -s "${software_api}" | grep "tag_name" | awk -F '"' '{print $4}' | tr " " "\n" | sort -rV | head -n 1)"
    # Query download address, E.g: https://github.com/fatedier/frp/releases/download/v0.43.0/frp_0.43.0_linux_arm64.tar.gz
    software_url="$(curl -s "${software_api}" | grep -oE "https:.*${software_latest_version}.*linux_arm64.*.tar.gz")"
    [[ -n "${software_url}" ]] || error_msg "The download address is empty!"
    echo -e "${INFO} Software download from: [ ${software_url} ]"

    # Download software, E.g: /tmp/tmp.xxx/frp_0.43.0_linux_arm64.tar.gz
    tmp_download="$(mktemp -d)"
    software_filename="${software_url##*/}"
    curl -fsSL "${software_url}" -o "${tmp_download}/${software_filename}"
    [[ "${?}" -eq "0" && -s "${tmp_download}/${software_filename}" ]] || error_msg "Software download failed!"
    echo -e "${INFO} Software downloaded successfully: $(ls ${tmp_download} -l)"

    # Unzip the download file
    tar -xf ${tmp_download}/${software_filename} -C ${tmp_download} && sync
    # software directory, E.g: /tmp/tmp.xxx/frp_0.43.0_linux_arm64
    software_dir="${tmp_download}/${software_filename/.tar.gz/}"
    echo -e "${INFO} Software directory: [ ${software_dir} ]"
}

software_install() {
    echo -e "${STEPS} Start installing [ ${software_name} ] software..."
    software_download

    # Generate random token
    random_token="$(cat /proc/sys/kernel/random/uuid)"
    # Generate random account
    random_account="$(cat /proc/sys/kernel/random/uuid)"
    random_user="${random_account:0:18}"
    random_pwd="${random_account:19}"

    # Add frps.ini
    echo -e "${INFO} Add frps.ini to: [ ${software_conf} ]"
    [[ -d "/etc/frp" ]] || sudo mkdir -p "/etc/frp"
    # Copy the complete setup example
    sudo cp -f ${software_dir}/frps_full.ini /etc/frp
    # Generate common custom settings file
    sudo cat >${software_conf} <<EOF
[common]
bind_addr = 0.0.0.0
bind_port = 7000
bind_udp_port = 7001
kcp_bind_port = 7000
vhost_http_port = 80
vhost_https_port = 443
dashboard_addr = 0.0.0.0
dashboard_port = 7500
dashboard_user = ${random_user}
dashboard_pwd = ${random_pwd}
log_file = /tmp/frps.log
log_level = info
log_max_days = 1
token = ${random_token}
subdomain_host = frps.com
max_pool_count = 5
tcp_mux = true

EOF

    # Copy frps
    echo -e "${INFO} Copy frps to: [ ${software_core} ]"
    sudo cp -f ${software_dir}/frps /usr/bin && chmod +x ${software_core}

    # Add frps.service
    echo -e "${INFO} Add frps.service to: [ ${software_service} ]"
    sudo cat >${software_service} <<EOF
[Unit]
Description=frps
After=network.target

[Service]
Type=simple
User=nobody
Restart=on-failure
RestartSec=5s
ExecStart=${software_core} -c ${software_conf}
LimitNOFILE=1048576

[Install]
WantedBy=multi-user.target

EOF

    sync

    # Set up to start automatically
    echo -e "${INFO} Start service..."
    sudo systemctl daemon-reload
    sudo systemctl enable --now frps
    sudo systemctl restart frps

    echo -e "${PROMPT} Instructions for use:"
    cat <<EOF
============================================[ Instructions for use of frps ]============================================
Official website: https://github.com/fatedier/frp
Documentation: https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/build-armbian/documents/armbian_software.md

Core file path: [ ${software_core} ]
Config file path: [ ${software_conf} ]
Service file path: [ ${software_service} ]

Operate:
        1. Edit config file: [ vi ${software_conf} ]
        2. restart the service: [ sudo systemctl restart frps ]
        3. Check running status: [ sudo systemctl status frps ]

Service management commands:
        sudo systemctl start frps
        sudo systemctl stop frps
        sudo systemctl restart frps
        sudo systemctl enable frps
        sudo systemctl status frps
========================================================================================================================

EOF

    echo -e "${SUCCESS} Software installed successfully."
    exit 0
}

software_update() {
    echo -e "${STEPS} Start updating [ ${software_name} ] software..."
    software_download

    # Stop frps.service
    sudo systemctl stop frps
    sync && sleep 3

    # Copy frps
    echo -e "${INFO} Copy frps to: [ ${software_core} ]"
    sudo cp -f ${software_dir}/frps /usr/bin && chmod +x ${software_core}
    sync

    # Set up to start automatically
    sudo systemctl daemon-reload
    sudo systemctl enable --now frps
    sudo systemctl restart frps

    echo -e "${SUCCESS} Software update successfully."
    exit 0
}

software_remove() {
    echo -e "${STEPS} Start removing [ ${software_name} ] software..."

    # Stop frps.service
    sudo systemctl stop frps
    sync && sleep 3

    # Remove frps and frps.service
    sudo rm -rf /etc/frp
    sudo rm -f ${software_core}
    sudo rm -f ${software_service} 2>/dev/null
    sudo systemctl daemon-reload

    echo -e "${SUCCESS} Software removed successfully."
    exit 0
}

software_help() {
    echo -e "${STEPS} Script usage instructions."

    cat <<EOF
=====================================================================
 Function         : [ Command ]
---------------------------------------------------------------------
 Install Software : [ ${0} install ]
 Update  Software : [ ${0} update  ]
 Remove  Software : [ ${0} remove  ]
=====================================================================
EOF
    exit 0
}

# Check script permission, supports running on Armbian system.
echo -e "${STEPS} Welcome to [ ${software_name} ] software script: [ ${0} ]"
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo ${0} ]"
#
# Execute script assist functions
case "${1}" in
    install) software_install ;;
    update)  software_update ;;
    remove)  software_remove ;;
    *)       software_help ;;
esac
