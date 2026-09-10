#!/bin/bash
# Board hooks sourced by ophub's normal /usr/sbin/armbian-install.

goldfinger_fail() {
    error_msg "Goldfinger V14: ${1}"
}

goldfinger_require_tools() {
    local tool

    if ! command -v fw_printenv >/dev/null 2>&1 || ! command -v fw_setenv >/dev/null 2>&1; then
        echo -e "${INFO} Installing the image's checksum-pinned U-Boot environment tools."
        dpkg -i /usr/lib/goldfinger-v14/packages/libubootenv0.1_0.3.5-0.1build1_arm64.deb \
            /usr/lib/goldfinger-v14/packages/libubootenv-tool_0.3.5-0.1build1_arm64.deb >/dev/null ||
            goldfinger_fail "could not install the included U-Boot environment tools"
    fi
    for tool in blockdev cmp date dd df e2fsck fsck.vfat fw_printenv fw_setenv lsblk mount sha256sum sfdisk stat truncate udevadm umount; do
        command -v "${tool}" >/dev/null 2>&1 || goldfinger_fail "required tool is missing: ${tool}"
    done
}

goldfinger_set_minimum_clock() {
    local build_epoch_file="/usr/lib/goldfinger-v14/image-build-epoch"
    local build_epoch current_epoch

    [[ -r "${build_epoch_file}" ]] ||
        goldfinger_fail "image build-time marker is missing"
    build_epoch="$(cat "${build_epoch_file}")"
    [[ "${build_epoch}" =~ ^[0-9]{10}$ ]] ||
        goldfinger_fail "image build-time marker is invalid"
    current_epoch="$(date -u +%s)"

    # Never move a plausible clock backwards. With no RTC, systemd initially
    # knows only the kernel build time, which can predate files in this image.
    if ((current_epoch < build_epoch)); then
        echo -e "${INFO} Advancing the offline clock to the image build time."
        date -u -s "@${build_epoch}" >/dev/null ||
            goldfinger_fail "could not establish a safe minimum system time"
    fi
}

goldfinger_compare_protected() {
    local boot_backup="${GF_RECOVERY_DIR}/bootloader-first-4MiB.img"

    # Sector-zero bytes 446-511 contain the MBR entries and signature. Those are
    # the only bytes below 4 MiB that this installer intentionally changes.
    cmp -s -n 446 "${boot_backup}" "${DEV_EMMC}" ||
        goldfinger_fail "bootloader bytes 0-445 changed unexpectedly"
    cmp -s -n $((4 * 1024 * 1024 - 512)) -i 512:512 "${boot_backup}" "${DEV_EMMC}" ||
        goldfinger_fail "bootloader bytes 512-4MiB changed unexpectedly"
    cmp -s "${GF_RECOVERY_DIR}/reserved-36-100MiB.img" \
        <(dd if="${DEV_EMMC}" bs=1M skip=36 count=64 status=none) ||
        goldfinger_fail "reserved region changed unexpectedly"
    cmp -s "${GF_RECOVERY_DIR}/environment-116-124MiB.img" \
        <(dd if="${DEV_EMMC}" bs=1M skip=116 count=8 status=none) ||
        goldfinger_fail "U-Boot environment changed before activation"

    if [[ -f "${GF_RECOVERY_DIR}/boot0.img" ]]; then
        cmp -s "${GF_RECOVERY_DIR}/boot0.img" "${DEV_EMMC}boot0" ||
            goldfinger_fail "eMMC boot0 changed unexpectedly"
    fi
    if [[ -f "${GF_RECOVERY_DIR}/boot1.img" ]]; then
        cmp -s "${GF_RECOVERY_DIR}/boot1.img" "${DEV_EMMC}boot1" ||
            goldfinger_fail "eMMC boot1 changed unexpectedly"
    fi
}

goldfinger_prepare_environment_access() {
    GF_ENV_CONFIG="/run/goldfinger-fw_env.config"
    printf '%s 0x7400000 0x10000 0x10000\n' "${DEV_EMMC}" >"${GF_ENV_CONFIG}"

    local current_bootcmd
    current_bootcmd="$(fw_printenv -c "${GF_ENV_CONFIG}" -n bootcmd 2>/dev/null)" ||
        goldfinger_fail "factory U-Boot environment was not readable at the validated offset"
    case "${current_bootcmd}" in
        "run storeboot" | "run start_autoscript; run storeboot") ;;
        *) goldfinger_fail "factory bootcmd is not a validated Goldfinger value" ;;
    esac
}

