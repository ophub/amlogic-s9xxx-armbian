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
# Function: Install Chinese language support for desktop systems
# Copyright (C) 2021- https://github.com/unifreq/openwrt_packit
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: sudo ./201-desktop-chinese-fonts.sh
#
#============================== Functions list ==============================
#
# error_msg               : Output error message and exit
# check_release           : Check and load system release information
# set_chinese_env         : Configure Chinese locale environment
# install_chinese_fonts   : Install Chinese language packages and fonts
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
    echo -e "${STEPS} Adding Chinese environment variable settings..."
    local etc_env="/etc/environment"
    sed -i '/^LANG=/d' ${etc_env}
    sed -i '/^LANGUAGE=/d' ${etc_env}
    sed -i '/^export LANG/d' ${etc_env}
    echo 'LANG="zh_CN.UTF-8"' >>${etc_env}
    echo 'LANGUAGE="zh_CN:zh:en_US:en"' >>${etc_env}
    echo 'export LANG' >>${etc_env}
    echo 'export LANGUAGE' >>${etc_env}
    echo -e "${INFO} Contents of [ ${etc_env} ]: \n$(cat ${etc_env})"

    local locale_conf="/etc/default/locale"
    [[ -f "${locale_conf}" ]] || touch ${locale_conf} 2>/dev/null
    sed -i '/^LANG=/d' ${locale_conf}
    sed -i '/^LANGUAGE=/d' ${locale_conf}
    sed -i '/^LC_MESSAGES=/d' ${locale_conf}
    sed -i '/^LC_ALL=/d' ${locale_conf}
    sed -i '/^export LANG/d' ${locale_conf}
    echo 'LANG=zh_CN.UTF-8' >>${locale_conf}
    echo 'LANGUAGE=zh_CN:zh:en_US:en' >>${locale_conf}
    echo 'LC_MESSAGES=zh_CN.UTF-8' >>${locale_conf}
    echo 'LC_ALL=zh_CN.UTF-8' >>${locale_conf}
    echo -e "${INFO} Contents of [ ${locale_conf} ]: \n$(cat ${locale_conf})"

    # Add locale support settings
    echo -e "${STEPS} Adding Chinese locale settings..."
    local local_set="/var/lib/locales/supported.d/local"
    [[ -f "${local_set}" ]] || {
        mkdir -p "/var/lib/locales/supported.d"
        touch ${local_set}
    }
    echo 'en_US.UTF-8 UTF-8' >${local_set}
    echo 'zh_CN.UTF-8 UTF-8' >>${local_set}
    echo 'zh_CN.GBK GBK' >>${local_set}
    echo 'zh_CN GB2312' >>${local_set}
    echo -e "${INFO} Contents of [ ${local_set} ]: \n$(cat ${local_set})"

    # Load settings
    echo -e "${STEPS} Generating Chinese locale..."
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

    # Packages of resolute/noble system
    local resolute_packages=(
        "kde-config-fcitx5"
    )

    # Packages of focal system
    local focal_packages=(
        "kde-config-fcitx"
    )

    # Packages of bookworm/bullseye/trixie system
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
    resolute | jammy | noble | oracular)
        echo -e "${STEPS} Installing Chinese language pack for [ ${VERSION_CODENAME} ]..."
        sudo apt-get install -y ${ubuntu_packages[*]}
        sudo apt-get install -y ${resolute_packages[*]}
        set_chinese_env
        ;;
    focal)
        echo -e "${STEPS} Installing Chinese language pack for [ ${VERSION_CODENAME} ]..."
        sudo apt-get install -y ${ubuntu_packages[*]}
        sudo apt-get install -y ${focal_packages[*]}
        set_chinese_env
        ;;
    bullseye | bookworm | trixie)
        echo -e "${STEPS} Installing Chinese language pack for [ ${VERSION_CODENAME} ]..."
        sudo apt-get install -y ${debian_packages[*]}
        set_chinese_env
        ;;
    *) error_msg "Unsupported system: [ ${VERSION_CODENAME} ]" ;;
    esac

    echo -e "${SUCCESS} Chinese desktop support has been installed successfully."
}

# Check script permission
[[ "$(id -u)" == "0" ]] || error_msg "Please run this script as root: [ sudo ${0} ]"

# Prompt for installation
echo -ne "${OPTIONS} Install Chinese desktop support? (y/n): "
read optid
optid="${optid/Y/y}" && optid="${optid/N/n}"
[[ "${optid:0:1}" == "y" ]] && install_chinese_fonts || echo -e "${INFO} Skipped Chinese desktop settings."
