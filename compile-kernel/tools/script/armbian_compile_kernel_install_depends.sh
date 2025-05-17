#!/bin/bash

# Set font color
STEPS="[\033[95m STEPS \033[0m]"
INFO="[\033[94m INFO \033[0m]"
SUCCESS="[\033[92m SUCCESS \033[0m]"
WARNING="[\033[93m WARNING \033[0m]"
ERROR="[\033[91m ERROR \033[0m]"

error_msg() {
    echo -e " ${ERROR} ${1}"
    exit 1
}

# Install basic dependencies
install_basic_depends() {
    echo -e "${STEPS} Start installing basic dependencies..."
    
    # Exclude packages that need special handling
    local basic_packages="acl aptly aria2 bc binfmt-support binutils bison btrfs-progs build-essential busybox ca-certificates ccache clang coreutils cpio crossbuild-essential-arm64 cryptsetup curl debian-archive-keyring debian-keyring debootstrap device-tree-compiler dialog dirmngr distcc dosfstools dwarves f2fs-tools fakeroot flex gawk gcc gdisk git gpg gzip imagemagick jq kmod libbison-dev libcrypto++-dev libelf-dev libfdt-dev libfile-fcntllock-perl libfl-dev libfuse-dev liblz4-tool libpython3-dev libssl-dev libusb-1.0-0-dev linux-base lld llvm locales lz4 lzma lzop mtools ncurses-base ncurses-term nfs-kernel-server ntpdate p7zip p7zip-full parallel parted patchutils pigz pixz pkg-config pv python3 python3-dev python3-setuptools qemu-user-static rename rsync swig tar udev unzip uuid uuid-dev uuid-runtime vim wget whiptail xz-utils zip zlib1g-dev zstd"
    
    sudo apt-get -qq update
    if ! sudo apt-get -qq install -y ${basic_packages}; then
        error_msg "Failed to install basic dependencies"
    fi
}

# Install ncurses related packages
install_ncurses_depends() {
    echo -e "${STEPS} Start installing ncurses related packages..."
    
    # Try to install ncurses5 version
    if ! sudo apt-get -qq install -y libncurses5 libncurses5-dev libncursesw5-dev; then
        echo -e "${WARNING} Failed to install ncurses5 version, trying ncurses6 version..."
        # Try to install ncurses6 version
        if ! sudo apt-get -qq install -y libncurses6 libncurses-dev libncursesw6; then
            error_msg "Failed to install ncurses related packages"
        fi
    fi
}

# Install python3-distutils
install_python_distutils() {
    echo -e "${STEPS} Start installing python3-distutils..."
    
    # Try to install python3-distutils
    if ! sudo apt-get -qq install -y python3-distutils; then
        echo -e "${WARNING} Failed to install python3-distutils, trying alternative package..."
        # python3-distutils is deprecated in new version of debian/ubuntu
        # install python3-full instead
        if ! sudo apt-get -qq install -y python3-full; then
            error_msg "Failed to install python3-distutils and its alternative package"
        fi
    fi
}

# Main function
main() {
    install_basic_depends
    install_ncurses_depends
    install_python_distutils
    echo -e "${SUCCESS} All dependencies installed successfully"
}

main 