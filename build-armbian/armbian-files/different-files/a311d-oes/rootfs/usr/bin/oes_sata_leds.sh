#!/bin/bash
# ==================================================================================
#
# 脚本用途: 监控 WXY-OES(A311d) 设备 SATA 端口的硬盘活动，并根据硬盘的存在与否控制对应的LED灯。
# 通用版本: 可自动适应 Armbian 和 OpenWrt 系统
#
# 作者: https://github.com/Onlyoooooo
# 版本: 1.1 (Beta)
#
# ==================================================================================

# 定义3个SATA端口到LED设备文件的映射
declare -A PORT_LED_MAP=(
    ["ata1"]="/sys/class/leds/green:disk/brightness"
    ["ata2"]="/sys/class/leds/green:disk_1/brightness"
    ["ata3"]="/sys/class/leds/green:disk_2/brightness"
)

# 优雅退出：熄灭所有LED，终止监控进程
monitor_pid=""
cleanup() {
    trap - TERM INT HUP
    [[ -n "${monitor_pid}" ]] && kill "${monitor_pid}" 2>/dev/null
    for port in "${!PORT_LED_MAP[@]}"; do
        [[ -w "${PORT_LED_MAP[$port]}" ]] && echo 0 >"${PORT_LED_MAP[$port]}"
    done
    exit 0
}
trap cleanup TERM INT HUP

# 获取当前所有活动的SATA端口ID (例如 ata1, ata2, ata3)
get_active_ata_ids() {
    find /sys/class/block/sd* -exec readlink -f {} + 2>/dev/null | grep -o 'ata[0-9]\+' | sort -u || true
}

echo "WXY-OES(A311d) 设备SATA硬盘监测脚本启动，正在初始化LED状态..."

# 用于存储每个端口的当前状态 (1=亮, 0=灭)
declare -A PORT_STATE

# 获取系统启动时已经连接的硬盘对应的端口列表
ACTIVE_PORTS_AT_BOOT=$(get_active_ata_ids)

echo "开机时检测到的活动端口: ${ACTIVE_PORTS_AT_BOOT}"

# 遍历关联数组 PORT_LED_MAP 的所有键 (即 "ata1", "ata2", "ata3")
for port in "${!PORT_LED_MAP[@]}"; do
    # 默认状态为0 (灯灭)
    initial_state=0

    # 检查当前遍历的端口是否存在于"活动端口列表"中
    # 使用静默模式，通过退出码(0为成功, 非0为失败)来判断
    if echo "${ACTIVE_PORTS_AT_BOOT}" | grep -q -x "${port}"; then
        # 如果找到，说明有硬盘，状态设为1 (灯亮)
        initial_state=1
        echo "  - 端口 ${port} 检测到活动设备，将点亮LED。"
    else
        echo "  - 端口 ${port} 未检测到活动设备，将熄灭LED。"
    fi

    # 将该端口的初始状态存入 PORT_STATE 关联数组中
    PORT_STATE["${port}"]=${initial_state}

    # 从 PORT_LED_MAP 关联数组中获取该端口对应的LED文件路径
    led_file="${PORT_LED_MAP[${port}]}"

    # 检查文件是否存在并且当前用户有写入权限
    if [[ -w "${led_file}" ]]; then
        # 将初始状态 (0或1) 写入文件来控制LED状态
        echo "${initial_state}" >"${led_file}"
    else
        echo "  - 警告: LED文件 '${led_file}' 不存在或不可写。"
    fi
done

echo "初始化完成，开始监控内核日志..."

# 初始化一个空变量，用于存储最终要执行的命令。
MONITOR_CMD=""

# 使用 'command -v' 检查 logread 命令是否存在，判断是否为 OpenWrt 系统。
if command -v logread >/dev/null 2>&1; then
    # 如果 logread 命令存在，我们判定为 OpenWrt 系统。
    echo "系统环境检测为: OpenWrt，使用 'logread' 进行监控。"
    MONITOR_CMD="logread -f"
else
    echo "系统环境检测为: Armbian，使用 'journalctl' 进行监控。"
    # -k 表示只看内核日志，-f 表示持续追踪，-n 0 表示不显示历史日志
    MONITOR_CMD="journalctl -k -f -n 0"
fi

echo "持续监听SATA硬盘状态..."

# 使用 coproc 启动监控进程以获取其 PID，确保 SIGTERM 可以终止它
coproc MONITOR { ${MONITOR_CMD} 2>/dev/null; }
monitor_pid=${MONITOR_PID}
exec {MONITOR[1]} >&-

while IFS= read -r line <&"${MONITOR[0]}"; do
    port=""
    new_value=""

    if [[ "${line}" =~ (ata[0-9]+):[[:space:]]SATA[[:space:]]link[[:space:]](up|down) ]]; then
        port="${BASH_REMATCH[1]}"
        [[ "${BASH_REMATCH[2]}" == "up" ]] && new_value=1 || new_value=0
    elif [[ "${line}" =~ (ata[0-9]+)\.00:[[:space:]]configured ]]; then
        port="${BASH_REMATCH[1]}"
        new_value=1
    elif [[ "${line}" =~ (ata[0-9]+):[[:space:]]device_remove ]]; then
        port="${BASH_REMATCH[1]}"
        new_value=0
    elif [[ "${line}" =~ (ata[0-9]+):[[:space:]]EH[[:space:]]complete ]]; then
        port="${BASH_REMATCH[1]}"
        # 错误处理完成后，重新检查sysfs中的设备状态
        [[ -d "/sys/class/ata_port/${port}/device" ]] && new_value=1 || new_value=0
    fi

    if [[ -n "${port}" && -v "PORT_LED_MAP[${port}]" ]]; then

        # 仅当新状态与我们记录的旧状态不同时才进行操作
        if [[ "${PORT_STATE[${port}]}" != "${new_value}" ]]; then
            echo ""
            echo "[$(date)] 检测到状态变化: 端口 ${port} -> ${new_value} (旧状态: ${PORT_STATE[${port}]})"

            led_file="${PORT_LED_MAP[${port}]}"

            if [[ -w "${led_file}" ]]; then
                echo "${new_value}" >"${led_file}"
                echo "  - LED状态已更新。"
            else
                echo "  - 警告: LED文件 '${led_file}' 不可写。"
            fi

            # 更新我们内存中的状态记录。
            PORT_STATE["${port}"]="${new_value}"
            echo "正在等待新的内核事件..."
        fi
    fi
done
