#!/bin/bash
# fnOS kernel boot repair script
# automatic startup, version validation, integrity repair, self-disable, and safe reboot

set -euo pipefail

KERNEL_VERSION="6.18.18.c821-trim"
BOOT_DIR="/boot"
MODULES_DIR="/lib/modules/${KERNEL_VERSION}"
SCRIPT_PATH="/usr/local/fix/fix-kernel-boot.sh"
SERVICE_NAME="fix-kernel-boot.service"
SERVICE_PATH="/etc/systemd/system/${SERVICE_NAME}"
SERVICE_LINK="/etc/systemd/system/multi-user.target.wants/${SERVICE_NAME}"
LOG_FILE="/var/log/fix-kernel-boot.log"
MARKER_FILE="/usr/local/fix/.fix-kernel-boot.completed"
RC_LOCAL="/etc/rc.local"
AUTO_MODE=false

REQUIRED_EXISTING_FILES=(
    "vmlinuz-${KERNEL_VERSION}"
    "config-${KERNEL_VERSION}"
    "System.map-${KERNEL_VERSION}"
)
AUTO_GENERATED_FILES=(
    "initrd.img-${KERNEL_VERSION}"
    "uInitrd-${KERNEL_VERSION}"
)
BOOT_FILES=("${REQUIRED_EXISTING_FILES[@]}" "${AUTO_GENERATED_FILES[@]}")

log() {
    local level="$1"
    local msg="$2"
    local prefix=""
    case "${level}" in
        INFO) prefix="[INFO]" ;;
        WARN) prefix="[WARN]" ;;
        ERROR) prefix="[ERROR]" ;;
        *) prefix="[INFO]" ;;
    esac
    echo "${prefix} ${msg}"
    printf '%s %s %s\n' "$(date '+%F %T')" "${prefix}" "${msg}" >> "${LOG_FILE}" 2>/dev/null || true
}

log_info() { log INFO "$1"; }
log_warn() { log WARN "$1"; }
log_error() { log ERROR "$1"; }

show_step() {
    log_info "STEP: $1"
}

safe_exit() {
    log_info "Safe exit: $1"
    exit 0
}

abort() {
    log_error "$1"
    exit 1
}

ensure_log() {
    mkdir -p "$(dirname "${LOG_FILE}")" >/dev/null 2>&1 || true
    touch "${LOG_FILE}" >/dev/null 2>&1 || true
    chmod 644 "${LOG_FILE}" >/dev/null 2>&1 || true
}

ensure_executable() {
    if [ -f "${SCRIPT_PATH}" ] && [ ! -x "${SCRIPT_PATH}" ]; then
        chmod +x "${SCRIPT_PATH}" >/dev/null 2>&1 || log_warn "Unable to set execute permission on the script"
    fi
}

parse_args() {
    while [ "$#" -gt 0 ]; do
        case "$1" in
            --auto)
                AUTO_MODE=true
                shift
                ;;
            --help|-h)
                echo "Usage: ${0} [--auto]"
                echo "  --auto    : Use when the script is auto-started by systemd; only then will self-disable and automatic reboot occur."
                exit 0
                ;;
            *)
                shift
                ;;
        esac
    done
}

check_root() {
    show_step "Checking root privileges"
    if [ "${EUID:-0}" -ne 0 ]; then
        abort "Please run this script as root."
    fi
}

already_completed() {
    if [ -f "${MARKER_FILE}" ]; then
        log_info "Repair marker exists; script already completed once, skipping execution."
        return 0
    fi
    return 1
}

detect_kernel_files() {
    local found=true
    if [ ! -f "${BOOT_DIR}/vmlinuz-${KERNEL_VERSION}" ]; then
        found=false
    fi
    if [ "${found}" = false ]; then
        log_info "Target kernel file not found: ${BOOT_DIR}/vmlinuz-${KERNEL_VERSION}. No repair needed."
        return 1
    fi
    show_step "Detected target kernel version: ${KERNEL_VERSION}"
    log_info "Proceeding to verification."
    return 0
}

