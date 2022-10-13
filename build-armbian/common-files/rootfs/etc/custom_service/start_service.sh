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

# Add custom enabled alias extension load module.
# For Tencent Aurora 3Pro (s905x3-b) box [ /etc/modprobe.d/blacklist.conf : blacklist btmtksdio ]
modprobe btmtksdio 2>/dev/null

# Add custom log
echo "[$(date +"%Y.%m.%d.%H%M")] Hello World..." >/tmp/ophub_start_service.log
