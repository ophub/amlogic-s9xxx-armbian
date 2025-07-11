#!/bin/bash
# =================================================================
#
# 脚本用途: 监控SATA端口的硬盘活动，并根据硬盘的存在与否控制对应的LED灯。
# 作者: https://github.com/Onlyoooooo
# 版本: 1.0 (Beta)
#
# =================================================================

# 获取当前所有活动的SATA端口ID (例如 ata1, ata2)
get_active_ata_ids() {
    find /sys/class/block/sd* -exec readlink -f {} + 2>/dev/null | grep -o 'ata[0-9]\+' | sort -u || true
}

# 定义SATA端口到LED设备文件的映射，wxy-oes(a311d)有3个SATA端口
declare -A PORT_LED_MAP=(
    ["ata1"]="/sys/class/leds/green:disk/brightness"
    ["ata2"]="/sys/class/leds/green:disk_1/brightness"
    ["ata3"]="/sys/class/leds/green:disk_2/brightness"
)

# 用于存储每个端口的当前状态 (1=亮, 0=灭)，以避免不必要的写入操作
declare -A PORT_STATE

echo "脚本启动，正在初始化LED状态..."

# 获取脚本启动时处于活动状态的端口列表
ACTIVE_PORTS_AT_BOOT=$(get_active_ata_ids)
echo "开机时检测到的活动端口: ${ACTIVE_PORTS_AT_BOOT}"

# --- 初始化阶段 ---
# 遍历所有已配置的端口，设置其初始LED状态
for port in "${!PORT_LED_MAP[@]}"; do
    initial_state=0

    # 检查当前端口是否在开机时检测到的活动端口列表中
    echo "正在检查端口 ${port} 的初始状态..."
    if echo "${ACTIVE_PORTS_AT_BOOT}" | grep -q -x "${port}"; then
        initial_state=1
        echo "  - 端口 ${port} 检测到活动设备，将点亮LED。"
    else
        echo "  - 端口 ${port} 未检测到活动设备，将熄灭LED。"
    fi

    # 记录初始状态
    PORT_STATE["$port"]=${initial_state}

    led_file="${PORT_LED_MAP[$port]}"
    # 写入前检查LED控制文件是否存在
    if [[ -w "${led_file}" ]]; then
        echo "${initial_state}" >"${led_file}"
    else
        echo "  - 警告: LED文件 ${led_file} 不存在或不可写。"
    fi
done

echo "初始化完成，开始监控内核日志..."

# --- 监控阶段 ---
# 检查日志文件是否存在且可读
if [[ ! -r "/var/log/kern.log" ]]; then
    echo "错误: 无法监控 /var/log/kern.log。请确保文件存在且可读。"
else
    # 使用 tail -F 持续监控日志文件的变化
    tail -F /var/log/kern.log 2>/dev/null | while read -r line; do
        port=""
        new_value=""

        # 根据不同的内核日志消息，解析端口和新状态
        if [[ "$line" =~ (ata[0-9]+):[[:space:]]SATA[[:space:]]link[[:space:]](up|down) ]]; then
            port="${BASH_REMATCH[1]}"
            [[ "${BASH_REMATCH[2]}" == "up" ]] && new_value=1 || new_value=0
        elif [[ "$line" =~ (ata[0-9]+)\.00:[[:space:]]configured ]]; then
            port="${BASH_REMATCH[1]}"
            new_value=1
        elif [[ "$line" =~ (ata[0-9]+):[[:space:]]device_remove ]]; then
            port="${BASH_REMATCH[1]}"
            new_value=0
        elif [[ "$line" =~ (ata[0-9]+):[[:space:]]EH[[:space:]]complete ]]; then
            port="${BASH_REMATCH[1]}"
            # 错误处理完成后，重新检查sysfs中的设备状态
            [[ -d "/sys/class/ata_port/${port}/device" ]] && new_value=1 || new_value=0
        fi

        # 如果成功解析出端口，并且该端口在我们的监控列表中
        if [[ -n "${port}" && -v "PORT_LED_MAP[$port]" ]]; then
            # 仅当状态发生变化时才更新LED
            if [[ "${PORT_STATE[$port]}" != "${new_value}" ]]; then
                echo "检测到状态变化: 端口 ${port} -> ${new_value} (旧状态: ${PORT_STATE[$port]})"
                led_file="${PORT_LED_MAP[$port]}"
                if [[ -w "${led_file}" ]]; then
                    echo "${new_value}" >"${led_file}"
                fi
                # 更新内部状态记录
                PORT_STATE["$port"]="${new_value}"
            fi
        fi
    done
fi
