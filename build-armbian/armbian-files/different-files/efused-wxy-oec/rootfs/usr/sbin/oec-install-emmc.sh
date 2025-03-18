#!/bin/bash
#================================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian and rewrite for being used in OEC.
# Original install script comes from https://github.com/ophub/amlogic-s9xxx-armbian
#
# Function: Install OEC armbian to eMMC
#
# Command: oec-install-emmc
#
#
#======================================== Functions list ========================================
#
# error_msg          : Output error message
# check_depends      : Check dependencies
# init_var           : Initialize all variables
# set_rootfs_type    : Set the type of file system
# repartition        : Repartition using sgdisk
# copy_bootfs        : Copy bootfs partition files
# copy_rootfs        : Copy rootfs partition files
#
#==================================== Set default parameters ====================================
#
# For e-fused Onething OEC Box, We start from the partition p6 and partitions before p6 must be remained.
PARTITION_BEGUN=6

# Set the installation file preprocessing directory
tmp_path="/ddbr"

# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
OPTIONS="[\033[93m OPTIONS \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#================================================================================================

# Encountered a serious error, abort the script execution
error_msg() {
    echo -e "${ERROR} ${1}"
    exit 1
}

# Check dependencies
check_depends() {
    echo -e "${STEPS} Checking dependencies..."

    is_missing="0"
    necessary_packages=("tar" "hexdump" "mkfs.vfat" "mkfs.ext4" "mkfs.btrfs" "parted" "losetup" "fdisk" "lsblk" "gdisk")
    install_packages=("tar" "bsdextrautils" "dosfstools" "e2fsprogs" "btrfs-progs" "parted" "mount" "fdisk" "util-linux" "gdisk")

    i="1"
    for package in ${necessary_packages[*]}; do
        [[ -n "$(which "${package}" 2>/dev/null)" ]] || is_missing="1"
        let i++
    done

    if [[ "${is_missing}" -eq "1" ]]; then
        echo -e "${INFO} Installing necessary dependencies."
        sudo apt-get update
        sudo apt-get install -y ${install_packages[*]}
        [[ "${?}" -eq "0" ]] || error_msg "Failed to install dependencies, aborting!"
    else
        echo -e "${INFO} Dependency check completed. Proceeding installation..."
    fi
}

# Initialize all variables
init_var() {
    echo -e "${STEPS} Initializing the environment..."

    # If it is followed by [ : ], it means that the option requires a parameter value
    get_all_ver="$(getopt "m:a:l:" "${@}")"

    while [[ -n "${1}" ]]; do
        case "${1}" in
        *)
            error_msg "Invalid option [ ${1} ]!"
            ;;
        esac
        shift
    done

    # Check if current system is booted from eMMC
    root_devname="$(df / | tail -n1 | awk '{print $1}' | awk -F '/' '{print substr($3, 1, length($3)-2)}')"
    if lsblk -l | grep -E "^${root_devname}boot0" >/dev/null; then
        error_msg "Current system is already installed to eMMC!"
    fi

    # Find emmc disk, first find emmc containing boot0 partition
    install_emmc="$(lsblk -l -o NAME | grep -oE '(mmcblk[0-9]?boot0)' | sed "s/boot0//g")"
    # Find emmc disk, find emmc that does not contain the boot0 partition
    [[ -z "${install_emmc}" ]] && install_emmc="$(lsblk -l -o NAME | grep -oE '(mmcblk[0-9]?)' | grep -vE ^${root_devname} | sort -u)"
    # Check if emmc exists
    [[ -z "${install_emmc}" ]] && error_msg "Internal eMMC storage was't found in this device!"
    # Location of emmc
    DEV_EMMC="/dev/${install_emmc}"
    echo -e "${INFO} Internal eMMC : [ ${DEV_EMMC} ]"

    # Create a file preprocessing directory
    DIR_INSTALL="${tmp_path}/install"
    [[ -d "${DIR_INSTALL}" ]] && rm -rf ${DIR_INSTALL}
    mkdir -p ${DIR_INSTALL} && chmod 777 ${tmp_path}

    # Regenerate new machine-id
    rm -f /etc/machine-id /var/lib/dbus/machine-id
    dbus-uuidgen --ensure=/etc/machine-id
    dbus-uuidgen --ensure

