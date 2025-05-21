# Armbian Build and Usage Guide

View Chinese description | [查看中文说明](README.cn.md)

GitHub Actions is a service launched by Microsoft that provides a virtual server environment with excellent performance configurations. Based on this, you can build, test, package, and deploy projects. For public repositories, it can be used free of charge with no time limit, and each build can last up to 6 hours. This is sufficient for compiling Armbian (usually we can complete a compilation within about 3 hours). Sharing is just for the purpose of exchanging experiences, please understand any shortcomings, do not initiate any harmful attacks on the network, and do not misuse GitHub Actions.

# Table of Contents

- [Armbian Build and Usage Guide](#armbian-build-and-usage-guide)
- [Table of Contents](#table-of-contents)
  - [1. Register your own Github account](#1-register-your-own-github-account)
  - [2. Set up private variable GITHUB\_TOKEN](#2-set-up-private-variable-github_token)
  - [3. Fork the repository and set Workflow permissions](#3-fork-the-repository-and-set-workflow-permissions)
  - [4. Customization instructions for personalized Armbian system files](#4-customization-instructions-for-personalized-armbian-system-files)
  - [5. Compile the system](#5-compile-the-system)
    - [5.1 Manual Compilation](#51-manual-compilation)
    - [5.2 Scheduled Compilation](#52-scheduled-compilation)
    - [5.3 Customizing Default System Configuration](#53-customizing-default-system-configuration)
    - [5.4 Expanding Github Actions Compilation Space Using Logical Volumes](#54-expanding-github-actions-compilation-space-using-logical-volumes)
  - [6. Saving the System](#6-saving-the-system)
  - [7. Downloading the System](#7-downloading-the-system)
  - [8. Installing Armbian to EMMC](#8-installing-armbian-to-emmc)
    - [8.1 Installation Method for Amlogic Series](#81-installation-method-for-amlogic-series)
    - [8.2 Installation Method for Rockchip Series](#82-installation-method-for-rockchip-series)
      - [8.2.1 Installation Method for Radxa-Rock5B](#821-installation-method-for-radxa-rock5b)
        - [8.2.1.1 Install the System to MicroSD](#8211-install-the-system-to-microsd)
        - [8.2.1.2 Install the System to eMMC](#8212-install-the-system-to-emmc)
        - [8.2.1.3 Installation of the system on NVMe](#8213-installation-of-the-system-on-nvme)
      - [8.2.2 Installation method for FastRhino R66S](#822-installation-method-for-fastrhino-r66s)
      - [8.2.3 Installation method for FastRhino R68S](#823-installation-method-for-fastrhino-r68s)
      - [8.2.4 Installation method for Beikeyun](#824-installation-method-for-beikeyun)
      - [8.2.5 Installation method for Chainedbox-L1-Pro](#825-installation-method-for-chainedbox-l1-pro)
      - [8.2.6 Installation method for lckfb-tspi](#826-installation-method-for-lckfb-tspi)
    - [8.3 Allwinner Series Installation Method](#83-allwinner-series-installation-method)
  - [9. Compiling Armbian Kernel](#9-compiling-armbian-kernel)
    - [9.1 How to Add Custom Kernel Patches](#91-how-to-add-custom-kernel-patches)
    - [9.2 How to Make Kernel Patches](#92-how-to-make-kernel-patches)
    - [9.3 How to Customize Compilation of Driver Modules](#93-how-to-customize-compilation-of-driver-modules)
  - [10. Updating Armbian Kernel](#10-updating-armbian-kernel)
  - [11. Installing Common Software](#11-installing-common-software)
  - [12. Frequently Asked Questions](#12-frequently-asked-questions)
    - [12.1 dtb and u-boot Correspondence Table for Each Box](#121-dtb-and-u-boot-correspondence-table-for-each-box)
    - [12.2 Instructions for LED Screen Display Control](#122-instructions-for-led-screen-display-control)
    - [12.3 How to Restore the Original Android TV System](#123-how-to-restore-the-original-android-tv-system)
      - [12.3.1 Backup and Restore Using Armbian-ddbr](#1231-backup-and-restore-using-armbian-ddbr)
      - [12.3.2 Recovering using Amlogic Flashing Tool](#1232-recovering-using-amlogic-flashing-tool)
    - [12.4 Setting the box to boot from USB/TF/SD](#124-setting-the-box-to-boot-from-usbtfsd)
      - [12.4.1 Initial Installation of Armbian System](#1241-initial-installation-of-armbian-system)
      - [12.4.2 Reinstallation of Armbian System](#1242-reinstallation-of-armbian-system)
    - [12.5 Disable Infrared Receiver](#125-disable-infrared-receiver)
    - [12.6 Boot file selection](#126-boot-file-selection)
    - [12.7 Network Configuration](#127-network-configuration)
      - [12.7.1 Network Configuration Using Interfaces](#1271-network-configuration-using-interfaces)
        - [12.7.1.1 IP Address Assignment via DHCP](#12711-ip-address-assignment-via-dhcp)
        - [12.7.1.2 Manual Setup of Static IP Address](#12712-manual-setup-of-static-ip-address)
        - [12.7.1.3 Establish Interconnected Network Using OpenWrt in Docker](#12713-establish-interconnected-network-using-openwrt-in-docker)
      - [12.7.2 Network Management Using NetworkManager](#1272-network-management-using-networkmanager)
        - [12.7.2.1 Create a New Network Connection](#12721-create-a-new-network-connection)
          - [12.7.2.1.1 Get Network Interface Name](#127211-get-network-interface-name)
          - [12.7.2.1.2 Get Existing Network Connection Name](#127212-get-existing-network-connection-name)
          - [12.7.2.1.3 Create a Wired Network Connection](#127213-create-a-wired-network-connection)
          - [12.7.2.1.4 Creating a Wireless Network Connection](#127214-creating-a-wireless-network-connection)
        - [12.7.2.2 Modify WiFi SSID or PASSWD in Wireless Network Connection](#12722-modify-wifi-ssid-or-passwd-in-wireless-network-connection)
        - [12.7.2.3 Modify Network Address Allocation Method](#12723-modify-network-address-allocation-method)
          - [12.7.2.3.1 Static IP address - IPv4](#127231-static-ip-address---ipv4)
          - [12.7.2.3.2 DHCP Obtains Dynamic IP Address - IPv4 / IPv6](#127232-dhcp-obtains-dynamic-ip-address---ipv4--ipv6)
        - [12.7.2.4 Modify Network Connection MAC Address](#12724-modify-network-connection-mac-address)
        - [12.7.2.5 How to Disable IPv6](#12725-how-to-disable-ipv6)
      - [12.7.3 How to Enable Wireless](#1273-how-to-enable-wireless)
      - [12.7.4 How to Enable Bluetooth](#1274-how-to-enable-bluetooth)
    - [12.8 How to Add Startup Tasks](#128-how-to-add-startup-tasks)
    - [12.9 How to Update Service Scripts in the System](#129-how-to-update-service-scripts-in-the-system)
    - [12.10 How to Get Android System Partition Information on eMMC](#1210-how-to-get-android-system-partition-information-on-emmc)
      - [12.10.1 Obtaining Partition Information](#12101-obtaining-partition-information)
      - [12.10.2 Sharing Partition Information](#12102-sharing-partition-information)
      - [12.10.3 Interpreting Partition Information](#12103-interpreting-partition-information)
      - [12.10.4 For eMMC Installation](#12104-for-emmc-installation)
    - [12.11 How to build the u-boot file](#1211-how-to-build-the-u-boot-file)
      - [12.11.1 How to build the u-boot file for Amlogic devices](#12111-how-to-build-the-u-boot-file-for-amlogic-devices)
        - [12.11.1.1 How to extract the bootloader and dtb files](#121111-how-to-extract-the-bootloader-and-dtb-files)
        - [12.11.1.2 How to create the acs.bin file](#121112-how-to-create-the-acsbin-file)
        - [12.11.1.3 How to build the u-boot file](#121113-how-to-build-the-u-boot-file)
      - [12.11.2 How to build the u-boot file for Rockchip devices](#12112-how-to-build-the-u-boot-file-for-rockchip-devices)
        - [12.11.2.1 How to use Radxa's u-boot building script](#121121-how-to-use-radxas-u-boot-building-script)
        - [12.11.2.2 How to use cm9vdA's u-boot building script](#121122-how-to-use-cm9vdas-u-boot-building-script)
    - [12.12 Error in Memory Size Recognition](#1212-error-in-memory-size-recognition)
    - [12.13 How to Decompile dtb Files](#1213-how-to-decompile-dtb-files)
    - [12.14 How to Modify cmdline Settings](#1214-how-to-modify-cmdline-settings)
    - [12.15 How to Add New Supported Devices](#1215-how-to-add-new-supported-devices)
      - [12.15.1 Add Device Configuration File](#12151-add-device-configuration-file)
      - [12.15.2 Add System Files](#12152-add-system-files)
      - [12.15.3 Add u-boot Files](#12153-add-u-boot-files)
      - [12.15.4 Add Process Control Files](#12154-add-process-control-files)
    - [12.16 How to Resolve the Issue of I/O Errors While Writing to eMMC](#1216-how-to-resolve-the-issue-of-io-errors-while-writing-to-emmc)
    - [12.17 How to Solve the Issue of No Sound in the Bullseye Version](#1217-how-to-solve-the-issue-of-no-sound-in-the-bullseye-version)
    - [12.18 How to build the boot.scr file](#1218-how-to-build-the-bootscr-file)
    - [12.19 How to Enable Remote Desktop and Modify the Default Port](#1219-how-to-enable-remote-desktop-and-modify-the-default-port)

## 1. Register your own Github account

Register your own account in order to continue with the customized operation of the system. Click the `Sign up` button in the upper right corner of the github.com website and follow the instructions to register your account.

## 2. Set up private variable GITHUB_TOKEN

According to the [GitHub Docs](https://docs.github.com/en/actions/security-guides/automatic-token-authentication), GitHub automatically creates a unique GITHUB_TOKEN secret at the start of every workflow job for use within the workflow. The `{{ secrets.GITHUB_TOKEN }}` can be used for authentication within the workflow job.

## 3. Fork the repository and set Workflow permissions

Now you can Fork the repository. Open the repository https://github.com/ophub/amlogic-s9xxx-armbian, click the Fork button in the upper right, copy a copy of the repository code to your account, wait a few seconds, after the Fork is complete, visit your own amlogic-s9xxx-armbian in your own repository. In the upper right corner, `Settings` > `Actions` > `General` > `Workflow permissions` on the left navigation bar and save. The illustration is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/167585338-841d3b05-8d98-4d73-ba72-475aad4a95a9.png width="300" />
</div>

## 4. Customization instructions for personalized Armbian system files

The system compilation process is controlled in the [.github/workflows/build-armbian.yml](../.github/workflows/build-armbian.yml) file. There are other .yml files in the workflows directory that implement different functions. The system is compiled in real time using Armbian's current official code, and related parameters can be referred to the official documentation.

```yaml
- name: Compile Armbian [ ${{ inputs.set_release }} ]
  id: compile
  if: ${{ steps.down.outputs.status }} == 'success' && !cancelled()
  run: |
    # Compile method and parameter description: https://docs.armbian.com/Developer-Guide_Build-Options
    cd build/
        ./compile.sh RELEASE=${{ inputs.set_release }} BOARD=odroidn2 BRANCH=current BUILD_MINIMAL=no \
                      BUILD_ONLY=default HOST=armbian BUILD_DESKTOP=no EXPERT=yes KERNEL_CONFIGURE=no \
                      COMPRESS_OUTPUTIMAGE="sha" SHARE_LOG=yes
    echo "status=success" >> ${GITHUB_OUTPUT}
```

## 5. Compile the system

There are many ways to compile the system. You can set up timed compilation, manual compilation, or set some specific events to trigger the compilation. Let's start with simple operations.

### 5.1 Manual Compilation

In your repository's navigation bar, click the Actions button, then sequentially click on Build armbian > Run workflow > Run workflow to start the compilation. Wait for approximately 3 hours. The compilation is completed once all processes have ended. The illustration is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163203938-e7762b09-e6b8-4cf5-b1f1-9c67c1a29953.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204044-9c7a7429-47ee-4fce-b7dd-e217bebf6133.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204127-6b16b558-7e78-4c22-a28a-7b37b5a34fa3.png width="300" />
</div>

### 5.2 Scheduled Compilation

In the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file, use Cron to set scheduled compilation. The five different positions respectively represent minutes (0 - 59) / hours (0 - 23) / date (1 - 31) / month (1 - 12) / day of the week (0 - 6)(Sunday - Saturday). Set the time by modifying the value at different positions. The system uses UTC standard time by default, please convert according to the time zone of your country.

```yaml
schedule:
  - cron: '0 17 * * *'
```

### 5.3 Customizing Default System Configuration

The default system configuration information is recorded in the [model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) file, in which the `BOARD` name must be unique.

For those with `BUILD` value as `yes`, they are part of the default packaged systems for the box, these boxes can be used directly. Those with the default value as `no` are not packaged. To use the unpackaged boxes, you need to download the same `FAMILY` packaged system (recommend downloading `5.15/5.4` kernel system), after writing to `USB`, you can open `USB's boot partition` on the computer, modify `FDT's dtb name` in `/boot/uEnv.txt` file, to adapt to other boxes in the list.

When compiling locally, specify with the `-b` parameter. When compiling in Actions on github.com, specify with the `armbian_board` parameter. Using `-b all` means to package all devices where `BUILD` is `yes`. When packaging with a specified `BOARD` parameter, it can be packaged whether `BUILD` is `yes` or `no`. For example: `-b r68s_s905x3-tx3_s905l3a-cm311`.

### 5.4 Expanding Github Actions Compilation Space Using Logical Volumes

The default compile space for Github Actions is 84G, with about 50G available after considering the system and necessary software packages. When compiling all firmware, you may encounter an issue with insufficient space, which can be addressed by using logical volumes to expand the compile space to approximately 110G. Refer to the method in the [.github/workflows/build-armbian.yml](../.github/workflows/build-armbian.yml) file, and use the commands below to create a logical volume. Then, use the path of the logical volume during the compilation process.

```yaml
- name: Create simulated physical disk
  run: |
    mnt_size=$(expr $(df -h /mnt | tail -1 | awk '{print $4}' | sed 's/[[:alpha:]]//g' | sed 's/\..*//') - 1)
    root_size=$(expr $(df -h / | tail -1 | awk '{print $4}' | sed 's/[[:alpha:]]//g' | sed 's/\..*//') - 4)
    sudo truncate -s "${mnt_size}"G /mnt/mnt.img
    sudo truncate -s "${root_size}"G /root.img
    sudo losetup /dev/loop6 /mnt/mnt.img
    sudo losetup /dev/loop7 /root.img
    sudo pvcreate /dev/loop6
    sudo pvcreate /dev/loop7
    sudo vgcreate github /dev/loop6 /dev/loop7
    sudo lvcreate -n runner -l 100%FREE github
    sudo mkfs.xfs /dev/github/runner
    sudo mkdir -p /builder
    sudo mount /dev/github/runner /builder
    sudo chown -R runner.runner /builder
    df -Th
```

## 6. Saving the System

The system save setting is also controlled in the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file. We upload the compiled system to the Releases provided by the official GitHub via script automatically.

```yaml
- name: Upload Armbian image to Release
  uses: ncipollo/release-action@main
  if: ${{ env.PACKAGED_STATUS }} == 'success' && !cancelled()
  with:
    tag: Armbian_${{ env.ARMBIAN_RELEASE }}_${{ env.PACKAGED_OUTPUTDATE }}
    artifacts: ${{ env.PACKAGED_OUTPUTPATH }}/*
    allowUpdates: true
    token: ${{ secrets.GITHUB_TOKEN }}
    body: |
      These are the Armbian OS image
      * OS information
      Default username: root
      Default password: 1234
      Install command: armbian-install
      Update command: armbian-update
```

## 7. Downloading the System

Enter the Release section in the lower right corner of the repository homepage, select the system that corresponds to your box model. The illustration is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163204798-0d98524c-73df-4876-8912-fcae2845fbba.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204879-4898babf-fa00-4e63-89ea-235129e2ce1d.png width="300" />
</div>

## 8. Installing Armbian to EMMC

Amlogic, Rockchip, and Allwinner have different installation methods. Different devices have different storage, some use an external microSD card, some have eMMC, some support the use of various storage media such as NVMe. According to the different devices, their installation methods are introduced separately. First, download the Armbian system for your device from [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases), and decompress it into .img format for standby. Depending on your device, use different installation methods in the following summary.

After the installation is completed, connect the Armbian device to the `router`, wait for the device to boot for `2 minutes`, and then check the `IP` of the device named Armbian in the router for management settings using `SSH` tool. The default username is `root`, the default password is `1234`, and the default port is `22`.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972715-addcd695-970a-43d6-8a34-24a9c4bc80a2.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972773-fc9e9ef9-69a7-4279-8329-6fad1cf2f5b9.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972818-b72e18cd-17d1-4f9f-a0fa-b6c22eef041d.png width="600" />
</div>

### 8.1 Installation Method for Amlogic Series

Log in to the Armbian system (default user: root, default password: 1234) → Enter the command:

```shell
armbian-install
```

| Optional Parameter | Default Value | Options | Description                          |
| ------------------ | ------------- | ------- | ------------------------------------ |
| -m                 | no            | yes/no  | Use Mainline u-boot                  |
| -a                 | yes           | yes/no  | Use [ampart](https://github.com/7Ji/ampart) partition table adjustment tool |
| -l                 | no            | yes/no  | List. Display the entire device list |

Example: `armbian-install -m yes -a no`

### 8.2 Installation Method for Rockchip Series

The installation method for each device is different, introduced separately as follows.

#### 8.2.1 Installation Method for Radxa-Rock5B

Radxa-Rock5B has multiple storage mediums such as microSD/eMMC/NVMe to choose from, and the corresponding installation methods are also different. Download [rk3588_spl_loader_v1.08.111.bin and spi_image.img](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/rock5b) files for later use. Download [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/Radxa_rock5b_RKDevTool_Release_v2.96__DriverAssitant_v5.1.1.tar.gz) tool and driver for later use. Download [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/) disc writing tools for later use.

##### 8.2.1.1 Install the System to MicroSD

Use Rufus or balenaEtcher and other tools to write the Armbian system image into microSD, then insert the microSD with the system written into the device for use.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972996-300f223b-f6f6-48af-86ca-bdc842e5017d.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202973216-85b1a21b-0763-4a36-8c57-84490e071fdd.png width="600" />
</div>

##### 8.2.1.2 Install the System to eMMC

- Install using a microSD card: Write the Armbian system image into a microSD card, insert the microSD card into the device and start, upload the `armbian.img` image file to the microSD card, use the `dd` command to write the Armbian image into NVMe, the command is as follows:

```Shell
dd if=armbian.img  of=/dev/mmcblk1  bs=1M status=progress
```

- Install using a USB to eMMC card reader: Connect the eMMC module to the computer, use Rufus or balenaEtcher and other tools to write the Armbian system image into eMMC, then insert the eMMC with the system written into the device for use.
- Install using the Maskrom mode: Turn off the power of the development board. Hold down the gold button. Insert the USB-A to Type-C cable into the ROCK 5B Type-C port, and the other end into the PC. Release the gold button. Check that the USB device prompts to find a MASKROM device. Right-click in the blank area of the list, and then choose to load the `rock-5b-emmc.cfg` configuration file (the configuration file and RKDevTool are in the same directory). Set `rk3588_spl_loader_v1.08.111.bin` and `Armbian.img` as shown below, and choose to write.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954901-c829d74d-c75a-4fd3-9bd0-aa3cdf2b77b4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954998-c08514e2-8760-4c0f-b5f7-0d30be635aa5.png width="600" />
</div>

##### 8.2.1.3 Installation of the system on NVMe

The ROCK-5B has an SPI flash memory on its motherboard. Installing the bootloader on this SPI flash memory can support other boot media (such as SATA, USB3, or NVMe) that are not directly supported by the SoC maskrom mode. To use NVMe, you must first write the SPI file. The method is as follows:

Power off the development board. Remove bootable devices, such as MicroSD cards, eMMC modules, etc. Press and hold the golden button (or silver button on some revisions of the development board). Insert the USB-A to C cable into the C-type port of ROCK-5B, and the other end into your PC. Release the golden button. Check for a MASKROM device in the USB devices. Right-click in the list box to load the configuration, then select the configuration file in the resource management folder (the configuration file and RKDevTool are in the same directory), select `rk3588_spl_loader_v1.08.111.bin` and `spi_image.img` files according to the figure below, and click write:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961420-8316c96c-2796-43ed-b5ed-2fa5bfa1ddff.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961447-49c0941a-e233-4b2a-b96b-b47636ce3cf2.png width="600" />
</div>

- Installation using a card reader: Insert the M.2 NVMe SSD into the M.2 NVMe SSD to USB3.0 card reader to connect to the host. Use tools such as Rufus or balenaEtcher to write the Armbian system image to NVMe, then plug the NVMe with the written system into the device to use.
- Installation using a microSD card: Write the Armbian system image to a microSD card, insert the microSD card into the device and boot, upload the `armbian.img` image file to the microSD card, use the `dd` command to write the Armbian image into NVMe, the command is as follows:

```Shell
dd if=armbian.img  of=/dev/nvme0n1  bs=1M status=progress
```

#### 8.2.2 Installation method for FastRhino R66S

Use tools such as Rufus or balenaEtcher to write the Armbian system image to microSD, then plug the microSD with the written system into the device to use.

#### 8.2.3 Installation method for FastRhino R68S

- Download the [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) tool and driver, unzip and install the DriverAssistant driver program, and open the RKDevTool tool for standby.
- In the shutdown state of R68s, insert the USB male-to-male cable first, then press and hold the Recovery key and plug in the 12V power supply, release the Recovery key after two seconds, the flashing tool will `discover a LOADER device`.
- Right-click in the blank space of the RKDevTool tool operation interface to add an item.
- The address is `0x00000000`, the name is `armbian`, and the path is to select the `armbian.img` system file.
- Select the added armbian line, `deselect other lines`, and click `execute` to write.
- Supplement: If other systems have been written into eMMC, please erase them in advanced features first, and then write the Armbian system. If it cannot be erased, write the `MiniLoaderAll.bin` bootloader file again first, then enter `MASKROM` to write the Armbian system. MiniLoaderAll.bin boot file settings: address `0xCCCCCCCC`, name `Loader`, path selects the `MiniLoaderAll.bin` file in the Image directory of the RKDevTool flashing tool.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202970301-d798677b-e875-4971-ac8f-ee58b2a1e686.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970405-cb68cb78-cd0f-43ee-b807-5e594ab73099.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970488-5f18c574-c11f-486f-8fe8-002f3ba2f3f4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970577-87549acf-b98b-441f-bb31-e4fd6608108d.png width="600" />
</div>

#### 8.2.4 Installation method for Beikeyun

The method is reproduced from [milton](https://www.cnblogs.com/milton/p/15391525.html)'s tutorial. Flashing requires entering the Maskrom mode. First, disconnect all connections, short the CLK and GND (using TTL's GND, or the GND next to the small button is fine) two touch points, and then connect the USB to the PC, and you will detect the MASKROM device. Short point position is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202977817-fb12d291-47e2-47e4-88c3-e21f9ae57922.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202977900-50b4770d-8444-42a0-8478-3234043455bd.png width="600" />
</div>

Open the RKDevTool flashing tool, right-click to add an item.

- Address `0xCCCCCCCC`, name `Boot`, path [select](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/beikeyun) `rk3328_loader_v1.14.249.bin`.
- Address `0x00000000`, name `system`, path select the `Armbian.img` system to be flashed.
- Check the "Force Write by Address" option, click "Execute," and wait for the progress to complete on the right-hand download panel.

#### 8.2.5 Installation method for Chainedbox-L1-Pro

The method is reproduced from [cc747](https://post.smzdm.com/p/a4wkdo7l/)'s tutorial. Flashing requires entering the Maskrom mode. Make Chainedbox-L1-Pro in a power-off state, unplug all cables. With a USB male-to-male cable, one end is inserted into the USB2.0 interface of Chainedbox-L1-Pro, and the other end is inserted into the computer. Insert a paperclip into the Reset hole and press and hold it. Insert the power cord. Wait a few seconds until the `discovered a LOADER device` appears at the bottom of the RKDevTool box before releasing the paperclip. Switch RKDevTool to `Advanced Features` and click the `Enter Maskrom` button, prompting `Found a MASKROM device`. Right-click to add an item.

- Address `0xCCCCCCCC`, name `Boot`, path [select](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/chainedbox) `rk3328_loader_v1.14.249.bin`.
- Address `0x00000000`, name `system`, path select the `Armbian.img` system to be flashed.
- Check the "Force Write by Address" option, click "Execute," and wait for the progress to complete on the right-hand download panel.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a6d2d8c0-35c5-44ba-be35-fd2e2758729b width="600" /><br />
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/13aab016-1b93-4ff1-b1ef-c202bd357068 width="600" />
</div>

#### 8.2.6 Installation method for lckfb-tspi
- Download the [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) tool and driver, unzip and install the DriverAssistant driver program, and open the RKDevTool tool .(Note, please use version 2.86 tool instead of 2.92, version 2.92 will crash when flashing)
- tspi in the shutdown state, press and hold the Recovery key and insert the type-c data cable. Release the Recovery key after RKDevTool prompts `A LOADER device was found'. Right click to add item.
- Address `0x00000000`, name `system`, path select the `Armbian.img` system to be flashed.
- Click Execute and wait for the progress to complete.


### 8.3 Allwinner Series Installation Method

Log in to the Armbian system (default user: root, default password: 1234) → Enter the command:

```shell
armbian-install
```

## 9. Compiling Armbian Kernel

Kernel compilation is supported in Ubuntu20.04/22.04, Debian11 or Armbian systems. Both local compilation and GitHub Actions cloud compilation are supported. For specific methods, please refer to the [Kernel Compilation Instructions](../../compile-kernel).

### 9.1 How to Add Custom Kernel Patches

When there is a common kernel patch directory (`common-kernel-patches`) in the kernel patch directory [tools/patch](../../compile-kernel/tools/patch), or when there is a directory named `the same as the kernel source repository` (for example, [linux-5.15.y](https://github.com/unifreq/linux-5.15.y)), you can use `-p true` to automatically apply the kernel patch. The naming convention for the patch directory is as follows:

```shell
~/amlogic-s9xxx-armbian
    └── compile-kernel
        └── tools
            └── patch
                ├── common-kernel-patches  # Fixed directory name: stores common kernel patches for all versions
                ├── linux-5.15.y           # Named after the kernel source library: stores dedicated patches
                ├── linux-6.1.y
                ├── linux-5.10.y-rk35xx
                └── more kernel directory...
```

- When compiling the kernel locally, you can manually create the corresponding directory and add the corresponding custom kernel patches.
- When cloud compiling with GitHub Actions, you can use the `kernel_patch` parameter to specify the directory of the kernel patch in your repository, such as the usage of [compile-beta-kernel.yml](https://github.com/ophub/kernel/blob/main/.github/workflows/compile-beta-kernel.yml) in the [kernel](https://github.com/ophub/kernel) repository:

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.15.1_6.1.1
    kernel_auto: true
    kernel_patch: kernel-patch/beta
    auto_patch: true
```

When using the `kernel_patch` parameter to specify a custom kernel patch, please name the patch directory according to the above convention.

### 9.2 How to Make Kernel Patches

- Obtained from repositories such as [Armbian](https://github.com/armbian/build) and [OpenWrt](https://github.com/openwrt/openwrt): for example, [armbian/patch/kernel](https://github.com/armbian/build/tree/main/patch/kernel/archive) and [openwrt/rockchip/patches-6.1](https://github.com/openwrt/openwrt/tree/main/target/linux/rockchip/patches-6.1), [lede/rockchip/patches-5.15](https://github.com/coolsnowwolf/lede/tree/master/target/linux/rockchip/patches-5.15) etc., patches from these mainline kernel using repositories can generally be used directly.
- Obtained from commits in github.com repositories: Adding a `.patch` suffix to the corresponding `commit` address can generate the corresponding patch.

Before adding a custom kernel patch, it needs to be compared with the upstream kernel source repository [unifreq/linux-k.x.y](https://github.com/unifreq) to confirm whether this patch has been added to avoid conflicts. Kernel patches that pass the test are recommended to be submitted to the series of kernel repositories maintained by unifreq. Each small step for a person is a big step for the world. Your contribution will make our use of Armbian and OpenWrt systems in the box more stable and interesting.


### 9.3 How to Customize Compilation of Driver Modules

In the mainline Linux kernel, some drivers are not yet supported, and you can customize the compilation of driver modules. Select drivers that are supported for use in the mainline kernel; Android drivers are generally not supported in the mainline kernel and cannot be compiled. For example:

```shell
# Step 1: Update to the latest kernel
# Due to incomplete header files in earlier versions, it is necessary to update to the latest kernel version.
# The requirement for each kernel version is not lower than 5.4.280, 5.10.222, 5.15.163, 6.1.100, 6.6.41.
armbian-sync
armbian-update -k 6.1


# Step 2: Install compilation tools
mkdir -p /usr/local/toolchain
cd /usr/local/toolchain
# Download the compilation tools
wget https://github.com/ophub/kernel/releases/download/dev/arm-gnu-toolchain-13.3.rel1-aarch64-aarch64-none-elf.tar.xz
# Extract
tar -Jxf arm-gnu-toolchain-13.3.rel1-aarch64-aarch64-none-elf.tar.xz
# Install additional compilation dependencies (optional; you can manually install missing components based on errors).
armbian-kernel -u


# Step 3: Download and compile the driver
# Download driver source code
cd ~/
git clone https://github.com/jwrdegoede/rtl8189ES_linux
cd rtl8189ES_linux
# Set up the compilation environment
gun_file="arm-gnu-toolchain-13.3.rel1-aarch64-aarch64-none-elf.tar.xz"
toolchain_path="/usr/local/toolchain"
toolchain_name="gcc"
export CROSS_COMPILE="${toolchain_path}/${gun_file//.tar.xz/}/bin/aarch64-none-linux-gnu-"
export CC="${CROSS_COMPILE}gcc"
export LD="${CROSS_COMPILE}ld.bfd"
export ARCH="arm64"
export KSRC=/usr/lib/modules/$(uname -r)/build
# Set the M variable according to the actual path of the source code
export M="/root/rtl8189ES_linux"
# Compile the driver
make


# Step 4: Install the driver
sudo cp -f 8189es.ko /lib/modules/$(uname -r)/kernel/drivers/net/wireless/
# Update module dependencies
sudo depmod -a
# Load the driver module
sudo modprobe 8189es
# Check if the driver is successfully loaded
lsmod | grep 8189es
# You should see the driver successfully loaded
8189es               1843200  0
cfg80211              917504  2 8189es,brcmfmac
```

The illustration is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/1a89cbe6-df38-4862-8d11-9d977e0f4191">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/1a1d0bb9-44d4-4de5-9907-47e5f20747a7">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/d1bd2eff-4c57-4e91-a870-08b0f8b1fe16">
</div>

## 10. Updating Armbian Kernel

Log in to the Armbian system → Enter the command:

```shell
# Run as root user (sudo -i)
# If no parameter is specified, it will be updated to the latest version.
armbian-update
```

| Optional Parameters | Default Value | Options | Description |
| -------- | ------------ | ------------- | -------------------------------- |
| -r | ophub/kernel | `<owner>/<repo>` | Set the repository to download the kernel from github.com |
| -u | Automatic | stable/flippy/dev/rk3588/rk35xx/h6 | Set the suffix of the used kernel's [tags](https://github.com/ophub/kernel/releases) |
| -k | Latest Version | Kernel Version | Set the [Kernel Version](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| -b | yes | yes/no | Automatically backup the kernel currently in use when updating the kernel |
| -m | no | yes/no | Use the mainline u-boot |
| -s | None | None/DiskName | [SOS] Restore the system kernel in eMMC/NVMe/sdX and other disks |
| -h | None | None | View the usage help |

Example: `armbian-update -k 5.15.50 -u dev`

When specifying the kernel version number through the `-k` parameter, you can accurately specify the specific version number, such as: `armbian-update -k 5.15.50`, or you can specify the kernel series vaguely, such as: `armbian-update -k 5.15`, when vaguely specifying, it will automatically use the latest version of the specified series.

When updating the kernel, the currently used system kernel will be automatically backed up, stored in the `/ddbr/backup` directory, retaining the three most recently used kernel versions. If the newly installed kernel is unstable, you can restore to the backed-up kernel at any time:
```shell
# Enter the backup kernel directory, such as 6.6.12
cd /ddbr/backup/6.6.12
# Execute the kernel update command, which will automatically install the kernel in the current directory
armbian-update
```

[SOS]: In case of incomplete updates or other issues preventing the system from booting from eMMC/NVMe/sdX due to special reasons, you can boot an Armbian system with any kernel version from another disk such as USB. Then, run the command `armbian-update -s` to update the kernel from the USB to eMMC/NVMe/sdX, achieving the purpose of rescue. If no disk parameter is specified, the kernel will be restored from the USB device to eMMC/NVMe/sdX by default. If there are multiple disks, you can accurately specify the disk name that needs to be restored. Examples are as follows:

```shell
# Restore the kernel in eMMC
armbian-update -s mmcblk1
# Restore the kernel in NVMe
armbian-update -s nvme0n1
# Restore the kernel in a removable storage device
armbian-update -s sda
# Disk names can be abbreviated as mmcblk1/nvme0n1/sda, etc., or use the complete name such as /dev/sda
armbian-update -s /dev/sda
# When the device has only one built-in storage (eMMC/NVMe/sdX), the disk name parameter can be omitted
armbian-update -s
```

If your network access to github.com is poor and you can't update online, you can manually download the kernel, upload it to any directory of the Armbian system, and enter the kernel directory to execute `armbian-update` for local installation. If there's a complete set of kernel files in the current directory, it will use the kernel from the current directory for the update (the four kernel files needed for the update are `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-xxx.tar.gz`, `modules-xxx.tar.gz`. Other kernel files are not necessary and their presence does not affect the update. The system can accurately identify needed kernel files). You can freely update among the optional kernels supported by the device, such as updating from kernel 6.6.12 to kernel 5.15.50.

Custom options set by `-r`/`-u`/`-b` parameters can be permanently written into the relevant parameters in the individual configuration file `/etc/ophub-release`, to avoid entering it each time. The corresponding settings are:

```shell
# Assign values to custom parameters
-r  :  KERNEL_REPO='ophub/kernel'
-u  :  KERNEL_TAGS='stable'
-b  :  KERNEL_BACKUP='yes'
```

## 11. Installing Common Software

Log in to the Armbian system → Enter the command:

```shell
armbian-software
```

Using the `armbian-software -u` command, you can update the local software center list. Based on user feedback in [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), we gradually integrate commonly used [software](../build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) to implement one-click installation/update/uninstallation and other quick operations. This includes `docker images`, `desktop software`, `application services`, etc. See more [instructions](armbian_software.md).

Use the `armbian-apt` command to select the appropriate software source for your country or region, which can improve software download speeds. For example, select the `mirrors.tuna.tsinghua.edu.cn` source in China:


```shell
armbian-apt

[ STEPS ] Welcome to the Armbian source change script.
[ INFO ] Please select a [ bookworm ] mirror site.
  ┌──────┬───────────────────┬────────────────────────────────┐
  │  ID  │  Country/Region   │  Mirror Site                   │
  ├──────┼───────────────────┼────────────────────────────────┤
  │   0  │  -                │  Restore default source        │
  │   1  │  China            │  mirrors.tuna.tsinghua.edu.cn  │
  │   2  │  China            │  mirrors.bfsu.edu.cn           │
  │   3  │  China            │  mirrors.aliyun.com            │
  │   4  │  Hongkong, China  │  mirrors.xtom.hk               │
  │   5  │  Taiwan, China    │  opensource.nchc.org.tw        │
  ├──────┼───────────────────┼────────────────────────────────┤
  │   6  │  United States    │  mirrors.ocf.berkeley.edu      │
  │   7  │  United States    │  mirrors.xtom.com              │
  │   8  │  United States    │  mirrors.mit.edu               │
  │   9  │  Canada           │  mirror.csclub.uwaterloo.ca    │
  │  10  │  Canada           │  muug.ca/mirror                │
  ├──────┼───────────────────┼────────────────────────────────┤
  │  11  │  Finland          │  mirror.kumi.systems           │
  │  12  │  Netherlands      │  mirrors.xtom.nl               │
  │  13  │  Germany          │  mirrors.xtom.de               │
  │  14  │  Russia           │  mirror.yandex.ru              │
  │  15  │  India            │  in.mirror.coganng.com         │
  ├──────┼───────────────────┼────────────────────────────────┤
  │  16  │  Estonia          │  mirrors.xtom.ee               │
  │  17  │  Australia        │  mirrors.xtom.au               │
  │  18  │  South Korea      │  mirror.yuki.net.uk            │
  │  19  │  Singapore        │  mirror.sg.gs                  │
  │  20  │  Japan            │  mirrors.xtom.jp               │
  └──────┴───────────────────┴────────────────────────────────┘
[ OPTIONS ] Please Input ID: 1
[ INFO ] Your selected source ID is: [ 1 ]
[ STEPS ] Start to change the source of the system: [ mirrors.tuna.tsinghua.edu.cn ]
[ INFO ] The system release is: [ bookworm ]
[ SUCCESS ] Change the source of the system successfully.
```

## 12. Frequently Asked Questions

Here's a compilation of some common issues you may encounter while using Armbian.

### 12.1 dtb and u-boot Correspondence Table for Each Box

The supported TV box list is located in the [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) configuration file in the `Armbian` system.

### 12.2 Instructions for LED Screen Display Control

Please refer to the [instructions](led_screen_display_control.md).

### 12.3 How to Restore the Original Android TV System

The Android TV system on the device is usually backed up and restored using `armbian-ddbr`.

In addition, the Android system can also be flashed into eMMC using the method of flashing via a cable. The download image of the Android system can be found in [Tools](https://github.com/ophub/kernel/releases/tag/tools).

#### 12.3.1 Backup and Restore Using Armbian-ddbr

We recommend that before you install the Armbian system on a brand new box, you first backup the original Android TV system that came with the box. This is in case you need to restore the system later. Boot the Armbian system from `TF/SD/USB`, input the `armbian-ddbr` command, and then enter `b` when prompted to back up the system. The backup file is stored in `/ddbr/BACKUP-arm-64-emmc.img.gz`, please download and save it. To restore the Android TV system, upload the backup file to the same path on the `TF/SD/USB` device, enter the `armbian-ddbr` command, and then enter `r` when prompted to restore the system.


#### 12.3.2 Recovering using Amlogic Flashing Tool

- Generally, if you can boot from USB by reinserting the power supply, you can just reinstall. Try it multiple times if necessary.

- If the screen is black after connecting to a monitor and you can't boot from USB, you'll need to short-circuit initialize the box. First, restore the box to the original Android system, and then reinstall the Armbian system. Download the [amlogic_usb_burning_tool](https://github.com/ophub/kernel/releases/tag/tools) recovery tool and install it. Prepare a [USB double male data cable](https://user-images.githubusercontent.com/68696949/159267576-74ad69a5-b6fc-489d-b1a6-0f8f8ff28634.png) and a [paperclip](https://user-images.githubusercontent.com/68696949/159267790-38cf4681-6827-4cb6-86b2-19c7f1943342.png).

- Taking x96max+ as an example, confirm the location of the [short-circuit point](https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg) on the box's motherboard, and download the box's [Android TV system package](https://github.com/ophub/kernel/releases/tag/tools). Android TV systems and corresponding short-circuit point diagrams for other common devices can also be [downloaded and viewed here](https://github.com/ophub/kernel/releases/tag/tools).

```shell
Operating method:

1. Open USB Burning Tool:
   [ File → Import image ]: X96Max_Plus2_20191213-1457_ATV9_davietPDA_v1.5.img
   [ Select ]：Erase Flash
   [ Select ]：Erase bootloader
   Click on the [ Start ] button
2. Use the [ paperclip ] to short-circuit the [ two short-circuit points ] on the box's motherboard,
   and at the same time use the [ USB double male data cable ] to connect the [ box ] with the [ computer ].
3. When you see the [ progress bar starts moving ], remove the paperclip, no longer short-circuit.
4. When you see [ progress bar at 100% ], the flashing is complete, the box has been restored to the Android TV system.
   Click on the [ Stop ] button, unplug the [ USB double male data cable ] between the [ box ] and [ computer ].
5. If any of the above steps fail, try again until successful.
   If the progress bar does not move, you can try plugging in the power supply. Normally, you don't need power support for flashing, only the power supply from the USB double male can meet the requirements.
```

Once you've finished restoring factory settings, the box has been restored to the Android TV system, and the other steps to install the Armbian system are the same as when you first installed the system, just repeat them.

### 12.4 Setting the box to boot from USB/TF/SD

Based on the situation of your own device, there are two methods to use: initial installation and reinstallation of the Armbian system.

#### 12.4.1 Initial Installation of Armbian System

- Insert the USB/TF/SD with the installed system into the box.
- Enable developer mode: Settings → About device → Version number (e.g., X96max plus...), rapidly click the left mouse button 5 times on the version number, until the system shows the prompt `You are now a developer`.
- Enable USB debugging: System → Advanced options → Developer options (set `USB debugging` to enabled). Enable `ADB` debugging.
- Install ADB tool: Download [adb](https://github.com/ophub/kernel/releases/tag/tools) and extract it, copy the three files `adb.exe`, `AdbWinApi.dll`, `AdbWinUsbApi.dll` to the `system32` and `syswow64` folders under `c://windows/`, then open the `cmd` command panel, use the `adb --version` command, if it shows, it means it can be used.
- Enter `cmd` command mode. Enter the `adb connect 192.168.1.137` command (modify the IP according to your box, you can check it in the router device that the box is connected to), if the connection is successful, it will display `connected to 192.168.1.137:5555`
- Enter the `adb shell reboot update` command, the box will reboot and boot from your inserted USB/TF/SD, you can enter the system by accessing the system's IP address from the browser, or via SSH.

#### 12.4.2 Reinstallation of Armbian System

- In normal situations, you can directly insert the USB flash drive with Armbian installed and boot from it. USB booting takes priority over eMMC.
- In some cases, the device may not boot from the USB flash drive. In such cases, you can rename the `boot.scr` file in the `/boot` directory of the Armbian system on the eMMC. For example, you can rename it to `boot.scr.bak`. After that, you can insert the USB flash drive and boot from it. This way, you will be able to boot from the USB flash drive.

### 12.5 Disable Infrared Receiver

By default, support for the infrared receiver is enabled, but if you are using your TV box as a server, you might want to disable the IR kernel module to prevent it from mistakenly turning off your box. To completely disable IR, add the following line:

```shell
blacklist meson_ir
```

to `/etc/modprobe.d/blacklist.conf` and restart.

### 12.6 Boot file selection

- Currently known devices, only `T95(s905x)` / `T95Z-Plus(s912)` / `BesTV-R3300L(s905l-b)` and a few other devices need to use the `/bootfs/extlinux/extlinux.conf` file, which has been added by default in the system. If other devices need it, you can write the system into USB, double-click to open the `boot` partition, and delete the `.bak` in the system's built-in `/boot/extlinux/extlinux.conf.bak` file name to use. When writing to eMMC, `armbian-install` will automatically check. If the `extlinux.conf` file exists, it will be created automatically.

- Other devices only need `/boot/uEnv.txt` to boot, do not modify the `extlinux.conf.bak` file.

### 12.7 Network Configuration

#### 12.7.1 Network Configuration Using Interfaces 

The default content of the network configuration file `/etc/network/interfaces` is as follows:

```shell
source /etc/network/interfaces.d/*
# Network is managed by Network manager
auto lo
iface lo inet loopback
```

##### 12.7.1.1 IP Address Assignment via DHCP

```shell
source /etc/network/interfaces.d/*

auto eth0
iface eth0 inet dhcp
```

##### 12.7.1.2 Manual Setup of Static IP Address

Modify the IP, gateway, and DNS according to your network condition.

```shell
source /etc/network/interfaces.d/*

auto eth0
allow-hotplug eth0
iface eth0 inet static
hwaddress ether 12:34:56:78:9A:DA
address 192.168.1.100
netmask 255.255.255.0
gateway 192.168.1.1
dns-nameservers 192.168.1.1
```

##### 12.7.1.3 Establish Interconnected Network Using OpenWrt in Docker 

Modify the MAC address according to your needs.

```shell
source /etc/network/interfaces.d/*

allow-hotplug eth0
no-auto-down eth0
auto eth0
iface eth0 inet manual

auto macvlan
iface macvlan inet dhcp
        pre-up ip link add macvlan link eth0 type macvlan mode bridge
        post-down ip link del macvlan link eth0 type macvlan mode bridge

auto lo
iface lo inet loopback
```

#### 12.7.2 Network Management Using NetworkManager

##### 12.7.2.1 Create a New Network Connection

Preparation work before creating or modifying a network connection.

###### 12.7.2.1.1 Get Network Interface Name

Check which network interfaces are available for establishing network connections.

```shell
nmcli device | grep -E "^[e].*|^[w].*|^[D].*|^[T].*" | awk '{printf "%-19s%-19s\n",$1,$2}'
```

The `DEVICE` column displays the network interface name, and the `TYPE` column displays the network interface type.

Where `eth0` = the name of the first Ethernet card, `eth1` = the name of the second Ethernet card, and so on. The same goes for wireless cards.

```shell
DEVICE             TYPE
eth0               ethernet
eth1               ethernet
eth2               ethernet
eth3               ethernet
wlan0              wifi
wlan1              wifi
```

###### 12.7.2.1.2 Get Existing Network Connection Name

Check existing network connections on the device, including used and unused connections. When creating a new network connection, it is recommended not to use existing connection names.

```shell
nmcli connection show | grep -E ".*|^[N].*" | awk '{printf "%-19s%-19s\n", $1,$3}'
```

The `NAME` column displays the name of the existing network connection, and the `TYPE` column displays the network interface type.

Where `ethernet` = Ethernet card, `wifi` = wireless card, `bridge` = bridge

```shell
NAME               TYPE
cnc                ethernet
lan                ethernet
lte                ethernet
tel                ethernet
docker0            bridge
titanium           wifi
cpe                wifi
```

###### 12.7.2.1.3 Create a Wired Network Connection

Create a new network connection on network interface `eth0` and make it effective immediately (`Dynamic IP Address` - `IPv4 / IPv6`).

```shell
# Set ENV
MYCON=ether1                  # New network connection name
MYETH=eth0                    # Network interface name = eth0 / eth1 / eht2 / eth3
IPV6AGM=stable-privacy        # IPv6 address status mode = stable-privacy / eui64

# Add ETH
nmcli connection add \
con-name $MYCON \
type ethernet \
ifname $MYETH \
autoconnect yes \
ipv6.addr-gen-mode $IPV6AGM
nmcli connection up $MYCON
ip -c -br address
```

Create a new network connection on network interface `eth0` and make it effective immediately (`Static IP Address` - `IPv4`).

```shell
# Set ENV
MYCON=ether1                  # Network connection name
MYETH=eth0                    # Network interface name = eth0 / eth1 / eht2 / eth3
IP=192.168.67.167/24          # HOST IP address, where 24 is the subnet mask corresponding to 255.255.255.0
GW=192.168.67.1               # Gateway
DNS=119.29.29.29,223.5.5.5    # DNS server address

# Chg CON
nmcli connection add \
con-name $MYCON \
type ethernet \
ifname $MYETH \
autoconnect yes \
ipv4.method manual \
ipv4.addresses $IP \
ipv4.gateway $GW \
ipv4.dns $DNS
nmcli connection up $MYCON
ip -c -br address
```

###### 12.7.2.1.4 Creating a Wireless Network Connection

Create a network connection on the `wlan0` network interface and take effect immediately (`Dynamic IP address` - `IPv4 / IPv6`).

```shell
# Set ENV
MYCON=ssid                    # Name of the new network connection, it is recommended to use WiFi SSID to specify the connection name
MYSSID=ssid                   # WiFi SSID, case sensitive
MYPSWD=passwd                 # WiFi password
MYWSKM=wpa-psk                # Security selection WPA-WPA2 = wpa-psk or WPA3 = sae (see which encryption method is in the wireless router or AP)
MYWLAN=wlan0                  # Network interface name = wlan0 / wlan1
IPV6AGM=stable-privacy        # IPv6 address status mode = stable-privacy / eui64

# Add WLAN
nmcli connection add \
con-name $MYCON \
type wifi \
ifname $MYWLAN \
autoconnect yes \
ipv6.addr-gen-mode $IPV6AGM \
wifi.ssid $MYSSID \
wifi-sec.key-mgmt $MYWSKM \
wifi-sec.psk $MYPSWD
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.2 Modify WiFi SSID or PASSWD in Wireless Network Connection

Modify the `WiFi SSID or PASSWD` in the wireless network connection `ssid` and take effect immediately.

```shell
# Set ENV
MYCON=ssid                    # Wireless network connection name
MYSSID=ssid                   # WiFi SSID, case sensitive
MYPSWD=passwd                 # WiFi password
MYWSKM=wpa-psk                # Security selection WPA-WPA2 = wpa-psk or WPA3 = sae

# Add WLAN
nmcli connection modify $MYCON \
wifi.ssid $MYSSID \
wifi-sec.key-mgmt $MYWSKM \
wifi-sec.psk $MYPSWD
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.3 Modify Network Address Allocation Method

###### 12.7.2.3.1 Static IP address - IPv4

Modify the IP address allocation method to `Static IP address` on the network connection `ether1` and take effect immediately.

*Applicable to wired connections / wireless connections

```shell
# Set ENV
MYCON=ether1                  # Network connection name
IP=192.168.67.167/24          # HOST IP address, where 24 is the subnet mask corresponding to 255.255.255.0
GW=192.168.67.1               # Gateway
DNS=119.29.29.29,223.5.5.5    # DNS server address

# Chg CON
nmcli connection modify $MYCON \
ipv4.method manual \
ipv4.addresses $IP \
ipv4.gateway $GW \
ipv4.dns $DNS
nmcli connection up $MYCON
ip -c -br address
```

###### 12.7.2.3.2 DHCP Obtains Dynamic IP Address - IPv4 / IPv6

Modify the IP address allocation method to `DHCP Obtains Dynamic IP Address` on the network connection `ether1` and take effect immediately.

*Applicable to wired connections / wireless connections

```shell
# Set ENV
MYCON=ether1                  # Network connection name

# Chg CON
nmcli connection modify $MYCON \
ipv4.method auto \
ipv6.method auto
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.4 Modify Network Connection MAC Address

Modify (clone) the `MAC Address` on the network connection `ether1` and take effect immediately to solve the problem of LAN MAC address conflict.

*Applicable to wired connections / wireless connections

```shell
# Set ENV
MYCON=ether1                  # Network connection name, note to match the network interface type
MYTYPE=ethernet               # Network interface type = Wired network card / Wireless network card = ethernet / wifi
MYMAC=12:34:56:78:9A:BC       # New MAC address

# Chg CON
nmcli connection modify ${MYCON} \
${MYTYPE}.cloned-mac-address ${MYMAC}
nmcli connection up ${MYETH}
ip -c -br address
```

* When creating or modifying some network parameters, the network connection may be disconnected and reconnected to the network.
* Due to different software and hardware environments (box, system, network equipment, etc.), it takes about `1-15` seconds to take effect. If it does not take effect for a longer time, it is recommended to check the software and hardware environment.

##### 12.7.2.5 How to Disable IPv6

You can use the `nmcli` utility to disable the `IPv6` protocol from the command line. Please refer to [disable-ipv6](https://access.redhat.com/documentation/zh-cn/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/using-networkmanager-to-disable-ipv6-for-a-specific-connection_configuring-and-managing-networking) for the source.

Step 1, use the `nmcli connection show` command to view the network connection list, and the returned result is as follows:

```shell
NAME                 UUID                                   TYPE       DEVICE
Wired connection 1   8a7e0151-9c66-4e6f-89ee-65bb2d64d366   ethernet   eth0
...
```

Step 2, set the ipv6.method parameter of the connection to disabled:

```shell
nmcli connection modify "Wired connection 1" ipv6.method "disabled"
```

Step 3, reconnect to the network:

```shell
nmcli connection up "Wired connection 1"
```

Step 4, check the network connection status. If there is no inet6 entry displayed, IPv6 is disabled on the device:

```shell
ip address show eth0
```

Step 5, verify whether the `/proc/sys/net/ipv6/conf/eth0/disable_ipv6` file now contains the value `1`

```shell
# cat /proc/sys/net/ipv6/conf/eth0/disable_ipv6
1
```

#### 12.7.3 How to Enable Wireless

Some devices support using wireless, the enablement method is as follows:

```shell
# Install management tool
sudo apt-get install network-manager

# Check network devices
sudo nmcli dev

# Enable wireless
sudo nmcli r wifi on

# Scan wireless
sudo nmcli dev wifi

# Connect to wireless
sudo nmcli dev wifi connect "wifi name" password "wifi password"

# Display the saved network connection list
sudo nmcli connection show

# Disconnect
sudo nmcli connection down "wifi name"

# Forget the connection and cancel automatic connection
sudo nmcli connection delete "wifi name"
```

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/230541872-565a655e-2781-4170-8898-0ae096725506.png">
</div>

#### 12.7.4 How to Enable Bluetooth

Some devices support using Bluetooth, the enablement method is as follows:

```shell
# Install Bluetooth support
armbian-config >> Network >> BT: Install Bluetooth support

# Reboot the system
reboot
```

After the system reboots, check whether the Bluetooth driver is normal. The desktop system can connect Bluetooth devices from the menu. It can also be installed using the terminal graphical interface.

```shell
dmesg | grep Bluetooth
```

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/230545883-755a137d-f574-4b32-a26b-bea9cfbf6384.png">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/230544120-5a63dcd4-9716-40d2-ba59-c27f7b9937f8.png">
</div>

```shell
# Connect Bluetooth device
armbian-config >> Network >> BT: Discover and connect Bluetooth devices
```

You can also install it using commands in the terminal:

```shell
# Check the Bluetooth service running status
sudo systemctl status bluetooth

# If not started, start the Bluetooth service first
sudo systemctl enable bluetooth
sudo systemctl start bluetooth

# Scan nearby Bluetooth devices
bluetoothctl scan on

# Enable Bluetooth discovery
bluetoothctl discoverable on

# Pair the Bluetooth MAC address
bluetoothctl pair 12:34:56:78:90:AB

# Check the paired Bluetooth devices
blluetoothctl paired-devices

# Connect to the Bluetooth device
bluetoothctl connect 12:34:56:78:90:AB

# Trust the device for easy connection next time
bluetoothctl trust 12:34:56:78:90:AB

# Disconnect the Bluetooth device
bluetoothctl disconnect 12:34:56:78:90:AB

# Unpair the Bluetooth device
bluetoothctl remove 12:34:56:78:90:AB

# Block the connected device
bluetoothctl block 12:34:56:78:90:AB
```

### 12.8 How to Add Startup Tasks

A custom script for startup tasks has already been added to the system. In the Armbian system, the path is [/etc/custom_service/start_service.sh](../build-armbian/armbian-files/common-files/etc/custom_service/start_service.sh). You can customize and add related tasks to this script according to your personal needs.

### 12.9 How to Update Service Scripts in the System

By using the `armbian-sync` command, you can update all service scripts in the local system to the latest version with one click.

If the `armbian-sync` update fails, this suggests that the version of the command is too old. You can update the command using the method below:

```shell
wget https://raw.githubusercontent.com/ophub/amlogic-s9xxx-armbian/main/build-armbian/armbian-files/common-files/usr/sbin/armbian-sync -O /usr/sbin/armbian-sync

chmod +x /usr/sbin/armbian-sync

armbian-sync
```

### 12.10 How to Get Android System Partition Information on eMMC

When we write the Armbian system into the eMMC system, we need to first confirm the Android system partition table of the device to ensure that data is written to a safe area. Try not to damage the Android system partition table to avoid problems such as the system not being able to start. If you write to an unsafe area, it may fail to start or display an error similar to the one below:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/187075834-4ac40263-52ae-4538-a4b1-d6f0d5b9c856.png">
</div>

#### 12.10.1 Obtaining Partition Information

If you are using Armbian released in this repository after 2022.11, you can copy and paste the following command to obtain a URL that records the complete partition information (the device itself does not need to be connected to the internet):

```shell
ampart /dev/mmcblk2 --mode webreport 2>/dev/null
```

*The webreport mode of ampart was introduced in the v1.2 version released on 2023.02.03. If there is no output when you use the above command, it may be an older version that does not support directly outputting URLs. You can instead use the following command:*

```shell
echo "https://7ji.github.io/ampart-web-reporter/?dsnapshot=$(ampart /dev/mmcblk2 --mode dsnapshot 2>/dev/null | head -n 1)&esnapshot=$(ampart /dev/mmcblk2 --mode esnapshot 2>/dev/null | head -n 1)"
```

The URL you obtain will look similar to the one below:

```shell
https://7ji.github.io/ampart-web-reporter/?esnapshot=bootloader:0:4194304:0%20reserved:37748736:67108864:0%20cache:113246208:754974720:2%20env:876609536:8388608:0%20logo:893386752:33554432:1%20recovery:935329792:33554432:1%20rsv:977272832:8388608:1%20tee:994050048:8388608:1%20crypt:1010827264:33554432:1%20misc:1052770304:33554432:1%20instaboot:1094713344:536870912:1%20boot:1639972864:33554432:1%20system:1681915904:1073741824:1%20params:2764046336:67108864:2%20bootfiles:2839543808:754974720:2%20data:3602907136:4131389440:4&dsnapshot=logo::33554432:1%20recovery::33554432:1%20rsv::8388608:1%20tee::8388608:1%20crypt::33554432:1%20misc::33554432:1%20instaboot::536870912:1%20boot::33554432:1%20system::1073741824:1%20cache::536870912:2%20params::67108864:2%20data::-1:4
```

Copy and paste this URL into your browser to view the clear and concise DTB partition information and eMMC partition information:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287642-e1b7be27-4d2c-4ac3-9fcc-15e06aebb97e.png">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287654-d1929e21-d2b3-4fb6-bcf0-c454c88e21b9.png">
</div>

#### 12.10.2 Sharing Partition Information

When you need to share partition information with others (for example, posting to this repository to report on a new device, or seeking help from others), try to share the URL itself rather than a screenshot. If you mind the URL being too long, you can use some free short URL tools.

- On the one hand, the partition information on the webpage is dynamically generated each time you visit. The annotation of whether certain partitions can be written to and the format of the table may be updated.
- On the other hand, others cannot conveniently copy partition parameters from screenshots for calculations, etc.

Also, you don't need to manually organize parameters into a table file. The layout of the table on the webpage has been specifically designed to be easily copied and pasted into Excel or LibreOffice Calc.

#### 12.10.3 Interpreting Partition Information

The DTB table records the partition layout that every box's **system** hopes for in Android DTB. This layout usually ends with a `data` partition of automatically filled size, so boxes of the same system (and therefore, the same model) will necessarily have the same layout here. The actual partition layout on the box may vary due to different eMMC capacities, but it is always determined by the DTB partition layout (i.e., given the DTB partition layout and the exact size of eMMC, the eMMC partition situation can be deduced. *Did you notice that the above DTB partition information and eMMC partition information do not come from the same box?*).

The eMMC table is the actual eMMC partition layout on the box. Each row represents a storage area, which could be a partition or a gap between partitions (due to Amlogic's quirky decision, there is at least an 8M gap between each partition, which was intended to be used for other purposes, but hasn't been used even in the latest S905X4, wasting space). In the row corresponding to a partition, the font is black, and both the offset and mask columns have values. In the row corresponding to a gap, the font is grey, the offset and mask columns have no values, and the partition name is `gap`.

In the eMMC table, the last column of each storage area indicates whether it can be written to. Green and `yes` mean the area can be written to, red and `no` mean the area absolutely cannot be written to, and yellow with a label indicates it can be written to under certain prerequisites, or only part of it can be written to.

Using the above table as an example, the `0+4M` (`0M~4M`) area corresponding to the `bootloader` partition absolutely cannot be written to, the `32M` gap (`4M~36M`) after it can be written to, the `36M+64M` (`36M~100M`) area corresponding to the `reserved` partition absolutely cannot be written to, the gap from there to the gap before `env` (`100M~836M`) can all be written to, the 1M after `env` (`837M to the end`) can be written to in case the Android boot logo is not needed, then the writable range on eMMC is:

- 4M~36M
- 100M~836M
- 837M~end

If the Android boot logo is needed, additionally, the 852M + 32M (`852M~884M`) area corresponding to the `logo` partition cannot be written to, then the writable range on eMMC is:

- 4M~36M
- 100M~836M
- 837M~852M
- 884M~end


#### 12.10.4 For eMMC Installation

If your device fails when using `armbian-install` and the `-a` parameter (use [ampart](https://github.com/7Ji/ampart) to adjust the eMMC partition layout) is `yes` (default value), then your box cannot use the optimal layout (that is, adjust the DTB partition information to only have `data`, then generate eMMC partition information from this, and then move all remaining partitions forward. In this way, the space from 117M onwards can be used). You need to modify the corresponding partition information in [armbian-install](../build-armbian/armbian-files/common-files/usr/sbin/armbian-install).

In this file, the key parameters for declaring the partition layout are three: `BLANK1`, `BOOT`, `BLANK2`. Among them, `BLANK1` represents the unusable size starting from the beginning of eMMC; `BOOT` represents the size of the partition created after `BLANK1` for storing the kernel, DTB, etc., preferably not less than 256M, `BLANK2` represents the unusable size after `BOOT`. The space after this will be used to create the `ROOT` partition to store all the data outside the `/boot` mount point in the system. All three should be integers, and the unit is MiB (1 MiB = 1024 KiB = 1024^2 Byte)

In the case discussed in the previous paragraph where the `logo` partition is not needed, we naturally hope to use all the usable space, but the area of `4M~36M` is too small to be used as `BOOT`, so it can only be counted in the unusable `BLANK1`. The area of `100M~836M` is more than enough to be used as `BOOT`, so this 736M can be allocated to `BOOT` entirely. After this, there is an unusable area of `836M~837M`, which is given to `BLANK2`, so the parameters to be used should be as follows (only `s905x3` is used as an example in the following text, if your SoC is other, you need to modify other corresponding code blocks):

```shell
# Set partition size (Unit: MiB)
elif [[ "${AMLOGIC_SOC}" == "s905x3" ]]; then
    BLANK1="100"
    BOOT="736"
    BLANK2="1"
```

### 12.11 How to build the u-boot file

The u-boot file is a crucial component for the proper startup of the system. The process of obtaining source code and the compilation workflow varies slightly for Amlogic, Allwinner, and Rockchip devices.

#### 12.11.1 How to build the u-boot file for Amlogic devices

Due to the fact that most manufacturers of Amlogic devices keep their source code closed, we need to extract u-boot related files from the device before proceeding with compilation. The method presented here is derived from the production tutorial shared by [unifreq](https://github.com/unifreq).

##### 12.11.1.1 How to extract the bootloader and dtb files

Extraction requires the HxD software. You can get the installation from the [official download link](https://mh-nexus.de/en/downloads.php?product=HxD20) or the [backup download link](https://github.com/ophub/kernel/releases/download/tools/HxDSetup.2.5.0.0.zip).

Execute the following commands one by one in the `cmd` panel to extract the relevant files and download them to your local computer.

```shell
# Use adb tool to enter the box
adb connect 192.168.1.111
adb shell

# Export bootloader command
dd if=/dev/block/bootloader of=/data/local/bootloader.bin

# Export dtb command
cat /dev/dtb >/data/local/mybox.dtb

# Export gpio command
cat /sys/kernel/debug/gpio >/data/local/mybox_gpio.txt

# Download the bootloader, dtb, and gpio files to the root directory of the C drive on your local computer in the mybox folder
adb pull /data/local/bootloader.bin C:\mybox
adb pull /data/local/mybox.dtb C:\mybox
adb pull /data/local/mybox_gpio.txt C:\mybox
```

##### 12.11.1.2 How to create the acs.bin file

The most important part of the mainline u-boot is the acs.bin, which is used to initialize part of the memory. The original factory u-boot is located at the very front of the system, at a 4MB position. Use the `bootloader.bin` file obtained just now to extract the `acs.bin` file.

Open the HxD software, open the exported `bootloader.bin` file, `Right click - Select range`, start position `F200`, length `1000`, select `hexadecimal`.

<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/187056711-1b58ce71-2a7d-4e9b-a976-e5f278edaa53.png">

Copy the selected result, then create a new file, paste in insert mode, ignore the warning, and save it as the acs.bin file.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056725-0a0e60af-6a21-4a6b-a2d5-f3d46b438a6a.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056827-8419c738-3428-473e-9a95-ab7270170d98.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056852-9f62f16a-f7f1-4c34-a2c2-78358d198f9a.png">
</div>

If the bootloader is locked, the code in this area is garbled and useless. Normally, there should be many `0`s as in the picture above, `cfg` will appear several times in succession, and `ddr` related words will appear in the middle. This normal code can be used.

##### 12.11.1.3 How to build the u-boot file

Creating u-boot requires source repositories https://github.com/unifreq/amlogic-boot-fip and https://github.com/unifreq/u-boot to compile two u-boot files for your device.

Within the amlogic-boot-fip source code, the only file that varies by device model is acs.bin, all other files are universal.

<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187057209-c4716384-46ef-4922-9710-8da7ae6db1e4.png">

For instructions on creating u-boot, see the specific instructions in https://github.com/unifreq/u-boot/tree/master/doc/board/amlogic, and choose your device model for compiling and testing.

Creating u-boot according to [unifreq](https://github.com/unifreq)'s method requires the use of the device's acs.bin, dts, and config files. The dts exported from the Android system cannot be directly converted into the Armbian format, so you need to write a corresponding dts file yourself. Based on the specific differences in hardware on your device, such as switches, LEDs, power control, TF card, SDIO wifi module, etc., modify and create a [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) file from the similar ones in the kernel source repository.

For example, creating a u-boot for X96Max Plus:

```shell
~/make-uboot
    ├── amlogic-boot-fip
    │   ├── x96max-plus                                     # Create directory yourself
    │   │   ├── asc.bin                                     # Self-made source file
    │   │   └── other-copy-files...                         # Copy files from other directories
    │   │
    │   ├── other-source-directories...
    │   └── other-source-files...
    │
    └── u-boot
        ├── configs
        │   └── x96max-plus_defconfig                       # Self-made source file
        ├── arch
        │   └── arm
        │       └── dts
        │           ├── meson-sm1-x96-max-plus-u-boot.dtsi  # Self-made source file
        │           ├── meson-sm1-x96-max-plus.dts          # Self-made source file
        │           └── Makefile                            # Edit
        ├── fip
        │   ├── u-boot.bin                                  # Generated file
        │   └── u-boot.bin.sd.bin                           # Generated file
        ├── u-boot.bin                                      # Generated file
        │
        ├── other-source-directories...
        └── other-source-files...
```

- Download the [amlogic-boot-fip](https://github.com/unifreq/amlogic-boot-fip) source code. In the root directory, create a [x96max-plus](https://github.com/unifreq/amlogic-boot-fip/tree/master/x96max-plus) directory. Other than the `asc.bin` file that you created, all other files can be copied from other directories.
- Download the [u-boot](https://github.com/unifreq/u-boot) source code. Create a corresponding [x96max-plus_defconfig](https://github.com/unifreq/u-boot/blob/master/configs/x96max-plus_defconfig) file and put it into the [configs](https://github.com/unifreq/u-boot/tree/master/configs) directory. Create the corresponding [meson-sm1-x96-max-plus-u-boot.dtsi](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus-u-boot.dtsi) and [meson-sm1-x96-max-plus.dts](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus.dts) files and put them in the [arch/arm/dts](https://github.com/unifreq/u-boot/tree/master/arch/arm/dts) directory, then edit the [Makefile](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/Makefile) in this directory to add the `meson-sm1-x96-max-plus.dtb` file index.
- In the root directory of the u-boot source code, follow the steps in the document https://github.com/unifreq/u-boot/blob/master/doc/board/amlogic/x96max-plus.rst.

Two types of files are ultimately generated: the `u-boot.bin` file in the u-boot root directory is an incomplete version of u-boot used in the `/boot` directory (corresponds to the [overload](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) directory in the repository); the `u-boot.bin` and `u-boot.bin.sd.bin` in the `fip` directory are complete versions of u-boot files used in the `/usr/lib/u-boot/` directory (corresponds to the [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) directory in the repository). The complete versions of the two files differ by 512 bytes, the larger one has 512 bytes of 0 filled in front.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189039426-c127631f-77ca-4fcb-9fb6-4220045d712b.png">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189029320-e43a4cc9-b4b5-4de4-92fe-b17bd29020d0.png">
</div>

💡 Tip: Before writing to eMMC for testing, please refer to section 12.3 for unbricking methods. Be sure to understand the short circuit point location, have the original .img format Android system file, and have performed short-circuit flashing tests. Ensure that you have mastered all the unbricking methods before proceeding with writing tests.


#### 12.11.2 How to build the u-boot file for Rockchip devices

Since most manufacturers of Rockchip devices have opened up their u-boot source code, it's relatively easy to obtain the relevant u-boot source code from the manufacturer's source code repository and proceed with the compilation. Additionally, some open-source enthusiasts have also shared numerous user-friendly u-boot compilation scripts. Below, I'll provide a few examples to illustrate various compilation methods.

##### 12.11.2.1 How to use Radxa's u-boot building script

Taking compiling [Rock5b(rk3588)](https://wiki.radxa.com/Rock5/guide/build-u-boot-on-5b) as an example.

```shell
# 01.Install the necessary build dependencies
sudo apt-get update
sudo apt-get install -y git device-tree-compiler libncurses5 libncurses5-dev build-essential libssl-dev mtools bc python dosfstools flex bison

# 02.Set up your workspace and get the source code from the Radxa Git repositories
mkdir ~/rk3588-sdk && cd ~/rk3588-sdk
git clone -b stable-5.10-rock5 https://github.com/radxa/u-boot.git
git clone -b master https://github.com/radxa/rkbin.git
git clone -b debian https://github.com/radxa/build.git

# Explanation of the source code:
# ~/rk3588-sdk/build/: Radxa helper script files and configuration files for building U-Boot, Linux kernel and rootfs.
# ~/rk3588-sdk/rkbin/: Pre-built Rockchip binaries, including first stage loader and ATF (Arm Trustzone Firmware)
# ~/rk3588-sdk/u-boot/: Second stage bootloader used to start the OS (e.g. Linux or Android)

# 03.Build u-boot （For ROCK 5B）
cd ~/rk3588-sdk
./build/mk-uboot.sh rk3588-rock-5b

# 04.After a successful build, the ~/rk3588-sdk/out/u-boot directory will be populated
~/rk3588-sdk/out/u-boot
├── idbloader.img
├── rk3588_spl_loader_v1.08.111.bin
├── spi
│   └── spi_image.img
└── u-boot.itb
```

By adding more options in the `board_configs.sh` and `mk-uboot.sh` within the [radxa/build](https://github.com/radxa/build) source code, it's possible to compile u-boot files for other devices as well. For instance, you can follow the instructions provided for compiling the [Beelink-IPC-R(rk3588)](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415#issuecomment-1508234307) device.


##### 12.11.2.2 How to use cm9vdA's u-boot building script

cm9vdA provides scripts and usage instructions for compiling u-boot and the kernel in his open-source project [cm9vdA/build-linux](https://github.com/cm9vdA/build-linux). I have utilized his project for u-boot compilation in various Rockchip devices and documented the processes for reference. Here are some excerpts:

- Build u-boot for Lenovo-Leez-P710 (rk3399) device: [Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609#issuecomment-1681494735)
- Build u-boot for DLFR100 (rk3399) device: [Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522#issuecomment-1622919423)
- Build u-boot for ZYSJ (rk3399) device: [Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380#issuecomment-1539325464)


### 12.12 Error in Memory Size Recognition

If the memory size is recognized incorrectly (it is abnormal for 4G memory to be recognized as 1-2G, and it is normal to be recognized as 3.7G), you can try to manually copy a `/boot/UBOOT_OVERLOAD` file (please note it's "copy", "do not rename", as renaming it will make the system unable to boot after installation and updates). When used in `USB`, save it as `/boot/u-boot.ext`, and when used in `eMMC`, save it as `/boot/u-boot.emmc`.

Apart from trying to solve memory problems, do not manually copy the u-boot file. Incorrect addition will cause the system to fail to boot and various problems to occur.

### 12.13 How to Decompile dtb Files

Some new devices are not currently supported (or have variants), and you can try to adjust related parameters by decompiling.

```shell
# Install dependencies
sudo apt-get update
sudo apt-get install -y device-tree-compiler

# 1. Decompilation command (generate dts source code using dtb file)
dtc -I dtb -O dts -o xxx.dts xxx.dtb

# 2. Compilation command (generate dtb file using dts)
dtc -I dts -O dtb -o xxx.dtb xxx.dts

# 3. Save data and reboot
sync && reboot

# 4. [Optional action] Perform testing based on requirements
# e.g., reinstall for testing when addressing the issue mentioned in 12.16
armbian-install
```

### 12.14 How to Modify cmdline Settings

In Amlogic devices, you can add/modify/delete settings in the `/boot/uEnv.txt` file. In Rockchip and Allwinner devices, you can set in the `/boot/armbianEnv.txt` file (add to `extraargs` or `extraboardargs` parameters). Devices using `/boot/extlinux/extlinux.conf` configure in this file. You need to restart after each change for it to take effect.

- For instance, the `Home Assistant Supervisor` application only supports the `docker cgroup v1` version, while the currently default installed version for Docker is the latest v2. If you need to switch to the v1 version, you can add the `systemd.unified_cgroup_hierarchy=0` parameter setting in cmdline. After restarting, you can switch to the `docker cgroup v1` version.

- By adding `max_loop=128` in cmdline, you can adjust the allowable amount of loop mounts.

- By adding `usbcore.usbfs_memory_mb=1024` in cmdline, you can permanently change the USBFS memory buffer from the default `16 mb` to larger (`cat /sys/module/usbcore/parameters/usbfs_memory_mb`), improving the ability to transfer large files over USB.

- By adding `usbcore.usb3_disable=1` in cmdline, you can disable all USB 3.0 devices.

- By adding `extraargs=video=HDMI-A-1:1920x1080@60` in cmdline, you can force the video display mode to 1080p.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://user-images.githubusercontent.com/68696949/216220941-47db0183-7b26-4768-81cf-2ee73d59d23e.png">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a600dcad-d817-47eb-b529-4014019915b3">
</div>

### 12.15 How to Add New Supported Devices

To build an Armbian system for a device, you need to use the `device configuration file`, `system file`, `u-boot file`, and `process control file`. The specific addition methods are introduced as follows:

#### 12.15.1 Add Device Configuration File

In the configuration file [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf), add the corresponding configuration information according to the device's test support status. The `BUILD` value is `yes` for some default build devices, the corresponding `BOARD` value `must be unique`, these boxes can use the default built Armbian system directly.

The default value is `no` without packaging, these devices need to download the same `FAMILY` Armbian system when using, after writing to `USB`, you can open `USB's boot partition` on your computer, modify the `FDT dtb name` in `/boot/uEnv.txt` file, adapt to other devices in the list.

#### 12.15.2 Add System Files

Common files are placed in the `build-armbian/armbian-files/common-files` directory, universally applicable across platforms.

Platform files are respectively placed in `build-armbian/armbian-files/platform-files/<platform>` directory, [Amlogic](../build-armbian/armbian-files/platform-files/amlogic), [Rockchip](../build-armbian/armbian-files/platform-files/rockchip), and [Allwinner](../build-armbian/armbian-files/platform-files/allwinner) share files of their respective platforms. The `bootfs` directory contains /boot partition files, and the `rootfs` directory contains Armbian system files.

If individual devices have special differential setting requirements, add an independent directory named after `BOARD` in the `build-armbian/armbian-files/different-files` directory, create `bootfs` directory as needed to add related files under system `/boot` partition, create `rootfs` directory as needed to add system files. All folder names are based on the actual path in the `Armbian` system. Used to add new files, or to override the same name files added from the common files and platform files.

#### 12.15.3 Add u-boot Files

`Amlogic` series devices, share [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) files and [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) files. If there are new files, put them in the corresponding directory. The `bootloader` files will automatically be added to the Armbian system's `/usr/lib/u-boot` directory during system construction, and `u-boot` files will be automatically added to the `/boot` directory.

`Rockchip` and `Allwinner` series devices, add an independent [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot) file directory named after `BOARD` for each device, and the corresponding series files are placed in this directory.

During the Armbian image construction, these u-boot files will be written into the corresponding Armbian image files by the rebuild script according to the configuration in [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf).

#### 12.15.4 Add Process Control Files

Add the corresponding `BOARD` option to `armbian_board` in the [yml workflow control file](../../.github/workflows/build-armbian.yml), which supports use in `Actions` on github.com.

### 12.16 How to Resolve the Issue of I/O Errors While Writing to eMMC

Some devices can normally boot Armbian from USB/SD/TF, but when writing to eMMC, an I/O write error is reported, such as the case in [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues/989), with the following error message:

```shell
[  284.338449] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.341544] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.446972] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.450074] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.497746] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.500871] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
```

In such a situation, you can adjust the working mode speed and frequency of the dtb being used to stabilize the read and write support for storage. When using sdr mode, the frequency is twice the speed. When using ddr mode, the frequency equals the speed. For example:

```shell
sd-uhs-sdr12
sd-uhs-sdr25
sd-uhs-sdr50
sd-uhs-ddr50
sd-uhs-sdr104

max-frequency = <208000000>;
```

Take the code snippet in the [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) file of the kernel source code as an example:

```shell
/* SD card */
&sd_emmc_b {
	status = "okay";

	bus-width = <4>;
	cap-sd-highspeed;
	sd-uhs-sdr12;
	sd-uhs-sdr25;
	sd-uhs-sdr50;
	max-frequency = <100000000>;
};

/* eMMC */
&sd_emmc_c {
	status = "okay";

	bus-width = <8>;
	cap-mmc-highspeed;
	max-frequency = <100000000>;
};
```

Generally, reducing the frequency of `&sd_emmc_c` from `max-frequency = <200000000>;` to `max-frequency = <100000000>;` can solve the problem. If it doesn't work, you can continue to reduce it to `50000000` for testing, and adjust `&sd_emmc_b` to set `USB/SD/TF`, you can also use `sd-uhs-sdr` to limit speed. You can modify the dts file and [compile](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel) to get the test file, or you can use the method introduced in `Section 12.13` to decompile and modify the existing dtb file to generate the test file. When modifying the decompiled dtb file, use hexadecimal values, where the decimal `200000000` corresponds to the hexadecimal `0xbebc200`, the decimal `100000000` corresponds to the hexadecimal `0x5f5e100`, the decimal `50000000` corresponds to the hexadecimal `0x2faf080`, and the decimal `25000000` corresponds to the hexadecimal `0x17d7840`.

In addition to solving this issue through the system software layer, it can also be resolved through [money ability](https://github.com/ophub/amlogic-s9xxx-armbian/issues/998) and [hands-on ability](https://www.right.com.cn/forum/thread-901586-1-1.html).

### 12.17 How to Solve the Issue of No Sound in the Bullseye Version

Error log information for the sound issue:

```shell
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: dpcm_fe_dai_prepare() failed (-22)
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: no backend DAIs enabled for fe.dai-link-0
```

Please refer to the method in [Bullseye NO Sound](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1000) for settings.

```shell
curl -fsSOL https://github.com/ophub/kernel/releases/download/tools/bullseye_g12_sound-khadas-utils-4-2-any.tar.gz
tar -xzf bullseye_g12_sound-khadas-utils-4-2-any.tar.gz -C /

systemctl enable sound.service
systemctl restart sound.service
```

Restart Armbian for testing. If the sound still doesn't work, it may be because your box is using the old conf corresponding to the sound output route. You need to comment out the new configuration corresponding to `L137-L142` in /usr/bin/g12_sound.sh (mainly for G12B, that is, S922X, before the old G12A/S905X2, and most of the SM1/S905X3 based on G12A can't be used), and then uncomment the old configuration corresponding to `L130-L134`.

### 12.18 How to build the boot.scr file

In the Armbian system, the `boot.scr` file in the `/boot` directory is used for booting the system. `boot.scr` is the compiled version of the `boot.cmd` file. `boot.cmd` is the source code file for `boot.scr`. You can modify the `boot.cmd` file to make changes to the `boot.scr` file and then compile it into a `boot.scr` file using the mkimage command.

Normally, these two files do not need to be modified. If adjustments are necessary, you can follow the methods below.

```shell
# Install dependencies
sudo apt-get update
sudo apt-get install -y u-boot-tools

# Edit the boot.cmd file
cd /boot
copy /boot/boot.cmd /boot/boot.cmd.bak
copy /boot/boot.scr /boot/boot.scr.bak
nano boot.cmd

# Compile the boot.scr file
mkimage -C none -A arm -T script -d boot.cmd boot.scr

# Restart to test
sync
reboot

# Additional Explanation
# For Amlogic devices, the file used in USB is /boot/boot.scr, while the file used for writing to eMMC is /boot/boot-emmc.scr.
```

### 12.19 How to Enable Remote Desktop and Modify the Default Port

In the software center `armbian-software`, selecting `201` allows you to install a desktop. When installing the desktop, you will be asked whether to enable the remote desktop, input `y` to enable. The default port for the remote desktop is `3389`, and you can use a custom port according to your needs:

```shell
sudo nano /etc/xrdp/xrdp.ini
# Change to a custom port, for example 5000
port=5000
```

