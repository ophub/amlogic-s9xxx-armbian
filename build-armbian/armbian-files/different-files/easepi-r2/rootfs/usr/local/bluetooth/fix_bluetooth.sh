#!/bin/bash
# EasePi-R2 蓝牙初始化脚本
# AP6255 WiFi+BT Combo模块 (BCM43455)
# DTS中uart9已定义bluetooth子节点(compatible="brcm,bcm4345c5")
# 主线内核hci_bcm驱动通过serdev自动初始化，hciattach仅作fallback

echo "[BT] 开始蓝牙初始化..."

# 检查rfkill状态并解除阻塞
rfkill unblock all 2>/dev/null || true

# 等待serdev自动初始化（DTS bluetooth子节点 -> hci_bcm serdev绑定）
if [ ! -d /sys/class/bluetooth/hci0 ]; then
    echo "[BT] 等待serdev自动初始化..."
    for i in $(seq 1 5); do
        sleep 1
        [ -d /sys/class/bluetooth/hci0 ] && break
    done
fi

# Fallback: serdev未自动初始化时，手动hciattach
if [ ! -d /sys/class/bluetooth/hci0 ]; then
    # uart9在RK3588主线内核中对应ttyS9
    for tty_dev in /dev/ttyS9 /dev/ttyS1; do
        if [ -c "$tty_dev" ]; then
            echo "[BT] 尝试通过 $tty_dev 初始化AP6255蓝牙..."
            hciattach "$tty_dev" bcm43xx 1500000 flow bdaddr 2>/dev/null && break
            hciattach "$tty_dev" any 1500000 flow 2>/dev/null && break
        fi
    done
    sleep 1
fi

# 确保蓝牙服务开机自启
systemctl enable bluetooth 2>/dev/null || true

# 启动蓝牙服务
if ! systemctl is-active --quiet bluetooth; then
    echo "[BT] 启动蓝牙服务..."
    systemctl start bluetooth 2>/dev/null || true
fi

# 检查蓝牙适配器是否已上电
if [ -d /sys/class/bluetooth/hci0 ]; then
    hciconfig hci0 up 2>/dev/null || true
    hciconfig hci0 piscan 2>/dev/null || true
    echo "[BT] 蓝牙初始化完成"
else
    echo "[BT] 警告：蓝牙设备未就绪"
    echo "[BT] 请检查: 1) DTS uart9 bluetooth节点 2) hci_bcm驱动 3) /dev/ttyS9"
fi

exit 0