board_create_partition() {
    local model device_name device_bytes device_sectors available_kb timestamp
    local template mbr_patch

    goldfinger_require_tools
    goldfinger_set_minimum_clock

    model="$(tr -d '\000' </proc/device-tree/model 2>/dev/null || true)"
    [[ "${model}" == "Goldfinger V14 r6" ]] ||
        goldfinger_fail "running DTB does not identify this as the validated board"
    [[ "${AMLOGIC_SOC}" == "s905x3" && "${boxid}" == "528" ]] ||
        goldfinger_fail "the selected installer profile is not Goldfinger V14"
    [[ "${BOARD_COMPATIBILITY_ID:-}" == "goldfinger-v14-2021-12-07" &&
        "${BOARD_PCB_MARKING:-}" == "GOLDFINGER_V14" &&
        "${BOARD_PCB_DATE:-}" == "2021-12-07" ]] ||
        goldfinger_fail "image PCB compatibility metadata is missing or invalid"

    [[ -b "${DEV_EMMC}" ]] || goldfinger_fail "internal eMMC device is missing"
    device_name="$(basename "${DEV_EMMC}")"
    [[ "${device_name}" == mmcblk* ]] || goldfinger_fail "target is not an MMC whole disk"
    [[ "$(blockdev --getss "${DEV_EMMC}")" == "512" ]] ||
        goldfinger_fail "eMMC does not use validated 512-byte logical sectors"
    device_bytes="$(blockdev --getsize64 "${DEV_EMMC}")"
    [[ "${device_bytes}" -ge 14000000000 && "${device_bytes}" -le 17500000000 ]] ||
        goldfinger_fail "eMMC is outside the validated 16GB capacity class"
    device_sectors=$((device_bytes / 512))
    [[ "${device_sectors}" -gt 1318912 ]] || goldfinger_fail "eMMC is too small"

    if lsblk -nrpo MOUNTPOINT "${DEV_EMMC}" | awk 'NF { found=1 } END { exit !found }'; then
        goldfinger_fail "eMMC or one of its partitions is mounted"
    fi

    mkdir -p /ddbr
    available_kb="$(df -Pk /ddbr | awk 'NR==2 {print $4}')"
    [[ "${available_kb}" -ge 100000 ]] ||
        goldfinger_fail "installation USB needs at least 100MB free for boot-chain recovery data"
    timestamp="$(date -u +%Y%m%dT%H%M%SZ)"
    GF_RECOVERY_DIR="/ddbr/goldfinger-v14-${timestamp}"
    mkdir -m 0700 "${GF_RECOVERY_DIR}"

    echo -e "${INFO} Backing up this unit's protected boot-chain regions to USB."
    dd if="${DEV_EMMC}" of="${GF_RECOVERY_DIR}/bootloader-first-4MiB.img" bs=1M count=4 status=progress conv=fsync
    dd if="${DEV_EMMC}" of="${GF_RECOVERY_DIR}/reserved-36-100MiB.img" bs=1M skip=36 count=64 status=progress conv=fsync
    dd if="${DEV_EMMC}" of="${GF_RECOVERY_DIR}/environment-116-124MiB.img" bs=1M skip=116 count=8 status=progress conv=fsync
    [[ -b "${DEV_EMMC}boot0" ]] &&
        dd if="${DEV_EMMC}boot0" of="${GF_RECOVERY_DIR}/boot0.img" bs=1M status=progress conv=fsync
    [[ -b "${DEV_EMMC}boot1" ]] &&
        dd if="${DEV_EMMC}boot1" of="${GF_RECOVERY_DIR}/boot1.img" bs=1M status=progress conv=fsync
    (cd "${GF_RECOVERY_DIR}" && sha256sum ./*.img >SHA256SUMS)
    chmod 0600 "${GF_RECOVERY_DIR}"/*
    (cd "${GF_RECOVERY_DIR}" && sha256sum --status -c SHA256SUMS) ||
        goldfinger_fail "boot-chain recovery data failed verification"

    # Prove that Linux can parse the environment before making any eMMC change.
    goldfinger_prepare_environment_access

    # Construct an MBR on a sparse regular file, then copy only its 64-byte
    # partition table and 2-byte signature. No generic partitioner touches the
    # live device's bootloader or protected gaps.
    template="/run/goldfinger-partition-template.img"
    mbr_patch="/run/goldfinger-mbr-partitions.bin"
    truncate -s "${device_bytes}" "${template}"
    sfdisk "${template}" >/dev/null <<EOF || goldfinger_fail "could not generate the exact partition table"
label: dos
unit: sectors

start=270336, size=1046528, type=c
start=1318912, size=$((device_sectors - 1318912)), type=83
EOF
    dd if="${template}" of="${mbr_patch}" bs=1 skip=446 count=66 status=none
    [[ "$(stat -c %s "${mbr_patch}")" == "66" ]] ||
        goldfinger_fail "generated MBR partition data has the wrong size"

    echo -e "${INFO} Writing the validated Goldfinger partition entries."
    dd if="${mbr_patch}" of="${DEV_EMMC}" bs=1 seek=446 count=66 conv=notrunc,fsync status=none
    cmp -s -n 66 -i 0:446 "${mbr_patch}" "${DEV_EMMC}" ||
        goldfinger_fail "partition-table read-back verification failed"
    blockdev --rereadpt "${DEV_EMMC}" || goldfinger_fail "kernel could not reread the partition table"
    udevadm settle

    [[ -b "${DEV_EMMC}p1" && -b "${DEV_EMMC}p2" ]] ||
        goldfinger_fail "expected eMMC partitions did not appear"
    [[ "$(cat "/sys/class/block/${device_name}p1/start")" == "270336" ]] ||
        goldfinger_fail "BOOT_EMMC starts at the wrong sector"
    [[ "$(cat "/sys/class/block/${device_name}p1/size")" == "1046528" ]] ||
        goldfinger_fail "BOOT_EMMC has the wrong size"
    [[ "$(cat "/sys/class/block/${device_name}p2/start")" == "1318912" ]] ||
        goldfinger_fail "ROOTFS_EMMC starts at the wrong sector"

    goldfinger_compare_protected
    AMPART_STATUS="no"
}

goldfinger_restore_environment() {
    echo -e "${INFO} Restoring the original U-Boot environment."
    dd if="${GF_RECOVERY_DIR}/environment-116-124MiB.img" of="${DEV_EMMC}" \
        bs=1M seek=116 count=8 conv=notrunc,fsync status=none || return 1
    cmp -s "${GF_RECOVERY_DIR}/environment-116-124MiB.img" \
        <(dd if="${DEV_EMMC}" bs=1M skip=116 count=8 status=none)
}

goldfinger_verify_install() {
    local verify_mount="${DIR_INSTALL}"

    fsck.vfat -n "${DEV_EMMC}p1" >/dev/null || goldfinger_fail "BOOT_EMMC filesystem check failed"
    e2fsck -fn "${DEV_EMMC}p2" >/dev/null || goldfinger_fail "ROOTFS_EMMC filesystem check failed"

    mount -o ro "${DEV_EMMC}p1" "${verify_mount}" || goldfinger_fail "could not verify BOOT_EMMC"
    cmp -s /boot/zImage "${verify_mount}/zImage" || { umount "${verify_mount}"; goldfinger_fail "kernel copy differs"; }
    cmp -s /boot/uInitrd "${verify_mount}/uInitrd" || { umount "${verify_mount}"; goldfinger_fail "initrd copy differs"; }
    cmp -s /boot/dtb/amlogic/meson-sm1-goldfinger-v14-r6.dtb \
        "${verify_mount}/dtb/amlogic/meson-sm1-goldfinger-v14-r6.dtb" || {
        umount "${verify_mount}"
        goldfinger_fail "Goldfinger DTB copy differs"
    }
    [[ -s "${verify_mount}/emmc_autoscript" ]] || { umount "${verify_mount}"; goldfinger_fail "eMMC boot script is missing"; }
    grep -q '^FDT=/dtb/amlogic/meson-sm1-goldfinger-v14-r6.dtb$' "${verify_mount}/uEnv.txt" || {
        umount "${verify_mount}"
        goldfinger_fail "uEnv.txt does not select the Goldfinger DTB"
    }
    umount "${verify_mount}" || goldfinger_fail "could not unmount verified BOOT_EMMC"

    mount -o ro "${DEV_EMMC}p2" "${verify_mount}" || goldfinger_fail "could not verify ROOTFS_EMMC"
    grep -q "UUID=${ROOTFS_UUID}.* / .*ext4" "${verify_mount}/etc/fstab" || {
        umount "${verify_mount}"
        goldfinger_fail "installed fstab does not contain the new root UUID"
    }
    [[ -x "${verify_mount}/usr/sbin/armbian-install" ]] || {
        umount "${verify_mount}"
        goldfinger_fail "installed root filesystem is incomplete"
    }
    umount "${verify_mount}" || goldfinger_fail "could not unmount verified ROOTFS_EMMC"

    goldfinger_compare_protected
}

board_activate_boot() {
    local answer current expected update_file

    goldfinger_verify_install

    cat <<'EOF'

Armbian has been installed and verified on eMMC.

The remaining step changes the factory U-Boot startup instructions so the
device boots Armbian. The factory bootloader itself will not be replaced.

This boot method is proven on the reference hardware, but this unit's first
unattended eMMC boot cannot occur until the saved environment is changed.
If booting fails, recovery requires this installation USB and UART access.
EOF
    read -r -p "Activate Armbian eMMC boot now? (y/n): " answer
    [[ "${answer}" == "y" || "${answer}" == "Y" ]] || goldfinger_fail "boot activation declined; eMMC files remain installed"
    read -r -p "Confirm activation again (y/n): " answer
    [[ "${answer}" == "y" || "${answer}" == "Y" ]] || goldfinger_fail "boot activation declined; eMMC files remain installed"

    update_file="/run/goldfinger-env-update.txt"
    cat >"${update_file}" <<'EOF'
bootcmd=run start_autoscript; run storeboot
start_autoscript=if mmcinfo; then run start_mmc_autoscript; fi; if usb start; then run start_usb_autoscript; fi; run start_emmc_autoscript
start_emmc_autoscript=if fatload mmc 1 1020000 emmc_autoscript; then autoscr 1020000; fi;
start_mmc_autoscript=if fatload mmc 0 1020000 s905_autoscript; then autoscr 1020000; fi;
start_usb_autoscript=for usbdev in 0 1 2 3; do if fatload usb ${usbdev} 1020000 s905_autoscript; then autoscr 1020000; fi; done
upgrade_step=2
EOF

    echo -e "${INFO} Activating the verified Armbian eMMC boot path."
    if ! fw_setenv -c "${GF_ENV_CONFIG}" -s "${update_file}"; then
        goldfinger_restore_environment || goldfinger_fail "activation failed and automatic environment rollback also failed"
        goldfinger_fail "activation failed; the original environment was restored"
    fi

    expected="run start_autoscript; run storeboot"
    current="$(fw_printenv -c "${GF_ENV_CONFIG}" -n bootcmd 2>/dev/null || true)"
    if [[ "${current}" != "${expected}" ]]; then
        goldfinger_restore_environment || goldfinger_fail "verification failed and automatic environment rollback also failed"
        goldfinger_fail "activation verification failed; the original environment was restored"
    fi
    expected="if fatload mmc 1 1020000 emmc_autoscript; then autoscr 1020000; fi;"
    current="$(fw_printenv -c "${GF_ENV_CONFIG}" -n start_emmc_autoscript 2>/dev/null || true)"
    if [[ "${current}" != "${expected}" ]]; then
        goldfinger_restore_environment || goldfinger_fail "verification failed and automatic environment rollback also failed"
        goldfinger_fail "activation verification failed; the original environment was restored"
    fi

    dd if="${DEV_EMMC}" bs=1M skip=116 count=8 status=none |
        sha256sum >"${GF_RECOVERY_DIR}/environment-after-activation.sha256"
    chmod 0600 "${GF_RECOVERY_DIR}/environment-after-activation.sha256"
    sync
    echo -e "${SUCCESS} U-Boot environment activation was read back and verified."
    echo -e "${INFO} Boot-chain recovery data remains on the installation USB: [ ${GF_RECOVERY_DIR} ]"
}
