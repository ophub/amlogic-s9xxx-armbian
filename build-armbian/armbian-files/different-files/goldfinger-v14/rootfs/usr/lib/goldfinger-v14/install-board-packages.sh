#!/bin/sh
set -eu

marker_dir="/var/lib/goldfinger-v14"
marker="$marker_dir/bluez-installed"
package="/usr/lib/goldfinger-v14/packages/bluez_5.72-0ubuntu5.5_arm64.deb"

if dpkg-query -W -f='${Status}' bluez 2>/dev/null | grep -qx 'install ok installed'; then
    installed="yes"
else
    installed="no"
fi

if [ "$installed" != "yes" ]; then
    printf '%s  %s\n' \
        'ced50bcaee2c563ba965ff5faa70ae54fc3e22b8ebc09ccf9474beed63eda9d4' \
        "$package" | sha256sum --status -c -
    dpkg -i "$package"
fi

command -v bluetoothctl >/dev/null 2>&1
systemctl enable --now bluetooth.service >/dev/null
systemctl is-active --quiet bluetooth.service
mkdir -p "$marker_dir"
touch "$marker"
