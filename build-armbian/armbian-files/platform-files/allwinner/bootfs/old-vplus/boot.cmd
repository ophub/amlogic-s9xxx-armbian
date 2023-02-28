echo "start mainline u-boot"
setenv loadaddr "0x44000000"
setenv l_mmc "0 1 2 3"
for devtype in "usb mmc" ; do
	if test "${devtype}" = "mmc"; then
		setenv l_mmc "0"
	fi 
	for devnum in ${l_mmc} ; do
		if test -e ${devtype} ${devnum} uEnv.txt; then
			load ${devtype} ${devnum} ${loadaddr} uEnv.txt
			env import -t ${loadaddr} ${filesize}
			setenv bootargs ${APPEND}
			if printenv mac; then
				setenv bootargs ${bootargs} mac=${mac}
			elif printenv eth_mac; then
				setenv bootargs ${bootargs} mac=${eth_mac}
			elif printenv ethaddr; then
				setenv bootargs ${bootargs} mac=${ethaddr}
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
