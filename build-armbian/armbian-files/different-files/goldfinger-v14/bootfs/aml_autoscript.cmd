# Goldfinger V14 removable-media boot. All changes remain in RAM.
echo Goldfinger USB boot: RAM-only session
usb start

if fatload usb 0:1 0x01000000 uEnv.txt; then
  env import -t 0x01000000 ${filesize}
  if fatload usb 0:1 0x11000000 ${LINUX}; then
    if fatload usb 0:1 0x15000000 ${INITRD}; then
      if fatload usb 0:1 0x01000000 ${FDT}; then
        setenv bootargs ${APPEND}
        booti 0x11000000 0x15000000 0x01000000
      fi
    fi
  fi
fi

echo Goldfinger USB boot failed; returning to U-Boot
