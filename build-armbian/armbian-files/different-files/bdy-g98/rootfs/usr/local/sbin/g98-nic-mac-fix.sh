#!/bin/bash
# --------------------------------------------------------
# G98 2.5G NIC blank-EEPROM fallback:
# Units shipped with a blank RTL8125/RTL8126 config EEPROM
# get a random MAC from the driver on every boot
# (addr_assign_type=3). When that is detected, derive a
# stable per-unit MAC from machine-id + PCI address and
# persist it via a udev .link file. NICs with a factory
# MAC are left untouched.
# --------------------------------------------------------

LINK_DIR="/etc/systemd/network"
mkdir -p "${LINK_DIR}"

for pci in 0002:21:00.0 0003:31:00.0; do
    nd="$(ls -d /sys/bus/pci/devices/${pci}/net/* 2>/dev/null | head -n1)"
    [ -n "${nd}" ] || { echo "${pci}: no NIC found, skipped"; continue; }
    ifname="$(basename "${nd}")"
    linkfile="${LINK_DIR}/10-g98-nic-${pci}.link"

    type="$(cat "${nd}/addr_assign_type" 2>/dev/null)"
    if [ "${type}" != "3" ]; then
        echo "${ifname} (${pci}): factory MAC present (type=${type}), nothing to do"
        continue
    fi

    hash="$(echo "$(cat /etc/machine-id)-${pci}" | sha256sum | cut -c1-10)"
    mac="$(echo "${hash}" | sed 's/\(..\)\(..\)\(..\)\(..\)\(..\).*/02:\1:\2:\3:\4:\5/')"

    # Rewrite on every run: the content is deterministic
    cat > "${linkfile}" <<EOF
[Match]
Path=pci-${pci} platform-*-pci-${pci}

[Link]
MACAddress=${mac}
EOF
    echo "${ifname} (${pci}): random MAC fallback -> ${mac} (${linkfile})"

    # Apply the MAC right away so that even the very first boot
    # (no .link in place yet) needs no reboot. A failure here is
    # harmless: udev will apply the .link on the next boot.
    cur="$(cat "${nd}/address" 2>/dev/null)"
    if [ "${cur}" != "${mac}" ]; then
        ip link set dev "${ifname}" address "${mac}" 2>/dev/null \
            && echo "${ifname}: MAC applied immediately" \
            || echo "${ifname}: immediate apply failed, .link takes effect on next boot"
    fi
done
