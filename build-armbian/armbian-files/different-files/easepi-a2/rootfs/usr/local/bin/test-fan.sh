#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

STOP_FLAG=0
TEMP_PID=""

log() {
    printf "${BLUE}[%s]${NC} %s\n" "$(date '+%H:%M:%S')" "$1"
}

log_success() {
    printf "${GREEN}[%s] SUCCESS:${NC} %s\n" "$(date '+%H:%M:%S')" "$1"
}

log_fail() {
    printf "${RED}[%s] FAIL:${NC} %s\n" "$(date '+%H:%M:%S')" "$1"
}

log_info() {
    printf "${YELLOW}[%s] INFO:${NC} %s\n" "$(date '+%H:%M:%S')" "$1"
}

log_warn() {
    printf "${CYAN}[%s] WARN:${NC} %s\n" "$(date '+%H:%M:%S')" "$1"
}

get_temp() {
    cat /sys/class/thermal/thermal_zone0/temp 2>/dev/null | awk '{printf "%.1f", $1/1000}'
}

get_fan_state() {
    cat /sys/class/thermal/"$1"/cur_state 2>/dev/null
}



get_gpio_state() {
    cat /sys/kernel/debug/gpio 2>/dev/null | grep "$1" | awk '{print $5}'
}

stop_fans() {
    echo 0 > /sys/class/thermal/cooling_device0/cur_state 2>/dev/null
    echo 0 > /sys/class/thermal/cooling_device1/cur_state 2>/dev/null
    sleep 1
}

cleanup() {
    STOP_FLAG=1
    if [ -n "$TEMP_PID" ]; then
        kill "$TEMP_PID" 2>/dev/null
        TEMP_PID=""
    fi
    stop_fans
    echo enabled > /sys/class/thermal/thermal_zone0/mode 2>/dev/null
    printf "\n"
    log_warn "测试已停止"
    printf "\n"
}

trap 'cleanup' SIGINT SIGTERM

# Use \033[60G to position right border at fixed column, immune to ANSI color codes
show_menu() {
    clear
    printf "\n"
    printf "╔══════════════════════════════════════════════════════════════╗\n"
    printf "║              EasePi A2 风扇测试工具                          ║\n"
    printf "╚══════════════════════════════════════════════════════════════╝\n"
    printf "\n"
    printf "  ┌──────────────────────────────────────────────────────────┐\n"
    printf "  │  请选择测试模式:                                          \033[60G│\n"
    printf "  │                                                         \033[60G│\n"
    printf "  │  ${CYAN}[1]${NC} 测试 fan0 (12V)\033[60G│\n"
    printf "  │  ${CYAN}[2]${NC} 测试 fan1 (5V)\033[60G│\n"
    printf "  │  ${CYAN}[3]${NC} 测试双风扇 (fan0 + fan1)\033[60G│\n"
    printf "  │  ${CYAN}[0]${NC} 退出\033[60G│\n"
    printf "  │                                                          \033[60G│\n"
    printf "  │  提示: 测试过程中按 Ctrl+C 可停止并返回菜单\033[60G│\n"  
    printf "  └──────────────────────────────────────────────────────────┘\n"
    printf "\n"
    printf "  ${YELLOW}CPU温度: %s°C${NC}\n" "$(get_temp)"
    printf "\n"
    printf "  请输入选择 [0-3]: "
}

# Use save/restore cursor to eliminate flicker during read
start_temp_monitor() {
    while true; do
        printf "\033[s"
        printf "\033[2A"
        printf "\r  ${YELLOW}CPU温度: %s°C${NC}        " "$(get_temp)"
        printf "\033[u"
        sleep 1
    done &
    TEMP_PID=$!
}

stop_temp_monitor() {
    if [ -n "$TEMP_PID" ]; then
        kill "$TEMP_PID" 2>/dev/null
        TEMP_PID=""
    fi
}

