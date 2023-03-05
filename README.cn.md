# Armbian / 岸边

查看英文说明 | [View English description](README.md)

[Armbian（中文名：岸边）](https://www.armbian.com/) 系统是基于 Debian/Ubuntu 而构建的专门用于 ARM 芯片的轻量级 Linux 系统。Armbian 系统精益、干净，并且 100% 兼容并继承了 Debian/Ubuntu 系统的功能和丰富的软件生态，可以安全稳定地运行在 TF/SD/USB 及设备的 eMMC 里。

现在你可以将电视盒子的安卓 TV 系统更换为 Armbian 系统，让他成为一台功能强大的服务器。本项目为 `Amlogic`，`Rockchip` 和 `Allwinner` 盒子构建 Armbian 系统。支持写入 eMMC 中使用，支持更新内核等功能。使用方法详见[Armbian 使用文档](./build-armbian/documents/README.cn.md)。

最新的 Armbian 固件可以在 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 中下载。欢迎 `Fork` 并进行个性化定制。如果对你有用，可以点仓库右上角的 `Star` 表示支持。

## Armbian 固件说明

| 芯片  | 设备 | [可选内核](https://github.com/ophub/kernel/tree/main/pub/stable) | Armbian 固件 |
| ---- | ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/99) | 全部 | aml_a311d.img |
| s922x | [Beelink-GT-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/370), [Beelink-GT-King-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/707), [Ugoos-AM6-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/213), [ODROID-N2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/201), [X88-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/988) | 全部 | aml_s922x.img |
| s905x3 | [X96-Max+](https://github.com/ophub/amlogic-s9xxx-armbian/issues/351), [HK1-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/414), [Vontar-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1006), [H96-Max-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/850), [Ugoos-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/782), [TX3(QZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/644), [TX3(BZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1077), [X96-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/366), [X96-Max+_A100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/779), [A95XF3-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/157), [Tencent-Aurora-3Pro(s905x3-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/506), [X96-Max+Q1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/788), [X96-Max+100W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/909), [X96-Max+_2101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1086) | 全部 | aml_s905x3.img |
| s905x2 | [X96Max-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/453), [X96Max-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/95), [MECOOL-KM3-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/79), [Tanix-Tx5-Max](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/351), [A95X-F2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/851) | 全部 | aml_s905x2.img |
| s912 | [Tanix-TX8-Max](https://github.com/ophub/amlogic-s9xxx-armbian/issues/500), [Tanix-TX9-Pro(3G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/315), [Tanix-TX9-Pro(2G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/740), [Tanix-TX92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1012790770), [Nexbox-A1](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [Nexbox-A95X-A2](https://www.cafago.com/en/p-v2979eu-2g.html),  [A95X](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [H96-Pro-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1013071513), [VORKE-Z6-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72), [Mecool-M8S-PRO-L](https://github.com/ophub/amlogic-s9xxx-armbian/issues/158), [Vontar-X92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/525), [T95Z-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/668), [Octopus-Planet](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1020), [Phicomm-T1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/522) | 全部 | aml_s912.img |
| s905d | [MECOOL-KI-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/59), [Phicomm-N1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/925) | 全部 | aml_s905d.img |
| s905x | [HG680P](https://github.com/ophub/amlogic-s9xxx-armbian/issues/368), [B860H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/60), [TBee-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/98), [T95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/285), [TX9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/645), [Q96-mini(s905l-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/734) | 全部 | aml_s905x.img |
| s905w | [X96-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/621), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1062), [W95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/570), [X96W/FunTV](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1044) | 全部 | aml_s905w.img |
| s905 | [Beelink-Mini-MX-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/127), [Sunvell-T95M](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/337), [MXQ-Pro+4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/715) | 全部 | aml_s905.img |
| s905l3a | [E900V22C/D](https://github.com/Calmact/e900v22c), [CM311-1a-YST](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/732), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [UNT413A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [ZTE-B863AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/741) | 全部 | aml_s905l3a.img |
| s905lb/3b | [M302A/M304A](https://github.com/ophub/amlogic-s9xxx-armbian/pull/615), [E900V22E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/939), [BesTV-R3300L(s905l-b)](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993) | 全部 | aml_s905l2.img<br />aml_s905lb-r3300l.img |
| s905l2/3 | [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [MGV3000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921), [Wojia-TV-IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [CM311-1(s905l3)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/763), [HG680-LC(s905l3)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/978), [M401A(s905l3)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921#issuecomment-1453143251) | 全部 | aml_s905l2.img |
| rk3588 | [Radxa-Rock5B](https://wiki.radxa.com/Rock5/5b), [HinLink-H88K](http://www.hinlink.com/index.php?id=138) | [rk3588](https://github.com/ophub/kernel/tree/main/pub/rk3588) | rockchip_boxname.img |
| rk3568 | [FastRhino-R66S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1061), [FastRhino-R68S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/774), [HinLink-H66K](http://www.hinlink.com/index.php?id=137), [HinLink-H68K](http://www.hinlink.com/index.php?id=119), [Radxa-E25](https://wiki.radxa.com/Rock3/CM/CM3I/E25) | [6.x.y](https://github.com/ophub/kernel/tree/main/pub/stable) | rockchip_boxname.img |
| rk3399 | [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [KING3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [KYLIN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132) | [6.x.y](https://github.com/ophub/kernel/tree/main/pub/stable) | rockchip_boxname.img |
| rk3328 | [beikeyun](https://github.com/ophub/amlogic-s9xxx-armbian/issues/852), [l1pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1007) | 全部 | rockchip_boxname.img |
| allwinner | [vplus(h6)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1100) | 全部 | allwinner_boxname.img |

💡提示：当前 ***`s905`*** 的盒子只能在 `TF/SD/USB` 中使用，其他型号的盒子同时支持写入 `EMMC` 中使用。更多信息请查阅[支持的设备列表说明](build-armbian/documents/amlogic_model_database.md)。

## 安装及升级 Armbian 的相关说明

选择和你的盒子型号对应的 Armbian 固件，使用 [Rufus](https://rufus.ie/) 或者 [balenaEtcher](https://www.balena.io/etcher/) 等工具将固件写入 USB 里，然后把写好固件的 USB 插入盒子。

- ### 安装 Armbian 到 EMMC

1. `Rockchip` 平台的安装方法请查看说明文档中的 [第 8 章节](build-armbian/documents/README.cn.md)。

2. `Amlogic` 和 `Allwinner` 平台，登录 Armbian 系统 (默认用户: root, 默认密码: 1234) → 输入命令：

```yaml
armbian-install
```

| 可选参数  | 默认值   | 选项     | 说明                |
| -------  | ------- | ------  | -----------------   |
| -m       | no      | yes/no  | 使用 Mainline u-boot |
| -a       | yes     | yes/no  | 使用 [ampart](https://github.com/7Ji/ampart) 分区表调整工具 |
| -l       | no      | yes/no  | List. 显示全部设备列表 |

举例: `armbian-install -m yes -a no`

- ### 更新 Armbian 内核

登录 Armbian 系统 → 输入命令：

```yaml
# 使用 root 用户运行 (sudo -i)
# 如果不指定参数，将更新为最新版本。
armbian-update
```

| 可选参数  | 默认值     | 选项       | 说明               |
| -------  | --------  | --------  | ----------------  |
| -k       | 最新版     | [内核名称](https://github.com/ophub/kernel/tree/main/pub/stable) | 设置更新内核名称  |
| -v       | stable    | stable/rk3588/dev | 指定内核版本分支     |
| -m       | no        | yes/no    | 使用主线 u-boot     |
| -b       | yes       | yes/no    | 更新内核时自动备份当前系统使用的内核    |
| -r       | ""        | ""        | [救援] 使用 USB 中的系统内核更新 eMMC |

举例: `armbian-update -k 5.15.50 -v dev -m yes`

如果当前目录下有成套的内核文件，将使用当前目录的内核进行更新（更新需要的 4 个内核文件是 `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-xxx.tar.gz`, `modules-xxx.tar.gz`。其他内核文件不需要，如果同时存在也不影响更新，系统可以准确识别需要的内核文件）。如果当前目录没有内核文件，将从服务器查询并下载同系列的最新内核进行更新。在设备支持的可选内核里可以自由更新，如从 5.10.125 内核更新为 5.15.50 内核。

更新内核时会自动备份当前系统使用的内核，存储路径在 `/ddbr/backup` 目录里，如果不需要可以删除。

💡 因特殊原因导致的更新不完整等问题，造成系统无法从 eMMC 启动时，可以从 USB 中启动任意内核版本的 Armbian 系统，运行 `armbian-update -r` 命令可以把 USB 中的系统内核更新至 eMMC 中，实现救援的目的。

- ### 安装常用软件

登录 Armbian 系统 → 输入命令：

```yaml
armbian-software
```

使用 `armbian-software -u` 命令可以更新本地的软件中心列表。根据用户在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中的需求反馈，逐步整合常用[软件](build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf)，实现一键安装/更新/卸载等快捷操作。包括 `docker 镜像`、`桌面软件`、`应用服务` 等。详见更多[说明](build-armbian/documents/armbian_software.md)。

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
armbian-openvfd
```

根据 [LED 屏显示控制说明](build-armbian/documents/led_screen_display_control.md) 进行调试。

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

- ### 在 Armbian 中编译内核

在 Armbian 中编译内核的用法详见 [编译内核](compile-kernel/README.cn.md) 说明文档。登录 Armbian 系统 → 输入命令：

```yaml
armbian-kernel -u
armbian-kernel -k 5.10.125
```

- ### 更多使用说明

将本地系统中的全部服务脚本更新到最新版本，可以登录 Armbian 系统 → 输入命令：

```yaml
armbian-sync
```

在 Armbian 的使用中，一些可能遇到的常见问题详见 [documents](build-armbian/documents/README.cn.md)

## 本地化打包

1. 克隆仓库到本地 `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. 安装必要的软件包（脚本仅在 x86_64 Ubuntu-20.04/22.04 下做过测试）

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-22.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2204-build-armbian-depends)
```

3. 进入 `~/amlogic-s9xxx-armbian` 根目录，在根目录下创建文件夹 `build/output/images` ，并上传 Armbian 镜像文件 ( 如：`Armbian_21.11.0-trunk_Odroidn2_current_5.15.50.img` ) 到 `~/amlogic-s9xxx-armbian/build/output/images` 目录里。原版 Armbian 镜像文件名称中的发行版本号（如：`21.11.0`）和内核版本号（如：`5.15.50`）请保留，它将在重构后用作 Armbian 固件的名称。

4. 进入 `~/amlogic-s9xxx-armbian` 根目录，然后运行 `sudo ./rebuild -b s905x3 -k 5.10.125` 命令即可生成指定 board 的 Armbian 镜像文件。生成的文件保存在 `build/output/images` 目录里。

- ### 本地化打包参数说明

| 参数  | 含义       | 说明        |
| ---- | ---------- | ---------- |
| -b   | Board      | 指定电视盒子型号，如 `-b s905x3` . 多个型号使用 `_` 进行连接，如 `-b s905x3_s905d` . 使用 `all` 表示全部型号。型号代码详见 [model_database.conf](build-armbian/armbian-files/common-files/etc/model_database.conf) 中的 `BOARD` 设置。 |
| -k   | Kernel     | 指定 [kernel](https://github.com/ophub/kernel/tree/main/pub/stable) 名称，如 `-k 5.10.125` . 多个内核使用 `_` 进行连接，如 `-k 5.10.125_5.15.50` |
| -a   | AutoKernel | 设置是否自动采用同系列最新版本内核。当为 `true` 时，将自动在内核库中查找在 `-k` 中指定的内核如 5.10.125 的同系列是否有更新的版本，如有 5.10.125 之后的最新版本时，将自动更换为最新版。设置为 `false` 时将编译指定版本内核。默认值：`true` |
| -v   | Version    | 指定内核 [版本分支](https://github.com/ophub/kernel/tree/main/pub) 名称，如 `-v stable_rk3588` 。指定的名称须与分支目录名称相同。默认使用 `stable_rk3588` 分支版本。 |
| -r   | KernelRepo | 指定内核仓库地址，如 `-r https://github.com/ophub/kernel/tree/main/pub` |
| -s   | Size       | 对固件的 ROOTFS 分区大小进行设置，默认大小为 2560MiB, 固件大小必须大于 2048MiB. 例如： `-s 2560` |
| -t   | RootfsType | 对固件的 ROOTFS 分区的文件系统类型进行设置，默认为 `ext4` 类型，可选项为 `ext4` 或 `btrfs` 类型。例如： `-t btrfs` |
| -n   | CustomName | 设置固件名称中的签名部分。默认值为空。可根据需要添加签名如 `_server`，`_gnome_desktop` 或 `_ophub` 等，设置自定义签名时请勿包含空格。 |
| -g   | GH_TOKEN   | 可选项。设置 ${{ secrets.GH_TOKEN }}，用于 [api.github.com](https://docs.github.com/en/rest/overview/resources-in-the-rest-api?apiVersion=2022-11-28#requests-from-personal-accounts) 查询。默认值：无 |

- `sudo ./rebuild` : 使用默认配置，对全部型号的电视盒子进行打包。
- `sudo ./rebuild -b s905x3 -k 5.10.125` : 推荐使用. 使用默认配置进行相关内核打包。
- `sudo ./rebuild -b s905x3_s905d -k 5.10.125_5.15.50` : 使用默认配置，进行多个内核同时打包。使用 `_` 进行多内核参数连接。
- `sudo ./rebuild -b s905x3 -k 5.10.125 -s 2560` : 使用默认配置，指定一个内核，一个型号进行打包，固件大小设定为2560MiB。
- `sudo ./rebuild -b s905x3 -v dev -k 5.10.125` : 使用默认配置，指定型号，指定版本分支，指定内核进行打包。
- `sudo ./rebuild -b s905x3_s905d`  使用默认配置，对多个型号的电视盒子进行全部内核打包, 使用 `_` 进行多型号连接。
- `sudo ./rebuild -k 5.10.125_5.15.50` : 使用默认配置，指定多个内核，进行全部型号电视盒子进行打包, 内核包使用 `_` 进行连接。
- `sudo ./rebuild -k 5.10.125_5.15.50 -a true` : 使用默认配置，指定多个内核，进行全部型号电视盒子进行打包, 内核包使用 `_` 进行连接。自动升级到同系列最新内核。
- `sudo ./rebuild -t btrfs -s 2560 -k 5.10.125` : 使用默认配置，设置文件系统为 btrfs 格式，分区大小为 2560MiB, 并指定内核为 5.10.125 ，对全部型号电视盒子进行打包。

## 使用 GitHub Actions 进行编译

1. 关于 Workflows 文件的配置在 [build-armbian.yml](.github/workflows/build-armbian.yml) 文件里。

2. 全新编译：在 [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 ***`Build armbian`*** ，根据 Armbian 官方支持的 OS 版本，可以选择 Ubuntu 系列：`jammy`，或者 Debian 系列：`bullseye` 等。点击 ***`Run workflow`*** 按钮即可编译。更多参数的设置方法可以在 [Armbian 官方文档](https://docs.armbian.com/Developer-Guide_Build-Options/) 里查阅。

3. 再次编译：如果 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 中有已经编译好的 `Armbian_.*-trunk_.*.img.gz` 文件，你只是想再次制作其他不同 board 的盒子，可以跳过 Armbian 源文件的编译，直接进行二次制作。在 [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面中选择  ***`Use Releases file to build armbian`*** ，点击 ***`Run workflow`*** 按钮即可二次编译。

4. 使用其他 Armbian 固件，如 Armbian 官方固件下载网站 [armbian.tnahosting.net](https://armbian.tnahosting.net/dl/) 提供的 [odroidn2](https://armbian.tnahosting.net/dl/odroidn2/archive/) 固件，仅在流程控制文件 [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml) 中引入本仓库的脚本进行 Armbian 重构，即可适配其他盒子的使用。在 [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 ***`Rebuild armbian`*** ，输入 Armbian 的网络下载地址如 `https://dl.armbian.com/*/Armbian_*.img.xz` ，或者在流程控制文件 [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml) 中通过 `armbian_path` 参数设定重构文件的加载路径。代码如下:

```yaml
- name: Rebuild Armbian
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img
    armbian_board: s905d_s905x3_s922x_s905x
    armbian_kernel: 5.10.125_5.15.50
    gh_token: ${{ secrets.GH_TOKEN }}
```

- ### GitHub Actions 输入参数说明

关于 GitHUB RELEASES_TOKEN 的相关设置可参考：[RELEASES_TOKEN](build-armbian/documents/README.cn.md#3-fork-仓库并设置-gh_token)。相关参数与`本地打包命令`相对应，请参考上面的说明。

| 参数              | 默认值             | 说明                                             |
|------------------|-------------------|--------------------------------------------------|
| armbian_path     | 无                | 设置原版 Armbian 文件的路径，支持使用当前工作流中的文件路径如 `build/output/images/*.img` ，也支持使用网络下载地址如： `https://dl.armbian.com/*/Armbian_*.img.xz` |
| armbian_board    | s905d_s905x3      | 设置打包盒子的 `board` ，功能参考 `-b`                 |
| armbian_kernel   | 5.10.125_5.15.50  | 设置内核 [版本](https://github.com/ophub/kernel/tree/main/pub/stable)，功能参考 `-k` |
| auto_kernel      | true              | 设置是否自动采用同系列最新版本内核，功能参考 `-a`       |
| version_branch   | stable_rk3588     | 指定内核 [版本分支](https://github.com/ophub/kernel/tree/main/pub) 名称，功能参考 `-v` |
| kernel_repo      | [ophub/kernel](https://github.com/ophub/kernel/tree/main/pub) | 指定内核仓库地址，功能参考 `-r` |
| armbian_size     | 2560              | 设置固件 ROOTFS 分区的大小，功能参考 `-s`            |
| armbian_fstype   | ext4              | 设置固件 ROOTFS 分区的文件系统类型，功能参考 `-t`     |
| armbian_sign     | 无                | 设置固件名称中的签名部分，功能参考 `-n`               |
| gh_token         | 无                | 可选项。设置 ${{ secrets.GH_TOKEN }}。功能参考 `-g`      |

- ### GitHub Actions 输出变量说明

上传到 `Releases` 需要给仓库添加 `${{ secrets.GITHUB_TOKEN }}` 和 `${{ secrets.GH_TOKEN }}` 并设置 `Workflow 读写权限`，详见[使用说明](build-armbian/documents/README.cn.md#2-设置隐私变量-github_token)。

| 参数                                      | 默认值             | 说明                       |
|------------------------------------------|-------------------|---------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}           | out               | 打包后的固件所在文件夹的路径   |
| ${{ env.PACKAGED_OUTPUTDATE }}           | 04.13.1058        | 打包日期（月.日.时分）        |
| ${{ env.PACKAGED_STATUS }}               | success           | 打包状态：success / failure |

## Armbian 固件默认信息

| 名称 | 值 |
| ---- | ---- |
| 默认 IP | 从路由器获取 IP |
| 默认账号 | root |
| 默认密码 | 1234 |

## 使用 GitHub Actions 编译内核

内核的编译方法详见 [compile-kernel](compile-kernel/README.cn.md)

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.10.125_5.15.50
    kernel_auto: true
    kernel_sign: -yourname
```

## Armbian 贡献者

首先感谢 [150balbes](https://github.com/150balbes) 为在 Amlogic 电视盒子中使用 Armbian 所做出的杰出贡献和奠定的良好基础。这里编译的 [Armbian](https://github.com/armbian/build) 系统直接使用了官方当前的最新源码进行实时编译。程序的开发思路来自 [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box) 等作者的教程。感谢各位的奉献和分享，让我们可以在更多盒子里使用 Armbian 系统。

本系统所使用的 `kernel` / `u-boot` 等资源主要从 [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) 的项目中复制而来，部分文件由用户在 [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) / [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian) / [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic) / [kernel](https://github.com/ophub/kernel) 等项目的 [Pull](https://github.com/ophub/amlogic-s9xxx-armbian/pulls) 和 [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中提供分享。为感谢这些开拓者和分享者，从现在开始（本源代码库创建于2021-09-19），我统一在 [CONTRIBUTORS.md](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/CONTRIBUTORS.md) 中进行了记录。再次感谢大家为盒子赋予了新的生命和意义。

## 其他发行版

- [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) 项目提供了在盒子中使用的 `OpenWrt` 系统，在支持 Armbian 的相关设备中同样适用。
- [unifreq](https://github.com/unifreq/openwrt_packit) 为晶晨、瑞芯微和全志等更多盒子制作了 `OpenWrt` 系统，属于盒子圈的标杆，推荐使用。
- [Scirese](https://github.com/Scirese/alarm) 在安卓电视盒子里测试了 `Arch Linux ARM` / `Manjaro` 系统的制作、安装和使用，具体详见他仓库中的相关说明。
- [7Ji](https://7ji.github.io/) 在他的博客中发表了一些在 Amlogic 平台上的逆向工程和开发的文章，比如以 ArchLinux 的方式安装 ArchLinux ARM 系统，对 Amlogic 平台的启动机制的介绍等。在他的 [ampart](https://github.com/7Ji/ampart) 项目中，提供了一款分区工具，能够读取并编辑 Amlogic 的 eMMC 分区表和 DTB 内分区，可以 100% 利用 eMMC 空间。在 [amlogic-s9xxx-archlinuxarm](https://github.com/7Ji/amlogic-s9xxx-archlinuxarm) 项目中提供了 `Arch Linux ARM` 系统的制作和使用方法。

## 链接

- [armbian](https://github.com/armbian/build)
- [unifreq](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## License

The amlogic-s9xxx-armbian © OPHUB is licensed under [GPL-2.0](LICENSE)

