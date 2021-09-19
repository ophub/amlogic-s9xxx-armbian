#!/bin/bash
#========================================================================================================================
# Description: Automatically Build the kernel for OpenWrt
# Copyright (C) 2021 https://github.com/ophub/build-armbian
#========================================================================================================================

#===== Do not modify the following parameter settings, Start =====
make_path=${PWD}
tmp_path=${make_path}/tmp_kernel
armbian_tmp=${tmp_path}/armbian_tmp
kernel_tmp=${tmp_path}/kernel_tmp
dtb_tmp=${tmp_path}/dtb_tmp
modules_tmp=${tmp_path}/modules_tmp
armbian_outputpath=${make_path}/build/output/images
armbian_dtbpath=https://github.com/ophub/amlogic-s9xxx-openwrt/trunk/amlogic-s9xxx/amlogic-dtb
#===== Do not modify the following parameter settings, End =======

die() {
    echo -e " [ \033[1;31m Error \033[0m ] ${1}"
    exit 1
}

echo "Start build kernel for amlogic-s9xxx-openwrt ..."

    mkdir -p ${armbian_tmp} ${kernel_tmp} ${dtb_tmp} ${modules_tmp}

cd  ${tmp_path}
    echo "copy armbian to tmp folder ..."
    cp ${armbian_outputpath}/*.img . && sync
    armbian_old=$( ls *.img 2>/dev/null | head -n 1 )
    echo "armbian: ${armbian_old}"

    echo "mount armbian ..."
    loop_old=$(losetup -P -f --show "${armbian_old}")
    [ ${loop_old} ] || die "losetup ${armbian_old} failed."

    if ! mount ${loop_old}p1 ${armbian_tmp}; then
        die "mount ${loop_old}p1 failed!"
    fi
    sync && sleep 3

    echo "Copy modules files"
    cp -rf ${armbian_tmp}/lib/modules/* ${modules_tmp} && sync
    if [ $( ls ${modules_tmp}/ -l 2>/dev/null | grep "^d" | wc -l ) -eq 0 ]; then
       die "The modules files is Missing!"
    fi
    armbian_version=$(ls ${modules_tmp}/)
    echo "armbian_version: ${armbian_version}"
    kernel_version=$(echo ${armbian_version} | grep -oE '[1-9].[0-9]{1,2}.[0-9]+')
    mkdir -p ${kernel_version}
    echo "kernel_version: ${kernel_version}"

    echo "Copy five boo kernel files"
    cp -f ${armbian_tmp}/boot/{config-*,initrd.img-*,System.map-*,uInitrd-*,vmlinuz-*} ${kernel_tmp} && sync
    if [ ! -f ${kernel_tmp}/config-* -o ! -f ${kernel_tmp}/initrd.img-* -o ! -f ${kernel_tmp}/System.map-* -o ! -f ${kernel_tmp}/uInitrd-* -o ! -f ${kernel_tmp}/vmlinuz-* ]; then
       die "The five boot kernel files is Missing!"
    fi

    echo "supplement .dtb file from github.com ..."
    svn checkout ${armbian_dtbpath} ${dtb_tmp} >/dev/null 2>&1
    echo "Copy .dtb files"
    cp -f ${armbian_tmp}/boot/dtb-*/amlogic/*.dtb ${dtb_tmp} && sync
    sync

cd  ${modules_tmp}
    echo "Package modules-${armbian_version}.tar.gz"
    tar -czf modules-${armbian_version}.tar.gz *
    mv -f modules-${armbian_version}.tar.gz ../${kernel_version} && sync

cd  ${dtb_tmp}
    echo "Package dtb-amlogic-${armbian_version}.tar.gz"
    rm -rf $(find . -type d) 2>/dev/null && sync
    rm -rf .svn && sync
    tar -czf dtb-amlogic-${armbian_version}.tar.gz *
    mv -f dtb-amlogic-${armbian_version}.tar.gz ../${kernel_version} && sync

cd  ${kernel_tmp}
    echo "Package boot-${armbian_version}.tar.gz"
    tar -czf boot-${armbian_version}.tar.gz *
    mv -f boot-${armbian_version}.tar.gz ../${kernel_version} && sync

cd  ${tmp_path}
    echo "umount old armbian ..."
    umount -f ${armbian_tmp} 2>/dev/null
    losetup -d ${loop_old} 2>/dev/null
    echo "mv ${kernel_version} folder to ${armbian_outputpath}"
    mv -f ${kernel_version} ${armbian_outputpath}/ && sync

cd ${armbian_outputpath}
    echo "kernel save path: ${armbian_outputpath}/${kernel_version}.tar.gz"
    tar -czf amlogic-s9xxx-openwrt-kernel-${kernel_version}.tar.gz ${kernel_version} && sync
    rm -rf ${kernel_version} && sync

cd  ${make_path}
    echo "delete tmp folders ..."
    rm -rf ${tmp_path} && sync

echo "build kernel complete ..."

