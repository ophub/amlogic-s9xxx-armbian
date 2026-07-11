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
# Function: Customize the startup script. Add content as needed.
# Dependent script: /etc/rc.local (which runs with 'set -e')
# File path: /etc/custom_service/start_service.sh
#
# Version: v1.3
#
#========================================================================================

set +euo pipefail

trap 'exit 0' EXIT

# Custom Service Log - all script output will be logged here
custom_log="/tmp/ophub_start_service.log"

# A helper function for logging with a timestamp
log_message() {
    echo "[$(date +"%Y.%m.%d.%H:%M:%S")] $1" >>"${custom_log}" 2>/dev/null || true
}

# Start of the script
log_message "Starting custom services..."

# Disabled verbose kernel messages on console
dmesg -n 1 >/dev/null 2>&1 || true
log_message "Kernel console logging level set to 1 (Panic only)."

# Search for the FDTFILE file (only the basename of the .dtb is needed)
ophub_release_file="/etc/ophub-release"
FDTFILE=""
# 1) /etc/ophub-release : FDTFILE='xxx.dtb'
[[ -f "${ophub_release_file}" ]] &&
    FDTFILE="$(awk -F"'" '/^FDTFILE=/ {print $2; exit}' "${ophub_release_file}" 2>/dev/null)"
# 2) /boot/uEnv.txt : FDT=/dtb/.../xxx.dtb  (or FDT=xxx.dtb)
[[ -z "${FDTFILE}" && -f "/boot/uEnv.txt" ]] &&
    FDTFILE="$(grep -E '^FDT=.*\.dtb$' /boot/uEnv.txt 2>/dev/null | head -n1 | sed -E 's#^FDT=##; s#.*/##')"
# 3) /boot/extlinux/extlinux.conf : "    fdt /dtb/.../xxx.dtb"
[[ -z "${FDTFILE}" && -f "/boot/extlinux/extlinux.conf" ]] &&
    FDTFILE="$(grep -Eo '/dtb/[^[:space:]]+\.dtb' /boot/extlinux/extlinux.conf 2>/dev/null | head -n1 | sed -E 's#.*/##')"
# 4) /boot/armbianEnv.txt : fdtfile=vendor/xxx.dtb  (or fdtfile=xxx.dtb)
[[ -z "${FDTFILE}" && -f "/boot/armbianEnv.txt" ]] &&
    FDTFILE="$(grep -E '^fdtfile=.*\.dtb$' /boot/armbianEnv.txt 2>/dev/null | head -n1 | sed -E 's#^fdtfile=##; s#.*/##')"
log_message "Detected FDT file: ${FDTFILE:-not found}"

# Device-Specific Services

# Add rknpu module to the system module load list
ophub_load_conf="/etc/modules-load.d/ophub-load-list.conf"
[[ -f "${ophub_load_conf}" ]] || touch "${ophub_load_conf}"
if modinfo rknpu >/dev/null 2>&1; then
    grep -q -x "rknpu" "${ophub_load_conf}" 2>/dev/null || echo "rknpu" >>"${ophub_load_conf}"
else
    grep -q -x "rknpu" "${ophub_load_conf}" 2>/dev/null && sed -i '/^rknpu$/d' "${ophub_load_conf}"
fi
log_message "Adjusted the rknpu module in the system module load list."

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

# For rk3399-zysj board: release the on-board USB HUB reset
if [[ "${FDTFILE}" == "rk3399-zysj.dtb" ]]; then
    (
        # The 5V rails (vcc-sys / vcc5v0-host / vbus-typec) are already driven high by
        # their regulators, but the USB HUB reset line (usb-hub-res = gpio1.18) is left
        # un-driven, so the HUB stays in reset and none of the expanded ports enumerate.
        # Resolve the sysfs numbers from each gpiochip base, because newer kernels (6.18)
        # allocate the base dynamically and gpio0 no longer starts at 0.
        gpio1_base="" gpio4_base=""
        for chip in /sys/class/gpio/gpiochip*; do
            case "$(cat "${chip}/label" 2>/dev/null)" in
            gpio1) gpio1_base="$(cat "${chip}/base" 2>/dev/null)" ;;
            gpio4) gpio4_base="$(cat "${chip}/base" 2>/dev/null)" ;;
            esac
        done
        if [[ -n "${gpio1_base}" && -n "${gpio4_base}" ]]; then
            pwr_en="$((gpio4_base + 28))"  # gpio4.28, board GPIO note "156"
            hub_rst="$((gpio1_base + 18))" # gpio1.18, usb-hub-res

            # Assert the extra power enable.
            echo "${pwr_en}" >/sys/class/gpio/export 2>/dev/null
            echo "out" >"/sys/class/gpio/gpio${pwr_en}/direction" 2>/dev/null
            echo "1" >"/sys/class/gpio/gpio${pwr_en}/value" 2>/dev/null

            # Release the USB HUB reset (gpio1.18) with a short low->high pulse.
            echo "${hub_rst}" >/sys/class/gpio/export 2>/dev/null
            echo "out" >"/sys/class/gpio/gpio${hub_rst}/direction" 2>/dev/null
            echo "0" >"/sys/class/gpio/gpio${hub_rst}/value" 2>/dev/null
            sleep 1
            echo "1" >"/sys/class/gpio/gpio${hub_rst}/value" 2>/dev/null
        fi
    ) &
    log_message "USB HUB reset released for rk3399-zysj."
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

# Enable UDP GRO forwarding on all physical ethernet interfaces
# View command: ethtool -k eth0 | grep -i udp
if command -v ethtool >/dev/null 2>&1; then
    shopt -s nullglob
    for iface in /sys/class/net/*/device; do
        iface_name="$(basename "${iface%/device}")"
        # Skip non-ethernet interfaces (type != 1) and wireless interfaces
        [[ "$(cat /sys/class/net/${iface_name}/type 2>/dev/null)" != "1" ]] && continue
        [[ -d "/sys/class/net/${iface_name}/wireless" ]] && continue
        ethtool -K "${iface_name}" rx-udp-gro-forwarding on >/dev/null 2>&1
        log_message "Enabled rx-udp-gro-forwarding on ${iface_name}."
    done
    shopt -u nullglob
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
    log_message "Root partition resize completed."
}

# Finalization
log_message "All custom services have been processed successfully."
trap '' HUP INT QUIT TERM PIPE
exit 0