# Use the same UUID as the Image do.
	ROOT_DEVICE=$(findmnt -n -o SOURCE /)
	ROOTFS_UUID=$(blkid -s UUID -o value "$ROOT_DEVICE")
	BOOTSIZE="512"


# Set the type of file system
# Not recommend to use other file system types since the SoC is efused.
set_rootfs_type() {
        file_system_type="ext4"
        uenv_mount_string="UUID=${ROOTFS_UUID} rootflags=data=writeback rw rootwait rootfstype=ext4"
        fstab_mount_string="defaults,noatime,nodiratime,commit=600,errors=remount-ro"
    fi
}

# We use sgdisk to partition disk instead of ampart_tool in rockchip platform. :D
repartition() {
# Check if there exists partition starts with the amount in eMMC.

	PARTITION_COUNT=$(sgdisk -p "$DEV_EMMC" | grep -oP '^\s*\K[0-9]+(?=\s+)' | tail -n 1)
	if [ "$PARTITION_COUNT" -ge $PARTITION_BEGUN ]; then
		echo "Checking if partition starts with $PARTITION_BEGUN exist in mmc device..."
# Delete partitions with sgdisk.
		for ((i=${PARTITION_BEGUN}; i<=PARTITION_COUNT; i++)); do
		echo "Deleting partition $i..."
		sgdisk -d "$i" "$DEV_EMMC"
		[[ "${?}" -eq "0" ]] || error_msg "Failed to erase partition."
	else
		echo "No existing partitions after $PARTITION_BEGUN , contiuning..."
    fi

    echo "Creating partitions ..."
	LAST_PARTITION_NUM=$(sgdisk -p $DEV_EMMC | grep -oP '^\s*\K\d+(?=\s+)' | tail -n 1)
	LAST_PARTITION_END=$(sgdisk -i $LAST_PARTITION_NUM $DEV_EMMC | grep "Last sector" | awk '{print $3}')
	# Calculating the sector of the last partition.
	BOOT_PARTITION_NUM=$((LAST_PARTITION_NUM + 1))
	BOOT_PARTITION_START=$((LAST_PARTITION_END + 1))
	#-n x:y:z Create a new partition which num is x，begin with y，end with z .
    sgdisk -n ${BOOT_PARTITION_NUM}:${BOOT_PARTITION_START}:+${BOOTSIZE}M $DEV_EMMC
    [[ "${?}" -eq "0" ]] || error_msg "Failed to create boot partition."
	BOOT_PARTITION_END=$(sgdisk -i $PARTITION_BEGUN $DEV_EMMC | grep "Last sector" | awk '{print $3}')
	ROOTFS_PARTITION_NUM=$((PARTITION_BEGUN + 1))
	ROOTFS_PARTITION_START=$((BOOT_PARTITION_END + 1))
	sgdisk -n ${ROOTFS_PARTITION_NUM}:${ROOTFS_PARTITION_START}:0 $DEV_EMMC
	[[ "${?}" -eq "0" ]] || error_msg "Failed to create rootfs partition."
  done
}

