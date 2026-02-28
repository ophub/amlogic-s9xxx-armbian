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
# Version: v1.3
#
#========================================================================================

# Custom Service Log - all script output will be logged here
custom_log="/tmp/ophub_start_service.log"

# A helper function for logging with a timestamp
log_message() {
    echo "[$(date +"%Y.%m.%d.%H:%M:%S")] $1" >>"${custom_log}"
}

# Start of the script.
log_message "Start the custom service..."

# Disabled verbose kernel messages on console
dmesg -n 1 >/dev/null 2>&1 || true
log_message "Kernel console logging level set to 1 (Panic only)."

# System Identification
# Set the release check file to identify the device type
ophub_release_file="/etc/ophub-release"
[[ -f "${ophub_release_file}" ]] && source "${ophub_release_file}" 2>/dev/null || true
FDTFILE="${FDTFILE:-no_found.dtb}"
log_message "Detected FDT file: ${FDTFILE}"

# Device-Specific Services

# Add rknpu module to the system module load list
ophub_load_conf="/etc/modules-load.d/ophub-load-list.conf"
[[ -f "${ophub_load_conf}" ]] || touch "${ophub_load_conf}"
if modinfo rknpu >/dev/null 2>&1; then
    grep -q -x "rknpu" "${ophub_load_conf}" 2>/dev/null || echo "rknpu" >>"${ophub_load_conf}"
else
    grep -q -x "rknpu" "${ophub_load_conf}" 2>/dev/null && sed -i '/^rknpu$/d' "${ophub_load_conf}"
fi
log_message "Adjust the rknpu module in the system module load list"

# For Tencent Aurora 3Pro (s905x3-b) box: Load Bluetooth module
if [[ "${FDTFILE}" == "meson-sm1-skyworth-lb2004-a4091.dtb" ]]; then
    grep -q -x "btmtksdio" "${ophub_load_conf}" 2>/dev/null || echo "btmtksdio" >>"${ophub_load_conf}"
    log_message "Attempted to load btmtksdio module for Tencent-Aurora-3Pro."
fi

# For swan1-w28(rk3568) board: USB power and switch control
if [[ "${FDTFILE}" == "rk3568-swan1-w28.dtb" ]]; then
    (
        # GPIO operations are critical, but we also add error suppression.
        gpioset 0 21=1 >/dev/null 2>&1
        gpioset 3 20=1 >/dev/null 2>&1
        gpioset 4 21=1 >/dev/null 2>&1
        gpioset 4 22=1 >/dev/null 2>&1
    ) &
    log_message "USB power control GPIOs set for Swan1-w28."
fi

# For smart-am60(rk3588)/orangepi-5b(rk3588s) board: Bluetooth control
if [[ "${FDTFILE}" =~ ^(rk3588-smart-am60\.dtb|rk3588s-orangepi-5b\.dtb)$ ]]; then
    # This is a sequence of commands, with the last one running in the background.
    # The background command (&) won't affect the script's exit code.
    (
        rfkill block all
        chmod a+x /lib/firmware/ap6276p/brcm_patchram_plus1 >/dev/null 2>&1
        sleep 2
        rfkill unblock all
        /lib/firmware/ap6276p/brcm_patchram_plus1 --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/ap6275p/BCM4362A2.hcd /dev/ttyS9 &
    ) &
    log_message "Bluetooth firmware download process started for Smart-am60/orangepi-5b."
fi

# For nsy-g16-plus/nsy-g68-plus/bdy-g18-pro board
if [[ "${FDTFILE}" =~ ^(rk3568-nsy-g16-plus\.dtb|rk3568-nsy-g68-plus\.dtb|rk3568-bdy-g18-pro\.dtb)$ ]]; then
    (
        # Wait for network to be up
        sleep 10

        # Set MTU to 1500 for eth0 and br0
        set_mtu() {
            [[ -d "/sys/class/net/${1}" ]] && ip link set "${1}" mtu 1500 >/dev/null 2>&1
        }
        set_mtu eth0
        set_mtu br0

        # Close offloading features to improve stability
        if [[ -d "/sys/class/net/eth0" ]] && command -v ethtool >/dev/null 2>&1; then
            ethtool -K eth0 tso off gso off gro off tx off rx off >/dev/null 2>&1
        fi
    ) &
    log_message "Network optimizations for ${FDTFILE} applied."
fi

# General System Services

# Restart ssh service
mkdir -p -m0755 /var/run/sshd >/dev/null 2>&1 || true
if [[ -f "/etc/init.d/ssh" ]]; then
    (sleep 5 && /etc/init.d/ssh restart >/dev/null 2>&1) &
    log_message "SSH service restart attempted."
fi

# Add network performance optimization
if [[ -x "/usr/sbin/balethirq.pl" ]]; then
    (perl /usr/sbin/balethirq.pl >/dev/null 2>&1) &
    log_message "Network optimization service (balethirq.pl) execution attempted."
fi

# Led display control, Only for Amlogic devices (meson-*) with valid boxid.
openvfd_enable="no"  # yes or no, set to "yes" to enable OpenVFD service.
openvfd_boxid="15"   # Set the boxid according to your device. Refer to the documentation for details.
openvfd_restart="no" # yes or no, set to "yes" to restart the OpenVFD service.
if [[ "${openvfd_boxid}" != "0" && "${FDTFILE}" =~ ^meson- ]]; then
    (
        # Start OpenVFD service
        [[ "${openvfd_enable}" == "yes" ]] && armbian-openvfd "${openvfd_boxid}" >/dev/null 2>&1
        # Some devices require a restart to clear 'BOOT' and related messages
        [[ "${openvfd_restart}" == "yes" ]] && {
            armbian-openvfd "0" >/dev/null 2>&1
            sleep 3
            armbian-openvfd "${openvfd_boxid}" >/dev/null 2>&1
        }
        log_message "OpenVFD service execution attempted."
    ) &
fi

# For vplus(Allwinner h6) led color lights
if [[ -x "/usr/bin/rgb-vplus" ]]; then
    rgb-vplus --RedName=RED --GreenName=GREEN --BlueName=BLUE >/dev/null 2>&1 &
    log_message "Vplus RGB LED service started in background."
fi

# For fan control service
if [[ -x "/usr/bin/pwm-fan.pl" ]]; then
    perl /usr/bin/pwm-fan.pl >/dev/null 2>&1 &
    log_message "Fan control service (pwm-fan.pl) started in background."
fi

# For oes(A311d) SATA LED status monitoring
if [[ -x "/usr/bin/oes_sata_leds.sh" ]]; then
    /usr/bin/oes_sata_leds.sh >/var/log/oes-sata-leds.log 2>&1 &
    log_message "SATA status check service (oes_sata_leds.sh) started in background."
fi

# For pveproxy startup service
if [[ -n "$(dpkg -l | awk '{print $2}' | grep -w "^pve-manager$" || true)" ]]; then
    # Restarting systemd services can sometimes fail during early boot.
    (systemctl restart pveproxy >/dev/null 2>&1) &
    log_message "PVE proxy service restart attempted."
fi

# Maximize root partition size
todo_rootfs_resize="/root/.no_rootfs_resize"
[[ -f "${todo_rootfs_resize}" && "$(cat ${todo_rootfs_resize} 2>/dev/null | xargs)" == "yes" ]] && {
    armbian-tf >/dev/null 2>&1 &
    log_message "Root partition resized successfully."
}

# Finalization
log_message "All custom services have been processed."
