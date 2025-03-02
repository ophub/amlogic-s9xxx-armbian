#!/bin/bash
#========================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Function: Customize the startup script, adding content as needed.
# Dependent script: /etc/rc.local
# File path: /etc/custom_service/start_service.sh
#
#========================================================================================

# Custom Service Log
custom_log="/tmp/ophub_start_service.log"

# Add custom log
echo "[$(date +"%Y.%m.%d.%H:%M:%S")] Start the custom service..." >${custom_log}

# Set the release check file
ophub_release_file="/etc/ophub-release"
[[ -f "${ophub_release_file}" ]] && FDT_FILE="$(cat ${ophub_release_file} | grep -oE 'meson.*dtb')" || FDT_FILE=""
[[ -z "${FDT_FILE}" && -f "/boot/uEnv.txt" ]] && FDT_FILE="$(grep -E '^FDT=.*\.dtb$' /boot/uEnv.txt | sed -E 's#.*/##')" || FDT_FILE=""
[[ -z "${FDT_FILE}" && -f "/boot/armbianEnv.txt" ]] && FDT_FILE="$(grep -E '^fdtfile=.*\.dtb$' /boot/armbianEnv.txt | sed -E 's#.*/##')" || FDT_FILE=""

# For Tencent Aurora 3Pro (s905x3-b) box [ /etc/modprobe.d/blacklist.conf : blacklist btmtksdio ]
[[ "${FDT_FILE}" == "meson-sm1-skyworth-lb2004-a4091.dtb" ]] && {
    modprobe btmtksdio 2>/dev/null &&
        echo "[$(date +"%Y.%m.%d.%H:%M:%S")] The Tencent-Aurora-3Pro's btmtksdio module loaded successfully." >>${custom_log}
}

# For swan1-w28(rk3568) board USB power and switch contrl
[[ "${FDT_FILE}" == "rk3568-swan1-w28.dtb" ]] && {
    # USB 5V Power buick ON
    gpioset 0 21=1 2>/dev/null
    # USB3.0 Port ON
    gpioset 3 20=1 2>/dev/null
    # USB2.0 Port ON
    gpioset 4 21=1 2>/dev/null
    gpioset 4 22=1 2>/dev/null
    echo "[$(date +"%Y.%m.%d.%H:%M:%S")] USB successfully enabled on Swan1-w28(rk3568)." >>${custom_log}
}

# For smart-am60(rk3588) board Bluetooth contrl
[[ "${FDT_FILE}" == "rk3588-smart-am60.dtb" ]] && {
    rfkill block all
    chmod a+x /lib/firmware/ap6276p/brcm_patchram_plus1
    sleep .5
    rfkill unblock all
    /lib/firmware/ap6276p/brcm_patchram_plus1 --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/ap6276p/ /dev/ttyS9 &

    echo "[$(date +"%Y.%m.%d.%H:%M:%S")] Bluetooth firmware successfully download on Smart-am60(rk3588)." >>${custom_log}
}

# Restart ssh service
[[ -d "/var/run/sshd" ]] || mkdir -p -m0755 /var/run/sshd 2>/dev/null
[[ -f "/etc/init.d/ssh" ]] && {
    sleep 5 && /etc/init.d/ssh restart 2>/dev/null &&
        echo "[$(date +"%Y.%m.%d.%H:%M:%S")] The ssh service restarted successfully." >>${custom_log}
}

# Add network performance optimization
[[ -x "/usr/sbin/balethirq.pl" ]] && {
    perl /usr/sbin/balethirq.pl 2>/dev/null &&
        echo "[$(date +"%Y.%m.%d.%H:%M:%S")] The network optimization service started successfully." >>${custom_log}
}

# Led display control
openvfd_enable="no"
openvfd_boxid="15"
[[ "${openvfd_enable}" == "yes" && -n "${openvfd_boxid}" && -x "/usr/sbin/armbian-openvfd" ]] && {
    armbian-openvfd ${openvfd_boxid} &&
        echo "[$(date +"%Y.%m.%d.%H:%M:%S")] The openvfd service started successfully." >>${custom_log}
}

# For vplus(Allwinner h6) led color lights
[[ -x "/usr/bin/rgb-vplus" ]] && {
    rgb-vplus --RedName=RED --GreenName=GREEN --BlueName=BLUE 2>/dev/null &
    echo "[$(date +"%Y.%m.%d.%H:%M:%S")] The LED of Vplus is enabled successfully." >>${custom_log}
}

# For fan control service
[[ -x "/usr/bin/pwm-fan.pl" ]] && {
    perl /usr/bin/pwm-fan.pl 2>/dev/null &
    echo "[$(date +"%Y.%m.%d.%H:%M:%S")] The fan control service enabled successfully." >>${custom_log}
}

# For pveproxy startup service
[[ -n "$(dpkg -l | awk '{print $2}' | grep -w "^pve-manager$")" ]] && {
    sudo systemctl restart pveproxy &&
        echo "[$(date +"%Y.%m.%d.%H:%M:%S")] The pveproxy service started successfully." >>${custom_log}
}

# Add custom log
echo "[$(date +"%Y.%m.%d.%H:%M:%S")] All custom services executed successfully!" >>${custom_log}