check_required_files() {
    show_step "Validating required boot files"
    local missing=false
    for file in "${REQUIRED_EXISTING_FILES[@]}"; do
        if [ ! -s "${BOOT_DIR}/${file}" ]; then
            log_warn "Required file missing or empty: ${file}"
            missing=true
        else
            log_info "Required file exists: ${file}"
        fi
    done

    if [ "${missing}" = true ]; then
        log_warn "Current kernel version does not meet repair conditions; skipping repair."
        return 1
    fi

    if [ ! -d "${MODULES_DIR}" ] || [ -z "$(ls -A "${MODULES_DIR}" 2>/dev/null || true)" ]; then
        log_warn "Kernel module directory missing or empty: ${MODULES_DIR}. Repair cannot proceed."
        return 1
    fi

    log_info "Required kernel files passed basic checks."
    return 0
}

ensure_initramfs_enabled() {
    show_step "Ensuring initramfs generation is enabled"
    local conf_file="/etc/initramfs-tools/update-initramfs.conf"
    if [ -f "${conf_file}" ]; then
        if grep -Eq '^update_initramfs=no' "${conf_file}"; then
            log_info "Enabling initramfs generation configuration."
            sed -i 's/^update_initramfs=no/update_initramfs=yes/' "${conf_file}" >/dev/null 2>&1 || true
        fi
    fi
}

generate_initrd() {
    show_step "Generating initrd image"
    if [ -s "${BOOT_DIR}/initrd.img-${KERNEL_VERSION}" ]; then
        log_info "initrd file already exists: initrd.img-${KERNEL_VERSION}"
        return 0
    fi

    log_info "Generating initrd: initrd.img-${KERNEL_VERSION}"
    ensure_initramfs_enabled

    if command -v update-initramfs >/dev/null 2>&1; then
        update-initramfs -c -k "${KERNEL_VERSION}" >/dev/null 2>&1
    elif command -v mkinitramfs >/dev/null 2>&1; then
        mkinitramfs -o "${BOOT_DIR}/initrd.img-${KERNEL_VERSION}" "${KERNEL_VERSION}" >/dev/null 2>&1
    else
        abort "Could not find update-initramfs or mkinitramfs; cannot generate initrd."
    fi

    if [ ! -s "${BOOT_DIR}/initrd.img-${KERNEL_VERSION}" ]; then
        abort "initrd generation failed: ${BOOT_DIR}/initrd.img-${KERNEL_VERSION}"
    fi
    log_info "initrd generated successfully."
}

generate_uinitrd() {
    show_step "Generating uInitrd image"
    if [ -s "${BOOT_DIR}/uInitrd-${KERNEL_VERSION}" ]; then
        log_info "uInitrd file already exists: uInitrd-${KERNEL_VERSION}"
        return 0
    fi

    log_info "Generating uInitrd: uInitrd-${KERNEL_VERSION}"
    if [ ! -s "${BOOT_DIR}/initrd.img-${KERNEL_VERSION}" ]; then
        abort "Missing initrd file; cannot generate uInitrd."
    fi

    if ! command -v mkimage >/dev/null 2>&1; then
        abort "Could not find mkimage; please install u-boot-tools and retry."
    fi

    mkimage -A arm64 -O linux -T ramdisk -C gzip \
        -d "${BOOT_DIR}/initrd.img-${KERNEL_VERSION}" \
        "${BOOT_DIR}/uInitrd-${KERNEL_VERSION}" >/dev/null 2>&1

    if [ ! -s "${BOOT_DIR}/uInitrd-${KERNEL_VERSION}" ]; then
        abort "uInitrd generation failed: ${BOOT_DIR}/uInitrd-${KERNEL_VERSION}"
    fi
    log_info "uInitrd generated successfully."
}

update_boot_links() {
    show_step "Updating U-Boot boot symlinks"
    log_info "Updating U-Boot boot symlinks."
    ln -sf "vmlinuz-${KERNEL_VERSION}" "${BOOT_DIR}/vmlinuz"
    ln -sf "vmlinuz-${KERNEL_VERSION}" "${BOOT_DIR}/Image"
    ln -sf "uInitrd-${KERNEL_VERSION}" "${BOOT_DIR}/uInitrd"
    log_info "Symlink update complete."
}

