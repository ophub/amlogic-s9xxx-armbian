#!/bin/bash
#=========================================================================
# Description: Build armbian for amlogic s9xxx
# Copyright (C) 2021 https://github.com/ophub/amlogic-s9xxx-armbian
#=========================================================================

#===== Do not modify the following parameter settings, Start =====
make_path=${PWD}
armbian_outputpath=${make_path}/build/output/images
amlogic_path=${make_path}/amlogic-s9xxx
armbian_path=${amlogic_path}/amlogic-armbian
dtb_path=${amlogic_path}/amlogic-dtb
uboot_path=${amlogic_path}/amlogic-u-boot
configfiles_path=${amlogic_path}/common-files
tmp_outpath=${make_path}/tmp_out
tmp_armbian=${make_path}/tmp_armbian
tmp_build=${make_path}/tmp_build
tmp_aml_image=${make_path}/tmp_aml_image
#===== Do not modify the following parameter settings, End =======

build_armbian=("s922x" "s905x3" "s905x2" "s912" "s905d")
if [ -n "${1}" ]; then
    unset build_armbian
    oldIFS=$IFS
    IFS=_
    build_armbian=(${1})
    IFS=$oldIFS
fi

die() {
    echo -e " [\033[1;31m Error \033[0m] ${1}"
    exit 1
}

process() {
    echo -e " [\033[1;32m ${build_soc} \033[0m] ${1}"
}

