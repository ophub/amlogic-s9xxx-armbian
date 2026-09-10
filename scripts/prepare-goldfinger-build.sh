#!/bin/sh
set -eu

repo_dir="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
board_dir="$repo_dir/build-armbian/armbian-files/different-files/goldfinger-v14"
boot_dir="$board_dir/bootfs"
firmware_dir="$board_dir/rootfs/usr/lib/firmware/brcm"
firmware_doc_dir="$board_dir/rootfs/usr/share/doc/goldfinger-v14/firmware"
package_dir="$board_dir/rootfs/usr/lib/goldfinger-v14/packages"
download_dir="$repo_dir/build/goldfinger-inputs"

rkwifibt_commit="b2af9d94739922cbafb8c92514d2fa79c1e71a59"
rkwifibt_base="https://raw.githubusercontent.com/nishantpoorswani/rkwifibt/$rkwifibt_commit"
wifi_hash="e59d485296365ca17bd7f9cfa7be390b0b58019ee9e2d59fb78445fa33d27d48"
nvram_hash="92d89e67df52b9ffebde9ae852bb54f3fa10d5e3f8b4b777c9ff2fc5dd5fbf29"
bt_hash="afc05608aa0058cde4ddc0f51138ff1b7629997c9f53d67c4948838d783b1fa6"
rkwifibt_notice_hash="38751245389e1e23f73e6f5384b5cbe7fa972cc4410c5adc9c04b082a0b9561a"
env_tool_url="https://ports.ubuntu.com/ubuntu-ports/pool/main/libu/libubootenv/libubootenv-tool_0.3.5-0.1build1_arm64.deb"
env_lib_url="https://ports.ubuntu.com/ubuntu-ports/pool/main/libu/libubootenv/libubootenv0.1_0.3.5-0.1build1_arm64.deb"
bluez_url="https://ports.ubuntu.com/ubuntu-ports/pool/main/b/bluez/bluez_5.72-0ubuntu5.5_arm64.deb"
env_tool_hash="fe98b3ed1731ff4660c874b4e9262864c1fa1fb0be0cd093b43c84699c5237b5"
env_lib_hash="d6bac432ceb214b40b648f0e19cb9e3197ef3f8ce24f12da5e03e55a93127cb8"
bluez_hash="ced50bcaee2c563ba965ff5faa70ae54fc3e22b8ebc09ccf9474beed63eda9d4"

for tool in curl dpkg-deb mkimage sha256sum; do
    command -v "$tool" >/dev/null 2>&1 || {
        printf 'Error: required build tool is missing: %s\n' "$tool" >&2
        exit 1
    }
done

mkdir -p "$download_dir" "$firmware_dir" "$firmware_doc_dir" "$package_dir"
wifi="$download_dir/brcmfmac4359-sdio.amlogic,sm1.bin"
nvram="$download_dir/brcmfmac4359-sdio.amlogic,sm1.txt"
bt="$download_dir/BCM4359C0.hcd"
rkwifibt_notice="$download_dir/NOTICE.rkwifibt"

verify_firmware() {
    printf '%s  %s\n' \
        "$wifi_hash" "$wifi" \
        "$nvram_hash" "$nvram" \
        "$bt_hash" "$bt" \
        "$rkwifibt_notice_hash" "$rkwifibt_notice" | sha256sum --status -c -
}

