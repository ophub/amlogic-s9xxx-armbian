# Goldfinger V14 eMMC boot using the preserved factory U-Boot.
setenv dtb_addr 0x01000000
setenv env_addr 0x01040000
setenv kernel_addr 0x11000000
setenv initrd_addr 0x15000000

if fatload mmc 1:1 ${env_addr} uEnv.txt; then
  env import -t ${env_addr} ${filesize}
  setenv bootargs ${APPEND}
  if fatload mmc 1:1 ${kernel_addr} ${LINUX}; then
    if fatload mmc 1:1 ${initrd_addr} ${INITRD}; then
      if fatload mmc 1:1 ${dtb_addr} ${FDT}; then
        booti ${kernel_addr} ${initrd_addr} ${dtb_addr}
      fi
    fi
  fi
fi

echo Goldfinger eMMC boot failed; returning to factory U-Boot
