#!/bin/bash
# EasePi-R2 红外初始化脚本
# DTS中已定义 ir-receiver 节点(compatible="gpio-ir-receiver", GPIO3_B2)
# 内核通过DTS匹配自动加载gpio-ir-recv驱动并创建/dev/lirc0

echo "[IR] 开始红外初始化..."

# 等待DTS自动绑定gpio-ir-recv驱动
if [ ! -c /dev/lirc0 ]; then
    echo "[IR] 等待DTS自动绑定gpio-ir-recv..."
    for i in $(seq 1 3); do
        sleep 1
        [ -c /dev/lirc0 ] && break
    done
fi

# Fallback: DTS未自动绑定时，手动加载模块
if [ ! -c /dev/lirc0 ]; then
    modprobe gpio-ir-recv 2>/dev/null || true
    sleep 1
fi

if [ ! -c /dev/lirc0 ]; then
    echo "[IR] 警告：未找到 /dev/lirc0 设备"
    echo "[IR] 请检查: 1) DTS ir-receiver节点 2) gpio-ir-recv驱动 3) GPIO3_B2"
    exit 1
fi

# 确保lirc运行目录存在
mkdir -p /var/run/lirc
chmod 755 /var/run/lirc

# 检查lircd服务状态
if systemctl is-active --quiet lircd; then
    echo "[IR] lircd服务已经在运行"
else
    echo "[IR] 启动lircd服务..."
    systemctl restart lircd 2>/dev/null || true
fi

# 确保lircd服务开机自启
systemctl enable lircd 2>/dev/null || true

echo "[IR] 红外初始化完成"
exit 0
