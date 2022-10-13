#!/bin/bash
#========================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Armbian for Amlogic TV Boxes
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Function: Customize the startup script, adding content as needed
# Dependent script: /etc/rc.local
# File path: /etc/custom_service/start_service.sh
#
#========================================================================================

# Set the release check file
ophub_release_file="/etc/ophub-release"
[[ -f "${ophub_release_file}" ]] && FDTFILE="$(cat ${ophub_release_file} | grep -oE 'meson.*dtb')" || FDTFILE=""

# Add custom enabled alias extension load module.
# For Tencent Aurora 3Pro (s905x3-b) box [ /etc/modprobe.d/blacklist.conf : blacklist btmtksdio ]
[[ "${FDTFILE}" == "meson-sm1-skyworth-lb2004-a4091.dtb" ]] && modprobe btmtksdio 2>/dev/null

# Add custom log
echo "[$(date +"%Y.%m.%d.%H%M")] Hello World..." >/tmp/ophub_start_service.log
