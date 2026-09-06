#!/usr/bin/env python3
"""Verify W103D firmware and optionally the assembled kernel/boot/module set."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess


def output(*args):
    return subprocess.check_output(args, text=True).strip()


def verify(rootfs, bootfs=None, release=None):
    board = Path(__file__).resolve().parents[1]
    manifest = json.loads((board / 'firmware/manifest.json').read_text(encoding='utf-8'))
    for entry in manifest['files']:
        path = rootfs / entry['path']
        if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != entry['sha256']:
            raise RuntimeError(f'W103D firmware missing or unvalidated: {path}')
    if bootfs is None:
        return
    if not release or not re.fullmatch(r'6\.(12|18)\.\d+-ophub', release):
        raise RuntimeError(f'Unsupported W103D target release: {release}')
    config = (bootfs / ('config-' + release)).read_text(encoding='utf-8')
    for setting in ['CONFIG_MT7663S=m', 'CONFIG_BT_MTKSDIO=m',
                    'CONFIG_MT7663S_W103D_COMBO=y', 'CONFIG_MMC_MESON_GX=y']:
        if setting not in config.splitlines():
            raise RuntimeError(f'W103D kernel configuration missing {setting}')
    image = (bootfs / 'zImage').read_bytes()
    if image[56:60] != b'ARM\x64' or ('Linux version ' + release + ' ').encode() not in image:
        raise RuntimeError('W103D Image header/release does not match target')
    if b'direct completion for copy-free SDIO requests' not in image:
        raise RuntimeError('W103D Image lacks the tested Meson MMC completion path')
    dtb = bootfs / 'dtb/amlogic/meson-g12a-w103d.dtb'
    if not dtb.is_file() or b'zte,w103d\0' not in dtb.read_bytes():
        raise RuntimeError('Missing W103D device tree in kernel package')
    modules = rootfs / 'usr/lib/modules' / release
    for name in ['mt7663s', 'btmtksdio']:
        found = [p for p in modules.rglob(name + '.ko*')
                 if p.name in [name + '.ko', name + '.ko.xz', name + '.ko.zst', name + '.ko.gz']]
        if len(found) != 1:
            raise RuntimeError(f'Expected one {name} module for {release}, found {len(found)}')
        module = str(found[0])
        if output('modinfo', '-F', 'vermagic', module).split()[0] != release:
            raise RuntimeError(f'W103D module ABI mismatch: {module}')
        if name == 'mt7663s':
            required = set('mt76-connac-lib,btmtksdio,mt7663-usb-sdio-common,mt7615-common,mt76-sdio,mt76,mac80211,cfg80211'.split(','))
            if not required <= set(output('modinfo', '-F', 'depends', module).split(',')):
                raise RuntimeError('W103D Wi-Fi/combo module dependencies are incomplete')
            if 'sdio:c*v037Ad7603*' not in output('modinfo', '-F', 'alias', module).splitlines():
                raise RuntimeError('W103D Wi-Fi SDIO alias is missing')
    initrd = bootfs / ('initrd.img-' + release)
    entries = output('lsinitramfs', str(initrd)).splitlines()
    versions = {m[1] for line in entries if (m := re.search(r'(?:^|/)lib/modules/([^/\s]+)', line))}
    if versions != {release}:
        raise RuntimeError(f'W103D initramfs module releases do not match: {versions}')
    ramdisk = (bootfs / 'uInitrd').read_bytes()
    if ramdisk[:4] != bytes.fromhex('27051956') or ramdisk[64:] != initrd.read_bytes():
        raise RuntimeError('W103D active uInitrd does not wrap the matching initramfs')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rootfs', type=Path, required=True)
    parser.add_argument('--bootfs', type=Path)
    parser.add_argument('--release')
    args = parser.parse_args()
    verify(args.rootfs, args.bootfs, args.release)
    print('PASS: W103D pinned firmware' + (' and assembled kernel/DTB/initramfs/modules' if args.bootfs else ''))


if __name__ == '__main__':
    main()