test_fan() {
    local fan_name="$1"
    local cooling_device="$2"
    local hwmon="$3"
    local gpio="$4"
    local duration="$5"

    log_info "开始测试 $fan_name..."
    echo 1 > /sys/class/thermal/"$cooling_device"/cur_state
    sleep 2

    local state=$(get_fan_state "$cooling_device")
    local gpio_state=$(get_gpio_state "$gpio")

    if [ "$state" = "1" ]; then
        log_success "$fan_name 状态: $state (开启)"
    else
        log_fail "$fan_name 状态: $state (应为 1)"
    fi

    if [ "$gpio_state" = "hi" ] || [ "$gpio_state" = "out" ]; then
        log_success "$fan_name GPIO$gpio: $gpio_state"
    else
        log_fail "$fan_name GPIO$gpio: $gpio_state (应为 hi)"
    fi

    log_info "$fan_name 将持续运行 ${duration}秒..."
    printf "\n"
    for i in $(seq 1 "$duration"); do
        if [ $STOP_FLAG -eq 1 ]; then
            return 1
        fi
        printf "\r\033[2K${YELLOW}CPU温度: %s°C${NC}\n" "$(get_temp)"
        printf "\r\033[2K进度: [%-60s] %d/%d 秒" "$(printf '#%.0s' $(seq 1 $i))" "$i" "$duration"
        sleep 1
        printf "\033[A"
    done
    printf "\n\n"

    log_info "关闭 $fan_name..."
    echo 0 > /sys/class/thermal/"$cooling_device"/cur_state
    sleep 1

    state=$(get_fan_state "$cooling_device")

    if [ "$state" = "0" ]; then
        log_success "$fan_name 状态: $state (关闭)"
    else
        log_fail "$fan_name 状态: $state (应为 0)"
    fi

    return 0
}

test_single_fan0() {
    printf "\n"
    printf "╔══════════════════════════════════════════════════════════════╗\n"
    printf "║              测试 fan0 (12V)                                ║\n"
    printf "╚══════════════════════════════════════════════════════════════╝\n"
    printf "\n"

    log "步骤 1/2: 关闭 thermal 自动控制..."
    echo disabled > /sys/class/thermal/thermal_zone0/mode
    if [ "$(cat /sys/class/thermal/thermal_zone0/mode)" = "disabled" ]; then
        log_success "thermal 自动控制已关闭"
    else
        log_fail "无法关闭 thermal 自动控制"
        return 1
    fi

    test_fan "fan0" "cooling_device0" "hwmon3" "146" "60"
    if [ $? -ne 0 ]; then
        return 1
    fi

    log "步骤 2/2: 恢复 thermal 自动控制..."
    echo enabled > /sys/class/thermal/thermal_zone0/mode
    if [ "$(cat /sys/class/thermal/thermal_zone0/mode)" = "enabled" ]; then
        log_success "thermal 自动控制已恢复"
    else
        log_fail "无法恢复 thermal 自动控制"
        return 1
    fi

    log_info "最终温度（CPU）: $(get_temp)°C"

    printf "\n"
    printf "╔══════════════════════════════════════════════════════════════╗\n"
    printf "║              fan0 测试完成                                   ║\n"
    printf "╚══════════════════════════════════════════════════════════════╝\n"
    printf "\n"
}

test_single_fan1() {
    printf "\n"
    printf "╔══════════════════════════════════════════════════════════════╗\n"
    printf "║              测试 fan1 (5V)                                 ║\n"
    printf "╚══════════════════════════════════════════════════════════════╝\n"
    printf "\n"

    log "步骤 1/2: 关闭 thermal 自动控制..."
    echo disabled > /sys/class/thermal/thermal_zone0/mode
    if [ "$(cat /sys/class/thermal/thermal_zone0/mode)" = "disabled" ]; then
        log_success "thermal 自动控制已关闭"
    else
        log_fail "无法关闭 thermal 自动控制"
        return 1
    fi

    test_fan "fan1" "cooling_device1" "hwmon4" "101" "60"
    if [ $? -ne 0 ]; then
        return 1
    fi

    log "步骤 2/2: 恢复 thermal 自动控制..."
    echo enabled > /sys/class/thermal/thermal_zone0/mode
    if [ "$(cat /sys/class/thermal/thermal_zone0/mode)" = "enabled" ]; then
        log_success "thermal 自动控制已恢复"
    else
        log_fail "无法恢复 thermal 自动控制"
        return 1
    fi

    log_info "最终温度（CPU）: $(get_temp)°C"

    printf "\n"
    printf "╔══════════════════════════════════════════════════════════════╗\n"
    printf "║              fan1 测试完成                                   ║\n"
    printf "╚══════════════════════════════════════════════════════════════╝\n"
    printf "\n"
}

