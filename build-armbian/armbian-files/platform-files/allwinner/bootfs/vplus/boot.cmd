echo "Start running sunxi mainline u-boot"
#setenv loadaddr "0x42000000"
#setenv kernel_addr_r "0x40080000"
#setenv ramdisk_addr_r "0x4FF00000"
#setenv fdt_addr_r "0x4FA00000"
setenv dev_nums "0 1 2 3"
for devtype in "usb mmc" ; do
	for devnum in ${dev_nums} ; do
		if test -e ${devtype} ${devnum} uEnv.txt; then
			load ${devtype} ${devnum} ${loadaddr} uEnv.txt
			env import -t ${loadaddr} ${filesize}
			setenv bootargs ${APPEND}
			if printenv ethaddr; then
				setenv bootargs ${bootargs} mac=${ethaddr}
			elif printenv eth_mac; then
				setenv bootargs ${bootargs} mac=${eth_mac}
			elif printenv mac; then
				setenv bootargs ${bootargs} mac=${mac}
			fi
			if load ${devtype} ${devnum} ${kernel_addr_r} ${LINUX}; then
				if load ${devtype} ${devnum} ${ramdisk_addr_r} ${INITRD}; then
					if load ${devtype} ${devnum} ${fdt_addr_r} ${FDT}; then
						fdt addr ${fdt_addr_r}
						booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}
					fi
				fi
			fi
		fi
	done
done
# Recompile with:
# mkimage -C none -A arm -T script -d boot.cmd boot.scr
