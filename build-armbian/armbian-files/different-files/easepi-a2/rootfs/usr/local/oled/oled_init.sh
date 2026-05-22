#!/bin/bash
# EasePi A2 OLED 128x32 诊断和配置工具 (Go)
# 使用方法：sudo bash /usr/local/oled/oled_init.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="/usr/local/oled"
OLED_BIN="${SCRIPT_DIR}/oled"
FONT_FILE="${SCRIPT_DIR}/DejaVuSansMono.ttf"
SERVICE_FILE="/etc/systemd/system/oled.service"

check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo -e "${RED}[ERROR]${NC} 请以 root 权限运行此脚本（sudo bash ${SCRIPT_DIR}/oled_init.sh）"
        exit 1
    fi
}

check_files() {
    echo -e "${GREEN}[INFO]${NC} 检查 OLED 相关文件..."
    if [ ! -f "${OLED_BIN}" ]; then
        echo -e "${RED}[ERROR]${NC} 程序缺失：${OLED_BIN}"
        exit 1
    fi
    if [ ! -x "${OLED_BIN}" ]; then
        echo -e "${GREEN}[INFO]${NC} 为 oled 添加执行权限"
        chmod +x "${OLED_BIN}"
    fi
    if [ ! -f "${FONT_FILE}" ]; then
        echo -e "${RED}[ERROR]${NC} 字体缺失：${FONT_FILE}"
        exit 1
    fi
    if [ ! -f "${SERVICE_FILE}" ]; then
        echo -e "${RED}[ERROR]${NC} 服务文件缺失：${SERVICE_FILE}"
        exit 1
    fi
    echo -e "${GREEN}[INFO]${NC} ✓ 文件检查通过"
}

check_deps() {
    echo -e "${GREEN}[INFO]${NC} 检查系统依赖..."
    if ! command -v i2cdetect &>/dev/null; then
        echo -e "${YELLOW}[WARN]${NC} 缺失 i2c-tools，正在安装..."
        apt-get update -y -qq
        apt-get install -y --no-install-recommends i2c-tools
    else
        echo -e "${GREEN}[INFO]${NC} ✓ i2c-tools 已安装"
    fi
}

check_i2c() {
    echo -e "${GREEN}[INFO]${NC} 检查 I2C 总线和 OLED 设备..."
    if ! lsmod | grep -q "i2c_dev"; then
        echo -e "${GREEN}[INFO]${NC} 加载 i2c-dev 模块"
        modprobe i2c-dev 2>/dev/null
    fi
    if [ -e "/dev/i2c-3" ]; then
        echo -e "${GREEN}[INFO]${NC} ✓ i2c-3 设备节点存在"
        chmod 666 /dev/i2c-3 2>/dev/null
    else
        echo -e "${RED}[ERROR]${NC} ✗ i2c-3 设备节点不存在"
        exit 1
    fi
    if i2cdetect -y 3 2>/dev/null | grep -q "3c\|3d"; then
        echo -e "${GREEN}[INFO]${NC} ✓ 在 i2c-3 上发现 OLED 设备"
    else
        echo -e "${YELLOW}[WARN]${NC} ⚠ 未检测到 OLED 设备，请检查硬件连接"
        i2cdetect -y 3
    fi
}

check_service() {
    echo -e "${GREEN}[INFO]${NC} 检查 OLED 服务状态..."
    if systemctl is-active --quiet oled.service; then
        echo -e "${GREEN}[INFO]${NC} ✓ oled.service 正在运行"
    else
        echo -e "${YELLOW}[WARN]${NC} ⚠ oled.service 未运行"
        echo -e "${GREEN}[INFO]${NC} 正在启动服务..."
        systemctl start oled.service
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}[INFO]${NC} ✓ 服务启动成功"
        else
            echo -e "${RED}[ERROR]${NC} ✗ 服务启动失败"
            journalctl -u oled.service -n 20 --no-pager
        fi
    fi
    if systemctl is-enabled --quiet oled.service 2>/dev/null; then
        echo -e "${GREEN}[INFO]${NC} ✓ oled.service 已设置开机自启"
    else
        echo -e "${YELLOW}[WARN]${NC} ⚠ oled.service 未设置开机自启"
        systemctl enable oled.service
    fi
}

restart_service() {
    echo -e "${GREEN}[INFO]${NC} 重启 OLED 服务..."
    systemctl restart oled.service
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}[INFO]${NC} ✓ 服务重启成功"
        sleep 2
        if systemctl is-active --quiet oled.service; then
            echo -e "${GREEN}[INFO]${NC} ✓ 服务运行正常"
        else
            echo -e "${RED}[ERROR]${NC} ✗ 服务运行异常"
        fi
    else
        echo -e "${RED}[ERROR]${NC} ✗ 服务重启失败"
    fi
}

show_usage() {
    local cpu_usage cpu_temp rx tx
    cpu_usage=$(grep 'cpu ' /proc/stat 2>/dev/null | awk '{usage=($2+$4)*100/($2+$4+$5)} END {printf "%.0f", usage}' 2>/dev/null || echo "N/A")
    cpu_temp=$(awk '{printf "%.0f", $1/1000}' /sys/class/thermal/thermal_zone0/temp 2>/dev/null || echo "N/A")
    local ip
    ip=$(ip -4 addr show scope global 2>/dev/null | grep -oP 'inet \K[\d.]+' | head -1 || echo "N/A")

    echo -e "\n${GREEN}=== EasePi A2 OLED 诊断工具 (Go) ===${NC}"
    echo -e "${YELLOW}📊 当前状态：${NC}"
    echo -e "  CPU: ${cpu_usage}%  温度: ${cpu_temp}°C   IP: ${ip}"
    echo -e "\n${YELLOW}📌 管理命令：${NC}"
    echo -e "  • 查看状态：  systemctl status oled.service"
    echo -e "  • 重启服务：  systemctl restart oled.service"
    echo -e "  • 停止服务：  systemctl stop oled.service"
    echo -e "  • 前台运行：  ${OLED_BIN}"
    echo -e "  • 检测设备：  i2cdetect -y 3"
    echo -e "\n${YELLOW}📋 显示模式：${NC}"
    echo -e "  • 空闲(1行)：仅 IP 地址，底部对齐"
    echo -e "  • CPU高(2行)：CPU+温度 在上，IP 在下"
    echo -e "  • NET高(2行)：网速 在上，IP 在下"
    echo -e "  • 高负载(3行)：CPU | NET | IP"
    echo -e "\n${YELLOW}⚙️ 阈值：${NC}"
    echo -e "  CPU > 30% 或 温度 > 60°C → CPU 模式"
    echo -e "  NET > 100KB/s → NET 模式"
    echo -e "  同时触发 → 3 行模式"
}

main() {
    clear
    show_usage
    echo -e "\n${GREEN}开始诊断...${NC}\n"

    check_root
    check_files
    check_deps
    check_i2c
    check_service

    echo -e "\n${GREEN}=== 诊断完成 ===${NC}"
    echo -e "${YELLOW}是否要重启 OLED 服务？(y/n)${NC}"
    read -r response
    if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]; then
        restart_service
    fi
}

main