#!/bin/bash

# EasePi A2 红外功能一键修复脚本（优化版）
# 飞牛系统专属 | 自动安装依赖/配置服务/自我禁用
# 最佳路径：/usr/local/ir/fix_infrared.sh | 日志路径：/var/log/ir/ir.log

# 颜色定义，交互更友好
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # 无颜色

# 全局变量
SCRIPT_DIR="/usr/local/ir"
SERVICE_FILE="/etc/systemd/system/ir-keymap.service"
LOG_DIR="/var/log/ir"
LOG_FILE="${LOG_DIR}/ir.log"
INIT_MARKER="${SCRIPT_DIR}/.init_completed"
RC_LOCAL="/etc/rc.local"

# 提前创建日志目录
if [ ! -d "${LOG_DIR}" ]; then
    mkdir -p "${LOG_DIR}" > /dev/null 2>&1
    chmod 755 "${LOG_DIR}" > /dev/null 2>&1
fi
touch "${LOG_FILE}" > /dev/null 2>&1
chmod 644 "${LOG_FILE}" > /dev/null 2>&1

# 日志打印函数（控制台+文件双输出）
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
    echo "$(date +'%Y-%m-%d %H:%M:%S') - INFO - $1" >> ${LOG_FILE} 2>&1
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
    echo "$(date +'%Y-%m-%d %H:%M:%S') - WARN - $1" >> ${LOG_FILE} 2>&1
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
    echo "$(date +'%Y-%m-%d %H:%M:%S') - ERROR - $1" >> ${LOG_FILE} 2>&1
}

# 检查是否已经初始化完成
check_init_status() {
    if [ -f "${INIT_MARKER}" ]; then
        log_info "红外功能初始化已经完成，跳过执行;再次执行需手动删除标记文件：${INIT_MARKER}"
        exit 0
    fi
}

# 检查服务状态函数
check_service_status() {
    log_info "检查服务状态..."
    
    # 检查 ir-keymap.service 状态
    if systemctl is-active --quiet ir-keymap.service; then
        log_info "✅ ir-keymap.service 已执行"
    else
        log_info "❌ ir-keymap.service 未执行，正在执行..."
        systemctl start ir-keymap.service
        if [ $? -eq 0 ]; then
            log_info "✅ ir-keymap.service 执行成功"
        else
            log_warn "⚠️ ir-keymap.service 执行失败，但不影响基本功能"
        fi
    fi
    
    # 检查协议配置
    log_info "检查协议配置..."
    PROTOCOLS=$(ir-keytable 2>&1 | grep "Enabled kernel protocols")
    log_info "当前协议: $PROTOCOLS"
    
    if echo "$PROTOCOLS" | grep -q "nec"; then
        log_info "✅ NEC 协议已启用"
    else
        log_info "❌ NEC 协议未启用，正在启用..."
        ir-keytable -p nec
        if [ $? -eq 0 ]; then
            log_info "✅ NEC 协议启用成功"
        else
            log_error "❌ NEC 协议启用失败"
            return 1
        fi
    fi
    
    log_info "服务状态检查完成，核心功能正常！"
    return 0
}

# 1. 根权限校验
check_root() {
    if [ "$EUID" -ne 0 ]; then
        log_error "请以root权限运行此脚本（sudo bash ${SCRIPT_DIR}/fix_infrared.sh）"
        exit 1
    fi
}

# 2. 检查脚本文件完整性
check_files() {
    log_info "检查脚本目录与文件完整性"
    if [ ! -d "${SCRIPT_DIR}" ]; then
        log_info "创建脚本目录：${SCRIPT_DIR}"
        mkdir -p "${SCRIPT_DIR}" || { log_error "创建目录失败"; exit 1; }
        chmod 755 "${SCRIPT_DIR}"
    fi
    if [ ! -d "${SCRIPT_DIR}/backup_20260218" ]; then
        log_error "备份目录缺失：${SCRIPT_DIR}/backup_20260218，请检查镜像打包是否完整"
        exit 1
    fi
    if [ ! -f "${SCRIPT_DIR}/backup_20260218/infrared.conf" ]; then
        log_error "配置文件缺失：${SCRIPT_DIR}/backup_20260218/infrared.conf，请检查镜像打包是否完整"
        exit 1
    fi
    if [ ! -f "${SCRIPT_DIR}/backup_20260218/final_remote_config" ]; then
        log_error "配置文件缺失：${SCRIPT_DIR}/backup_20260218/final_remote_config，请检查镜像打包是否完整"
        exit 1
    fi
    if [ ! -f "${SCRIPT_DIR}/backup_20260218/systemd/ir-keymap.service" ]; then
        log_error "服务文件缺失：${SCRIPT_DIR}/backup_20260218/systemd/ir-keymap.service，请检查镜像打包是否完整"
        exit 1
    fi
    # 确保执行权限
    if [ ! -x "${0}" ]; then
        chmod +x "${0}"
    fi
}

