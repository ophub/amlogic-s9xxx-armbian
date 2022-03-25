# 可以安装在 Amlogic s9xxx 电视盒子中使用的 Armbian 系统

查看英文说明 | [View English description](README.md)

[Armbian](https://www.armbian.com/) 系统是基于 Debian/Ubuntu 而构建的专门用于 ARM 芯片的轻量级 Linux 系统。Armbian 系统精益、干净，并且 100% 兼容并继承了 Debian/Ubuntu 系统的功能和丰富的软件生态，可以安全稳定地运行在 TF/SD/USB 及设备的 eMMC 里。

现在你可以将使用 Amlogic 芯片的电视盒子的安卓 TV 系统更换为 Armbian 系统，让他成为一台功能强大的服务器。本项目为 Amlogic s9xxx 电视盒子构建 Armbian 系统。支持写入 EMMC 中使用，支持更新内核等功能。支持的 Amlogic S9xxx 系列型号有 ***`a311d, s922x, s905x3, s905x2, s905l3a, s912, s905d, s905x, s905w, s905`*** 等，例如 ***`Belink GT-King, Belink GT-King Pro, UGOOS AM6 Plus, X96-Max+, HK1-Box, H96-Max-X3, Phicomm-N1, Octopus-Planet, Fiberhome HG680P, ZTE B860H`*** 等盒子。使用方法详见[Armbian 使用文档](build-armbian/armbian-docs)。

最新的 Armbian 固件可以在 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 中下载。欢迎 `Fork` 并进行个性化定制。如果对你有用，可以点仓库右上角的 `Star` 表示支持。

## Armbian 固件说明

| 芯片  | 设备 | [可选内核](https://github.com/ophub/kernel/tree/main/pub/stable) | Armbian 固件 |
| ---- | ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://www.gearbest.com/boards---shields/pp_3008145189226460.html) | 全部 | armbian_aml_a311d_*.img |
| s922x | [Beelink-GT-King](https://tokopedia.link/RAgZmOM41db), [Beelink-GT-King-Pro](https://www.gearbest.com/tv-box/pp_3008857542462482.html), [Ugoos-AM6-Plus](https://tokopedia.link/pHGKXuV41db), [ODROID-N2](https://www.tokopedia.com/search?st=product&q=ODROID-N2) | 全部 | armbian_aml_s922x_*.img |
| s905x3 | [X96-Max+](https://tokopedia.link/uMaH09s41db), [HK1-Box](https://tokopedia.link/xhWeQgTuwfb), [H96-Max-X3](https://tokopedia.link/KuWvwoYuwfb), [Ugoos-X3](https://tokopedia.link/duoIXZpdGgb), [TX3](https://www.aliexpress.com/item/1005003772717802.html), [X96-Air](https://www.gearbest.com/tv-box/pp_3002885621272175.html), [A95XF3-Air](https://tokopedia.link/ByBL45jdGgb) | 全部 | armbian_aml_s905x3_*.img |
| s905x2 | [X96Max-4G](https://tokopedia.link/HcfLaRzjqeb), [X96Max-2G](https://tokopedia.link/HcfLaRzjqeb), [MECOOL-KM3-4G](https://www.gearbest.com/tv-box/pp_3008133484979616.html) | 全部 | armbian_aml_s905x2_*.img |
| s905l3a | [E900V22C/D](https://github.com/Calmact/e900v22c) | 全部 | armbian_aml_s905l3a_*.img |
| s912 | [H96-Pro-Plus](https://www.gearbest.com/tv-box-mini-pc/pp_503486.html), [Tanix-TX92](http://www.tanix-box.com/project-view/tanix-tx92-android-tv-box-powered-amlogic-s912/), [VORKE-Z6-Plus](http://www.vorke.com/project/vorke-z6-2/), [Mecool-M8S-PRO-L](https://www.gearbest.com/tv-box/pp_3005746210753315.html), Octopus-Planet | 全部 | armbian_aml_s912_*.img |
| s905d | [MECOOL-KI-Pro](https://www.gearbest.com/tv-box-mini-pc/pp_629409.html), Phicomm-N1 | 全部 | armbian_aml_s905d_*.img |
| s905x | [HG680P](https://tokopedia.link/HbrIbqQcGgb), [B860H](https://www.zte.com.cn/global/products/cocloud/201707261551/IP-STB/ZXV10-B860H), [TBee-Box](https://www.tbee.com/product/tbee-box/) | 全部 | armbian_aml_s905x_*.img |
| s905w | [X96-Mini](https://tokopedia.link/ro207Hsjqeb), [TX3-Mini](https://www.gearbest.com/tv-box/pp_009748238474.html) | 5.4.* | armbian_aml_s905w_*.img |
| s905 | [Beelink-Mini-MX-2G](https://www.gearbest.com/tv-box-mini-pc/pp_321409.html), [MXQ-Pro+4K](https://www.gearbest.com/tv-box-mini-pc/pp_354313.html) | 全部 | armbian_aml_s905_*.img |

💡提示：当前 ***`s905`*** 的盒子只能在 `TF/SD/USB` 中使用，其他型号的盒子同时支持写入 `EMMC` 中使用。当前 ***`s905w`*** 系列的盒子只支持使用 `5.4` 内核，不能使用 5.10 或更高版本，其他型号的盒子可任选内核版本使用。每个盒子的 dtb 和 u-boot 请查阅[说明](build-armbian/amlogic-u-boot/README.md)。

## 安装及升级 Armbian 的相关说明

选择和你的盒子型号对应的 Armbian 固件，使用 [Rufus](https://rufus.ie/) 或者 [balenaEtcher](https://www.balena.io/etcher/) 等工具将固件写入 USB 里，然后把写好固件的 USB 插入盒子。

- ### 安装 Armbian 到 EMMC

登录 Armbian 系统 (默认用户: root, 默认密码: 1234) → 输入命令：

```yaml
armbian-install
```

默认将安装主线 u-boot，以便支持 5.10 及以上内核的使用。如果选择不安装，请在第 `1` 个输入参数中指定，如 `armbian-install no`

- ### 更新 Armbian 内核

登录 Armbian 系统 → 输入命令：

```yaml
# 使用 root 用户运行 (sudo -i)
# 如果不指定其他参数，以下更新命令将更新到当前同系列内核的最新版本。
armbian-update
```

如果当前目录下有成套的内核文件，将使用当前目录的内核进行更新（更新需要的 4 个内核文件是 `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-amlogic-xxx.tar.gz`, `modules-xxx.tar.gz`。其他内核文件不需要，如果同时存在也不影响更新，系统可以准确识别需要的内核文件）。如果当前目录没有内核文件，将从服务器查询并下载同系列的最新内核进行更新。你也可以查询[可选内核](https://github.com/ophub/kernel/tree/main/pub/stable)版本，进行指定版本更新：`armbian-update 5.4.180`。在设备支持的可选内核里可以自由更新，如从 5.4.180 内核更新为 5.15.25 内核。内核更新时，默认从 [stable](https://github.com/ophub/kernel/tree/main/pub/stable) 内核版本分支下载，如果下载其他 [版本分支](https://github.com/ophub/kernel/tree/main/pub) 的内核，请在第 `2` 个参数中根据分支文件夹名称指定，如 `armbian-update 5.7.19 dev` 。默认会自动安装主线 u-boot，这对使用 5.10 以上版本的内核有更好的支持，如果选择不安装，请在第 `3` 个输入参数中指定，如 `armbian-update 5.4.180 stable no`

内核中的 `headers` 文件默认安装在 `/use/local/include` 目录下。在编译应用程序的时候，在 `GCC` 的 `CFLAG` 参数中添加 `-I /usr/local/include` 即可找到头文件。

内核更新脚本会在开发中不断更新，可使用此命令同步更新本地的脚本：`wget -O /usr/sbin/armbian-update git.io/armbian-update` 。或者直接使用服务器端最新脚本进行内核更新：`bash <(curl -fsSL git.io/armbian-update) 5.4.180`

- ### 安装 Docker 服务

登录 Armbian 系统 → 输入命令：

```yaml
armbian-docker
```

在安装 docker 结束后，可以选择是否安装 `portainer` 可视化管理面板。拟使用`局域网 IP` 进行管理的用户可选择（`h`）安装 `http://ip:9000` 版本；拟使用`域名`进行远程管理的用户可选择（`s`）安装 `https://domain:9000` 域名证书版本（请把域名 `SSL` 证书命名为 `portainer.crt` 和 `portainer.key` 上传至 `/etc/ssl/` 目录下）；`不需要`安装的用户可选择（`n`）结束安装。

- ### 修改 Armbian 配置

登录 Armbian 系统 → 输入命令：

```yaml
armbian-config
```

- ### 为 Armbian 创建 swap

如果你在使用 `docker` 等内存占用较大的应用时，觉得当前盒子的内存不够使用，可以创建 `swap` 虚拟内存分区，将磁盘空间的一定容量虚拟成内存来使用。下面命令输入参数的单位是 `GB`，默认为 `1`。

登录 Armbian 系统 → 输入命令：

```yaml
armbian-swap 1
```

- ### 控制 LED 显示

登录 Armbian 系统 → 输入命令：

```yaml
armbian-led
```

根据 [LED 屏显示控制说明](build-armbian/armbian-docs/led_screen_display_control.md) 进行调试。

- ### 在 TF/SD/USB 中使用 Armbian

激活 TF/SD/USB 的剩余空间，请登录 Armbian 系统 → 输入命令：

```yaml
armbian-tf
```

根据提示输入 `e` 将剩余空间扩容至当前系统分区和文件系统，输入 `c` 将创建新的第 3 分区。

<details>
  <summary>或者手动分配剩余空间 </summary>

#### 查看 [操作截图](https://user-images.githubusercontent.com/68696949/137860992-fbd4e2fa-e90c-4bbb-8985-7f5db9f49927.jpg)

```yaml
# 1. 根据空间大小确认 TF/SD/USB 的名称，TF/SD 为 [ mmcblk ]，USB 为[ sd ]
在命令行中: 输入 [ fdisk -l | grep "sd" ] 查看卡的名称

# 2. 获取剩余空间的起始值，复制并保存，下面使用（例如：5382144）
在命令行中: 输入 [ fdisk -l | grep "sd" | sed -n '$p' | awk '{print $3}' | xargs -i expr {} + 1 ] 得到剩余空间起始值

# 3. 开始分配未使用的空间（例如：sda、mmcblk0 或 mmcblk1）
在命令行中: 输入 [ fdisk /dev/sda ] 开始分配剩余空间
在命令行中: 输入 [ n ] 创建新分区
在命令行中: 输入 [ p ] 指定分区类型为主分区
在命令行中: 将分区号设置为 [ 3 ]
在命令行中: 分区的起始值，输入第二步得到的值 [ 5382144 ]
在命令行中: 分区的结束值，按 [ 回车 ] 使用默认值
在命令行中: 如果提示是否删除签名？[Y]es/[N]o: 输入 [ Y ]
在命令行中: 输入 [ t ] 指定分区类型
在命令行中: 输入分区编号 [ 3 ]
在命令行中: 指定分区类型为 Linux，输入代码 [ 83 ]
在命令行中: 输入 [ w ] 保存结果
在命令行中: 输入 [ reboot ] 重启

# 4. 重新启动后，格式化新分区
在命令行中: 输入 [ mkfs.ext4 -F -L SHARED /dev/sda3 ] 格式新分区

# 5. 为新分区设置挂载目录
在命令行中: 输入 [ mkdir -p /mnt/share ] 创建新分区的挂载目录
在命令行中: 输入 [ mount -t ext4 /dev/sda3 /mnt/share ] 进行挂载

# 6. 添加开机自动挂载
在命令行中: [ vi /etc/fstab ]
# 按 [ i ] 进入编译模式，将下面的代码复制，黏贴到文件的末尾处
/dev/sda3 /mnt/share ext4 defaults 0 0
# 按 [ esc ] 键退出，输入 [ :wq! ] 后按 [ 回车 ] 保存退出，结束设置。
```
</details>

- ### 备份/还原 EMMC 原系统

支持在 `TF/SD/USB` 中对盒子的 `EMMC` 分区进行备份/恢复。建议您在全新的盒子里安装 Armbian 系统前，先对当前盒子自带的安卓 TV 系统进行备份，以便日后在恢复电视系统等情况下使用。

请从 `TF/SD/USB` 启动 Armbian 系统 → 输入命令：

```yaml
armbian-ddbr
```

根据提示输入 `b` 进行系统备份，输入 `r` 进行系统恢复。

## 打包命令的相关参数说明

| 参数  | 含义       | 说明        |
| ---- | ---------- | ---------- |
| -d   | Defaults   | 使用默认配置 |
| -b   | BuildSoC   | 指定电视盒子型号，如 `-b s905x3` . 多个型号使用 `_` 进行连接，如 `-b s905x3_s905d` . 可以指定的型号有: `a311d`, `s905x3`, `s905x2`, `s905l3a`, `s905x`, `s905w`, `s905d`, `s905d-ki`, `s905`, `s922x`, `s922x-n2`, `s912`, `s912-m8s` 。说明：`s922x-reva` 是 `s922x-gtking-pro-rev_a`，`s922x-n2` 是 `s922x-odroid-n2` ，`s912-m8s` 是 `s912-mecool-m8s-pro-l` ，`s905d-ki` 是 `s912-mecool-ki-pro`，`s905x2-km3` 是 `s905x2-mecool-km3` |
| -v   | Version    | 指定内核 [版本分支](https://github.com/ophub/kernel/tree/main/pub) 名称，如 `-v stable` 。指定的名称须与分支目录名称相同。默认使用 `stable` 分支版本。 |
| -k   | Kernel     | 指定 [kernel](https://github.com/ophub/kernel/tree/main/pub/stable) 名称，如 `-k 5.4.180` . 多个内核使用 `_` 进行连接，如 `-k 5.15.25_5.4.180` |
| -a   | AutoKernel | 设置是否自动采用同系列最新版本内核。当为 `true` 时，将自动在内核库中查找在 `-k` 中指定的内核如 5.4.180 的 5.4 同系列是否有更新的版本，如有 5.4.180 之后的最新版本时，将自动更换为最新版。设置为 `false` 时将编译指定版本内核。默认值：`true` |
| -s   | Size       | 对固件的 ROOTFS 分区大小进行设置，默认大小为 2748M, 固件大小必须大于 2000M. 例如： `-s 2748` |
| -t   | RootfsType | 对固件的 ROOTFS 分区的文件系统类型进行设置，默认为 `ext4` 类型，可选项为 `ext4` 或 `btrfs` 类型。例如： `-t btrfs` |

- `sudo ./rebuild -d` : 使用默认配置，对全部型号的电视盒子进行打包。
- `sudo ./rebuild -d -b s905x3 -k 5.4.180` : 推荐使用. 使用默认配置进行相关内核打包。
- `sudo ./rebuild -d -b s905x3_s905d -k 5.15.25_5.4.180` : 使用默认配置，进行多个内核同时打包。使用 `_` 进行多内核参数连接。
- `sudo ./rebuild -d -b s905x3 -k 5.4.180 -s 2748` : 使用默认配置，指定一个内核，一个型号进行打包，固件大小设定为2748M。
- `sudo ./rebuild -d -b s905x3 -v dev -k 5.7.19` : 使用默认配置，指定型号，指定版本分支，指定内核进行打包。
- `sudo ./rebuild -d -b s905x3_s905d`  使用默认配置，对多个型号的电视盒子进行全部内核打包, 使用 `_` 进行多型号连接。
- `sudo ./rebuild -d -k 5.15.25_5.4.180` : 使用默认配置，指定多个内核，进行全部型号电视盒子进行打包, 内核包使用 `_` 进行连接。
- `sudo ./rebuild -d -k 5.15.25_5.4.180 -a true` : 使用默认配置，指定多个内核，进行全部型号电视盒子进行打包, 内核包使用 `_` 进行连接。自动升级到同系列最新内核。
- `sudo ./rebuild -d -t btrfs -s 2748 -k 5.4.180` : 使用默认配置，设置文件系统为 btrfs 格式，分区大小为 2748M, 并指定内核为 5.4.180 ，对全部型号电视盒子进行打包。

- ### 本地化打包

1. 安装必要的软件包（如 Ubuntu 20.04 LTS 用户）

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/ubuntu-2004-server)
```

2. 克隆仓库到本地 `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

3. 在根目录下创建文件夹 `build/output/images` ，并上传 Armbian 镜像文件 ( 如：`Armbian_21.11.0-trunk_Lepotato_current_5.15.25.img` ) 到 `~/amlogic-s9xxx-armbian/build/output/images` 目录里。原版 Armbian 镜像文件名称中的发行版本号（如：`21.11.0`）和内核版本号（如：`5.15.25`）请保留，它将在重构后用作 Armbian 固件的名称。

4. 进入 `~/amlogic-s9xxx-armbian` 根目录，然后运行 `sudo ./rebuild -d -b s905x3 -k 5.4.180` 命令即可生成指定 soc 的 Armbian 镜像文件。生成的文件保存在 `build/output/images` 目录里。

- ### 使用 GitHub Actions 进行编译

1. 关于 Workflows 文件的配置在 [.yml](.github/workflows/build-armbian.yml) 文件里。可以设置需要编译的盒子的 `SOC` 等参数，具体详见 `Rebuild Armbian for amlogic s9xxx` 节点。

2. 全新编译：在 [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 ***`Build armbian`*** ，根据 Armbian 官方支持的 OS 版本，可在 [RELEASE](https://docs.armbian.com/Developer-Guide_Build-Options/) 里选择 Ubuntu 系列：`focal`，或者 Debian 系列：`bullseye` / `buster` 。在 `BOARD` 里可选 `lepotato` / `odroidn2` 等。可根据需要在 `More build options` 里为 `compile.sh` 添加更多设置选项。点击 ***`Run workflow`*** 按钮即可编译。

3. 再次编译：如果 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 中有已经编译好的 `Armbian_.*-trunk_.*.img.gz` 文件，你只是想再次制作其他不同 soc 的盒子，可以跳过 Armbian 源文件的编译，直接进行二次制作。在 [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面中选择  ***`Use Releases file to build armbian`*** ，点击 ***`Run workflow`*** 按钮即可二次编译。

- ### 仅单独引入 GitHub Actions 进行 Armbian 重构

你可以使用其他方式构建 Armbian 固件。或者使用 [Armbian](https://armbian.tnahosting.net/dl/) 官方提供的 [lepotato](https://armbian.tnahosting.net/dl/lepotato/archive/) 等分支的固件，仅在流程控制文件 [.yml](.github/workflows/rebuild-armbian.yml) 中引入本仓库的脚本进行 Armbian 重构，适配 Amlogic S9xxx 系列盒子的使用。在 [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 ***`Rebuild armbian`*** ，输入 Armbian 的网络下载地址如 `https://dl.armbian.com/*/Armbian_*.img.xz` ，或者在流程控制文件 [.yml](.github/workflows/rebuild-armbian.yml) 中通过 `armbian_path` 参数设定重构文件的加载路径。代码如下:

```yaml
- name: Rebuild the Armbian for Amlogic s9xxx
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img
    armbian_soc: s905d_s905x3_s922x_s905x
    armbian_kernel: 5.15.25_5.4.180
```

- GitHub Actions 输入参数说明

关于 GitHUB RELEASES_TOKEN 的相关设置可参考：[RELEASES_TOKEN](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/router-config/README.cn.md#3-fork-仓库并设置-releases_token)。相关参数与`本地打包命令`相对应，请参考上面的说明。

| 参数              | 默认值             | 说明                                            |
|------------------|-------------------|------------------------------------------------|
| armbian_path     | no                | 设置原版 Armbian 文件的路径，支持使用当前工作流中的文件路径如 `build/output/images/*.img` ，也支持使用网络下载地址如： `https://dl.armbian.com/*/Armbian_*.img.xz` |
| armbian_soc      | s905d_s905x3      | 设置打包盒子的 `SOC` ，功能参考 `-b`                |
| version_branch   | stable            | 指定内核 [版本分支](https://github.com/ophub/kernel/tree/main/pub) 名称，功能参考 `-v` |
| armbian_kernel   | 5.15.25_5.4.180  | 设置内核 [版本](https://github.com/ophub/kernel/tree/main/pub/stable)，功能参考 `-k` |
| auto_kernel      | true              | 设置是否自动采用同系列最新版本内核，功能参考 `-a`      |
| armbian_size     | 2748              | 设置固件 ROOTFS 分区的大小，功能参考 `-s`           |
| armbian_fstype   | ext4              | 设置固件 ROOTFS 分区的文件系统类型，功能参考 `-t`     |

- GitHub Actions 输出变量说明

| 参数                                      | 默认值             | 说明                       |
|------------------------------------------|-------------------|---------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}           | out               | 打包后的固件所在文件夹的路径   |
| ${{ env.PACKAGED_OUTPUTDATE }}           | 2021.04.13.1058   | 打包日期                    |
| ${{ env.PACKAGED_STATUS }}               | success           | 打包状态：success / failure |

## 编译自定义内核

自定义内核的编译方法详见 [compile-kernel](compile-kernel/README.cn.md)

```yaml
- name: Compile the kernel for Amlogic s9xxx
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.15.25_5.4.180
    kernel_auto: true
    kernel_sign: -meson64-dev
```

## Armbian 贡献者名单

首先感谢 [150balbes](https://github.com/150balbes) 为在 Amlogic 盒子中使用 Armbian 所做出的杰出贡献和奠定的良好基础。这里编译的 [armbian](https://github.com/armbian/build) 系统直接使用了官方当前的最新源码进行实时编译。程序的开发思路来自 [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box) 等作者的教程。感谢各位的奉献和分享，让我们可以在 Amlogic s9xxx 盒子里使用 Armbian 系统。

本系统所使用的 `kernel` / `u-boot` 等资源主要从 [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) 的项目中复制而来，部分文件由用户在 [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) / [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian) / [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic) / [kernel](https://github.com/ophub/kernel) / [script](https://github.com/ophub/script) 等项目的 [Pull](https://github.com/ophub/amlogic-s9xxx-armbian/pulls) 和 [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中提供分享。为感谢这些开拓者和分享者，从现在开始（本源代码库创建于2021-09-19），我统一在 [CONTRIBUTOR.md](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/CONTRIBUTOR.md) 中进行了记录。再次感谢大家为盒子赋予了新的生命和意义。

## 鸣谢

- [armbian/build](https://github.com/armbian/build)
- [unifreq/kernel](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## License

The amlogic-s9xxx-armbian © OPHUB is licensed under [GPL-2.0](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/LICENSE)

