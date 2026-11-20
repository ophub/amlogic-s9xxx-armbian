#!/bin/dash
# W1 BT: aml_hciattach UART_DEV_PORT_BT is hardcoded to /dev/ttyS1,
# but the BT UART is ttyS2 (serial@80000). Provide the alias and wait
# for the SDIO BT channel created by sdio_bt.ko.
i=0
while [ ! -e /dev/stpbt ] && [ $i -lt 30 ]; do sleep 1; i=$((i+1)); done
if [ ! -e /dev/ttyS2 ]; then
  i=0
  while [ ! -e /dev/ttyS2 ] && [ $i -lt 15 ]; do sleep 1; i=$((i+1)); done
fi
ls -la /dev/ttyS* /dev/stpbt > /dev/kmsg 2>&1
ln -sf /dev/ttyS2 /dev/ttyS1
echo "w1-bt: ttyS1 -> $(ls -la /dev/ttyS1 2>&1)" > /dev/kmsg
exit 0