verify_integrity() {
    show_step "Verifying boot file integrity"
    local all_ok=true
    for file in "${BOOT_FILES[@]}"; do
        if [ -s "${BOOT_DIR}/${file}" ]; then
            log_info "Verified: ${file}"
        else
            log_warn "Verification failed: ${file} is missing or empty"
            all_ok=false
        fi
    done

    if [ ! -L "${BOOT_DIR}/vmlinuz" ] || [ "$(readlink -f "${BOOT_DIR}/vmlinuz")" != "${BOOT_DIR}/vmlinuz-${KERNEL_VERSION}" ]; then
        log_warn "vmlinuz symlink is incorrect or missing."
        all_ok=false
    fi
    if [ ! -L "${BOOT_DIR}/Image" ] || [ "$(readlink -f "${BOOT_DIR}/Image")" != "${BOOT_DIR}/vmlinuz-${KERNEL_VERSION}" ]; then
        log_warn "Image symlink is incorrect or missing."
        all_ok=false
    fi
    if [ ! -L "${BOOT_DIR}/uInitrd" ] || [ "$(readlink -f "${BOOT_DIR}/uInitrd")" != "${BOOT_DIR}/uInitrd-${KERNEL_VERSION}" ]; then
        log_warn "uInitrd symlink is incorrect or missing."
        all_ok=false
    fi

    if [ "${all_ok}" = true ]; then
        log_info "Boot file integrity verification passed."
        return 0
    fi

    log_warn "Boot file integrity verification failed."
    return 1
}

disable_autostart() {
    show_step "Disabling autostart"
    log_info "Disabling script autostart."
    if [ -f "${SERVICE_LINK}" ]; then
        rm -f "${SERVICE_LINK}" >/dev/null 2>&1
        log_info "Removed systemd enable link."
    fi
    if command -v systemctl >/dev/null 2>&1; then
        systemctl disable "${SERVICE_NAME}" >/dev/null 2>&1 || true
    fi
    if [ -f "${RC_LOCAL}" ]; then
        if grep -Fq "${SCRIPT_PATH}" "${RC_LOCAL}" >/dev/null 2>&1; then
            sed -i.bak "s|^\(.*${SCRIPT_PATH}.*\)$|# \1|" "${RC_LOCAL}" >/dev/null 2>&1 || true
            log_info "Commented out the script autostart line in rc.local."
        fi
    fi
    touch "${MARKER_FILE}" >/dev/null 2>&1 || true
    chmod 644 "${MARKER_FILE}" >/dev/null 2>&1 || true
}

schedule_reboot() {
    show_step "Scheduling reboot"
    log_info "Repair complete, preparing to reboot the system for changes to take effect."
    log_info "Reboot will occur in 10 seconds; press Ctrl+C to interrupt if running interactively."
    sleep 10
    if command -v systemctl >/dev/null 2>&1; then
        systemctl reboot -i
    else
        reboot
    fi
}

check_already_valid() {
    if verify_integrity >/dev/null 2>&1; then
        log_info "Current kernel files are already complete; no additional repair performed."
        return 0
    fi
    return 1
}

main() {
    parse_args "$@"
    ensure_log
    check_root
    ensure_executable

    if already_completed; then
        exit 0
    fi

    if ! detect_kernel_files; then
        safe_exit "Target kernel is missing, skipping repair."
    fi

    if ! check_required_files; then
        safe_exit "Repair conditions not met; system startup is unaffected."
    fi

    if check_already_valid; then
        if [ "${AUTO_MODE}" = true ]; then
            disable_autostart
            safe_exit "Repair conditions already met; autostart has been disabled."
        fi
        safe_exit "Repair conditions already met; manual execution will not self-disable."
    fi

    generate_initrd
    generate_uinitrd
    update_boot_links

    if verify_integrity; then
        if [ "${AUTO_MODE}" = true ]; then
            disable_autostart
            schedule_reboot
            exit 0
        fi
        safe_exit "Repair complete; manual execution will not auto-disable or reboot."
    fi

    abort "Integrity verification failed after repair; please inspect /boot manually."
}

main "$@"
