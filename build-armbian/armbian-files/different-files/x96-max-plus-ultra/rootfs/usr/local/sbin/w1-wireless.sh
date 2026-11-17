#!/bin/dash
MODDIR=$(dirname "$(ls /lib/modules/*/aml_sdio.ko 2>/dev/null | head -1)")
modprobe cfg80211 2>/dev/null
modprobe mac80211 2>/dev/null
lsmod | grep -q "^aml_sdio" || insmod "$MODDIR/aml_sdio.ko"
sleep 3
lsmod | grep -q "^vlsicomm" || insmod "$MODDIR/vlsicomm.ko"
sleep 2
lsmod | grep -q "^sdio_bt" || insmod "$MODDIR/sdio_bt.ko"
i=0
while [ ! -e /dev/stpbt ] && [ $i -lt 30 ]; do sleep 1; i=$((i+1)); done
exit 0
