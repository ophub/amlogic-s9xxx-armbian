# Armbian

Read the Chinese version: [查看中文说明](README.cn.md)

[Armbian](https://www.armbian.com/) is a lightweight Linux-based operating system based on Debian/Ubuntu built specifically for ARM devices. Armbian is lean, clean, 100% compatible and inherits the functions and rich software ecosystem of Debian/Ubuntu. It can run safely and stably from TF/SD/USB and can be installed to the eMMC on most TV boxes. Now you can replace the Android system of your TV box with Armbian, turning it into a powerful server.

This project relies on many [contributors](CONTRIBUTORS.md) to build Armbian systems for `Amlogic`, `Rockchip`, and `Allwinner` boxes. It supports writing to eMMC, updating the kernel, and other functions. Please refer to the [Armbian Documentation](./build-armbian/documents) for more information on Armbian. The latest Armbian builds can be downloaded from [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases). Welcome to the [Armbian Discussions](https://github.com/ophub/amlogic-s9xxx-armbian/discussions) to discuss and share. You are welcome to fork and personalize it. If it is useful to you then please click on the star in the upper right corner of the github repository page to show your support. Thanks!

## Armbian system specific instructions

| SoC  | Device | [Kernel](https://github.com/ophub/kernel) | [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian/releases) |
| ---- | ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/99) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_a311d.img |
| s922x | [Beelink-GT-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/370), [Beelink-GT-King-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/707), [Ugoos-AM6-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/213), [ODROID-N2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/201), [X88-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/988), [Ali-CT2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1150) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s922x.img |
| s905x3 | [X96-Max+](https://github.com/ophub/amlogic-s9xxx-armbian/issues/351), [HK1-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/414), [Vontar-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1006), [H96-Max-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1250), [Ugoos-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/782), [TX3(QZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/644), [TX3(BZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1077), [X96-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/366), [X96-Max+_A100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/779), [A95XF3-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/157), [Tencent-Aurora-3Pro(s905x3-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/506), [X96-Max+Q1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/788), [X96-Max+100W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/909), [X96-Max+_2101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1086), [Infinity-B32](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1181), [Whale](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1166) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905x3.img |
| s905x2 | [X96Max-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/453), [X96Max-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/95), [MECOOL-KM3-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/79), [Tanix-Tx5-Max](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/351), [A95X-F2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/851) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905x2.img |
| s912 | [Tanix-TX8-Max](https://github.com/ophub/amlogic-s9xxx-armbian/issues/500), [Tanix-TX9-Pro(3G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/315), [Tanix-TX9-Pro(2G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/740), [Tanix-TX92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1012790770), [Nexbox-A1](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [Nexbox-A95X-A2](https://www.cafago.com/en/p-v2979eu-2g.html),  [A95X](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [H96-Pro-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1013071513), [VORKE-Z6-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72), [Mecool-M8S-PRO-L](https://github.com/ophub/amlogic-s9xxx-armbian/issues/158), [Vontar-X92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/525), [T95Z-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/668), [Octopus-Planet](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1020), [Phicomm-T1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/522) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s912.img |
| s905d | [MECOOL-KI-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/59), [Phicomm-N1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/925) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905d.img |
| s905x | [HG680P](https://github.com/ophub/amlogic-s9xxx-armbian/issues/368), [B860H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/60), [TBee-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/98), [T95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/285), [TX9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/645), [XiaoMI-3S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1405) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905x.img |
| s905w | [X96-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/621), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1062), [W95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/570), [X96W/FunTV](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1044), [MXQ-Pro-4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1140) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905w.img |
| s905l2 | [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [MGV3000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921), [Wojia-TV-IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [M301A](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/405), [E900v21E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1278) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l2.img |
| s905l3 | [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1318), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/763), [HG680-LC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/978), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921#issuecomment-1453143251), [UNT400G1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1277) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l3.img |
| s905l3a | [E900V22C/D](https://github.com/Calmact/e900v22c), [CM311-1a-YST](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/732), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [UNT413A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [ZTE-B863AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/741) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l3a.img |
| s905l3b | [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1180), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1268), [E900V22D](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1256), [E900V22E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/939), [M302A/M304A](https://github.com/ophub/amlogic-s9xxx-armbian/pull/615), [Hisense-IP103H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1154), [TY1608](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1332) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905l3b.img |
| s905lb | [Q96-mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/734), [BesTV-R3300L](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993), [SumaVision-Q7](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1190) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905lb.img |
| s905 | [Beelink-Mini-MX-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/127), [Sunvell-T95M](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/337), [MXQ-Pro+4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/715), [SumaVision-Q5](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1175) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | amlogic_s905.img |
| rk3588 | [Radxa-Rock5B](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1240), [HinLink-H88K](http://www.hinlink.com/index.php?id=138), [Beelink-IPC-R](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415) | [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) | rockchip_boxname.img |
| rk3568 | [FastRhino-R66S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1061), [FastRhino-R68S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/774), [HinLink-H66K](http://www.hinlink.com/index.php?id=137), [HinLink-H68K](http://www.hinlink.com/index.php?id=119), [HinLink-H69K](http://www.hinlink.com/index.php?id=119), [Radxa-E25](https://wiki.radxa.com/Rock3/CM/CM3I/E25), [NanoPi-R5S](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217) | [6.x.y](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| rk3566 | [Panther-X2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1319) | [6.x.y](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| rk3399 | [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132), [ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247), [CRRC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1280), [SMART-AM40](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1317), [SW799](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1326), [ZYSJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380) | [6.x.y](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| rk3328 | [BeikeYun](https://github.com/ophub/amlogic-s9xxx-armbian/issues/852), [L1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1007), [Station-M1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Bqeel-MVR9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | rockchip_boxname.img |
| h6 | [Vplus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1100), [Tanix-TX6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1120) | [All](https://github.com/ophub/kernel/releases/tag/kernel_stable) | allwinner_boxname.img |

💡Tip: Currently [s905 Boxes](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1173) can only boot from TF/SD/USB. Most other TV boxes support installing Armbian to eMMC. Detailed configuration information of the device can be viewed in [the device model database](build-armbian/armbian-files/common-files/etc/model_database.conf).

## Generic installation instructions

The exact installation process for Armbian varies depending upon your devices chipset and model but these instructions are written to be as generic as possible and should apply to all TV boxes.

### Hardware support

You need to know the exact make and model of your box and which .dtb (Device Tree Blob) file you need to use to get your box to boot with all supported devices working. Under Android, go to **Settings -> System -> About phone** to find out the exact model of your TV box.

If you have got your box working but its not listed on either page then see section 12.15 of the documentation for details on how to [add support for new devices](build-armbian/documents/README.md#1215-how-to-add-new-supported-devices).


### Write the Armbian image to a disk

For instructions covering installing Armbian on Rockchip TV boxes, please refer to [Section 8](build-armbian/documents) of the documentation.

For Amlogic and Allwinner boxes，you will have to uncompress the installation image using `gunzip` first before writing the image to a TF, SD or USB disk using [Rufus](https://rufus.ie/), [balenaEtcher](https://www.balena.io/etcher/) or `gnome-disk-utility`. You could also use `dd` but that is not recommended.

After writing the Armbian image, you may need to edit the **FDT** line in `uEnv.txt` on the **BOOT** partition of the disk to use the correct **.dtb** file for your TV box.

To find the correct dtb to use with Armbian for your box, see [Description of Supported Device List](build-armbian/armbian-files/common-files/etc/model_database.conf) or search for your box in the list of system specific instructions at the top of this page.

### Enable multi-boot mode - Amlogic TV boxes only

If this is your first time attempting to boot from a SD card or USB disk on an Amlogic TV box, you must first enable multi-boot mode. Insert the Armbian disk then hold down the button in the AV port by inserting a paper clip, a pin or something similar and keep it depressed while powering the device on. Keep the button pressed until you see the boot screen of your box. After a few seconds it should reboot from your SD card or USB disk. You are only required to enable multi-boot mode once.

### Running `armbian-install`

On first boot, Armbian will resize its root partition to use all of the space available on its disk and it will ask you several system configuration questions like your timezone, keyboard layout, username and password etc. After logging in as **root** or using `sudo` run:

```bash
armbian-install
```

To start the eMMC installation script. The available options are:

| Optional  | Default  | Value    | Description           |
| --------- | -------  | -------- | --------------------  |
| -m        | no       | yes/no   | Use Mainline u-boot   |
| -a        | yes      | yes/no   | Use [ampart](https://github.com/7Ji/ampart) tool |
| -l        | no       | yes/no   | List all .dtb files         |

If you can boot Armbian fine from TF/SD/USB and the install script says it installed Armbian to eMMC OK but Armbian fails to boot from eMMC then you should try re-installing Armbian using mainline u-boot instead by running:

`armbian-install -m yes`

### Updating the Armbian Kernel

You won't be able to use all of your hardware properly unless you are running the correct kernel and using the right .dtb. Unfortuntely Armbian cannot auto-detect these for you. In most cases, you are recommended to run the latest kernel version available for your TV box but some features on some devices will only work on specific kernels. For example, 4K HDMI output only works on the X96 Max+ 2T when running a 5.10 series kernel, 4K output doesn't currently work under the 5.15.x or 6.x kernels for that specific model.

Login in to Armbian and run:

```bash
# Run as root user (sudo -i)
# If no parameter is specified, it will update to the latest version.
armbian-update
```

`armbian-update`'s options include:

| Optional  | Default      | Value          | Description                                                  |
| --------- | ------------ | -------------- | ------------------------------------------------------------ |
| -r        | ophub/kernel | `<owner>/<repo>` | Set the repository for downloading kernels from github.com |
| -u        | automate     | stable/flippy/dev/rk3588 | Set the [tags suffix](https://github.com/ophub/kernel/releases) of the kernel used |
| -k        | latest       | kernel-version | Set the [kernel version](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| -c        | None         | domain-name    | Set the cdn domain name for accelerated access to github.com |
| -b        | yes          | yes/no         | Automatically backup the current system kernel               |
| -m        | no           | yes/no         | Use Mainline u-boot                                          |
| -s        | None         | None           | [SOS] Restore eMMC with system kernel from USB               |
| -h        | None         | None           | View usage help                                              |

Example: `armbian-update -k 5.15.50 -u dev`

See here for a list of all [currently available stable kernel tarballs.](https://github.com/ophub/kernel/releases/tag/kernel_stable)

When updating the kernel, the current kernel will be automatically backed up. The three most recently used kernels are stored in the `/ddbr/backup` directory. If the newly installed kernel is unstable, the backed up kernel can be restored at any time. If the update fails and the system cannot be started, you can start any version of Armbian via USB/TF/SD to recover the system in eMMC. For more instructions, see the help [documentation](build-armbian/documents/).

### Update Armbian scripts

To update all Armbian scripts to the latest versions, run:

```bash
armbian-sync
```

### armbian-software store

`armbian-software` can be used to install various popular apps, added to the Armbian software center according to user demand and feedback given in github [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues), gradually integrating commonly used [software](build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) to achieve one-click install/update/uninstall of popular programs such as **docker images**, **desktop software**, **popular server solutions**, etc. See more [here](build-armbian/documents/armbian_software.md).

### Armbian config tool

Armbian has a handy TUI config tool for changing key system settings:

```bash
armbian-config
```

### Create swap for Armbian

If you feel that your TV box has insufficient memory for running demanding applications such as `docker`, you can create a swap virtual memory partition. `armbian-swap` lets the user configure how much disk space is reserved for swap memory. The input parameter defines how many gigabytes of space you want to reserve for swap. The default is **1** GB.

```bash
armbian-swap 1
```

### Configuring the TV box LED display

It is worthwhile configuring your LED display, if your box has one, if only so that you know when your TV box has finished booting when it is not connected to a HDMI screen.

```bash
armbian-openvfd
```

Full configuration instructions for the [LED display are here.](build-armbian/documents/led_screen_display_control.md)

### Backup and restore the original eMMC Android OS

Armbian supports backing up and restoring your TV box's Android eMMC OS partition to TF/SD/USB. It is recommended that you back up the Android system that comes with your box before installing Armbian so that you have the option to restore your TV box to its original state.

You may back up your TV box's Android OS by running:

```bash
armbian-ddbr
```

Following the prompt, enter `b` to perform a system backup or `r` to perform a system recovery.

### Compile the kernel in Armbian

To build your own Armbian kernel, see the [compile-kernel](compile-kernel) documentation. Please login in to armbian → input command:

```bash
armbian-kernel -u
armbian-kernel -k 5.10.125
```


## Local build instructions

1. Clone the repository `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install the necessary packages (The script has only been tested on x86_64 Ubuntu-20.04/22.04)

```bash
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-22.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2204-build-armbian-depends)
```

3. Enter the `~/amlogic-s9xxx-armbian` root directory, create the `build/output/images` folder and then upload the Armbian image ( Eg: `Armbian_21.11.0-trunk_Odroidn2_current_5.15.50.img` ) to this `~/amlogic-s9xxx-armbian/build/output/images` directory. Please keep the release version number (e.g. `21.11.0`) and kernel version number (e.g. `5.15.50`) in the name of the original Armbian image file, It will be used as the name of the armbian system after rebuilding.

4. Enter the `~/amlogic-s9xxx-armbian` root directory, and then run, for example `sudo ./rebuild -b s905x3 -k 5.10.125` to build armbian for `amlogic s9xxx`. The generated Armbian image is in the `build/output/images` directory under the root directory.


### Description of localized packaging parameters

| Optional | Meaning    | Description                               |
| -------- | ---------- | ----------------------------------------- |
| -b       | Board      | Specify the Build system type. Write the build system name individually, such as `-b s905x3` . Multiple system use `_` connect such as `-b s905x3_s905d` . Use `all` for all board models. The model code is detailed in the `BOARD` setting in [model_database.conf](build-armbian/armbian-files/common-files/etc/model_database.conf) file. Default value: `all` |
| -r       | KernelRepo | Specifies the `<owner>/<repo>` of the github.com kernel repository. Default value: `ophub/kernel` |
| -u       | kernelUsage | Set the `tags suffix` of the kernel used, such as [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable), [flippy](https://github.com/ophub/kernel/releases/tag/kernel_flippy), [dev](https://github.com/ophub/kernel/releases/tag/kernel_dev). Default value: `stable` |
| -k       | Kernel     | Specify the [kernel version](https://github.com/ophub/kernel/releases/tag/kernel_stable), Such as `-k 5.10.125` . Multiple kernel use `_` connection such as `-k 5.10.125_5.15.50` . The kernel version freely specified by the `-k` parameter is only valid for the kernel in the `stable/flippy/dev`. Other kernel families such as [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) can only use specific kernels. |
| -a       | AutoKernel | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find in the kernel library whether there is an updated version of the kernel specified in `-k` such as 5.10.125 version. If there is the latest version of same series, it will automatically Replace with the latest version. When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| -t       | RootfsType | Set the file system type of the ROOTFS partition of the system, and the options are `ext4` or `btrfs` type. Such as `-t btrfs`. Default value: `ext4` |
| -s       | Size       | Specify the ROOTFS partition size for the system, and the specified size must be greater than 2048MiB. Such as `-s 2560`, Default value: `2560` |
| -n       | CustomName | Set the signature part of the system name. You can add signatures such as `_server`, `_gnome_desktop` or `_ophub` as needed. Do not include spaces when setting custom signatures. Default value: `None` |
| -g       | GH_TOKEN   | Optional. Set ${{ secrets.GH_TOKEN }} for [api.github.com](https://docs.github.com/en/rest/overview/resources-in-the-rest-api?apiVersion=2022-11-28#requests-from-personal-accounts) query. Default value: `None` |

- `sudo ./rebuild`: Use the default configuration to build all TV box images.
- `sudo ./rebuild -b s905x3 -k 5.10.125`: recommend. Use the default configuration, specify a kernel and a system for compilation.
- `sudo ./rebuild -b s905x3_s905d -k 5.10.125_5.15.50`: Use the default configuration, specify multiple cores, and multiple system for compilation. use `_` to connect.
- `sudo ./rebuild -b s905x3 -k 5.10.125 -s 2560`: Use the default configuration, specify a kernel, a system, and set the partition size for compilation.
- `sudo ./rebuild -b s905x3_s905d`: Use the default configuration, specify multiple system, use `_` to connect. compile all kernels.
- `sudo ./rebuild -k 5.10.125_5.15.50`: Use the default configuration. Specify multiple cores, use `_` to connect.
- `sudo ./rebuild -k 5.10.125_5.15.50 -a true`: Use the default configuration. Specify multiple cores, use `_` to connect. Auto update to the latest kernel of the same series.
- `sudo ./rebuild -t btrfs -s 2560 -k 5.10.125`: Use the default configuration, set the file system to btrfs format and the partition size to 2560MiB, and only compile the armbian system with the kernel version 5.10.125.

## Use GitHub Actions to build

1. Workflows configuration in [build-armbian.yml](.github/workflows/build-armbian.yml) file.

2. New compilation: Select ***`Build armbian`*** on the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, According to the OS version officially supported by Armbian, you can choose Ubuntu series: `jammy`, or Debian series: `bullseye`. Click the ***`Run workflow`*** button. More parameter setting methods can be found in [the official document of Armbian](https://docs.armbian.com/Developer-Guide_Build-Options/).

3. Compile again: If there is an `Armbian_.*-trunk_.*.img.gz` file in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases), you do not need to compile it completely, you can directly use this file to `build amlogic armbian` for a different board. Select ***`Use Releases file to build armbian`*** on the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

4. Use other Armbian system, such as [odroidn2](https://armbian.tnahosting.net/dl/odroidn2/archive/) provided by the official Armbian system download site [armbian.tnahosting.net](https://armbian.tnahosting.net/dl/), only by introducing the script of this repository in the process control file [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml) for Armbian reconstruction, it can be adapted to the use of other TV Boxes. In the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Rebuild armbian`***, and enter the Armbian network download url such as `https://dl.armbian.com/*/Armbian_*.img.xz`, or in the process control file [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml), set the load path of the rebuild file through the `armbian_path` parameter. code show as below:

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

### GitHub Actions Input parameter description

For the related settings of GitHUB RELEASES_TOKEN, please refer to: [RELEASES_TOKEN](build-armbian/documents/README.md#3-fork-repository-and-set-gh_token). The relevant parameters correspond to the `local packaging command`, please refer to the above description.

| Optional           | Defaults          | Description                         |
| ------------------ | ----------------- | ----------------------------------- |
| armbian_path       | None              | Set the path of the original Armbian file, support the file path in the current workflow such as `build/output/images/*.img`, and also support the use of the network download address such as: `https://dl.armbian.com/*/Armbian_*.img.xz`  |
| armbian_board      | all               | Set the `board` of the packaged TV Boxes, function reference `-b`    |
| kernel_repo        | ophub/kernel      | Specifies the `<owner>/<repo>` of the github.com kernel repository, function reference `-r` |
| kernel_usage       | stable            | Set the `tags suffix` of the kernel used, function reference `-u` |
| armbian_kernel     | 6.1.1_5.15.1      | Set kernel [version](https://github.com/ophub/kernel/releases/tag/kernel_stable), function reference `-k`        |
| auto_kernel        | true              | Set whether to automatically use the latest version of the same series of kernels, function reference `-a` |
| armbian_fstype     | ext4              | Set the file system type of the system ROOTFS partition, function reference `-t` |
| armbian_size       | 2560              | Set the size of the system ROOTFS partition, function reference `-s`             |
| armbian_sign       | None              | Set the signature part of the system name, function reference `-n`               |
| gh_token           | None              | Optional. Set ${{ secrets.GH_TOKEN }}, function reference `-g` |

### GitHub Actions Output variable description

To upload to `Releases`, you need to add `${{ secrets.GITHUB_TOKEN }}` and `${{ secrets.GH_TOKEN }}` to the repository and set `Workflow read and write permissions`, see the [instructions for details](build-armbian/documents#2-set-the-privacy-variable-github_token).

| Parameter                                | For example       | Description                         |
|------------------------------------------|-------------------|-------------------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}           | out               | OpenWrt system storage path       |
| ${{ env.PACKAGED_OUTPUTDATE }}           | 04.13.1058        | Packing date(month.day.hour.minute) |
| ${{ env.PACKAGED_STATUS }}               | success           | Package status: success / failure   |

## Default account for Armbian system

| Name             | Value                  |
| ---------------- | ---------------------- |
| Default IP       | Get IP from the router |
| Default username | root                   |
| Default password | 1234                   |

## Compile the kernel using GitHub Actions

For the compilation method of the kernel, see [compile-kernel](compile-kernel)

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

First of all, I would like to thank [150balbes](https://github.com/150balbes) for his outstanding contributions and a good foundation for using Armbian in the Amlogic TV Boxes. The [Armbian](https://github.com/armbian/build) system compiled here directly uses the latest official source code for real-time compilation. The development idea of the program comes from the tutorials of authors such as [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box). Thank you for your dedication and sharing, so that we can use the Armian system in more boxes.

The `kernel` / `u-boot` and other resources used by this system are mainly copied from the project of [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit), Some files are shared by users in [Pull](https://github.com/ophub/amlogic-s9xxx-armbian/pulls) and [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues) of [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) / [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian) / [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic) / [kernel](https://github.com/ophub/kernel) and other projects. To thank these pioneers and sharers, From now on (This source code repository was created on 2021-09-19), I have recorded them in [CONTRIBUTORS.md](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/CONTRIBUTORS.md). Thanks again everyone for giving new life and meaning to the TV Boxes.

## Other distributions

- The [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) project provides the `OpenWrt` system used in the box, which is also applicable to the relevant devices that support Armbian.
- [unifreq](https://github.com/unifreq/openwrt_packit) has made `OpenWrt` system for more boxes such as `Amlogic`, `Rockchip` and `Allwinner`, which is a benchmark in the box circle and is recommended for use.
- [Scirese](https://github.com/Scirese/alarm) tested the production, installation and use of `Arch Linux ARM` / `Manjaro` system in the Android TV boxes, please refer to the relevant instructions in his repository for details.
- [7Ji](https://7ji.github.io/) published some articles on reverse engineering and development on the Amlogic platform in his blog, such as installing the ArchLinux ARM system in the way of ArchLinux, and introducing the startup mechanism of the Amlogic platform. In his [ampart](https://github.com/7Ji/ampart) project, he provided a partitioning tool that supports reading and editing Amlogic's eMMC partition table and DTB partitions, and can make 100% use of the eMMC space. The method of making and using `Arch Linux ARM` is provided in [amlogic-s9xxx-archlinuxarm](https://github.com/7Ji/amlogic-s9xxx-archlinuxarm) project. Yet Another openvfd Daemon is provided in [YAopenvfD](https://github.com/7Ji/YAopenvfD) project.

## Links

- [armbian](https://github.com/armbian/build)
- [unifreq](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## License

The amlogic-s9xxx-armbian © OPHUB is licensed under [GPL-2.0](LICENSE)

