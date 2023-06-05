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
# Function: Execute desktop software install/update/remove command
# Copyright (C) 2021- https://github.com/unifreq/openwrt_packit
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
# Command: command-desktop.sh -s <software_id> -m <install/update/remove>
# Example: command-desktop.sh -s 201 -m install
#          command-desktop.sh -s 201 -m update
#          command-desktop.sh -s 201 -m remove
#
#============================== Software list ===============================
#
# software_201  : For desktop
# software_202  : For firefox
# software_203  : For vlc
# software_204  : For mpv
# software_205  : For gimp
# software_206  : For krita
# software_207  : For libreoffice
# software_208  : For shotcut
# software_209  : For kdenlive
# software_210  : For thunderbird
# software_211  : For evolution
# software_212  : For gwenview
# software_213  : For eog
# software_214  : For visual.studio.code
# software_215  : For gedit
# software_216  : For flameshot
#
#============================================================================

# Execute generic functions
software_path="/usr/share/ophub/armbian-software"
software_command="${software_path}/software-command.sh"
source "${software_command}"

# For desktop
software_201() {
    case "${software_manage}" in
    install)
        # Add login desktop system user
        echo -ne "${OPTIONS} Please input the login desktop system user(non-root): "
        read get_desktop_user
        if [[ -n "${get_desktop_user}" ]]; then
            sudo adduser ${get_desktop_user}
            sudo usermod -aG sudo ${get_desktop_user}
        else
            echo -e "${NOTE} You skipped adding the logged in desktop system user."
        fi

        if [[ "${VERSION_CODEID}" == "ubuntu" ]]; then
            # Install ubuntu-desktop(gdm3) on Ubuntu (lunar/jammy/focal)
            software_install "ubuntu-desktop lightdm lightdm-gtk-greeter"
        elif [[ "${VERSION_CODEID}" == "debian" ]]; then
            # Install Xfce(lightdm) on Debian (bookworm/bullseye)
            software_install "task-xfce-desktop lightdm lightdm-gtk-greeter"
        else
            error_msg "VERSION_CODEID not supported: [ ${VERSION_CODEID} ]"
        fi

        # Install Chinese desktop support
        sudo bash ${software_path}/201-desktop-chinese-fonts.sh

        sync && sleep 3
        echo -e "${SUCCESS} Desktop installation is successful, restarting..."
        reboot
        ;;
    update) software_update ;;
    remove)
        if [[ "${VERSION_CODEID}" == "ubuntu" ]]; then
            # Remove ubuntu-desktop(gdm3) on Ubuntu (lunar/jammy/focal)
            software_remove "ubuntu-desktop lightdm lightdm-gtk-greeter"
        elif [[ "${VERSION_CODEID}" == "debian" ]]; then
            # Remove Xfce(lightdm) on Debian (bookworm/bullseye)
            software_remove "task-xfce-desktop lightdm lightdm-gtk-greeter"
        else
            error_msg "VERSION_CODEID not supported: [ ${VERSION_CODEID} ]"
        fi

        sync && sleep 3
        echo -e "${SUCCESS} Desktop removed successfully, restarting..."
        reboot
        ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For firefox
software_202() {
    case "${software_manage}" in
    install)
        [[ "${VERSION_CODENAME}" == "jammy" || "${VERSION_CODENAME}" == "lunar" ]] && {
            sudo add-apt-repository ppa:mozillateam/ppa -y
            sudo apt-get update
            software_install "firefox-esr"
        }
        [[ "${VERSION_CODENAME}" == "focal" ]] && software_install "firefox"
        [[ "${VERSION_CODENAME}" == "bullseye" || "${VERSION_CODENAME}" == "bookworm" ]] && software_install "firefox-esr"
        ;;
    update) software_update ;;
    remove)
        [[ "${VERSION_CODENAME}" == "jammy" || "${VERSION_CODENAME}" == "lunar" ]] && {
            software_remove "firefox-esr"
            sudo add-apt-repository --remove ppa:mozillateam/ppa -y
        }
        [[ "${VERSION_CODENAME}" == "focal" ]] && software_remove "firefox"
        [[ "${VERSION_CODENAME}" == "bullseye" || "${VERSION_CODENAME}" == "bookworm" ]] && software_remove "firefox-esr"
        ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For vlc
software_203() {
    case "${software_manage}" in
    install) software_install "vlc" ;;
    update) software_update ;;
    remove) software_remove "vlc" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For mpv
software_204() {
    case "${software_manage}" in
    install) software_install "mpv" ;;
    update) software_update ;;
    remove) software_remove "mpv" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For gimp
software_205() {
    case "${software_manage}" in
    install) software_install "gimp" ;;
    update) software_update ;;
    remove) software_remove "gimp" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For krita
software_206() {
    case "${software_manage}" in
    install) software_install "krita" ;;
    update) software_update ;;
    remove) software_remove "krita" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For libreoffice
software_207() {
    case "${software_manage}" in
    install) software_install "libreoffice libreoffice-l10n-zh-cn libreoffice-help-zh-cn" ;;
    update) software_update ;;
    remove) software_remove "libreoffice libreoffice-l10n-zh-cn libreoffice-help-zh-cn" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For shotcut
software_208() {
    case "${software_manage}" in
    install) software_install "shotcut" ;;
    update) software_update ;;
    remove) software_remove "shotcut" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For kdenlive
software_209() {
    case "${software_manage}" in
    install) software_install "kdenlive" ;;
    update) software_update ;;
    remove) software_remove "kdenlive" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For thunderbird
software_210() {
    case "${software_manage}" in
    install) software_install "thunderbird" ;;
    update) software_update ;;
    remove) software_remove "thunderbird" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For evolution
software_211() {
    case "${software_manage}" in
    install) software_install "evolution" ;;
    update) software_update ;;
    remove) software_remove "evolution" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For gwenview
software_212() {
    case "${software_manage}" in
    install) software_install "gwenview" ;;
    update) software_update ;;
    remove) software_remove "gwenview" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For eog
software_213() {
    case "${software_manage}" in
    install) software_install "eog" ;;
    update) software_update ;;
    remove) software_remove "eog" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For visual.studio.code
software_214() {
    case "${software_manage}" in
    install)
        tmp_download="$(mktemp -d)/code_arm64.deb"
        curl -L https://aka.ms/linux-arm64-deb >${tmp_download}
        [[ "${?}" -eq "0" && -s "${tmp_download}" ]] || error_msg "Software download failed!"
        sudo dpkg -i ${tmp_download}
        ;;
    update) software_update ;;
    remove) software_remove "code" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For gedit
software_215() {
    case "${software_manage}" in
    install) software_install "gedit" ;;
    update) software_update ;;
    remove) software_remove "gedit" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# For flameshot
software_216() {
    case "${software_manage}" in
    install) software_install "flameshot" ;;
    update) software_update ;;
    remove) software_remove "flameshot" ;;
    *) error_msg "Invalid input parameter: [ ${@} ]" ;;
    esac
}

# Initialize variables
init_var "${@}"
software_${software_id} ${software_manage}
