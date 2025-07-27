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
# Dependent script: /etc/rc.local (which runs with 'set -e')
# File path: /etc/custom_service/start_service.sh
#
# Version: v1.1
#
#========================================================================================

# Custom Service Log - all script output will be logged here.
custom_log="/tmp/ophub_start_service.log"

# A helper function for logging with a timestamp.
log_message() {
    echo "[$(date +"%Y.%m.%d.%H:%M:%S")] $1" >>"${custom_log}"
}

# Start of the script.
log_message "Start the custom service..."

# System Identification
# Set the release check file to identify the device type.
ophub_release_file="/etc/ophub-release"
FDT_FILE="" # Initialize FDT_FILE to be empty.

[[ -f "${ophub_release_file}" ]] && { FDT_FILE="$(grep -oE 'meson.*dtb' "${ophub_release_file}")"; }
[[ -z "${FDT_FILE}" && -f "/boot/uEnv.txt" ]] && { FDT_FILE="$(grep -E '^FDT=.*\.dtb$' /boot/uEnv.txt | sed -E 's#.*/##')"; }
[[ -z "${FDT_FILE}" && -f "/boot/armbianEnv.txt" ]] && { FDT_FILE="$(grep -E '^fdtfile=.*\.dtb$' /boot/armbianEnv.txt | sed -E 's#.*/##')"; }
log_message "Detected FDT file: ${FDT_FILE:-'not found'}"

# Device-Specific Services

# For Tencent Aurora 3Pro (s905x3-b) box: Load Bluetooth module
if [[ "${FDT_FILE}" == "meson-sm1-skyworth-lb2004-a4091.dtb" ]]; then
    modprobe btmtksdio 2>/dev/null || true
    log_message "Attempted to load btmtksdio module for Tencent-Aurora-3Pro."
fi

# For swan1-w28(rk3568) board: USB power and switch control
if [[ "${FDT_FILE}" == "rk3568-swan1-w28.dtb" ]]; then
    # GPIO operations are critical, but we also add error suppression.
    gpioset 0 21=1 2>/dev/null
    gpioset 3 20=1 2>/dev/null
    gpioset 4 21=1 2>/dev/null
    gpioset 4 22=1 2>/dev/null
    log_message "USB power control GPIOs set for Swan1-w28."
fi

# For smart-am60(rk3588)/orangepi-5b(rk3588s) board: Bluetooth control
if [[ "${FDT_FILE}" =~ ^(rk3588-smart-am60\.dtb|rk3588s-orangepi-5b\.dtb)$ ]]; then
    # This is a sequence of commands, with the last one running in the background.
    # The background command (&) won't affect the script's exit code.
    rfkill block all
    chmod a+x /lib/firmware/ap6276p/brcm_patchram_plus1 2>/dev/null || true
    sleep 2
    rfkill unblock all
    /lib/firmware/ap6276p/brcm_patchram_plus1 --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/ap6275p/BCM4362A2.hcd /dev/ttyS9 &
    log_message "Bluetooth firmware download process started for Smart-am60/orangepi-5b."
fi

# General System Services

# Restart ssh service
mkdir -p -m0755 /var/run/sshd 2>/dev/null
if [[ -f "/etc/init.d/ssh" ]]; then
    (sleep 5 && /etc/init.d/ssh restart 2>/dev/null) || true
    log_message "SSH service restart attempted."
fi

# Add network performance optimization
if [[ -x "/usr/sbin/balethirq.pl" ]]; then
    (perl /usr/sbin/balethirq.pl 2>/dev/null) || true
    log_message "Network optimization service (balethirq.pl) execution attempted."
fi

# Led display control
openvfd_enable="no"
openvfd_boxid="15"
if [[ "${openvfd_enable}" == "yes" && -n "${openvfd_boxid}" && -x "/usr/sbin/armbian-openvfd" ]]; then
    (armbian-openvfd "${openvfd_boxid}") || true
    log_message "OpenVFD service execution attempted."
fi

# For vplus(Allwinner h6) led color lights
if [[ -x "/usr/bin/rgb-vplus" ]]; then
    rgb-vplus --RedName=RED --GreenName=GREEN --BlueName=BLUE 2>/dev/null &
    log_message "Vplus RGB LED service started in background."
fi

# For fan control service
if [[ -x "/usr/bin/pwm-fan.pl" ]]; then
    perl /usr/bin/pwm-fan.pl 2>/dev/null &
    log_message "Fan control service (pwm-fan.pl) started in background."
fi

# For oes(A311d) SATA LED status monitoring
if [[ -x "/usr/bin/oes_sata_leds.sh" ]]; then
    /usr/bin/oes_sata_leds.sh >/var/log/oes-sata-leds.log 2>&1 &
    log_message "SATA status check service (oes_sata_leds.sh) started in background."
fi

# For pveproxy startup service
if [[ -n "$(dpkg -l | awk '{print $2}' | grep -w "^pve-manager$")" ]]; then
    # Restarting systemd services can sometimes fail during early boot.
    # Using '|| true' makes this step fault-tolerant.
    (sudo systemctl restart pveproxy) || true
    log_message "PVE proxy service restart attempted."
fi

# Finalization
log_message "All custom services have been processed."