# 3. 安装系统依赖
install_deps() {
    log_info "开始安装红外功能依赖"
    if command -v apt &> /dev/null; then
        apt-get update -y >> ${LOG_FILE} 2>&1
        if ! command -v ir-keytable &> /dev/null; then
            log_info "安装ir-keytable工具"
            apt-get install -y --no-install-recommends ir-keytable >> ${LOG_FILE} 2>&1
            [ $? -eq 0 ] && log_info "✓ ir-keytable 安装成功" || log_warn "✗ ir-keytable 安装失败"
        else
            log_info "✓ ir-keytable 已安装"
        fi
    elif command -v opkg &> /dev/null; then
        opkg update >> ${LOG_FILE} 2>&1
        opkg install ir-keytable >> ${LOG_FILE} 2>&1
    else
        log_warn "未检测到包管理器，跳过依赖安装（请手动安装）"
    fi
}

# 4. 设置用户权限
set_permissions() {
    log_info "开始设置权限"
    
    # 确保video组存在
    log_info "确保video组存在..."
    groupadd -f video >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ video组已存在或创建成功"
    else
        log_warn "⚠️ 创建video组失败"
    fi
    
    # 将当前用户添加到video组
    log_info "将当前用户添加到video组..."
    CURRENT_USER=$(logname 2>/dev/null || echo "pi")
    usermod -a -G video "$CURRENT_USER" >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 用户 $CURRENT_USER 已添加到video组"
    else
        log_warn "⚠️ 添加用户到video组失败"
    fi
}

# 5. 复制配置文件
copy_configs() {
    log_info "开始复制配置文件到系统路径"
    
    # 创建必要的目录
    log_info "创建必要的目录..."
    mkdir -p /etc/modules-load.d/ > /dev/null 2>&1
    mkdir -p /etc/rc_keymaps/ > /dev/null 2>&1
    
    # 备份原有配置文件
    log_info "备份原有配置文件..."
    if [ -f /etc/modules-load.d/infrared.conf ]; then
        cp /etc/modules-load.d/infrared.conf /etc/modules-load.d/infrared.conf.bak >> ${LOG_FILE} 2>&1
    fi
    
    # 复制模块加载配置
    log_info "复制模块加载配置..."
    cp "${SCRIPT_DIR}/backup_20260218/infrared.conf" /etc/modules-load.d/ >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 模块加载配置复制成功"
    else
        log_error "❌ 模块加载配置复制失败"
        return 1
    fi
    
    # 复制按键映射配置
    log_info "复制按键映射配置..."
    cp "${SCRIPT_DIR}/backup_20260218/final_remote_config" /etc/rc_keymaps/easepi_remote >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 按键映射配置复制成功"
    else
        log_error "❌ 按键映射配置复制失败"
        return 1
    fi
    
    return 0
}

# 应用配置并启用协议
apply_config() {
    log_info "开始应用按键映射配置并启用NEC协议"
    
    # 应用按键映射
    ir-keytable -c -w /etc/rc_keymaps/easepi_remote >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 按键映射配置应用成功"
    else
        log_warn "⚠️ 按键映射配置应用失败"
    fi
    
    # 启用NEC协议
    ir-keytable -p nec >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ NEC 协议已启用"
    else
        log_error "❌ NEC 协议启用失败"
        return 1
    fi
    
    return 0
}

# 6. 配置systemd服务
config_service() {
    log_info "开始配置红外系统服务"
    
    # 复制系统服务文件
    log_info "复制系统服务文件..."
    cp "${SCRIPT_DIR}/backup_20260218/systemd/ir-keymap.service" /etc/systemd/system/ >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 系统服务文件复制成功"
    else
        log_error "❌ 系统服务文件复制失败"
        return 1
    fi
    
    # 重新加载systemd配置
    systemctl daemon-reload >> ${LOG_FILE} 2>&1
    
    # 启用服务
    systemctl enable ir-keymap.service >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 服务已设置为开机自启"
    else
        log_warn "⚠️ 服务开机自启配置失败，请手动执行：systemctl enable ir-keymap.service"
    fi
    
    # 启动服务
    systemctl start ir-keymap.service >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 服务已成功启动"
        sleep 2
        systemctl is-active --quiet ir-keymap.service && log_info "✅ 服务运行正常" || log_warn "⚠️ 服务状态异常"
    else
        log_warn "⚠️ 服务启动失败"
    fi
    
    return 0
}

