echo "Armbian: W152D native eMMC boot"
# Vendor MMC partition 2 is the original cache range.
if fatload mmc 1:2 0x030c0000 uEnv.txt && env import -t 0x030c0000 ${filesize} LINUX INITRD FDT APPEND; then
    if fatload mmc 1:2 0x08600000 ${LINUX} && itest ${filesize} -le 0x03200000; then
        if fatload mmc 1:2 0x0b800000 ${INITRD} && itest ${filesize} -le 0x01800000; then
            if fatload mmc 1:2 0x0d000000 ${FDT} && fdt addr 0x0d000000 && fdt resize 0x10000; then
                gpio clear GPIOAO_4
                gpio clear GPIOAO_5
                gpio clear GPIOAO_9
                gpio clear GPIOAO_7
                powerset 8 0
                if itest.l *0xfe00400c == 0; then
                    mw.l 0xfe00400c 0x00111111 1
                fi
                if itest.l *0xfe00400c == 0x00111111; then
                    gpio set GPIOX_6
                    setenv armbian_old_bootargs "${bootargs}"
                    setenv armbian_old_fdt_high "${fdt_high}"
                    setenv armbian_old_initrd_high "${initrd_high}"
                    setenv fdt_high 0xffffffffffffffff
                    setenv initrd_high 0xffffffffffffffff
                    setenv bootargs "${APPEND}"
                    booti 0x08600000 0x0b800000 0x0d000000
                    setenv bootargs "${armbian_old_bootargs}"
                    setenv fdt_high "${armbian_old_fdt_high}"
                    setenv initrd_high "${armbian_old_initrd_high}"
                    setenv armbian_old_bootargs
                    setenv armbian_old_fdt_high
                    setenv armbian_old_initrd_high
                fi
            fi
        fi
    fi
fi
echo "Armbian: W152D eMMC boot returned to the vendor bootloader"