test_both_fans() {
    printf "\n"
    printf "╔══════════════════════════════════════════════════════════════╗\n"
    printf "║              测试双风扇 (fan0 + fan1)                        ║\n"
    printf "╚══════════════════════════════════════════════════════════════╝\n"
    printf "\n"

    log "步骤 1/3: 关闭 thermal 自动控制..."
    echo disabled > /sys/class/thermal/thermal_zone0/mode
    if [ "$(cat /sys/class/thermal/thermal_zone0/mode)" = "disabled" ]; then
        log_success "thermal 自动控制已关闭"
    else
        log_fail "无法关闭 thermal 自动控制"
        return 1
    fi

    log "步骤 2/3: 同时开启两个风扇..."
    echo 1 > /sys/class/thermal/cooling_device0/cur_state
    echo 1 > /sys/class/thermal/cooling_device1/cur_state
    sleep 2

    local fan0_state=$(get_fan_state "cooling_device0")
    local fan1_state=$(get_fan_state "cooling_device1")

    if [ "$fan0_state" = "1" ] && [ "$fan1_state" = "1" ]; then
        log_success "双风扇状态: fan0=$fan0_state, fan1=$fan1_state (均开启)"
    else
        log_fail "双风扇状态: fan0=$fan0_state, fan1=$fan1_state (应为均为 1)"
    fi

    log_info "双风扇将持续运行 60秒..."
    printf "\n"
    for i in $(seq 1 60); do
        if [ $STOP_FLAG -eq 1 ]; then
            return 1
        fi
        printf "\r\033[2K${YELLOW}CPU温度: %s°C${NC}\n" "$(get_temp)"
        printf "\r\033[2K进度: [%-60s] %d/%d 秒" "$(printf '#%.0s' $(seq 1 $i))" "$i" "60"
        sleep 1
        printf "\033[A"
    done
    printf "\n\n"

    log "步骤 3/3: 关闭所有风扇..."
    echo 0 > /sys/class/thermal/cooling_device0/cur_state
    echo 0 > /sys/class/thermal/cooling_device1/cur_state
    sleep 1

    fan0_state=$(get_fan_state "cooling_device0")
    fan1_state=$(get_fan_state "cooling_device1")

    if [ "$fan0_state" = "0" ] && [ "$fan1_state" = "0" ]; then
        log_success "所有风扇已关闭"
    else
        log_fail "风扇关闭失败: fan0=$fan0_state, fan1=$fan1_state"
    fi

    log "恢复 thermal 自动控制..."
    echo enabled > /sys/class/thermal/thermal_zone0/mode
    if [ "$(cat /sys/class/thermal/thermal_zone0/mode)" = "enabled" ]; then
        log_success "thermal 自动控制已恢复"
    else
        log_fail "无法恢复 thermal 自动控制"
        return 1
    fi

    log_info "最终温度（CPU）: $(get_temp)°C"

    printf "\n"
    printf "╔══════════════════════════════════════════════════════════════╗\n"
    printf "║              双风扇测试完成                                   ║\n"
    printf "╚══════════════════════════════════════════════════════════════╝\n"
    printf "\n"
}

main() {
    while true; do
        show_menu
        start_temp_monitor
        read -r choice
        stop_temp_monitor
        printf "\n"

        case $choice in
            1)
                test_single_fan0
                ;;
            2)
                test_single_fan1
                ;;
            3)
                test_both_fans
                ;;
            0)
                stop_fans
                printf "\n"
                printf "╔══════════════════════════════════════════════════════════════╗\n"
                printf "║              退出测试工具                                    ║\n"
                printf "╚══════════════════════════════════════════════════════════════╝\n"
                printf "\n"
                exit 0
                ;;
            *)
                log_fail "无效选择，请输入 0-3 之间的数字"
                sleep 2
                ;;
        esac

        STOP_FLAG=0
        printf "\n"
        printf "按任意键返回菜单..."
        read -r -n 1 -s
    done
}

main