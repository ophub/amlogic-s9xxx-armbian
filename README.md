# Armbian

View Chinese description  |  [查看中文说明](README.cn.md)

The [Armbian](https://www.armbian.com/) system is a lightweight Linux system based on Debian/Ubuntu built specifically for ARM chips. The Armbian system is lean, clean, and 100% compatible and inherits the functions and rich software ecosystem of the Debian/Ubuntu system. It can run safely and stably in TF/SD/USB and the eMMC of the device.

Now you can replace the Android TV system of the TV box with the Armbian system, making it a powerful server. This project builds Armbian system for `Amlogic`, `Rockchip` and `Allwinner` boxes. including install to eMMC and update kernel related functions. Please refer to the [Armbian Documentation](./build-armbian/documents) for the usage method.

The latest version of the Armbian firmware can be downloaded in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases). Welcome to `Fork` and personalize it. If it is useful to you, you can click on the `Star` in the upper right corner of the repository to show your support.

## Armbian Firmware instructions

| SoC  | Device | [Optional kernel](https://github.com/ophub/kernel/tree/main/pub/stable) | Armbian Firmware |
| ---- | ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/99) | All | aml_a311d.img |
| s922x | [Beelink-GT-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/370), [Beelink-GT-King-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/707), [Ugoos-AM6-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/213), [ODROID-N2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/201), [X88-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/988) | All | aml_s922x.img |
| s905x3 | [X96-Max+](https://github.com/ophub/amlogic-s9xxx-armbian/issues/351), [HK1-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/414), [Vontar-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1006), [H96-Max-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/850), [Ugoos-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/782), [TX3(QZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/644), [TX3(BZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1077), [X96-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/366), [X96-Max+_A100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/779), [A95XF3-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/157), [Tencent-Aurora-3Pro(s905x3-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/506), [X96-Max+Q1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/788), [X96-Max+100W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/909), [X96-Max+_2101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1086) | All | aml_s905x3.img |
| s905x2 | [X96Max-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/453), [X96Max-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/95), [MECOOL-KM3-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/79), [Tanix-Tx5-Max](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/351), [A95X-F2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/851) | All | aml_s905x2.img |
| s912 | [Tanix-TX8-Max](https://github.com/ophub/amlogic-s9xxx-armbian/issues/500), [Tanix-TX9-Pro(3G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/315), [Tanix-TX9-Pro(2G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/740), [Tanix-TX92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1012790770), [Nexbox-A1](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [Nexbox-A95X-A2](https://www.cafago.com/en/p-v2979eu-2g.html),  [A95X](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [H96-Pro-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1013071513), [VORKE-Z6-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72), [Mecool-M8S-PRO-L](https://github.com/ophub/amlogic-s9xxx-armbian/issues/158), [Vontar-X92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/525), [T95Z-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/668), [Octopus-Planet](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1020), [Phicomm-T1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/522) | All | aml_s912.img |
| s905d | [MECOOL-KI-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/59), [Phicomm-N1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/925) | All | aml_s905d.img |
| s905x | [HG680P](https://github.com/ophub/amlogic-s9xxx-armbian/issues/368), [B860H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/60), [TBee-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/98), [T95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/285), [TX9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/645), [Q96-mini(s905l-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/734) | All | aml_s905x.img |
| s905w | [X96-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/621), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1062), [W95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/570), [X96W/FunTV](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1044) | All | aml_s905w.img |
| s905 | [Beelink-Mini-MX-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/127), [Sunvell-T95M](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/337), [MXQ-Pro+4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/715) | All | aml_s905.img |
| s905l3a | [E900V22C/D](https://github.com/Calmact/e900v22c), [CM311-1a-YST](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/732), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [UNT413A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [ZTE-B863AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/741) | All | aml_s905l3a.img |
| s905lb/3b | [M302A/M304A](https://github.com/ophub/amlogic-s9xxx-armbian/pull/615), [E900V22E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/939), [BesTV-R3300L(s905l-b)](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993) | All | aml_s905l2.img<br />aml_s905lb-r3300l.img |
| s905l2/3 | [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [MGV3000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921), [Wojia-TV-IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [CM311-1(s905l3)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/763), [HG680-LC(s905l3)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/978), [M401A(s905l3)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921#issuecomment-1453143251) | All | aml_s905l2.img |
| rk3588 | [Radxa-Rock5B](https://wiki.radxa.com/Rock5/5b), [HinLink-H88K](http://www.hinlink.com/index.php?id=138) | [rk3588](https://github.com/ophub/kernel/tree/main/pub/rk3588) | rockchip_boxname.img |
| rk3568 | [FastRhino-R66S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1061), [FastRhino-R68S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/774), [HinLink-H66K](http://www.hinlink.com/index.php?id=137), [HinLink-H68K](http://www.hinlink.com/index.php?id=119), [Radxa-E25](https://wiki.radxa.com/Rock3/CM/CM3I/E25) | [6.x.y](https://github.com/ophub/kernel/tree/main/pub/stable) | rockchip_boxname.img |
| rk3399 | [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [KING3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [KYLIN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132) | [6.x.y](https://github.com/ophub/kernel/tree/main/pub/stable) | rockchip_boxname.img |
| rk3328 | [beikeyun](https://github.com/ophub/amlogic-s9xxx-armbian/issues/852), [l1pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1007) | All | rockchip_boxname.img |
| allwinner | [vplus(h6)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1100) | All | allwinner_boxname.img |

💡Tip: The ***`s905`*** TV Boxes can only be used in `TF/SD/USB`, other types of TV Boxes also support writing to `EMMC`. For more information, please refer to [Description of Supported Device List](build-armbian/documents/amlogic_model_database.md).

## Install to EMMC and update instructions

Choose the corresponding firmware according to your box. Then write the IMG file to the USB hard disk through software such as [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/). Insert the USB hard disk into the box. Common for all `Amlogic s9xxx TV Boxes`.

- ### Install Armbian to EMMC

1. For the installation method of the `Rockchip` platform, please refer to [Section 8](build-armbian/documents) in the documentation。

2. `Amlogic` and `Allwinner` platform，Login in to armbian (default user: root, default password: 1234) → input command:

```yaml
armbian-install
```

| Optional  | Default  | Value    | Description           |
| --------- | -------  | -------- | --------------------  |
| -m        | no       | yes/no   | Use Mainline u-boot   |
| -a        | yes      | yes/no   | Use [ampart](https://github.com/7Ji/ampart) tool |
| -l        | no       | yes/no   | List show all         |

Example: `armbian-install -m yes -a no`

- ### Update Armbian Kernel

Login in to armbian → input command:

```yaml
# Run as root user (sudo -i)
# If no parameter is specified, it will update to the latest version.
armbian-update
```

| Optional  | Default     | Value       | Description                   |
| -------   | -------     | ----------  | ---------------------------   |
| -k        | latest      | [kernel name](https://github.com/ophub/kernel/tree/main/pub/stable)  | Set the kernel name |
| -v        | stable      | stable/rk3588/dev  | Set the kernel version branch |
| -m        | no          | yes/no      | Use Mainline u-boot           |
| -b        | yes         | yes/no      | Automatically backup the current system kernel  |
| -r        | ""          | ""          | [Rescue] Update eMMC with system kernel from USB |

Example: `armbian-update -k 5.15.50 -v dev -m yes`

If there is a set of kernel files in the current directory, it will be updated with the kernel in the current directory (The 4 kernel files required for the update are `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-xxx.tar.gz`, `modules-xxx.tar.gz`. Other kernel files are not required. If they exist at the same time, it will not affect the update. The system can accurately identify the required kernel files). If there is no kernel file in the current directory, it will query and download the latest kernel of the same series from the server for update. The optional kernel supported by the device can be freely updated, such as from 5.10.125 kernel to 5.15.50 kernel.

When updating the kernel, it will automatically back up the kernel used by the current system. The storage path is in the `/ddbr/backup` directory, which can be deleted if not needed.

💡 When the system cannot be started from eMMC due to incomplete updates and other problems caused by special reasons, you can start any kernel version of the Armian system from USB, and run the `armbian-update -r` command to update the system kernel in USB to eMMC to achieve the purpose of rescue.

- ### Install common software

Login in to armbian → input command:

```yaml
armbian-software
```

Use the `armbian-software -u` command to update the local software center list. According to the user's demand feedback in the [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), gradually integrate commonly used [software](build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) to achieve one-click install/update/uninstall and other shortcut operations. Including `docker images`, `desktop software`, `application services`, etc. See more [Description](build-armbian/documents/armbian_software.md).

- ### Modify Armbian Config

Login in to armbian → input command:

```yaml
armbian-config
```

- ### Create swap for Armbian

If you feel that the memory of the current box is not enough when you are using applications with a large memory footprint such as `docker`, you can create a `swap` virtual memory partition, Change the disk space a certain capacity is virtualized into memory for use. The unit of the input parameter of the following command is `GB`, and the default is `1`.

Login in to armbian → input command:

```yaml
armbian-swap 1
```

- ### Controlling the LED display

Login in to armbian → input command:

```yaml
armbian-openvfd
```

Debug according to [LED screen display control instructions](build-armbian/documents/led_screen_display_control.md).

- ### Use Armbian in TF/SD/USB

To activate the remaining space of TF/SD/USB, please login in to armbian → input command:

```yaml
armbian-tf
```

According to the prompt, enter `e` to expand the remaining space to the current system partition and file system, and enter `c` to create a new third partition.

<details>
  <summary>Or manually allocate the remaining space</summary>

#### View [Operation screenshot](https://user-images.githubusercontent.com/68696949/137860992-fbd4e2fa-e90c-4bbb-8985-7f5db9f49927.jpg)

```yaml
# 1. Confirm the name of the TF/SD/USB according to the size of the space. The TF/SD is [ `mmcblk` ], USB is [ `sd` ]
Command: Enter [ fdisk -l | grep "sd" ]

# 2. Get the starting value of the remaining space, Copy and save, used below  (E.g: 5382144)
Command: Enter [ fdisk -l | grep "sd" | sed -n '$p' | awk '{print $3}' | xargs -i expr {} + 1 ]

# 3. Start allocating unused space (E.g: sda, mmcblk0 or mmcblk1)
Command: Enter [ fdisk /dev/sda ] Start allocating the remaining space
Command: Select [ n ] to create a partition
Command: Select [ p ] to specify the partition type as primary partition
Command: Set the partition number to [ 3 ]
Command: The start value of the partition, enter the value obtained in the second step, E.g: [ 5382144 ]
Command: End value, press [ Enter ] to use the default value
Command: If there is a hint: Do you want to remove the signature? [Y]es/[N]o: Enter [ Y ]
Command: Enter [ t ] to specify the partition type
Command: Enter Partition number [ 3 ]
Command: Enter Hex code (type L to list all codes): [ 83 ]
Command: Enter [ w ] to save
Command: Enter [ reboot ] to restart

# 4. After restarting, format the new partition
Command: Enter [ mkfs.ext4 -F -L SHARED /dev/sda3 ] to format the new partition

# 5. Set the mount directory for the new partition
Command: Enter [ mkdir -p /mnt/share ] to Create mount directory
Command: Enter [ mount -t ext4 /dev/sda3 /mnt/share ] to Mount the newly created partition to the directory

# 6. Add automatic mount at boot
Command: Enter [ vi /etc/fstab ]
# Press [ i ] to enter the input mode, copy the following values to the end of the file
/dev/sda3 /mnt/share ext4 defaults 0 0
# Press [ esc ] to exit, Input [ :wq! ] and [ Enter ] to Save, Finish.
```
</details>

- ### Backup/restore the original EMMC system

Supports backup/restore of the box's `EMMC` partition in `TF/SD/USB`. It is recommended that you back up the Android TV system that comes with the current box before installing the Armbian system in a brand new box, so that you can use it in the future when restoring the TV system.

Please login in to armbian → input command:

```yaml
armbian-ddbr
```

According to the prompt, enter `b` to perform system backup, and enter `r` to perform system recovery.

- ### Compile the kernel in Armbian

For the usage of compiling the kernel in Armbian, see the [compile-kernel](compile-kernel) documentation. please login in to armbian → input command:

```yaml
armbian-kernel -u
armbian-kernel -k 5.10.125
```

- ### More instructions for use

To update all service scripts in the local system to the latest version, you can login in to armbian → input command:

```yaml
armbian-sync
```

In the use of Armbian, please refer to [documents](build-armbian/documents) for some common problems that may be encountered.

## Local build instructions

1. Clone the repository to the local. `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install the necessary packages (The script has only been tested on x86_64 Ubuntu-20.04/22.04)

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-22.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2204-build-armbian-depends)
```

3. Enter the `~/amlogic-s9xxx-armbian` root directory, and then create the `build/output/images` folder, and upload the Armbian image ( Eg: `Armbian_21.11.0-trunk_Odroidn2_current_5.15.50.img` ) to this `~/amlogic-s9xxx-armbian/build/output/images` directory. Please keep the release version number (e.g. `21.11.0`) and kernel version number (e.g. `5.15.50`) in the name of the original Armbian image file, It will be used as the name of the armbian firmware after rebuilding.

4. Enter the `~/amlogic-s9xxx-armbian` root directory, and then run Eg: `sudo ./rebuild -b s905x3 -k 5.10.125` to build armbian for `amlogic s9xxx`. The generated Armbian image is in the `build/output/images` directory under the root directory.


- ### Description of localized packaging parameters

| Optional | Meaning    | Description                               |
| -------- | ---------- | ----------------------------------------- |
| -b       | Board      | Specify the Build firmware type. Write the build firmware name individually, such as `-b s905x3` . Multiple firmware use `_` connect such as `-b s905x3_s905d` . Use `all` for all board models. The model code is detailed in the `BOARD` setting in [model_database.conf](build-armbian/armbian-files/common-files/etc/model_database.conf). |
| -k       | Kernel     | Specify the [kernel version](https://github.com/ophub/kernel/tree/main/pub/stable), Such as `-k 5.10.125` . Multiple kernel use `_` connection such as `-k 5.10.125_5.15.50` |
| -a       | AutoKernel | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find in the kernel library whether there is an updated version of the kernel specified in `-k` such as 5.10.125 version. If there is the latest version of same series, it will automatically Replace with the latest version. When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| -v       | Version    | Specify the [version branch](https://github.com/ophub/kernel/tree/main/pub), Such as `-v stable_rk3588`. The specified name must be the same as the branch directory name. The `stable_rk3588` branch version is used by default. |
| -r       | KernelRepo | Specify the name of the kernel repository, Such as `-r https://github.com/ophub/kernel/tree/main/pub` |
| -s       | Size       | Specify the ROOTFS partition size for the firmware. The default is 2560MiB, and the specified size must be greater than 2048MiB. Such as `-s 2560` |
| -t       | RootfsType | Set the file system type of the ROOTFS partition of the firmware, the default is `ext4` type, and the options are `ext4` or `btrfs` type. Such as `-t btrfs` |
| -n       | CustomName | Set the signature part of the firmware name. The default value is empty. You can add signatures such as `_server`, `_gnome_desktop` or `_ophub` as needed. Do not include spaces when setting custom signatures. |
| -g       | GH_TOKEN   | Optional. Set ${{ secrets.GH_TOKEN }} for [api.github.com](https://docs.github.com/en/rest/overview/resources-in-the-rest-api?apiVersion=2022-11-28#requests-from-personal-accounts) query. Default: `None` |

- `sudo ./rebuild`: Use the default configuration to pack all TV Boxes.
- `sudo ./rebuild -b s905x3 -k 5.10.125`: recommend. Use the default configuration, specify a kernel and a firmware for compilation.
- `sudo ./rebuild -b s905x3_s905d -k 5.10.125_5.15.50`: Use the default configuration, specify multiple cores, and multiple firmware for compilation. use `_` to connect.
- `sudo ./rebuild -b s905x3 -k 5.10.125 -s 2560`: Use the default configuration, specify a kernel, a firmware, and set the partition size for compilation.
- `sudo ./rebuild -b s905x3 -v dev -k 5.10.125`: Use the default configuration, specify the model, specify the version branch, and specify the kernel for packaging.
- `sudo ./rebuild -b s905x3_s905d`: Use the default configuration, specify multiple firmware, use `_` to connect. compile all kernels.
- `sudo ./rebuild -k 5.10.125_5.15.50`: Use the default configuration. Specify multiple cores, use `_` to connect.
- `sudo ./rebuild -k 5.10.125_5.15.50 -a true`: Use the default configuration. Specify multiple cores, use `_` to connect. Auto update to the latest kernel of the same series.
- `sudo ./rebuild -t btrfs -s 2560 -k 5.10.125`: Use the default configuration, set the file system to btrfs format and the partition size to 2560MiB, and only compile the armbian firmware with the kernel version 5.10.125.

## Use GitHub Actions to build

1. Workflows configuration in [build-armbian.yml](.github/workflows/build-armbian.yml) file.

2. New compilation: Select ***`Build armbian`*** on the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, According to the OS version officially supported by Armbian, you can choose Ubuntu series: `jammy`, or Debian series: `bullseye`, etc., Click the ***`Run workflow`*** button. More parameter setting methods can be found in [the official document of Armbian](https://docs.armbian.com/Developer-Guide_Build-Options/).

3. Compile again: If there is an `Armbian_.*-trunk_.*.img.gz` file in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases), you do not need to compile it completely, you can directly use this file to `build amlogic armbian` of different board. Select ***`Use Releases file to build armbian`*** on the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

4. Use other Armbian firmware, such as [odroidn2](https://armbian.tnahosting.net/dl/odroidn2/archive/) provided by the official Armbian firmware download site [armbian.tnahosting.net](https://armbian.tnahosting.net/dl/), only by introducing the script of this repository in the process control file [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml) for Armbian reconstruction, it can be adapted to the use of other TV Boxes. In the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Rebuild armbian`***, and enter the Armbian network download url such as `https://dl.armbian.com/*/Armbian_*.img.xz`, or in the process control file [rebuild-armbian.yml](.github/workflows/rebuild-armbian.yml), set the load path of the rebuild file through the `armbian_path` parameter. code show as below:

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

- ### GitHub Actions Input parameter description

For the related settings of GitHUB RELEASES_TOKEN, please refer to: [RELEASES_TOKEN](build-armbian/documents/README.md#3-fork-repository-and-set-gh_token). The relevant parameters correspond to the `local packaging command`, please refer to the above description.

| Optional           | Defaults          | Description                         |
| ------------------ | ----------------- | ----------------------------------- |
| armbian_path       | None              | Set the path of the original Armbian file, support the file path in the current workflow such as `build/output/images/*.img`, and also support the use of the network download address such as: `https://dl.armbian.com/*/Armbian_*.img.xz`  |
| armbian_board      | s905d_s905x3      | Set the `board` of the packaged TV Boxes, function reference `-b`    |
| armbian_kernel     | 5.10.125_5.15.50  | Set kernel [version](https://github.com/ophub/kernel/tree/main/pub/stable), function reference `-k`        |
| auto_kernel        | true              | Set whether to automatically use the latest version of the same series of kernels, function reference `-a` |
| version_branch     | stable_rk3588     | Specify the name of the kernel [version branch](https://github.com/ophub/kernel/tree/main/pub), function reference `-v` |
| kernel_repo        | [ophub/kernel](https://github.com/ophub/kernel/tree/main/pub) | Specify the name of the kernel Repository, function reference `-r` |
| armbian_size       | 2560              | Set the size of the firmware ROOTFS partition, function reference `-s`             |
| armbian_fstype     | ext4              | Set the file system type of the firmware ROOTFS partition, function reference `-t` |
| armbian_sign       | None              | Set the signature part of the firmware name, function reference `-n`               |
| gh_token           | None              | Optional. Set ${{ secrets.GH_TOKEN }}, function reference `-g` |

- ### GitHub Actions Output variable description

To upload to `Releases`, you need to add `${{ secrets.GITHUB_TOKEN }}` and `${{ secrets.GH_TOKEN }}` to the repository and set `Workflow read and write permissions`, see the [instructions for details](build-armbian/documents#2-set-the-privacy-variable-github_token).

| Parameter                                | For example       | Description                         |
|------------------------------------------|-------------------|-------------------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}           | out               | OpenWrt firmware storage path       |
| ${{ env.PACKAGED_OUTPUTDATE }}           | 04.13.1058        | Packing date(month.day.hour.minute) |
| ${{ env.PACKAGED_STATUS }}               | success           | Package status: success / failure   |

## Armbian firmware default information

| Name | Value |
| ---- | ---- |
| Default IP | Get IP from the router |
| Default username | root |
| Default password | 1234 |

## Compile the kernel using GitHub Actions

For the compilation method of the kernel, see [compile-kernel](compile-kernel)

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.10.125_5.15.50
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
- [7Ji](https://7ji.github.io/) published some articles on reverse engineering and development on the Amlogic platform in his blog, such as installing the ArchLinux ARM system in the way of ArchLinux, and introducing the startup mechanism of the Amlogic platform. In his [ampart](https://github.com/7Ji/ampart) project, he provided a partitioning tool that supports reading and editing Amlogic's eMMC partition table and DTB partitions, and can make 100% use of the eMMC space. The method of making and using `Arch Linux ARM` is provided in project [amlogic-s9xxx-archlinuxarm](https://github.com/7Ji/amlogic-s9xxx-archlinuxarm).

## Links

- [armbian](https://github.com/armbian/build)
- [unifreq](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## License

The amlogic-s9xxx-armbian © OPHUB is licensed under [GPL-2.0](LICENSE)

