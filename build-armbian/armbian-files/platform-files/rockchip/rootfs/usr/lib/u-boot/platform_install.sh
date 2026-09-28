#!/bin/bash
#================================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Function: Provide the bootloader write hook for Rockchip boards, so that the
#           Armbian installer engine can write the bootloader to eMMC.
#
#   The installer engine (shipped by the armbian-config package, reachable as
#   `armbian-config --api module_partitioner` or through the documented
#   `armbian-install` name) sources this file at runtime:
#
#       [[ -f /usr/lib/u-boot/platform_install.sh ]] && source /usr/lib/u-boot/platform_install.sh
#
#   and offers the `--boot emmc` mode only when the function below exists:
#
#       [[ "$(type -t write_uboot_platform)" == function ]] && have_uboot=1
#
#   Images that install their u-boot from release files (as this project does)
#   never install a u-boot package, so this file never appears and the eMMC
#   install mode silently disappears. This file closes that gap.
#
#   Coverage: one file has to serve every Rockchip board of this project, whose
#   u-boot directories hold very different sets of files (a single combined
#   image, idbloader + FIT, or vendor blobs such as bootloader.bin). Instead of
#   guessing from the file names, the hook reads the per-board records the
#   image was built with: /etc/ophub-release carries MAINLINE_UBOOT,
#   BOOTLOADER_IMG and TRUST_IMG (generated from model_database.conf), and the
#   write order and offsets below mirror the Rockchip branch of the image build
#   (rebuild: "Write the specific bootloader for [ Rockchip ] boxes"), so an
#   installed eMMC gets the same bootloader layout as a written SD image.
#
#   Two adjustments are required when the hook is driven by the installer
#   engine rather than by a build:
#
#     1. Fall back to the well-known u-boot directories, because the engine may
#        call the hook with an empty directory argument.
#     2. Return a non-zero status instead of exiting, so the engine can report
#        the failure itself and its own log stays complete.
#
#   Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: sourced by armbian-config; not meant to be executed directly.
#
#================================================================================================

write_uboot_platform() {
	# write_uboot_platform <u-boot directory> <target disk>
	local dir="$1"
	# This is run board-side too, so account for the non-existance of run_host_command_logged
	local logging_prelude=""
	[[ $(type -t run_host_command_logged) == function ]] && logging_prelude="run_host_command_logged"

	# The installer engine may hand us an empty directory: fall back to the
	# well-known u-boot locations.
	if [[ -z "$dir" || ! -d "$dir" ]]; then
		local candidate
		for candidate in /usr/lib/u-boot "${MOUNT}/usr/lib/u-boot"; do
			[[ -n "$candidate" && -d "$candidate" ]] && { dir="$candidate"; break; }
		done
	fi

	# Read one value out of the records this image was built with. Parsed, not
	# sourced: that file defines generic names such as PLATFORM and BOARD which
	# the caller may be using.
	_wup_record() {
		grep -m1 "^$1=" /etc/ophub-release 2>/dev/null | cut -d"'" -f2
	}

	# Resolve one loader file name: the directory we were given, then the
	# running system, then the target rootfs mount point the engine exports.
	_wup_resolve() {
		local name="$1" p
		[[ -n "$name" ]] || return 1
		for p in "${dir}/${name}" "/usr/lib/u-boot/${name}" "${MOUNT}/usr/lib/u-boot/${name}"; do
			[[ -f "$p" ]] && { printf '%s' "$p"; return 0; }
		done
		return 1
	}

	local bootloader_img="" mainline_uboot="" trust_img=""
	if [[ -f /etc/ophub-release ]]; then
		[[ -n "$(_wup_record BOOTLOADER_IMG)" ]] && bootloader_img="$(_wup_resolve "$(basename "$(_wup_record BOOTLOADER_IMG)")")"
		[[ -n "$(_wup_record MAINLINE_UBOOT)" ]] && mainline_uboot="$(_wup_resolve "$(basename "$(_wup_record MAINLINE_UBOOT)")")"
		[[ -n "$(_wup_record TRUST_IMG)" ]] && trust_img="$(_wup_resolve "$(basename "$(_wup_record TRUST_IMG)")")"
	fi

	if [[ -n "$bootloader_img" && -n "$mainline_uboot" && -n "$trust_img" ]]; then # loader + mainline + trust
		${logging_prelude} dd if="$bootloader_img" of="$2" conv=fsync,notrunc bs=512 seek=64
		${logging_prelude} dd if="$mainline_uboot" of="$2" conv=fsync,notrunc bs=512 seek=16384
		${logging_prelude} dd if="$trust_img" of="$2" conv=fsync,notrunc bs=512 seek=24576
	elif [[ -n "$bootloader_img" && -n "$mainline_uboot" ]]; then # loader + mainline
		${logging_prelude} dd if="$bootloader_img" of="$2" conv=fsync,notrunc bs=512 seek=64
		${logging_prelude} dd if="$mainline_uboot" of="$2" conv=fsync,notrunc bs=512 seek=16384
	elif [[ "${bootloader_img##*/}" == "u-boot-rockchip.bin" && -n "$bootloader_img" ]]; then # combined image
		${logging_prelude} dd if="$bootloader_img" of="$2" conv=fsync,notrunc bs=512 seek=64
	elif [[ -n "$bootloader_img" ]]; then # vendor blob with a 64-sector header
		${logging_prelude} dd if="$bootloader_img" of="$2" conv=fsync,notrunc bs=512 skip=64 seek=64
	else
		# Not an image built from this project's records: fall back to the file
		# names of the upstream Rockchip families.
		if [ -f "$dir/u-boot-rockchip.bin" ]; then # "$BOOT_SCENARIO" == "binman"
			${logging_prelude} dd if="$dir/u-boot-rockchip.bin" of="$2" bs=32k seek=1 conv=notrunc status=none
		elif [ -f "$dir/rksd_loader.img" ]; then # legacy rk3399 loader
			${logging_prelude} dd if="$dir/rksd_loader.img" of="$2" seek=64 conv=notrunc status=none
		elif [[ -f "$dir/u-boot.itb" && -f "$dir/idbloader.img" ]]; then # "blobless" or "tpl-spl-blob"
			${logging_prelude} dd if="$dir/idbloader.img" of="$2" seek=64 conv=notrunc status=none
			${logging_prelude} dd if="$dir/u-boot.itb" of="$2" seek=16384 conv=notrunc status=none
		elif [[ -f "$dir/uboot.img" ]]; then # "only-blobs"
			${logging_prelude} dd if="$dir/idbloader.bin" of="$2" seek=64 conv=notrunc status=none
			${logging_prelude} dd if="$dir/uboot.img" of="$2" seek=16384 conv=notrunc status=none
			${logging_prelude} dd if="$dir/trust.bin" of="$2" seek=24576 conv=notrunc status=none
		else
			echo "write_uboot_platform: no usable u-boot files found in '${dir:-<empty>}'" >&2
			return 1
		fi
	fi
}