make_image() {
    cd ${make_path}

        rm -rf ${tmp_armbian} ${tmp_build} ${tmp_outpath} ${tmp_aml_image} 2>/dev/null && sync
        mkdir -p ${tmp_armbian} ${tmp_build} ${tmp_outpath} ${armbian_outputpath} ${tmp_aml_image} && sync

        # Get armbian version and release
        armbian_image_name=$(ls ${armbian_outputpath}/*.img 2>/dev/null | awk -F "/Armbian_" '{print $2}')
        out_release=$(echo ${armbian_image_name} | awk -F "buster" '{print $1}' | grep -oE '[1-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}' 2>/dev/null)
        out_version=$(echo ${armbian_image_name} | awk -F "buster" '{print $NF}' | grep -oE '[1-9].[0-9]{1,3}.[0-9]{1,3}' 2>/dev/null)

        # Make Amlogic s9xxx armbian
        build_image_file="${tmp_outpath}/Armbian_${out_release}_Aml_${build_soc}_buster_${out_version}_$(date +"%Y.%m.%d.%H%M").img"
        rm -f ${build_image_file}
        sync

        SKIP_MB=68
        BOOT_MB=256
        ROOT_MB=2304
        IMG_SIZE=$((SKIP_MB + BOOT_MB + ROOT_MB))

        dd if=/dev/zero of=${build_image_file} bs=1M count=${IMG_SIZE} >/dev/null 2>&1

        parted -s ${build_image_file} mklabel msdos 2>/dev/null
        parted -s ${build_image_file} mkpart primary fat32 $((SKIP_MB))M $((SKIP_MB + BOOT_MB -1))M 2>/dev/null
        parted -s ${build_image_file} mkpart primary ext4 $((SKIP_MB + BOOT_MB))M 100% 2>/dev/null

        loop_new=$(losetup -P -f --show "${build_image_file}")
        [ ${loop_new} ] || die "losetup ${build_image_file} failed."

        mkfs.vfat -n "BOOT" ${loop_new}p1 >/dev/null 2>&1
        mke2fs -F -q -t ext4 -L "ROOTFS" -m 0 ${loop_new}p2 >/dev/null 2>&1
}

extract_armbian() {

    cd ${make_path}

        armbian_image_file="${tmp_aml_image}/armbian-${build_soc}.img"
        rm -f ${armbian_image_file} 2>/dev/null && sync
        cp -f $( ls ${armbian_outputpath}/*.img 2>/dev/null | head -n 1 ) ${tmp_aml_image}/armbian-${build_soc}.img && sync && sleep 3

        loop_old=$(losetup -P -f --show "${armbian_image_file}")
        [ ${loop_old} ] || die "losetup ${armbian_image_file} failed."

        if ! mount ${loop_old}p1 ${tmp_armbian}; then
            die "mount ${loop_old}p1 failed!"
        fi

    cd ${tmp_armbian}

        # Delete soft link (ln -sf TrueFile Link)
        rm -rf lib sbin tmp bin var/sbin

        #Check if writing to EMMC is supported
        MODULES_NOW=$(ls usr/lib/modules/ 2>/dev/null)
        VERSION_NOW=$(echo ${MODULES_NOW} | grep -oE '^[1-9].[0-9]{1,3}' 2>/dev/null)
        #echo -e "This Kernel [ ${VERSION_NOW} ]"

        k510_ver=$(echo "${VERSION_NOW}" | cut -d '.' -f1)
        k510_maj=$(echo "${VERSION_NOW}" | cut -d '.' -f2)
        if  [ "${k510_ver}" -eq "5" ];then
            if  [ "${k510_maj}" -ge "10" ];then
                K510="1"
            else
                K510="0"
            fi
        elif [ "${k510_ver}" -gt "5" ];then
            K510="1"
        else
            K510="0"
        fi
        #echo -e "K510: [ ${K510} ]"

        case "${build_soc}" in
            s905x3 | x96 | hk1 | h96 | ugoosx3)
                FDTFILE="meson-sm1-x96-max-plus-100m.dtb"
                UBOOT_OVERLOAD="u-boot-x96maxplus.bin"
                MAINLINE_UBOOT="/lib/u-boot/x96maxplus-u-boot.bin.sd.bin"
                ANDROID_UBOOT="/lib/u-boot/hk1box-bootloader.img"
                AMLOGIC_SOC="s905x3"
                ;;
            s905x2 | x96max4g | x96max2g)
                FDTFILE="meson-g12a-x96-max.dtb"
                UBOOT_OVERLOAD="u-boot-x96max.bin"
                MAINLINE_UBOOT="/lib/u-boot/x96max-u-boot.bin.sd.bin"
                ANDROID_UBOOT=""
                AMLOGIC_SOC="s905x2"
                ;;
            s905x | hg680p | b860h)
                FDTFILE="meson-gxl-s905x-p212.dtb"
                UBOOT_OVERLOAD="u-boot-p212.bin"
                MAINLINE_UBOOT=""
                ANDROID_UBOOT=""
                AMLOGIC_SOC="s905x"
                ;;
            s905w | x96mini | tx3mini)
                FDTFILE="meson-gxl-s905w-tx3-mini.dtb"
                UBOOT_OVERLOAD="u-boot-s905x-s912.bin"
                MAINLINE_UBOOT=""
                ANDROID_UBOOT=""
                AMLOGIC_SOC="s905w"
                ;;
            s905d | n1)
                FDTFILE="meson-gxl-s905d-phicomm-n1.dtb"
                UBOOT_OVERLOAD="u-boot-n1.bin"
                MAINLINE_UBOOT=""
                ANDROID_UBOOT="/lib/u-boot/u-boot-2015-phicomm-n1.bin"
                AMLOGIC_SOC="s905d"
                ;;
            s912 | h96proplus | octopus)
                FDTFILE="meson-gxm-octopus-planet.dtb"
                UBOOT_OVERLOAD="u-boot-zyxq.bin"
                MAINLINE_UBOOT=""
                ANDROID_UBOOT=""
                AMLOGIC_SOC="s912"
                ;;
            s922x | belink | belinkpro | ugoos)
                FDTFILE="meson-g12b-gtking-pro.dtb"
                UBOOT_OVERLOAD="u-boot-gtkingpro.bin"
                MAINLINE_UBOOT="/lib/u-boot/gtkingpro-u-boot.bin.sd.bin"
                ANDROID_UBOOT=""
                AMLOGIC_SOC="s922x"
                ;;
            *)
                die "Have no this soc: [ ${build_soc} ]"
                ;;
        esac

}

copy_files() {
    cd ${make_path}

        mkdir -p ${tmp_build}/boot
        # Copy the original boot core file
        cp -f ${tmp_armbian}/boot/config-* ${tmp_build}/boot
        cp -f ${tmp_armbian}/boot/initrd.img-* ${tmp_build}/boot
        cp -f ${tmp_armbian}/boot/System.map-* ${tmp_build}/boot
        cp -f ${tmp_armbian}/boot/uInitrd-* ${tmp_build}/boot
        cp -f ${tmp_armbian}/boot/vmlinuz-* ${tmp_build}/boot
        cp -f ${tmp_armbian}/boot/uInitrd-* ${tmp_build}/boot/uInitrd
        cp -f ${tmp_armbian}/boot/vmlinuz-* ${tmp_build}/boot/zImage
        chmod +x ${tmp_build}/boot/*
        # Unzip the relevant files in the boot directory
        tar -xzf "${armbian_path}/boot-common.tar.gz" -C ${tmp_build}/boot
        # Complete the u-boot file to facilitate the booting of the 5.10+ kernel
        cp -f ${uboot_path}/* ${tmp_build}/boot
        # Complete the dtb file
        cp -f ${dtb_path}/* ${tmp_build}/boot/dtb/amlogic
        sync

        tag_bootfs="${tmp_outpath}/bootfs"
        tag_rootfs="${tmp_outpath}/rootfs"

        mkdir -p ${tag_bootfs} ${tag_rootfs}
        sync

        if ! mount ${loop_new}p1 ${tag_bootfs}; then
            die "mount ${loop_new}p1 failed!"
        fi
        if ! mount ${loop_new}p2 ${tag_rootfs}; then
            die "mount ${loop_new}p2 failed!"
        fi

        cp -rf ${tmp_build}/boot/* ${tag_bootfs} && sync
        cp -rf ${tmp_armbian}/* ${tag_rootfs} && sync

        # Complete file for ${root}: [ /etc ], [ /lib/u-boot ] etc.
        if [ "$(ls ${configfiles_path}/files 2>/dev/null | wc -w)" -ne "0" ]; then
            cp -rf ${configfiles_path}/files/* ${tag_rootfs}
        fi
        
    cd ${tag_bootfs}

        # Add u-boot.ext for 5.10 kernel
        if [[ "${K510}" -eq "1" && -n "${UBOOT_OVERLOAD}" ]]; then
           if [ -f "${UBOOT_OVERLOAD}" ]; then
              cp -f ${UBOOT_OVERLOAD} u-boot.ext && sync
              chmod +x u-boot.ext
           else
              die "${build_soc} have no the 5.10 kernel u-boot file: [ ${UBOOT_OVERLOAD} ]"
           fi
        fi

        # Edit the uEnv.txt
        if [  ! -f "uEnv.txt" ]; then
           die "The uEnv.txt File does not exist"
        else
           old_fdt_dtb="meson-gxl-s905d-phicomm-n1.dtb"
           sed -i "s/${old_fdt_dtb}/${FDTFILE}/g" uEnv.txt
        fi

        sync

    cd ${tag_rootfs}

        # Synchronize the data of the boot partition
        rm -rf boot && sync && mkdir boot
        cp -rf ${tag_bootfs}/* boot/ && sync

        # Add soft connection
        ln -sf /usr/lib lib
        ln -sf /usr/sbin sbin
        ln -sf /var/tmp tmp
        ln -sf /usr/bin bin
        ln -sf /usr/share/zoneinfo/Asia/Shanghai etc/localtime
        chmod 777 var/tmp

        # Delete related files
        rm -f etc/apt/sources.list.save 2>/dev/null
        rm -f etc/apt/*.gpg~ 2>/dev/null
        rm -f etc/systemd/system/basic.target.wants/armbian-resize-filesystem.service 2>/dev/null
        rm -f var/lib/dpkg/info/linux-image* 2>/dev/null

        # Add firmware information to the etc/armbian-aml-release
        echo "FDTFILE='${FDTFILE}'" >> etc/armbian-aml-release 2>/dev/null
        echo "U_BOOT_EXT='${K510}'" >> etc/armbian-aml-release 2>/dev/null
        echo "UBOOT_OVERLOAD='${UBOOT_OVERLOAD}'" >> etc/armbian-aml-release 2>/dev/null
        echo "MAINLINE_UBOOT='${MAINLINE_UBOOT}'" >> etc/armbian-aml-release 2>/dev/null
        echo "ANDROID_UBOOT='${ANDROID_UBOOT}'" >> etc/armbian-aml-release 2>/dev/null
        echo "KERNEL_VERSION='${KERNEL_VERSION}'" >> etc/armbian-aml-release 2>/dev/null
        echo "SOC='${AMLOGIC_SOC}'" >> etc/armbian-aml-release 2>/dev/null
        echo "K510='${K510}'" >> etc/armbian-aml-release 2>/dev/null

        sync
}

clean_tmp() {
    cd ${make_path}

        umount -f ${tmp_armbian} 2>/dev/null
        losetup -d ${loop_old} 2>/dev/null

        umount -f ${tag_bootfs} 2>/dev/null
        umount -f ${tag_rootfs} 2>/dev/null
        losetup -d ${loop_new} 2>/dev/null
        sync
        sleep 3

    cd ${tmp_outpath}
        gzip *.img && sync && mv -f *.img.gz ${armbian_outputpath} && sync

    cd ${make_path}
        rm -rf ${tmp_outpath} ${tmp_armbian} ${tmp_build} ${tag_bootfs} ${tag_rootfs} ${tmp_aml_image} 2>/dev/null && sync

}

[ $(id -u) = 0 ] || die "please run this script as root: [ sudo ./make ]"
echo -e "Welcome to build armbian for amlogic s9xxx STB! \n"
echo -e "Server space usage before starting to compile: \n$(df -hT ${PWD}) \n"
echo -e "Ready, start build armbian... \n"

# Start loop compilation
k=1
for b in ${build_armbian[*]}; do
    build_soc=${b}

    echo -n "(${k}) Start build armbian [ ${b} ]. "
    now_remaining_space=$(df -hT ${PWD} | grep '/dev/' | awk '{print $5}' | sed 's/.$//')
    if  [[ "${now_remaining_space}" -le "2" ]]; then
        echo "Remaining space is less than 2G, exit this packaging. \n"
        break
    else
        echo "Remaining space is ${now_remaining_space}G."
    fi

    process " (1/4) make new armbian image."
    make_image
    process " (2/4) extract old armbian files."
    extract_armbian
    process " (3/4) copy files to new image."
    copy_files
    process " (4/4) clear temp files."
    clean_tmp

    echo -e "(${k}) Build successfully. \n"

    let k++

done

echo -e "Server space usage after compilation: \n$(df -hT ${PWD}) \n"

