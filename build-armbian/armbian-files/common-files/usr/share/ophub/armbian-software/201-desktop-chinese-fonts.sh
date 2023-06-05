#!/bin/bash
#============================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Function: Desktop system install Chinese support
# Copyright (C) 2021- https://github.com/unifreq/openwrt_packit
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: sudo ./201-desktop-chinese-fonts.sh
#
#============================== Functions list ==============================
#
# error_msg               : Output error message
# check_release           : Check release file
# set_chinese_env         : Set Chinese environment
# install_chinese_fonts   : Install Chinese language packages
#
#========================== Set default parameters ==========================
#
# Get custom firmware information
ophub_release_file="/etc/ophub-release"

# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
OPTIONS="[\033[93m OPTIONS \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"
#
#============================================================================

# Show error message
error_msg() {
    echo -e "${ERROR} ${1}"
    exit 1
}

# Check release file
check_release() {
    if [[ -f "${ophub_release_file}" ]]; then
        source "${ophub_release_file}" 2>/dev/null
        VERSION_CODEID="${VERSION_CODEID}"
        VERSION_CODENAME="${VERSION_CODENAME}"
    else
        error_msg "${ophub_release_file} file is missing!"
    fi

    [[ -n "${VERSION_CODEID}" && -n "${VERSION_CODENAME}" ]] || error_msg "${ophub_release_file} value is missing!"
}

# Set Chinese environment
set_chinese_env() {
    # Add Chinese language setting
    echo -e "${STEPS} Add Chinese environment variable settings..."
    local etc_env="/etc/environment"
    sed -i '/^LANG=/d' ${etc_env}
    sed -i '/^LANGUAGE=/d' ${etc_env}
    sed -i '/^export LANG/d' ${etc_env}
    echo 'LANG="zh_CN.UTF-8"' >>${etc_env}
    echo 'LANGUAGE="zh_CN:zh:en_US:en"' >>${etc_env}
    echo 'export LANG' >>${etc_env}
    echo 'export LANGUAGE' >>${etc_env}
    echo -e "${INFO} [ ${etc_env} ] content information: \n$(cat ${etc_env})"

    # Add local support settings
    echo -e "${STEPS} Add Chinese local settings..."
    local local_set="/var/lib/locales/supported.d/local"
    [[ -f "${local_set}" ]] || {
        mkdir -p "/var/lib/locales/supported.d"
        touch ${local_set}
    }
    echo 'en_US.UTF-8 UTF-8' >${local_set}
    echo 'zh_CN.UTF-8 UTF-8' >>${local_set}
    echo 'zh_CN.GBK GBK' >>${local_set}
    echo 'zh_CN GB2312' >>${local_set}
    echo -e "${INFO} [ ${local_set} ] content information: \n$(cat ${local_set})"

    # Load settings
    echo -e "${STEPS} Start generating Chinese locale..."
    sudo locale-gen
}

# Install Chinese language packages
install_chinese_fonts() {
    # Get related variables
    check_release

    # Generic packages for Ubuntu system
    local ubuntu_packages=(
        "language-pack-zh-hans" "language-pack-gnome-zh-hans"
        "thunderbird-locale-zh-hans" "thunderbird-locale-zh-cn"
        "fonts-noto-cjk" "fonts-noto-cjk-extra" "fonts-droid-fallback" "fonts-wqy-zenhei"
        "fonts-wqy-microhei" "fonts-arphic-ukai" "fonts-arphic-uming"
        "ibus-table-wubi" "ibus-libpinyin"
    )

    # Packages of lunar/jammy system
    local jammy_packages=(
        "kde-config-fcitx5"
    )

    # Packages of focal system
    local focal_packages=(
        "kde-config-fcitx"
    )

    # Packages of bookworm/bullseye system
    local debian_packages=(
        "fonts-noto-cjk" "fonts-noto-cjk-extra" "fonts-droid-fallback" "fonts-wqy-zenhei"
        "fonts-wqy-microhei" "fonts-arphic-ukai" "fonts-arphic-uming"
        "fonts-arphic-bsmi00lp" "fonts-arphic-gbsn00lp" "fonts-arphic-gkai00mp"
        "fcitx" "fcitx-table*" "fcitx-frontend-gtk2" "fcitx-frontend-gtk3" "fcitx-frontend-qt*"
        "fcitx-table-wbpy" "fcitx-sunpinyin" "fcitx-googlepinyin" "fcitx-pinyin"
        "kde-config-fcitx" "ibus-table-wubi" "ibus-libpinyin"
    )

    # Install the Chinese language packages for the desktop system
    sudo apt-get update
    case "${VERSION_CODENAME}" in
    jammy | lunar)
        echo -e "${STEPS} Start to install Chinese language pack for [ ${VERSION_CODENAME} ]..."
        sudo apt-get install -y ${ubuntu_packages[*]}
        sudo apt-get install -y ${jammy_packages[*]}
        set_chinese_env
        ;;
    focal)
        echo -e "${STEPS} Start to install Chinese language pack for [ ${VERSION_CODENAME} ]..."
        sudo apt-get install -y ${ubuntu_packages[*]}
        sudo apt-get install -y ${focal_packages[*]}
        set_chinese_env
        ;;
    bullseye | bookworm)
        echo -e "${STEPS} Start to install Chinese language pack for [ ${VERSION_CODENAME} ]..."
        sudo apt-get install -y ${debian_packages[*]}
        set_chinese_env
        ;;
    *) error_msg "unsupported system: [ ${VERSION_CODENAME} ]" ;;
    esac

    echo -e "${SUCCESS} Chinese desktop support setting completed."
}

# Check script permission, supports running on Armbian system.
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo ${0} ]"

# Installation options
echo -ne "${OPTIONS} Install Chinese desktop support? (y/n): "
read optid
optid="${optid/Y/y}" && optid="${optid/N/n}"
[[ "${optid:0:1}" == "y" ]] && install_chinese_fonts || echo -e "${INFO} Skip Chinese desktop settings."
