#!/bin/bash
# Offline build verification only; this script never installs or packages modules.
set -euo pipefail

if [[ $# -lt 3 || $# -gt 4 ]]; then
    echo "Usage: $0 PATCHED_KERNEL_SOURCE OUTPUT_DIRECTORY OPHUB_CONFIG [JOBS]" >&2
    exit 2
fi

kernel_source=$(realpath "$1")
mkdir -p "$2"
kernel_output=$(realpath "$2")
base_config=$(realpath "$3")
repo=$(cd "$(dirname "$0")/../../.." && pwd)
jobs=${4:-$(nproc)}
[[ "$kernel_source" != "$kernel_output" ]]

build_args=(-C "$kernel_source" O="$kernel_output"
    ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- LOCALVERSION=-ophub)
version=$(make -s "${build_args[@]}" kernelversion)
series=${version%.*}
case "$series" in
    6.12|6.18) ;;
    *) echo "Unsupported kernel series: $version" >&2; exit 1 ;;
esac
fragment="$repo/board/w103d/linux/w103d-$series.config"
[[ -f "$kernel_source/include/linux/mt7663-combo.h" ]]
[[ -f "$kernel_source/drivers/net/wireless/mediatek/mt76/mt7663s/mt7663s_w103d.c" ]]

bash "$kernel_source/scripts/kconfig/merge_config.sh" -m -O "$kernel_output" \
    "$base_config" "$fragment"
"$kernel_source/scripts/config" --file "$kernel_output/.config" \
    --set-str LOCALVERSION '' --disable LOCALVERSION_AUTO
make "${build_args[@]}" olddefconfig
grep -qx 'CONFIG_MT7663S_W103D_COMBO=y' "$kernel_output/.config"
expected_release="$version-ophub"
[[ $(make -s "${build_args[@]}" kernelrelease) == "$expected_release" ]]

make "${build_args[@]}" -j"$jobs" Image modules amlogic/meson-g12a-w103d.dtb
wifi="$kernel_output/drivers/net/wireless/mediatek/mt76/mt7663s/mt7663s.ko"
bluetooth="$kernel_output/drivers/bluetooth/btmtksdio.ko"
for module in "$wifi" "$bluetooth"; do
    vermagic=$(modinfo -F vermagic "$module")
    [[ "${vermagic%% *}" == "$expected_release" ]]
    modinfo -F vermagic "$module"
done
depends=$(modinfo -F depends "$wifi")
for dependency in mt76-connac-lib mt7663-usb-sdio-common mt7615-common \
        mt76-sdio mt76 mac80211 cfg80211 btmtksdio; do
    [[ ",$depends," == *",$dependency,"* ]]
done
modinfo -F alias "$wifi" | grep -Fx 'sdio:c*v037Ad7603*'
echo "PASS: offline $expected_release Image, modules, DTB and module metadata"
echo 'Deployment requires a separately verified kernel/module pair; no deployment was performed.'
