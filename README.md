# Armbian

View Chinese description | [查看中文说明](README.cn.md)

[Armbian](https://www.armbian.com/) is a lightweight Linux system specially built for ARM chips based on Debian/Ubuntu. The Armbian system is lean, clean, and 100% compatible with and inherits the features and rich software ecosystem of the Debian/Ubuntu system. It can run securely and stably in TF/SD/USB and the device's eMMC. Now you can replace your Android TV system with the Armbian system, turning it into a powerful server.

This project relies on many [contributors](CONTRIBUTORS.md) to build the Armbian system for `Amlogic`, `Rockchip`, and `Allwinner` boxes, supports writing to eMMC for use, supports updating the kernel and other features. Detailed usage can be found in the [Armbian User Documentation](./documents). The latest Armbian system can be downloaded from [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases). Feel free to exchange and share in the [Armbian Discussion Area](https://github.com/ophub/amlogic-s9xxx-armbian/discussions). Welcome to `Fork` and personalize. If it's useful to you, you can click `Star` in the upper right corner of the repository to show support.

## Armbian System Description

| SoC | Device | [Kernel](https://github.com/ophub/kernel) | [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian/releases) |
| ---- | ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/99) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_a311d.img |
| s922x | [Beelink-GT-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/370), [Beelink-GT-King-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/707), [Ugoos-AM6-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/464), [ODROID-N2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/201), [X88-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/988), [Ali-CT2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1150) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s922x.img |
| s905x3 | [X96-Max+](https://github.com/ophub/amlogic-s9xxx-armbian/issues/351), [HK1-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/414), [Vontar-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1006), [H96-Max-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1250), [Ugoos-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/782), [TX3(QZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/644), [TX3(BZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1077), [X96-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/366), [X96-Max+_A100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/779), [A95XF3-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/157), [Tencent-Aurora-3Pro(s905x3-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/506), [X96-Max+Q1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/788), [X96-Max+100W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/909), [X96-Max+_2101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1086), [Infinity-B32](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1181), [Whale](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1166), [X88-Pro-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [X99-Max-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [Transpeed-X3-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905x3.img |
| s905x2 | [X96Max-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/453), [X96Max-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/95), [MECOOL-KM3-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/79), [Tanix-Tx5-Max](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/351), [A95X-F2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/851) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905x2.img |
| s912 | [Tanix-TX8-Max](https://github.com/ophub/amlogic-s9xxx-armbian/issues/500), [Tanix-TX9-Pro(3G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/315), [Tanix-TX9-Pro(2G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/740), [Tanix-TX92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1012790770), [Nexbox-A1](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [Nexbox-A95X-A2](https://www.cafago.com/en/p-v2979eu-2g.html),  [A95X](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [H96-Pro-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1013071513), [VORKE-Z6-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72), [Mecool-M8S-PRO-L](https://github.com/ophub/amlogic-s9xxx-armbian/issues/158), [Vontar-X92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/525), [T95Z-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/668), [Octopus-Planet](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1020), [Phicomm-T1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/522) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s912.img |
| s905d | [MECOOL-KI-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/59), [Phicomm-N1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/925), [SML-5442TW](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/451) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905d.img |
| s905x | [HG680P](https://github.com/ophub/amlogic-s9xxx-armbian/issues/262), [B860H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/60), [TBee-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/98), [T95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/285), [TX9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/645), [XiaoMI-3S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1405), [X96](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1480) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905x.img |
| s905w | [X96-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/621), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1062), [W95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/570), [X96W/FunTV](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1044), [MXQ-Pro-4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1140) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905w.img |
| s905l | [UNT402A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1481), [M201-S](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/444) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l.img |
| s905l2 | [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [MGV3000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921), [Wojia-TV-IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [M301A](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/405), [E900v21E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1278) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l2.img |
| s905l3 | [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1318), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/763), [HG680-LC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/978), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921#issuecomment-1453143251), [UNT400G1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1277), [UNT402A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1481), [ZXV10-BV310](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1512) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l3.img |
| s905l3a | [E900V22C/D](https://github.com/Calmact/e900v22c), [CM311-1a-YST](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/732), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [UNT413A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [ZTE-B863AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/741), [CM311-1a-CH](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1508), [IP112H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1520) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l3a.img |
| s905l3b | [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1180), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1268), [E900V22D](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1256), [E900V21E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1514), [E900V22E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/939), [M302A/M304A](https://github.com/ophub/amlogic-s9xxx-armbian/pull/615), [Hisense-IP103H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1154), [TY1608](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1332), [TY1608](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1568), [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1613), [B860AV-2.1M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1598) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l3b.img |
| s905lb | [Q96-mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/734), [BesTV-R3300L](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993), [SumaVision-Q7](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1190), [MG101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1570) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905lb.img |
| s905 | [Beelink-Mini-MX-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/127), [Sunvell-T95M](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/337), [MXQ-Pro+4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/715), [SumaVision-Q5](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1175) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905.img |
| rk3588 | [Radxa-Rock5B](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1240), [HinLink-H88K](http://www.hinlink.com/index.php?id=138), [Beelink-IPC-R](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415) | [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) | rockchip_boxname.img |
| rk3568 | [FastRhino-R66S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1061), [FastRhino-R68S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/774), [HinLink-H66K](http://www.hinlink.com/index.php?id=137), [HinLink-H68K](http://www.hinlink.com/index.php?id=119), [HinLink-H69K](http://www.hinlink.com/index.php?id=119), [Radxa-E25](https://wiki.radxa.com/Rock3/CM/CM3I/E25), [NanoPi-R5S](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217), [NanoPi-R5C](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217) | [6.x.y](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| rk3566 | [Panther-X2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1319) | [6.x.y](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| rk3399 | [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132), [ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247), [CRRC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1280), [SMART-AM40](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1317), [SW799](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1326), [ZYSJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380), [DG-3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1492), [DLFR100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522), [Emb3531](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1549), [Leez-p710](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609) | [6.x.y](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| rk3328 | [BeikeYun](https://github.com/ophub/amlogic-s9xxx-armbian/issues/852), [L1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1007), [Station-M1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Bqeel-MVR9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| h6 | [Vplus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1100), [Tanix-TX6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1120), [Tanix-TX6-A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1612) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | allwinner_boxname.img |

💡 Tip: At present, the [s905 box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1173) can only be used in `TF/SD/USB`. Other models of boxes support writing to `EMMC` for use. For more information, please refer to the [Supported Device List Description](build-armbian/armbian-files/common-files/etc/model_database.conf). You can refer to the method in Section 12.15 of the instruction document to [add new supported devices](documents/README.md#1215-how-to-add-new-supported-devices).

## Installation and Upgrade Instructions for Armbian

Choose the Armbian system that corresponds to your box model, and refer to the corresponding instructions for different devices.

- ### Install Armbian to EMMC

1. For `Rockchip` platform, please refer to the [Chapter 8](documents) in the instruction document.

2. For `Amlogic` and `Allwinner` platforms, use tools such as [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/) to write the system to a USB stick, then insert the USB stick with the written system into the box. Log in to the Armbian system (default user: root, default password: 1234) → Enter the command:

```shell
armbian-install
```

| Optional | Default | Options | Description       |
| -------- | ------- | ------- | ----------------- |
| -m       | no      | yes/no  | Use Mainline u-boot |
| -a       | yes     | yes/no  | Use [ampart](https://github.com/7Ji/ampart) partition adjustment tool |
| -l       | no      | yes/no  | List. Display all device lists |

Example: `armbian-install -m yes -a no`

- ### Update Armbian Kernel

Log in to the Armbian system → Enter the command:

```shell
# Run as root user (sudo -i)
# If no parameter is specified, it will be updated to the latest version.
armbian-update
```

| Optional | Default      | Options       | Description                      |
| -------- | ------------ | ------------- | -------------------------------- |
| -r       | ophub/kernel | `<owner>/<repo>` | Set the repository to download the kernel from github.com |
| -u       | Automation   | stable/flippy/dev/beta/rk3588 | Set the [tags suffix](https://github.com/ophub/kernel/releases) of the kernel in use |
| -k       | Latest version | Kernel version | Set the [kernel version](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| -c       | None         | Custom domain name | Set the cdn domain name for accelerated access to github.com |
| -b       | yes          | yes/no        | Automatically back up the current system's kernel when updating the kernel |
| -m       | no           | yes/no        | Use mainline u-boot |
| -s       | None         | None          | [SOS] Use the system kernel in USB to restore eMMC |
| -h       | None         | None          | View help |

Example: `armbian-update -k 5.15.50 -u dev`

When specifying the kernel version number through the `-k` parameter, you can accurately specify a specific version number, for example: `armbian-update -k 5.15.50`, or you can specify the kernel series vaguely, for example: `armbian-update -k 5.15`. When vaguely specified, the latest version of the specified series will be automatically used.

The current system's kernel is automatically backed up when updating the kernel. The storage path is in the `/ddbr/backup` directory. The three most recent versions of the kernel used are retained. If the newly installed kernel is unstable, you can restore the backed-up kernel at any time. If the update fails and the system cannot be started, you can use any version of Armbian to boot via USB to restore the system in eMMC. For more details, see the [Help Document](documents).

- ### Install Common Software

Log in to the Armbian system → Enter the command:

```shell
armbian-software
```

The command `armbian-software -u` can update the local software center list. Based on the demand feedback from users in [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), we gradually integrate commonly used [software](build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) to achieve one-click installation/update/uninstallation and other shortcut operations. This includes `docker images`, `desktop software`, `application services`, etc. For more details, see the [instructions](documents/armbian_software.md).

- ### Modify Armbian Configuration

Log in to the Armbian system → Enter the command:

```shell
armbian-config
```

- ### Create swap for Armbian

If you feel that the current box's memory is insufficient when using applications that consume a lot of memory, such as `docker`, you can create a `swap` virtual memory partition, virtually using a certain capacity of disk space as memory. The unit of the input parameter of the following command is `GB`, the default is `1`.

Log in to the Armbian system → Enter the command:

```shell
armbian-swap 1
```

- ### Control LED Display

Log in to the Armbian system → Enter the command:

```shell
armbian-openvfd
```

Debug according to the [LED Screen Display Control Instructions](documents/led_screen_display_control.md).

- ### Backup/Restore EMMC Original System

Supports backing up/restoring the `EMMC` partition of the box in `TF/SD/USB`. Before installing the Armbian system in a brand new box, it is recommended that you back up the Android TV system that comes with the current box so that it can be used in the future to restore the TV system and other situations.

Please boot the Armbian system from `TF/SD/USB` → Enter the command:

```shell
armbian-ddbr
```

Enter `b` according to the prompt to back up the system, and enter `r` to restore the system.

- ### Compile the Kernel in Armbian

For the usage of compiling the kernel in Armbian, please refer to the [Compile Kernel](compile-kernel) instruction document. Log in to the Armbian system → Enter the command:

```shell
armbian-kernel -u
armbian-kernel -k 5.10.125
```

- ### More Usage Instructions

To update all service scripts in the local system to the latest version, you can log in to the Armbian system → Enter the command:

```shell
armbian-sync
```

In the use of Armbian, some common problems that may be encountered can be found in [documents](documents)

## Local Packaging

1. Clone the repository to local `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install the necessary software packages (scripts have only been tested under x86_64 Ubuntu-20.04/22.04)

```shell
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-22.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2204-build-armbian-depends)
```

3. Enter the `~/amlogic-s9xxx-armbian` root directory, create a `build/output/images` folder in the root directory, and upload the Armbian image file (such as `Armbian_21.11.0-trunk_Odroidn2_current_5.15.50.img`) to `~/amlogic-s9xxx-armbian/build/output/images` directory. Please keep the release version number (such as `21.11.0`) and the kernel version number (such as `5.15.50`) in the original Armbian image file name. It will be used as the name of the Armbian system after restructuring.

4. Enter the `~/amlogic-s9xxx-armbian` root directory, and then run the `sudo ./rebuild -b s905x3 -k 5.10.125` command to generate the Armbian image file for the specified board. The generated file is saved in the `build/output/images` directory.

- ### Local Packaging Parameter Description

| Parameter | Meaning     | Description |
| ----      | ----------  | ----------  |
| -b        | Board      | Specify the TV box model, such as `-b s905x3`. Connect multiple models with `_`, such as `-b s905x3_s905d`. Use `all` to represent all models. The model code can be found in the `BOARD` setting in [model_database.conf](build-armbian/armbian-files/common-files/etc/model_database.conf). Default value: `all` |
| -r        | KernelRepo | Specify the `<owner>/<repo>` of the github.com kernel repository. Default value: `ophub/kernel` |
| -u        | kernelUsage | Set the `tags suffix` of the kernel used, such as [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable), [flippy](https://github.com/ophub/kernel/releases/tag/kernel_flippy), [dev](https://github.com/ophub/kernel/releases/tag/kernel_dev), [beta](https://github.com/ophub/kernel/releases/tag/kernel_beta). Default value: `stable` |
| -k        | Kernel     | Specify [kernel](https://github.com/ophub/kernel/releases/tag/kernel_stable) name, such as `-k 5.10.125`. Connect multiple kernels with `_`, such as `-k 5.10.125_5.15.50`. The kernel version freely specified by the `-k` parameter is only valid for kernels using `stable/flippy/dev/beta`. Other kernel series such as [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) can only use specific kernels. |
| -a        | AutoKernel | Set whether to automatically adopt the latest version of the same series of kernels. When it is `true`, it will automatically look for whether there is a newer version of the same series in the kernel library in the kernel specified in `-k`, such as 5.10.125. If there is a latest version after 5.10.125, it will be automatically changed to the latest version. When set to `false`, it will compile the specified version of the kernel. Default value: `true` |
| -t        | RootfsType | Set the file system type of the system's ROOTFS partition, the options are `ext4` or `btrfs` type. For example: `-t btrfs`. Default value: `ext4` |
| -s        | Size       | Set the size of the system's ROOTFS partition, the system size must be greater than 2048MiB. For example: `-s 2560`. Default value: `2560` |
| -n        | BuilderName | Set the Armbian system builder signature. Do not include spaces when setting the signature. Default value: `None` |
| -g        | GH_TOKEN   | Optional. Set `${{ secrets.GH_TOKEN }}`, used for [api.github.com](https://docs.github.com/en/rest/overview/resources-in-the-rest-api?apiVersion=2022-11-28#requests-from-personal-accounts) queries. Default value: `None` |

- `sudo ./rebuild`: Use the default configuration to package all models of TV boxes.
- `sudo ./rebuild -b s905x3 -k 5.10.125`: Recommended. Use the default configuration to package related kernels.
- `sudo ./rebuild -b s905x3_s905d -k 5.10.125_5.15.50`: Use the default configuration, package multiple kernels at the same time. Use `_` to connect multiple kernel parameters.
- `sudo ./rebuild -b s905x3 -k 5.10.125 -s 2560`: Use the default configuration, specify a kernel, a model for packaging, the system size is set to 2560MiB.
- `sudo ./rebuild -b s905x3_s905d` Use the default configuration, package all kernels for multiple models of TV boxes, use `_` to connect multiple models.
- `sudo ./rebuild -k 5.10.125_5.15.50`: Use the default configuration, specify multiple kernels, and package all models of TV boxes, the kernel package uses `_` for connection.
- `sudo ./rebuild -k 5.10.125_5.15.50 -a true`: Use the default configuration, specify multiple kernels, and package all models of TV boxes, the kernel package uses `_` for connection. Automatically upgrade to the latest kernel of the same series.
- `sudo ./rebuild -t btrfs -s 2560 -k 5.10.125`: Use the default configuration, set the file system to btrfs format, the partition size is 2560MiB, and specify the kernel as 5.10.125, package for all models of TV boxes.

## Use GitHub Actions for Compilation

1. The configuration of the Workflows file can be found in the [build-armbian.yml](.github/workflows/build-armbian.yml) file.

2. Full compile: On the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Build armbian`***. According to the OS versions supported by Armbian official, you can choose the Ubuntu series: `jammy`, or the Debian series: `bullseye` etc. Click the ***`Run workflow`*** button to compile. More parameter setting methods can be found in the [Armbian official documentation](https://docs.armbian.com/Developer-Guide_Build-Options/).

3. Recompile: If there are already compiled `Armbian_.*-trunk_.*.img.gz` files in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases), and you just want to make another box of different boards, you can skip the compilation of Armbian source files and proceed with the second production directly. On the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Use Releases file to build armbian`***, and click the ***`Run workflow`*** button to compile again.

4. Use other Armbian systems, such as the [odroidn2](https://armbian.tnahosting.net/dl/odroidn2/archive/) system provided by the Armbian official system download website [armbian.tnahosting.net](https://armbian.tnahosting.net/dl/), and only introduce the script of this repository in the process control file [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml) to restructure Armbian, which can be adapted to the use of other boxes. On the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Rebuild armbian`***, input the network download address of Armbian like `https://dl.armbian.com/*/Armbian_*.img.xz`, or set the loading path of the restructuring file in the process control file [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml) through the `armbian_path` parameter. The code is as follows:

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

- ### GitHub Actions Input Parameter Description

About the settings of GitHUB RELEASES_TOKEN, you can refer to: [RELEASES_TOKEN](documents/README.md#3-fork-the-repository-and-set-up-gh_token). The related parameters correspond to the `local packaging command`, please refer to the above description.

| Parameter              | Default                  | Description                                             |
|------------------------|--------------------------|---------------------------------------------------------|
| armbian_path           | None                     | Set the path of the original Armbian file, support the file path in the current workflow such as `build/output/images/*.img`, and also support the network download address such as: `https://dl.armbian.com/*/Armbian_*.img.xz` |
| armbian_board          | all                      | Set the `board` of the package box, refer to `-b`                 |
| kernel_repo            | ophub/kernel             | Specify `<owner>/<repo>` of the github.com kernel repository, refer to `-r` |
| kernel_usage           | stable                   | Set the `tags suffix` of the used kernel. Refer to `-u` |
| armbian_kernel         | 6.1.1_5.15.1             | Set the [version](https://github.com/ophub/kernel/releases/tag/kernel_stable) of the kernel, refer to `-k` |
| auto_kernel            | true                     | Set whether to automatically adopt the latest version of the same series kernel, refer to `-a`       |
| armbian_fstype         | ext4                     | Set the file system type of the system's ROOTFS partition, refer to `-t`     |
| armbian_size           | 2560                     | Set the size of the system's ROOTFS partition, refer to `-s`            |
| builder_name           | None                     | Set the Armbian system builder signature, refer to `-n`           |
| gh_token               | None                     | Optional. Set `${{ secrets.GH_TOKEN }}`. Refer to `-g` |

- ### GitHub Actions Output Variable Description

To upload to `Releases`, you need to add `${{ secrets.GITHUB_TOKEN }}` and `${{ secrets.GH_TOKEN }}` to the repository and set `Workflow read/write permissions`. For details, see the [usage instructions](documents/README.md#2-set-up-private-variable-github_token).

| Parameter                              | Default                  | Description                       |
|----------------------------------------|--------------------------|-----------------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}         | out                      | Path of the folder where the packaged system is located   |
| ${{ env.PACKAGED_OUTPUTDATE }}         | 04.13.1058               | Packaging date (month.day.hourminute)        |
| ${{ env.PACKAGED_STATUS }}             | success                  | Packaging status: success / failure |

## Default Information for Armbian System

| Name | Value |
| ---- | ---- |
| Default IP | Obtain IP from router |
| Default account | root |
| Default password | 1234 |

## Compiling Kernel using GitHub Actions

For the method of compiling the kernel, refer to [compile-kernel](compile-kernel)

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.15.1_6.1.1
    kernel_auto: true
    kernel_sign: -yourname
```

## Armbian Contributors

First of all, thanks to [150balbes](https://github.com/150balbes) for the outstanding contributions and the solid foundation laid for using Armbian in Amlogic TV boxes. The [Armbian](https://github.com/armbian/build) system compiled here directly uses the latest source code from the official in real-time. The development ideas of the program come from tutorials from authors such as [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box). Thank you all for your dedication and sharing, allowing us to use the Armbian system in more boxes.

The `kernel` / `u-boot` and other resources used in this system are mainly copied from the [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) project, and some files are provided by users in [Pull](https://github.com/ophub/amlogic-s9xxx-armbian/pulls) and [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues) of projects such as [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt), [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic), and [kernel](https://github.com/ophub/kernel). To thank these pioneers and sharers, from now on (this source code repository was created on `2021-09-19`), I have recorded it uniformly in [CONTRIBUTORS.md](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/CONTRIBUTORS.md). Thanks again to everyone for giving the box a new life and meaning.

## Other Distributions

- The [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) project provides the `OpenWrt` system used in the box, which is also applicable to devices that support Armbian.
- [unifreq](https://github.com/unifreq/openwrt_packit) has made `OpenWrt` system for more boxes such as Amlogic, Rockchip, and Allwinner, which is a benchmark in the box circle and recommended to use.
- [Scirese](https://github.com/Scirese/alarm) tested the production, installation, and use of `Arch Linux ARM` / `Manjaro` system in Android TV boxes, for details refer to the relevant instructions in his repository.
- [7Ji](https://7ji.github.io/) has published some articles on reverse engineering and development on the Amlogic platform in his blog, such as installing the ArchLinux ARM system in the way of ArchLinux, an introduction to the boot mechanism of the Amlogic platform, etc. In his [ampart](https://github.com/7Ji/ampart) project, a partition tool is provided that can read and edit the Amlogic eMMC partition table and DTB internal partition, and can utilize 100% of eMMC space. The [amlogic-s9xxx-archlinuxarm](https://github.com/7Ji/amlogic-s9xxx-archlinuxarm) project provides the production and use method of `Arch Linux ARM` system. In the [YAopenvfD](https://github.com/7Ji/YAopenvfD) project, another openvfd daemon is provided.

## Links

- [armbian](https://github.com/armbian/build)
- [unifreq](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## License

The amlogic-s9xxx-armbian © OPHUB is licensed under [GPL-2.0](LICENSE)

