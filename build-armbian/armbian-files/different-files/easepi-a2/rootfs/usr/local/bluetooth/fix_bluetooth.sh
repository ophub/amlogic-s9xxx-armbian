#!/bin/bash
# EasePi-A2 蓝牙初始化脚本 (v2 - with GPIO reset)
# AP6255 WiFi+BT Combo module (BCM43455)
# BT uses UART8 (ttyS8) with GPIO0_20 (BT_RESET) and GPIO0_34 (BT_WAKE)

echo "[BT] 开始蓝牙初始化..."

# RF kill unblock
rfkill unblock all 2>/dev/null || true

# Load HCI UART driver
modprobe hci_uart 2>/dev/null || true

# GPIO reset sequence for BCM43455
# BT,reset_gpio = GPIO0_20, BT,wake_gpio = GPIO0_34
BT_RESET=20
BT_WAKE=34
echo ${BT_RESET} > /sys/class/gpio/export 2>/dev/null || true
echo ${BT_WAKE} > /sys/class/gpio/export 2>/dev/null || true
sleep 0.1
echo out > /sys/class/gpio/gpio${BT_RESET}/direction 2>/dev/null || true
echo out > /sys/class/gpio/gpio${BT_WAKE}/direction 2>/dev/null || true
# Power on sequence: RESET low -> WAKE low -> RESET high -> WAKE high
echo 0 > /sys/class/gpio/gpio${BT_RESET}/value 2>/dev/null || true
echo 0 > /sys/class/gpio/gpio${BT_WAKE}/value 2>/dev/null || true
sleep 0.05
echo 1 > /sys/class/gpio/gpio${BT_RESET}/value 2>/dev/null || true
sleep 0.1
echo 1 > /sys/class/gpio/gpio${BT_WAKE}/value 2>/dev/null || true
sleep 0.2

# Try hciattach with firmware loading
hciattach /dev/ttyS8 bcm43xx 1500000 flow -t 10 2>/dev/null && {
    sleep 1
    if [ -d /sys/class/bluetooth/hci0 ]; then
        hciconfig hci0 up 2>/dev/null || true
        echo "[BT] 蓝牙初始化完成"
    fi
} || {
    echo "[BT] bcm43xx failed, trying generic..."
    hciattach /dev/ttyS8 any 1500000 flow 2>/dev/null
    sleep 1
    if [ -d /sys/class/bluetooth/hci0 ]; then
        echo "[BT] 蓝牙初始化完成 (generic mode)"
    else
        echo "[BT] 警告：蓝牙设备未就绪"
        echo "[BT] 请检查: 1) GPIO reset timing 2) UART pinmux 3) hci_bcm driver"
    fi
}

# Cleanup GPIO exports
echo ${BT_RESET} > /sys/class/gpio/unexport 2>/dev/null || true
echo ${BT_WAKE} > /sys/class/gpio/unexport 2>/dev/null || true

# Ensure bluetooth service is started
systemctl enable bluetooth 2>/dev/null || true
systemctl start bluetooth 2>/dev/null || true

exit 0