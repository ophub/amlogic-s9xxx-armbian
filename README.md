# Armbian for Amlogic s9xxx tv box

View Chinese description  |  [查看中文说明](README.cn.md)

Compile the Armbian for Amlogic s9xxx tv box. including install to EMMC and update related functions. Support Amlogic s9xxx tv box are ***`s922x, s905x3, s905x2, s912, s905d, s905x, s905w, s905`***, etc. such as ***`Belink GT-King, Belink GT-King Pro, UGOOS AM6 Plus, X96-Max+, HK1-Box, H96-Max-X3, Phicomm-N1, Octopus-Planet, Fiberhome HG680P, ZTE B860H`***, etc.

The latest version of the Armbian firmware can be downloaded in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases). Welcome to `Fork` and personalize it. If it is useful to you, you can click on the `Star` in the upper right corner of the warehouse to show your support.

## Armbian Firmware instructions

| SoC  | Device | [Optional kernel](https://github.com/ophub/kernel/tree/main/pub/stable) | Armbian Firmware |
| ---- | ---- | ---- | ---- |
| s922x | [Beelink-GT-King](https://tokopedia.link/RAgZmOM41db), [Beelink-GT-King-Pro](https://www.gearbest.com/tv-box/pp_3008857542462482.html), [Ugoos-AM6-Plus](https://tokopedia.link/pHGKXuV41db), [ODROID-N2](https://www.tokopedia.com/search?st=product&q=ODROID-N2) | All | armbian_aml_s922x_*.img |
| s905x3 | [X96-Max+](https://tokopedia.link/uMaH09s41db), [HK1-Box](https://tokopedia.link/xhWeQgTuwfb), [H96-Max-X3](https://tokopedia.link/KuWvwoYuwfb), [Ugoos-X3](https://tokopedia.link/duoIXZpdGgb), [X96-Air](https://www.gearbest.com/tv-box/pp_3002885621272175.html), [A95XF3-Air](https://tokopedia.link/ByBL45jdGgb) | All | armbian_aml_s905x3_*.img |
| s905x2 | [X96Max-4G](https://tokopedia.link/HcfLaRzjqeb), [X96Max-2G](https://tokopedia.link/HcfLaRzjqeb) | All | armbian_aml_s905x2_*.img |
| s912 | [H96-Pro-Plus](https://www.gearbest.com/tv-box-mini-pc/pp_503486.html), [T95Z-Plus](https://www.tokopedia.com/search?st=product&q=t95z%20plus), Octopus-Planet | All | armbian_aml_s912_*.img |
| s905d | [MECOOL-KI-Pro](https://www.gearbest.com/tv-box-mini-pc/pp_629409.html), Phicomm-N1 | All | armbian_aml_s905d_*.img |
| s905x | [HG680P](https://tokopedia.link/HbrIbqQcGgb), [B860H](https://www.zte.com.cn/global/products/cocloud/201707261551/IP-STB/ZXV10-B860H) | All | armbian_aml_s905x_*.img |
| s905w | [X96-Mini](https://tokopedia.link/ro207Hsjqeb), [TX3-Mini](https://www.gearbest.com/tv-box/pp_009748238474.html) | 5.4.* | armbian_aml_s905w_*.img |
| s905 | [Beelink-Mini-MX-2G](https://www.gearbest.com/tv-box-mini-pc/pp_321409.html), [MXQ-PRO+4K](https://www.gearbest.com/tv-box-mini-pc/pp_354313.html) | All | armbian_aml_s905_*.img |

💡Tip: The current box of ***`s905`*** can only be used in `TF/SD/USB`, and other types of boxes can also be used in `EMMC` at the same time. The ***`s905w`*** boxs currently only support `5.4` kernels, Cannot use kernel version 5.10 and above, Other devices can be freely selected. Please refer to the [instructions](build-armbian/amlogic-u-boot/README.md) for dtb and u-boot of each device.

## Install to EMMC and update instructions

Choose the corresponding firmware according to your box. Then write the IMG file to the USB hard disk through software such as [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/). Insert the USB hard disk into the box. Common for all `Amlogic s9xxx tv box`.

- ### Install Armbian to EMMC

Login in to armbian (default user: root, default password: 1234) → input command:

```yaml
armbian-install
```

- ### Update Armbian Kernel

Query the available [kernel_version](https://github.com/ophub/kernel/tree/main/pub/stable). Login in to armbian → input command:

```yaml
# Run as root user (sudo -i), input command: armbian-update <kernel_version>
armbian-update 5.4.170
```

If there is a complete set of kernel files in the current directory, you can run the `armbian-update` command to install this kernel. The kernel update script will be continuously updated during development. You can use this command to update the local script synchronously: `wget -O /usr/sbin/armbian-update git.io/armbian-update` . Or directly use the latest script on the server side to update the kernel: `bash <(curl -fsSL git.io/armbian-update) 5.4.170`

When the kernel is updated, By default, download from [stable](https://github.com/ophub/kernel/tree/main/pub/stable) kernel version branch, if you download other [version branch](https://github.com/ophub/kernel/tree/main/pub), please specify according to the branch folder name in the `second` parameter, such as `armbian-update 5.7.19 dev` . The mainline u-boot is automatically installed by default, which can better support the use of kernel series 5.10 and above. If you choose not to install, please specify in the `third` input parameter, such as `armbian-update 5.4.170 stable no`

The `headers` files in the kernel is installed in the `/use/local/include` directory. When compiling the application, add `-I /usr/local/include` to the `CFLAG` parameter of `GCC` to find the headers files.

- ### Install Docker Service

Login in to armbian → input command:

```yaml
armbian-docker
```

After installing docker, you can choose whether to install the `portainer` visual management panel. Users who intend to use the `LAN IP` for management can choose (`h`) to install the `http://ip:9000` version; Users who intend to use the `domain name` for remote management can choose (`s`) to install the `https://domain:9000` domain name certificate version (Please name the domain name `SSL` certificate as `portainer.crt` and `portainer.key` and upload it to the `/etc/ssl/` directory); Users who `do not need` to install can choose (`n`) to end the installation.

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

- ### Use Armbian in TF/USB

To activate the remaining space of TF/USB, please login in to armbian → input command:

```yaml
armbian-tf
```

According to the prompt, enter `e` to expand the remaining space to the current system partition and file system, and enter `c` to create a new third partition.

<details>
  <summary>Or manually allocate the remaining space</summary>

#### View [Operation screenshot](https://user-images.githubusercontent.com/68696949/137860992-fbd4e2fa-e90c-4bbb-8985-7f5db9f49927.jpg)

```yaml
# 1. Confirm the name of the TF/USB according to the size of the space. The TF is [ `mmcblk` ], USB is [ `sd` ]
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
ddbr
```

According to the prompt, enter `b` to perform system backup, and enter `r` to perform system recovery.

## Detailed build compile command

| Parameter | Meaning | Description |
| ---- | ---- | ---- |
| -d | Defaults | Compile all cores and all firmware types. |
| -b | Build | Specify the Build firmware type. Write the build firmware name individually, such as `-b s905x3` . Multiple firmware use `_` connect such as `-b s905x3_s905d` . You can use these codes: `s905x3`, `s905x2`, `s905x`, `s905w`, `s905d`, `s905d-ki`, `s905`, `s922x`, `s922x-n2`, `s912`, `s912-t95z` . Note: `s922x-n2` is `s922x-odroid-n2`, `s912-t95z` is `s912-t95z-plus`, `s905d-ki` is `mecool-ki-pro`. |
| -v | Version | Specify the [version branch](https://github.com/ophub/kernel/tree/main/pub), Such as `-v stable`. The specified name must be the same as the branch directory name. The `stable` branch version is used by default. |
| -k | Kernel | Specify the [kernel version](https://github.com/ophub/kernel/tree/main/pub/stable), Such as `-k 5.4.170` . Multiple kernel use `_` connection such as `-k 5.10.90_5.4.170` |
| -a | AutoKernel | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find in the kernel library whether there is an updated version of the kernel specified in `-k` such as 5.4.170 version. If there is the latest version of 5.4 same series, it will automatically Replace with the latest version. When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| -s | Size | Specify the size of the ROOTFS partition in MB. The default is 2748, and the specified size must be greater than 2000. Such as `-s 2748` |

- `sudo ./rebuild -d -b s905x3 -k 5.4.170`: recommend. Use the default configuration, specify a kernel and a firmware for compilation.
- `sudo ./rebuild -d -b s905x3_s905d -k 5.10.90_5.4.170`: Use the default configuration, specify multiple cores, and multiple firmware for compilation. use `_` to connect.
- `sudo ./rebuild -d`: Use the default configuration to pack all boxes.
- `sudo ./rebuild -d -b s905x3 -k 5.4.170 -s 2748`: Use the default configuration, specify a kernel, a firmware, and set the partition size for compilation.
- `sudo ./rebuild -d -b s905x3 -v dev -k 5.7.19`: Use the default configuration, specify the model, specify the version branch, and specify the kernel for packaging.
- `sudo ./rebuild -d -b s905x3_s905d`: Use the default configuration, specify multiple firmware, use `_` to connect. compile all kernels.
- `sudo ./rebuild -d -k 5.10.90_5.4.170`: Use the default configuration. Specify multiple cores, use `_` to connect.
- `sudo ./rebuild -d -k 5.10.90_5.4.170 -a true`: Use the default configuration. Specify multiple cores, use `_` to connect. Auto update to the latest kernel of the same series.
- `sudo ./rebuild -d -s 2748 -k 5.4.170`: Use the default configuration and set the partition size to 2748m, and only compile the armbian firmware with the kernel version 5.4.170.

- ### Local build instructions

1. Install the necessary packages (E.g Ubuntu 20.04 LTS user)

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/ubuntu-2004-server)
```

2. Clone the repository to the local. `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

3. Create the `build/output/images` folder, and upload the Armbian image ( Eg: `Armbian_21.11.0-trunk_Lepotato_current_5.10.90.img` ) to this `~/amlogic-s9xxx-armbian/build/output/images` directory. Please keep the release version number (e.g. `21.11.0`) and kernel version number (e.g. `5.10.90`) in the name of the original Armbian image file, It will be used as the name of the armbian firmware after rebuilding.

4. Enter the `~/amlogic-s9xxx-armbian` root directory. And run Eg: `sudo ./rebuild -d -b s905x3 -k 5.4.170` to build armbian for `amlogic s9xxx`. The generated Armbian image is in the `build/output/images` directory under the root directory.

- ### Use GitHub Action to build

1. Workflows configuration in [.yml](.github/workflows/build-armbian.yml) files. Set the armbian `SOC` you want to build in `Rebuild Armbian for amlogic s9xxx`.

2. New compilation: Select ***`Build armbian`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, According to the OS version officially supported by Armbian, In [RELEASE](https://docs.armbian.com/Developer-Guide_Build-Options/), you can choose Ubuntu series: `focal`, or Debian series: `bullseye` / `buster`, and in `BOARD`, you can choose `lepotato` / `odroidn2`, etc., You can add more setting options for `compile.sh` in `More build options` as needed. Click the ***`Run workflow`*** button.

3. Compile again: If there is an `Armbian_.*-trunk_.*.img.gz` file in [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases), you do not need to compile it completely, you can directly use this file to `build armbian` of different soc. Select ***`Use Releases file to build armbian`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

- ### Only import GitHub Action for Armbian rebuild

You can use other methods to build the Armbian system. Or use [Armbian](https://armbian.tnahosting.net/dl/) officially provided [lepotato](https://armbian.tnahosting.net/dl/lepotato/archive/) and other branch firmware. and only import the Action from this repository in the process control file [.yml](.github/workflows/rebuild-armbian.yml) to rebuild Armbian to adapt to the use of Amlogic S9xxx series boxes. In the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Rebuild armbian`***, and enter the Armbian network download url such as `https://dl.armbian.com/*/Armbian_*.img.xz`, or in the process control file [.yml](.github/workflows/rebuild-armbian.yml), set the load path of the rebuild file through the `armbian_path` parameter. code show as below:

```yaml
- name: Rebuild Armbian for Amlogic s9xxx
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    armbian_path: build/output/images/*.img
    armbian_soc: s905d_s905x3_s922x_s905x
    armbian_kernel: 5.10.90_5.4.170
```

- GitHub Action Input parameter description

| Parameter              | Defaults               | Description                                                   |
|------------------------|------------------------|---------------------------------------------------------------|
| armbian_path         | no                     | Set the path of the original Armbian file, support the file path in the current workflow such as `build/output/images/*.img`, and also support the use of the network download address such as: `https://dl.armbian.com/*/Armbian_*.img.xz` |
| armbian_soc        | s905d_s905x3           | Set the `SoC` of the packaging box, you can specify a single box such as `s905x3`, you can choose multiple boxes to use `_` connection such as `s905x3_s905d` . SOC code of each box is: `s905x3`, `s905x2`, `s905x`, `s905w`, `s905d`, `s905d-ki`, `s905`, `s922x`, `s922x-n2`, `s912`, `s912-t95z` . Note: `s922x-n2` is `s922x-odroid-n2`, `s912-t95z` is `s912-t95z-plus`, `s905d-ki` is `mecool-ki-pro`. |
| version_branch         | stable                 | Set the [version branch](https://github.com/ophub/kernel/tree/main/pub), Such as `stable`. The specified name must be the same as the branch directory name. The `stable` branch version is used by default. |
| armbian_kernel         | 5.10.90_5.4.170        | Set the [kernel version](https://github.com/ophub/kernel/tree/main/pub/stable), Such as `5.4.170` . Multiple kernel use `_` connection such as `5.10.90_5.4.170` |
| auto_kernel            | true                   | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find in the kernel library whether there is an updated version of the kernel specified in `amlogic_kernel`. such as 5.4.170 version. If there is the latest version of 5.4 same series, it will automatically Replace with the latest version. When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| armbian_size           | 2748                   | Set the size of the firmware ROOTFS partition, and the specified size must be greater than 2000. |

- GitHub Action Output variable description

| Parameter                                | For example             | Description                   |
|------------------------------------------|-------------------------|-------------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}           | ${PWD}/out              | OpenWrt firmware storage path |
| ${{ env.PACKAGED_OUTPUTDATE }}           | 2021.04.13.1058         | Packing date                  |
| ${{ env.PACKAGED_STATUS }}               | success / failure       | Package status                |

## Compile a custom kernel

For the compilation method of the custom kernel, see [compile-kernel](compile-kernel)

## Armbian contributor list

First of all, I would like to thank [150balbes](https://github.com/150balbes) for his outstanding contributions and a good foundation for using Armbian in the Amlogic box. The [armbian](https://github.com/armbian/build) system compiled here directly uses the latest official source code for real-time compilation, When making dedicated Armbian systems for different boxes, the kernel, scripts, u-boot and other resources made by [unifreq](https://github.com/unifreq/openwrt_packit) for `Amlogic s9xxx openwrt` are used. The development idea of the program comes from the tutorials of authors such as [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box). Thank you for your dedication and sharing, so that we can use the Armbian system in the Amlogic s9xxx box.

Because of these [innovations and contributors](CONTRIBUTOR.md), we can have the company of the box in the long river of years. We have grown up many years later, but this beautiful memory will always stay deep in the memory for a long time. From now on, record the achievements of these pioneers and leave them to the new friends who have joined the box circle.

## License

[LICENSE](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/LICENSE) © OPHUB

