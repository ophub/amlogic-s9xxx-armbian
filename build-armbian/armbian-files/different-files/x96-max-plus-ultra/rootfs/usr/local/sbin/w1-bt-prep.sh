#!/bin/dash
# Wait for BT UART node; create manually if devtmpfs missed it
i=0
while [ ! -e /dev/ttyS2 ] && [ $i -lt 15 ]; do sleep 1; i=$((i+1)); done
if [ ! -e /dev/ttyS2 ]; then
  echo "w1-bt: ttyS2 missing, listing /dev:" > /dev/kmsg
  ls -la /dev/tty* /dev/serial* >> /dev/kmsg 2>&1
  mknod /dev/ttyS2 c 4 66 2>/dev/null
  chmod 666 /dev/ttyS2 2>/dev/null
  echo "w1-bt: mknod ttyS2 -> $(ls -la /dev/ttyS2 2>&1)" > /dev/kmsg
fi
exit 0