if ! verify_firmware 2>/dev/null; then
    trap 'rm -f "$wifi.tmp" "$nvram.tmp" "$bt.tmp" "$rkwifibt_notice.tmp"' EXIT INT TERM
    printf 'Downloading and verifying licensed Goldfinger AP6398S firmware...\n'
    curl -fsSL "$rkwifibt_base/firmware/broadcom/AP6398S/wifi/fw_bcm4359c0_ag.bin" -o "$wifi.tmp"
    curl -fsSL "$rkwifibt_base/firmware/broadcom/AP6398S/wifi/nvram_ap6398s.txt" -o "$nvram.tmp"
    curl -fsSL "$rkwifibt_base/firmware/broadcom/AP6398S/bt/BCM4359C0.hcd" -o "$bt.tmp"
    curl -fsSL "$rkwifibt_base/NOTICE" -o "$rkwifibt_notice.tmp"
    mv "$wifi.tmp" "$wifi"
    mv "$nvram.tmp" "$nvram"
    mv "$bt.tmp" "$bt"
    mv "$rkwifibt_notice.tmp" "$rkwifibt_notice"
    trap - EXIT INT TERM
fi
verify_firmware || { printf 'Error: Goldfinger firmware checksum failed.\n' >&2; exit 1; }

cp "$wifi" "$firmware_dir/brcmfmac4359-sdio.amlogic,sm1.bin"
cp "$nvram" "$firmware_dir/brcmfmac4359-sdio.amlogic,sm1.txt"
cp "$bt" "$firmware_dir/BCM4359C0.hcd"
rm -f "$firmware_doc_dir/LICENCE.cypress"
cp "$rkwifibt_notice" "$firmware_doc_dir/NOTICE.rkwifibt"

env_tool="$download_dir/libubootenv-tool_0.3.5-0.1build1_arm64.deb"
env_lib="$download_dir/libubootenv0.1_0.3.5-0.1build1_arm64.deb"
if ! printf '%s  %s\n' "$env_tool_hash" "$env_tool" "$env_lib_hash" "$env_lib" |
    sha256sum --status -c - 2>/dev/null; then
    printf 'Downloading and verifying Ubuntu Noble ARM64 environment tools...\n'
    curl -fsSL "$env_tool_url" -o "$env_tool.tmp"
    curl -fsSL "$env_lib_url" -o "$env_lib.tmp"
    printf '%s  %s\n' "$env_tool_hash" "$env_tool.tmp" "$env_lib_hash" "$env_lib.tmp" |
        sha256sum --status -c - || { printf 'Error: environment-tool package checksum failed.\n' >&2; exit 1; }
    mv "$env_tool.tmp" "$env_tool"
    mv "$env_lib.tmp" "$env_lib"
fi
cp "$env_tool" "$package_dir/"
cp "$env_lib" "$package_dir/"

bluez="$download_dir/bluez_5.72-0ubuntu5.5_arm64.deb"
if ! printf '%s  %s\n' "$bluez_hash" "$bluez" |
    sha256sum --status -c - 2>/dev/null; then
    printf 'Downloading and verifying Ubuntu Noble ARM64 BlueZ...\n'
    curl -fsSL "$bluez_url" -o "$bluez.tmp"
    printf '%s  %s\n' "$bluez_hash" "$bluez.tmp" |
        sha256sum --status -c - || { printf 'Error: BlueZ package checksum failed.\n' >&2; exit 1; }
    mv "$bluez.tmp" "$bluez"
fi
[ "$(dpkg-deb -f "$bluez" Package)" = "bluez" ] || {
    printf 'Error: unexpected BlueZ package metadata.\n' >&2
    exit 1
}
[ "$(dpkg-deb -f "$bluez" Architecture)" = "arm64" ] || {
    printf 'Error: BlueZ package is not arm64.\n' >&2
    exit 1
}
cp "$bluez" "$package_dir/"

mkimage -A arm64 -T script -C none -n 'Goldfinger USB boot' \
    -d "$boot_dir/aml_autoscript.cmd" "$boot_dir/aml_autoscript" >/dev/null
cp "$boot_dir/aml_autoscript" "$boot_dir/s905_autoscript"
mkimage -A arm -T script -C none -n 'Goldfinger eMMC boot' \
    -d "$boot_dir/emmc_autoscript.cmd" "$boot_dir/emmc_autoscript" >/dev/null

printf 'Goldfinger board inputs are prepared and checksum-verified.\n'
