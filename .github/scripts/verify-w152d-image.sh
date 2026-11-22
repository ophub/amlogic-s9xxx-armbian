#!/usr/bin/env bash
set -euo pipefail

image=$1
scratch=$(mktemp -d "${TMPDIR:-/var/tmp}/w152d-verify.XXXXXX")
loop=
cleanup() {
    if [[ -n "$loop" ]]; then
        mountpoint -q "$scratch/root/tmp" && umount "$scratch/root/tmp"
        mountpoint -q "$scratch/root/dev" && umount "$scratch/root/dev"
        mountpoint -q "$scratch/root/boot" && umount "$scratch/root/boot"
        mountpoint -q "$scratch/root" && umount "$scratch/root"
        losetup -d "$loop"
    fi
    rm -f -- "$scratch/disk.img" "$scratch/initramfs.list"
    rmdir -- "$scratch/root" "$scratch"
}
trap cleanup EXIT
mkdir "$scratch/root"
gzip -dc -- "$image" >"$scratch/disk.img"
loop=$(losetup --find --show --read-only --partscan "$scratch/disk.img")
bootdev="$loop"p1
rootdev="$loop"p2
test "$(blkid -p -s PTTYPE -o value "$loop")" = dos
test "$(blkid -p -s TYPE -o value "$bootdev")" = vfat
test "$(blkid -p -s TYPE -o value "$rootdev")" = ext4
test ! -b "$loop"p3
fsck.fat -n "$bootdev"
e2fsck -fn "$rootdev"
mount -o ro,noload "$rootdev" "$scratch/root"
root="$scratch/root"
boot="$root/boot"
mount -o ro "$bootdev" "$boot"

root_uuid=$(blkid -p -s UUID -o value "$rootdev")
grep -E "^APPEND=.*root=UUID=$root_uuid([[:space:]]|$)" "$boot/uEnv.txt"
grep -F 'console=ttyAML0,921600n8' "$boot/uEnv.txt"
grep -qFx 'FDT=/dtb/amlogic/meson-gxlx4-w152d.dtb' "$boot/uEnv.txt"
awk -v uuid="$root_uuid" '$1 == "UUID=" uuid && $2 == "/" && $3 == "ext4" { found = 1 } END { exit !found }' "$root/etc/fstab"
test -s "$boot/dtb/amlogic/meson-gxlx4-w152d.dtb"
dumpimage -l "$boot/aml_autoscript"
dumpimage -l "$boot/uInitrd"
test "$(stat -c %s "$boot/uInitrd")" -le $((0x01800000))
test "$(stat -c %s "$boot/zImage")" -le $((0x03200000))
modules_path="$root/usr/lib/modules"
release=$(find "$modules_path" -mindepth 1 -maxdepth 1 -type d -name '6.12.118-*' -printf '%f\n')
test -n "$release"
test -s "$boot/initrd.img-$release"
cmp "$boot/uInitrd" "$boot/uInitrd-$release"
cmp "$boot/zImage" "$boot/vmlinuz-$release"
mount --bind /dev "$root/dev"
mount -t tmpfs -o size=256m tmpfs "$root/tmp"
chroot "$root" lsinitramfs "/boot/initrd.img-$release" >"$scratch/initramfs.list"
if ! grep -qF '/xhci-plat-hcd.ko' "$modules_path/$release/modules.builtin"; then
    grep -F '/xhci-plat-hcd.ko' "$scratch/initramfs.list"
fi
find "$modules_path/$release" -name 'sprdwl_ng.ko*' -print | grep -F 'sprdwl_ng.ko'
wifi_deps=$(chroot "$root" /usr/sbin/modinfo -k "$release" -F depends sprdwl_ng)
case ",$wifi_deps," in
    *,sunxi_addr,*) echo 'W152D sprdwl_ng requires the Allwinner MAC provider' >&2; exit 1 ;;
esac
grep -qx sprdwl_ng "$root/etc/modules-load.d/w152d.conf"
grep -qx "BOARD='w152d'" "$root/etc/ophub-release"
grep -qx "VENDOR_USB_BOOT='yes'" "$root/etc/ophub-release"
init=$(chroot "$root" readlink -f /sbin/init)
case "$init" in
    /usr/lib/systemd/systemd|/lib/systemd/systemd) ;;
    *) echo "Unexpected init: $init" >&2; exit 1 ;;
esac
test -L "$root/etc/systemd/system/multi-user.target.wants/armbian-front-panel.service"
if ! test -L "$root/etc/systemd/system/multi-user.target.wants/armbian-usb-rootfs-resize.service"; then
    test -x "$root/etc/rc.local"
    grep -F 'bash /etc/custom_service/start_service.sh &' "$root/etc/rc.local"
fi
grep -qx yes "$root/root/.no_rootfs_resize"
test ! -s "$root/root/.ssh/authorized_keys"
(
    cd "$root/usr/lib/firmware"
    sha256sum -c <<'SUMS'
2515a0a09d8e94da63aa2769b6457eee3c56c300641675da3aeb5ede09dacfaf  wcnmodem.bin
4c95dac0bf3681e1af31e89d6bd54494ce183839cf4a2e19c5953e2b243398f7  wifi_56630001_3ant.ini
SUMS
)
echo "Verified W152D USB image and $release"
