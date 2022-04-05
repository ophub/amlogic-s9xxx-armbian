FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive \
    TZ=Asia/Shanghai

RUN set -e \
    && sed -i 's/archive.ubuntu.com/mirrors.nju.edu.cn/g' /etc/apt/sources.list \
    && apt-get update \
    && apt-get install --no-install-recommends -y \
    # sorting by https://build.moz.one
    antlr3 asciidoc autoconf automake  \
    autopoint bc binfmt-support binutils  \
    bison btrfs-progs build-essential  \
    busybox bzip2 ccache clang  \
    crossbuild-essential-arm64 curl  \
    device-tree-compiler dosfstools fakeroot  \
    flex gawk gcc gcc-10-aarch64-linux-gnu  \
    gcc-aarch64-linux-gnu gdb-multiarch  \
    gettext git git-core gperf gzip  \
    initramfs-tools libatomic1-arm64-cross  \
    libc6-dev libc6-dev-arm64-cross  \
    libelf-dev libgcc-10-dev-arm64-cross  \
    libgcc-8-dev-arm64-cross libglib2.0-dev  \
    libidn11 libidn11-dev libncurses-dev  \
    libncurses5-dev libpixman-1-dev  \
    libssl-dev libtool libz-dev lld llvm  \
    llvm-dev lz4 lzma make minizip mount  \
    msmtp netfilter-persistent ninja-build  \
    openssl p7zip p7zip-full parted patch  \
    pigz pkg-config  \
    pkg-config-aarch64-linux-gnu python2.7  \
    python3 qemu qemu-system qemu-user  \
    qemu-user-static qemu-utils rename rsync  \
    subversion swig tar texinfo u-boot-tools  \
    uglifyjs unzip upx util-linux uuid  \
    uuid-dev uuid-runtime vim wget xmlto  \
    xz-utils zip zlib1g-dev zlibc zstd  \
    && apt-get autoremove --purge \
    && rm -rf /var/lib/apt/lists/*