# Copy bootfs partition files
copy_bootfs() {
    cd /
    echo -e "${STEPS} Processing BOOTFS partition..."
# For example, OEC'S DEFAULT PARTITION IS 6.
    PART_BOOT="${DEV_EMMC}p${ROOTFS_PARTITION_NUM}"

    if grep -q ${PART_BOOT} /proc/mounts; then
        echo -e "${INFO} Unmounting BOOT partiton."
        umount -f ${PART_BOOT}
        [[ "${?}" -eq "0" ]] || error_msg "Failed to umount [ ${PART_BOOT} ]."
    fi

    echo -e "${INFO} Formatting BOOTFS partition..."
    mkfs.vfat -F 32 -n "BOOT_EMMC" ${PART_BOOT}

    mount -o rw ${PART_BOOT} ${DIR_INSTALL}
    [[ "${?}" -eq "0" ]] || error_msg "Failed to mount BOOTFS partition."

    echo -e "${INFO} Copying BOOTFS ..."
    cp -rf /boot/* ${DIR_INSTALL}
    # Remove useless files
    rm -rf ${DIR_INSTALL}/'System Volume Information'

    sync && sleep 3
    umount -f ${DIR_INSTALL}
    [[ "${?}" -eq "0" ]] || error_msg "Failed to umount [ ${DIR_INSTALL} ]."
    # Update [ /boot/extlinux/extlinux.conf ] settings
    boot_extlinux_file="${DIR_INSTALL}/extlinux/extlinux.conf"
    echo -e "${INFO} Update the [ extlinux.conf ] file."
    BOOT_CONF="extlinux.conf"
    sed -i "s|root=.*console=ttyS2,1500000|root=${uenv_mount_string}|g" ${boot_extlinux_file}
}

# Copy rootfs partition files
copy_rootfs() {
    cd /
    echo -e "${STEPS} Start processing the rootfs partition..."
# For example, OEC's rootfs partition is p7.
    PART_ROOT="${DEV_EMMC}p${ROOTFS_PARTITION_NUM}"

    if grep -q ${PART_ROOT} /proc/mounts; then
        echo -e "${INFO} Unmounting ROOT partiton."
        umount -f ${PART_ROOT}
        [[ "${?}" -eq "0" ]] || error_msg "Failed to umount [ ${PART_ROOT} ]."
    fi

    echo -e "${INFO} Formatting ROOTFS ..."
        mkfs.ext4 -F -q -U ${ROOTFS_UUID} -L "ROOTFS_EMMC" -b 4k -m 0 ${PART_ROOT}
        [[ "${?}" -eq "0" ]] || error_msg "Failed to format ROOTFS with [ mkfs.ext4 ]"

        mount -t ext4 ${PART_ROOT} ${DIR_INSTALL}
        [[ "${?}" -eq "0" ]] || error_msg "Failed to mount ROOTFS partition."

    echo -e "${INFO} Copying ROOTFS ..."
    # Create relevant directories
    mkdir -p ${DIR_INSTALL}/{boot/,dev/,media/,mnt/,proc/,run/,sys/,tmp/}
    chmod 777 ${DIR_INSTALL}/tmp
    # Copy the relevant directory
    COPY_SRC="etc home opt root selinux srv usr var"
    for src in ${COPY_SRC}; do
        echo -e "${INFO} Copying [ ${src} ] ..."
        tar -cf - ${src} | (
            cd ${DIR_INSTALL}
            tar -mxf -
        )
    done
    # Create relevant symbolic link
    ln -sf /usr/bin ${DIR_INSTALL}/bin
    ln -sf /usr/lib ${DIR_INSTALL}/lib
    ln -sf /usr/sbin ${DIR_INSTALL}/sbin

    echo -e "${INFO} Generate the new fstab file."
    rm -f ${DIR_INSTALL}/etc/fstab
    cat >${DIR_INSTALL}/etc/fstab <<EOF
UUID=${ROOTFS_UUID}    /        ${file_system_type}    ${fstab_mount_string}      0 1
LABEL=BOOT_EMMC        /boot    vfat                   defaults                   0 2
tmpfs                  /tmp     tmpfs                  defaults,nosuid            0 0

EOF

    # Update the MAC address for the wireless network card
    echo -e "${INFO} Update the MAC address for the wireless network card."
    find "${DIR_INSTALL}/lib/firmware" \
        -type f -name "*.txt" ! -xtype l \
        -exec grep -q '^macaddr=.*' {} \; \
        -exec sh -c '
        new_mac=$(openssl rand -hex 6 | sed "s/\(..\)/\1:/g; s/.$//; s/^./2/")
        sed -i "s/^macaddr=.*/macaddr=${new_mac}/" "${1}"
    ' _ {} \; 2>/dev/null

    # Remove useless scripts
    rm -f ${DIR_INSTALL}/usr/sbin/armbian-tf
    rm -f ${DIR_INSTALL}/root/.no_rootfs_resize

    sync && sleep 3
    umount -f ${DIR_INSTALL}
    [[ "${?}" -eq "0" ]] || error_msg "Failed to umount [ ${DIR_INSTALL} ]."
}

# Check script permission
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo $0 ]"
echo -e "${STEPS} Installing Armbian to internal eMMC..."

# Check dependencies
check_depends
# Initialize all variables
init_var "${@}"
# Set the type of file system
set_rootfs_type
# Create emmc partition
repartition
# Copy bootfs partition files
copy_bootfs
# Copy rootfs partition files
copy_rootfs

echo -e "${SUCCESS} Installation successful. Run [ poweroff ], remove the installation media then re-insert the power supply to boot new system."
exit 0