# 7. 验证配置
verify_config() {
    log_info "开始验证配置"
    
    # 检查模块加载
    log_info "检查模块加载..."
    MODULES=$(lsmod | grep -E 'ir|gpio')
    if [ -n "$MODULES" ]; then
        log_info "✅ 红外模块加载正常"
        echo "$MODULES" >> ${LOG_FILE} 2>&1
    else
        log_warn "⚠️ 未检测到红外模块，请检查硬件或驱动"
    fi
    
    # 检查设备节点
    log_info "检查设备节点..."
    if [ -c "/dev/lirc0" ]; then
        log_info "✅ 红外设备节点存在"
        # 设置设备权限
        chown root:video /dev/lirc0 >> ${LOG_FILE} 2>&1
        chmod 660 /dev/lirc0 >> ${LOG_FILE} 2>&1
        log_info "✅ 设备权限已设置"
    else
        log_warn "⚠️ 红外设备节点不存在，请检查硬件或驱动"
    fi
    
    # 执行服务状态检查
    log_info "执行服务状态检查..."
    check_service_status
    
    # 最终确保NEC协议启用
    log_info "最终确保NEC协议启用..."
    MAX_RETRIES=5
    RETRY_COUNT=0
    
    while [ $RETRY_COUNT -lt $MAX_RETRIES ]; do
        # 启用NEC协议
        ir-keytable -p nec >> ${LOG_FILE} 2>&1
        
        # 等待1秒让配置生效
        sleep 1
        
        # 检查协议配置
        if ir-keytable 2>&1 | grep -q "nec"; then
            log_info "✅ NEC协议已成功启用并保持生效"
            break
        else
            log_warn "⚠️ NEC协议未启用，重试中... ($((RETRY_COUNT+1))/$MAX_RETRIES)"
            RETRY_COUNT=$((RETRY_COUNT+1))
            # 等待2秒后重试
            sleep 2
        fi
    done
    
    if [ $RETRY_COUNT -eq $MAX_RETRIES ]; then
        log_error "❌ 无法启用NEC协议，请手动检查"
    else
        log_info "✅ 最终NEC协议启用成功"
    fi
    
    # 验证最终协议配置
    PROTOCOLS=$(ir-keytable 2>&1 | grep "Enabled kernel protocols")
    log_info "最终协议配置: $PROTOCOLS"
}

# 8. 从rc.local中移除自身，实现自我禁用
disable_self() {
    log_info "开始禁用自身启动配置"
    
    # 检查rc.local文件是否存在
    if [ -f "${RC_LOCAL}" ]; then
        # 备份rc.local文件
        cp "${RC_LOCAL}" "${RC_LOCAL}.bak" >> ${LOG_FILE} 2>&1
        
        # 从rc.local中注释掉红外初始化命令
        sed -i 's|^\(.*fix_infrared.sh.*\)$|# \1|' "${RC_LOCAL}" >> ${LOG_FILE} 2>&1
        
        if [ $? -eq 0 ]; then
            log_info "✅ 已从rc.local中注释掉红外初始化命令"
        else
            log_warn "⚠️ 从rc.local中注释命令失败，请手动检查"
        fi
    else
        log_warn "⚠️ rc.local文件不存在，跳过移除操作"
    fi
    
    # 创建初始化完成标记文件
    touch "${INIT_MARKER}" >> ${LOG_FILE} 2>&1
    chmod 644 "${INIT_MARKER}" >> ${LOG_FILE} 2>&1
    if [ $? -eq 0 ]; then
        log_info "✅ 创建初始化完成标记文件"
    else
        log_warn "⚠️ 创建标记文件失败"
    fi
}

# 9. 显示管理命令
show_commands() {
    echo -e "\n${GREEN}✅ EasePi A2 飞牛系统 红外功能初始化完成！${NC}"
    echo -e "\n${YELLOW}📌 专属管理命令：${NC}"
    echo -e "  • 查看服务状态：systemctl status ir-keymap.service"
    echo -e "  • 重启红外服务：systemctl restart ir-keymap.service"
    echo -e "  • 停止红外服务：systemctl stop ir-keymap.service"
    echo -e "  • 查看运行日志：cat /var/log/ir/ir.log"
    echo -e "  • 实时日志：tail -f /var/log/ir/ir.log"
    echo -e "  • 重新初始化：sudo bash /usr/local/ir/fix_infrared.sh"
    echo -e "\n${YELLOW}🔧 功能测试命令：${NC}"
    echo -e "  • 测试红外信号接收：ir-keytable -t"
    echo -e "  • 查看协议配置：ir-keytable"
    echo -e "  • 检查红外模块：lsmod | grep -E 'ir|gpio'"
    echo -e ""
}

# 主执行流程
main() {
    clear
    echo -e "${GREEN}=== EasePi A2 飞牛系统 红外功能初始化 ===${NC}"
    echo -e "${YELLOW}📝 日志：${NC}${LOG_FILE}"
    echo -e "${YELLOW}📁 程序目录：${NC}${SCRIPT_DIR}"
    echo -e "${YELLOW}📡 功能：${NC}红外接收 | NEC协议 | 按键映射"
    echo -e ""
    
    # 检查是否已经初始化完成
    check_init_status
    
    check_root
    check_files
    install_deps
    set_permissions
    copy_configs
    apply_config
    config_service
    verify_config
    disable_self  # 关键步骤：禁用自身
    show_commands
    exit 0
}

# 启动主函数
main
