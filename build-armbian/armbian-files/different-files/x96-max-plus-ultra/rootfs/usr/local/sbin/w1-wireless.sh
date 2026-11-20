#!/bin/dash
# Load Amlogic W1 (W155S1) Wi-Fi + BT modules via depmod database
modprobe cfg80211 2>/dev/null
modprobe mac80211 2>/dev/null
modprobe rfkill 2>/dev/null
modprobe aml_sdio 2>/dev/null
sleep 3
modprobe vlsicomm 2>/dev/null
sleep 2
modprobe sdio_bt 2>/dev/null
i=0
while [ ! -e /dev/stpbt ] && [ $i -lt 30 ]; do sleep 1; i=$((i+1)); done
exit 0
