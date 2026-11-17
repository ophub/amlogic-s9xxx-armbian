#!/bin/dash
# Ensure /dev/ttyS2 is a usable char device for aml_hciattach
i=0
while [ ! -e /dev/ttyS2 ] && [ $i -lt 15 ]; do sleep 1; i=$((i+1)); done
ls -la /dev/ttyS* /dev/serial* > /dev/kmsg 2>&1
if [ ! -c /dev/ttyS2 ]; then
  rm -f /dev/ttyS2
  mknod /dev/ttyS2 c 4 66
  chmod 666 /dev/ttyS2
fi
echo "w1-bt: ttyS2 is $(ls -la /dev/ttyS2 2>&1)" > /dev/kmsg
exit 0
