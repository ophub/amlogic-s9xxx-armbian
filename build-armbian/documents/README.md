# Armbian Building and Usage Guide

View Chinese instructions | [查看中文说明](README.cn.md)

Github Actions is a service launched by Microsoft that provides virtual server environments with excellent performance configurations. Based on this, projects can be built, tested, packaged, and deployed. It can be used for public repositories free of charge and without time limits. With a single compile time of up to 6 hours, this is sufficient for compiling Armbian (we generally complete one compilation work in about 3 hours). Sharing is only for exchanging experiences, please understand the shortcomings, do not initiate various malicious attacks on the network, and do not maliciously use GitHub Actions.

# Table of Contents

- [Armbian Building and Usage Guide](#armbian-building-and-usage-guide)
- [Table of Contents](#table-of-contents)
  - [1. Register your GitHub account](#1-register-your-github-account)
  - [2. Set up privacy variable GITHUB\_TOKEN](#2-set-up-privacy-variable-github_token)
  - [3. Fork repository and set up GH\_TOKEN](#3-fork-repository-and-set-up-gh_token)
  - [4. Personalized Armbian system customization file description](#4-personalized-armbian-system-customization-file-description)
  - [5. Compile the system](#5-compile-the-system)
    - [5.1 Manual compilation](#51-manual-compilation)
    - [5.2 Scheduled compilation](#52-scheduled-compilation)
    - [5.3 Custom default system configuration](#53-custom-default-system-configuration)
  - [6. Save the system](#6-save-the-system)
  - [7. Download the system](#7-download-the-system)
  - [8. Install Armbian to eMMC](#8-install-armbian-to-emmc)
    - [8.1 Installation method for Amlogic series](#81-installation-method-for-amlogic-series)
    - [8.2 Installation method for Rockchip series](#82-installation-method-for-rockchip-series)
      - [8.2.1 Installation method for Radxa-Rock5B](#821-installation-method-for-radxa-rock5b)
        - [8.2.1.1 Install the system to microSD](#8211-install-the-system-to-microsd)
        - [8.2.1.2 Install the system to eMMC](#8212-install-the-system-to-emmc)
        - [8.2.1.3 Installing the system to NVMe](#8213-installing-the-system-to-nvme)
      - [8.2.2 Installation method of the FastRhino R66S](#822-installation-method-of-the-fastrhino-r66s)
      - [8.2.3 Installation method of the FastRhino R68S](#823-installation-method-of-the-fastrhino-r68s)
      - [8.2.4 Installation method of Beikeyun](#824-installation-method-of-beikeyun)
      - [8.2.5 Installation method of Chainedbox-L1-Pro](#825-installation-method-of-chainedbox-l1-pro)
    - [8.3 Allwinner Series Installation Method](#83-allwinner-series-installation-method)
  - [9. Compile Armbian Kernel](#9-compile-armbian-kernel)
    - [9.1 How to Add Custom Kernel Patches](#91-how-to-add-custom-kernel-patches)
    - [9.2 How to create kernel patches](#92-how-to-create-kernel-patches)
  - [10. Update Armbian Kernel](#10-update-armbian-kernel)
  - [11. Install Common Software](#11-install-common-software)
  - [12. Common Issues](#12-common-issues)
    - [12.1 Correspondence table between dtb and u-boot of each box](#121-correspondence-table-between-dtb-and-u-boot-of-each-box)
    - [12.2 LED screen display control instructions](#122-led-screen-display-control-instructions)
    - [12.3 How to restore the original Android TV system](#123-how-to-restore-the-original-android-tv-system)
      - [12.3.1 Backup and Restore with armbian-ddbr](#1231-backup-and-restore-with-armbian-ddbr)
      - [12.3.2 Restore with Amlogic flash tool](#1232-restore-with-amlogic-flash-tool)
    - [12.4 Set the box to boot from USB/TF/SD](#124-set-the-box-to-boot-from-usbtfsd)
    - [12.5 Disable the IR receiver](#125-disable-the-ir-receiver)
    - [12.6 Bootloader Selection](#126-bootloader-selection)
    - [12.7 Network Settings](#127-network-settings)
      - [12.7.1 Setting up network with interfaces](#1271-setting-up-network-with-interfaces)
        - [12.7.1.1 Use DHCP to dynamically allocate IP addresses](#12711-use-dhcp-to-dynamically-allocate-ip-addresses)
        - [12.7.1.2 Manually set static IP address](#12712-manually-set-static-ip-address)
        - [12.7.1.3 Establishing a network with OpenWrt in Docker](#12713-establishing-a-network-with-openwrt-in-docker)
      - [12.7.2 Managing Network with NetworkManager](#1272-managing-network-with-networkmanager)
        - [12.7.2.1 Creating a New Network Connection](#12721-creating-a-new-network-connection)
          - [12.7.2.1.1 Get the Network Interface Name](#127211-get-the-network-interface-name)
          - [12.7.2.1.2 Get the Existing Network Connection Names](#127212-get-the-existing-network-connection-names)
          - [12.7.2.1.3 Creating a Wired Network Connection](#127213-creating-a-wired-network-connection)
          - [12.7.2.1.4 Creating a Wireless Network Connection](#127214-creating-a-wireless-network-connection)
        - [12.7.2.2 Modifying WiFi SSID or PASSWD in a Wireless Network Connection](#12722-modifying-wifi-ssid-or-passwd-in-a-wireless-network-connection)
        - [12.7.2.3 Modifying Network Address Allocation Method](#12723-modifying-network-address-allocation-method)
          - [12.7.2.3.1 Static IP Address - IPv4](#127231-static-ip-address---ipv4)
          - [12.7.2.3.2 DHCP Obtains Dynamic IP Address - IPv4 / IPv6](#127232-dhcp-obtains-dynamic-ip-address---ipv4--ipv6)
        - [12.7.2.4 Modifying Network Connection MAC Address](#12724-modifying-network-connection-mac-address)
        - [12.7.2.5 How to Disable IPv6](#12725-how-to-disable-ipv6)
      - [12.7.3 How to Enable Wireless](#1273-how-to-enable-wireless)
      - [12.7.4 How to Enable Bluetooth](#1274-how-to-enable-bluetooth)
    - [12.8 How to Add Startup Tasks](#128-how-to-add-startup-tasks)
    - [12.9 How to Update Service Scripts in the System](#129-how-to-update-service-scripts-in-the-system)
    - [12.10 How to Get Partition Information of Android System on eMMC](#1210-how-to-get-partition-information-of-android-system-on-emmc)
      - [12.10.1 Get Partition Information](#12101-get-partition-information)
      - [12.10.2 Sharing Partition Information](#12102-sharing-partition-information)
      - [12.10.3 Interpreting Partition Information](#12103-interpreting-partition-information)
      - [12.10.4 For eMMC Installation](#12104-for-emmc-installation)
    - [12.11 How to make u-boot files](#1211-how-to-make-u-boot-files)
      - [12.11.1 Extracting bootloader and dtb files](#12111-extracting-bootloader-and-dtb-files)
      - [12.11.2 Creating the acs.bin file](#12112-creating-the-acsbin-file)
      - [12.11.3 Creating u-boot files](#12113-creating-u-boot-files)
    - [12.12 Memory size recognition error](#1212-memory-size-recognition-error)
    - [12.13 How to decompile dtb files](#1213-how-to-decompile-dtb-files)
    - [12.14 How to modify cmdline settings](#1214-how-to-modify-cmdline-settings)
    - [12.15 How to add support for new devices](#1215-how-to-add-support-for-new-devices)
      - [12.15.1 Add device configuration files](#12151-add-device-configuration-files)
      - [12.15.2 Add system files](#12152-add-system-files)
      - [12.15.3 Adding u-boot files](#12153-adding-u-boot-files)
      - [12.15.4 Adding workflow control files](#12154-adding-workflow-control-files)
    - [12.16 How to solve I/O errors when writing to eMMC](#1216-how-to-solve-io-errors-when-writing-to-emmc)
    - [12.17 How to solve the problem of no sound in Bullseye version](#1217-how-to-solve-the-problem-of-no-sound-in-bullseye-version)

## 1. Register your GitHub account

Register your account on GitHub to continue with system customization. Click the `Sign up` button in the top right corner of the github.com website and follow the prompts to register your account.

## 2. Set up privacy variable GITHUB_TOKEN

Set up the GitHub privacy variable `GITHUB_TOKEN`. After the system compilation is completed, we need to upload the system to Releases. We set this variable according to Github's official requirements as follows: Personal center: Settings > Developer settings > Personal access tokens > Generate new token (Name: GITHUB_TOKEN, Select: public_repo). Other options can be selected according to your own needs. Submit and save, copy the encrypted KEY generated by the system, save it to your computer's Notepad first, and we will use this value in the next step. The following picture shows the process:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/109418474-85032b00-7a03-11eb-85a2-759b0320cc2a.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418479-8b91a280-7a03-11eb-8383-9d970f4fffb6.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418483-90565680-7a03-11eb-8320-0df1174b0267.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418493-9815fb00-7a03-11eb-862e-deca4a976374.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418485-93514700-7a03-11eb-848d-36de784a4438.jpg width="300" />
</div>

## 3. Fork repository and set up GH_TOKEN

Now you can fork the repository. Open the repository https://github.com/ophub/amlogic-s9xxx-armbian, click the `Fork` button in the upper right corner, and copy a copy of the repository code to your account. After waiting for a few seconds, when prompted that Fork is completed, go to your account to access the amlogic-s9xxx-armbian in your own repository. In the upper right corner, click on `Settings` > `Secrets` > `Actions` > `New repostiory secret` (Name: `GH_TOKEN`, Value: `Fill in the value of GITHUB_TOKEN just now`) and save it. And under `Actions` > `General` > `Workflow permissions` in the left navigation bar, select `Read and write permissions` and save it. The following picture shows the process:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/109418568-0eb2f880-7a04-11eb-81c9-194e32382998.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163203032-f044c63f-d113-4076-bf94-41f86c7dd0ce.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418573-15417000-7a04-11eb-97a7-93973d7479c2.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/167579714-fdb331f3-5198-406f-b850-13da0024b245.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/167585338-841d3b05-8d98-4d73-ba72-475aad4a95a9.png width="300" />
</div>

## 4. Personalized Armbian system customization file description

The system compilation process is controlled in the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file, and there are other .yml files in the workflows directory to achieve other different functions. When compiling the system, the current code of the Armbian official website is used for real-time compilation, and related parameters can be found in the official documentation.

```yaml
- name: Compile Armbian [ ${{ env.ARMBIAN_BOARD }} ]
  id: compile
  run: |
    cd build/
    ./compile.sh RELEASE=${{ env.ARMBIAN_RELEASE }} BOARD=odroidn2 BRANCH=current BUILD_ONLY=default HOST=armbian EXPERT=yes \
        BUILD_DESKTOP=no BUILD_MINIMAL=no KERNEL_CONFIGURE=no CLEAN_LEVEL="make,cache,alldebs,sources" COMPRESS_OUTPUTIMAGE="sha"
    echo "build_tag=Armbian_${{ env.ARMBIAN_RELEASE }}_$(date +"%m.%d.%H%M")" >> ${GITHUB_OUTPUT}
    echo "status=success" >> ${GITHUB_OUTPUT}
```

## 5. Compile the system

There are many ways to compile the system, you can set up scheduled compilation, manual compilation, or set some specific events to trigger compilation. Let's start with a simple operation.

### 5.1 Manual compilation

In the navigation bar of your own repository, click the Actions button, and then click Build armbian > Run workflow > Run workflow in turn to start compiling. Wait for about 3 hours. After all the processes are completed, the compilation is complete. The following picture shows the process:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163203938-e7762b09-e6b8-4cf5-b1f1-9c67c1a29953.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204044-9c7a7429-47ee-4fce-b7dd-e217bebf6133.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204127-6b16b558-7e78-4c22-a28a-7b37b5a34fa3.png width="300" />
</div>

### 5.2 Scheduled compilation

In the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file, use Cron to set up scheduled compilation. The meanings of the 5 different positions represent minutes (0-59) / hours (0-23) / date (1-31) / month (1-12) / day of the week (0-6) (Sunday - Saturday). Modify the values in different positions to set the time. The system uses UTC standard time by default, please convert it according to the time zone of your country.

```yaml
schedule:
  - cron: '0 17 * * *'
```

### 5.3 Custom default system configuration

The configuration information of the default system is recorded in the [model_database.conf](../armbian-files/common-files/etc/model_database.conf) file, and the `BOARD` name must be unique.

The value of `BUILD` is `yes` for the default packaged systems of some boxes that can be used directly. Those with a default value of `no` are not packaged. When using these unpacaged boxes, you need to download the packaged system of the same `FAMILY` (it is recommended to download the system with kernel version `5.15/5.4`) and write it to the USB drive. After that, you can open the `boot partition` in the USB on the computer and modify the `dtb name of FDT` in the `/boot/uEnv.txt` file to adapt to other boxes in the compatibility list.

When compiling locally, specify it through the `-b` parameter. When compiling in github.com's Actions, specify it through the `armbian_board` parameter. Using `-b all` represents packaging all devices whose `BUILD` is `yes`. When packaging with a specified `BOARD` parameter, both `BUILD` being `yes` or `no` can be packaged, for example:`-b r68s_s905x3-tx3_s905l3a-cm311`.

## 6. Save the system

The system saving settings are also controlled in the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file. We use a script to automatically upload the compiled system to the Releases provided by GitHub.

```yaml
- name: Upload Armbian image to Release
  uses: ncipollo/release-action@main
  if: ${{ env.PACKAGED_STATUS }} == 'success' && !cancelled()
  with:
    tag: Armbian_${{ env.ARMBIAN_RELEASE }}_${{ env.PACKAGED_OUTPUTDATE }}
    artifacts: ${{ env.PACKAGED_OUTPUTPATH }}/*
    allowUpdates: true
    token: ${{ secrets.GH_TOKEN }}
    body: |
      These are the Armbian OS image
      * OS information
      Default username: root
      Default password: 1234
      Install command: armbian-install
      Update command: armbian-update
```

## 7. Download the system

Enter the Release section in the lower right corner of the repository's homepage, and select the system corresponding to your own box model. The following picture shows the process:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163204798-0d98524c-73df-4876-8912-fcae2845fbba.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204879-4898babf-fa00-4e63-89ea-235129e2ce1d.png width="300" />
</div>

## 8. Install Armbian to eMMC

The installation method for Amlogic, Rockchip, and Allwinner is different. Different devices have different storage methods. Some devices use external microSD cards, some have eMMC built-in, and some support multiple storage media such as NVMe. According to different devices, their installation methods are introduced separately. First, download the Armbian system of your own device from [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) and unzip it into .img format for later use. According to your own device, use different installation methods in the following summary.

After the installation is completed, connect the Armbian device to the `router`, and after the device has been turned on for `2 minutes`, check the `IP` of the device named Armbian in the router, and use an `SSH` tool to connect for management settings. The default username is `root`, the default password is `1234`, and the default port is `22`.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972715-addcd695-970a-43d6-8a34-24a9c4bc80a2.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972773-fc9e9ef9-69a7-4279-8329-6fad1cf2f5b9.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972818-b72e18cd-17d1-4f9f-a0fa-b6c22eef041d.png width="600" />
</div>

### 8.1 Installation method for Amlogic series

Login to the Armbian system (default user: root, default password: 1234) → Enter the command:

```shell
armbian-install
```

| Optional | Default | Options | Explanation |
| -------- | ------- | ------- | ----------- |
| -m       | no      | yes/no  | Use Mainline u-boot |
| -a       | yes     | yes/no  | Use [ampart](https://github.com/7Ji/ampart) partition table adjustment tool |
| -l       | no      | yes/no  | List. Display all device lists |

Example: `armbian-install -m yes -a no`

### 8.2 Installation method for Rockchip series

The installation methods of each device are different and are introduced below.

#### 8.2.1 Installation method for Radxa-Rock5B

Radxa-Rock5B has multiple storage media such as microSD/eMMC/NVMe to choose from, and the corresponding installation methods are also different. Download [rk3588_spl_loader_v1.08.111.bin and spi_image.img](../u-boot/rockchip/rock5b) files for backup. Download [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/Radxa_rock5b_RKDevTool_Release_v2.96__DriverAssitant_v5.1.1.tar.gz) tool and driver for later use. Download [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/) disk writing tools for backup.

##### 8.2.1.1 Install the system to microSD

Use tools such as Rufus or balenaEtcher to write the Armbian system image to the microSD, and then insert the microSD with the written system into the device to use.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972996-300f223b-f6f6-48af-86ca-bdc842e5017d.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202973216-85b1a21b-0763-4a36-8c57-84490e071fdd.png width="600" />
</div>

##### 8.2.1.2 Install the system to eMMC

- Install using microSD card: Write the Armbian system image to the microSD card, insert the microSD card into the device and boot it up, upload the `armbian.img` image file to the microSD card, and use the `dd` command to write the Armbian image to the NVMe, as follows:

```Shell
dd if=armbian.img of=/dev/mmcblk1 bs=1M status=progress
```

- Use USB to eMMC card reader for installation: Connect the eMMC module to the computer, and use tools such as Rufus or balenaEtcher to write the Armbian system image to the eMMC. Then insert the eMMC with the written system into the device to use.
- Use Maskrom mode for installation: Turn off the development board power. Hold down the gold button. Insert the USB-A to C cable into the ROCK 5B C port, and the other end into the PC. Release the gold button. Check the USB device prompt to find a MASKROM device. Right-click on the blank area of ​​the list, and then select to load the `rock-5b-emmc.cfg` configuration file (the configuration file and RKDevTool are in the same directory). Set `rk3588_spl_loader_v1.08.111.bin` and `Armbian.img` as shown below, and select write.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954901-c829d74d-c75a-4fd3-9bd0-aa3cdf2b77b4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954998-c08514e2-8760-4c0f-b5f7-0d30be635aa5.png width="600" />
</div>

##### 8.2.1.3 Installing the system to NVMe

ROCK-5B has an SPI flash on the motherboard. Installing the bootloader to the SPI flash can support other boot media (such as SATA, USB3 or NVMe) that SoC maskrom mode does not directly support. To use NVMe, you need to write the SPI file first. The method is as follows:

Turn off the development board power. Remove the bootable device, such as MicroSD card, eMMC module, etc. Hold down the gold (or silver on some revision of the development board) button. Insert the USB-A to C cable into the ROCK-5B C port and the other end into the PC. Release the gold button. Check the USB device to find a MASKROM device. Right-click in the list box to select Load Configuration, then select the configuration file in the resource management folder (the configuration file and RKDevTool are in the same directory), according to the figure below Select `rk3588_spl_loader_v1.08.111.bin` and `spi_image.img` files, click Write, as shown below:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961420-8316c96c-2796-43ed-b5ed-2fa5bfa1ddff.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961447-49c0941a-e233-4b2a-b96b-b47636ce3cf2.png width="600" />
</div>

- Install using a card reader: Insert the M.2 NVMe SSD into the M.2 NVMe SSD to USB3.0 card reader to connect to the host. Write the Armbian system image to the NVMe using tools such as Rufus or balenaEtcher and then insert the NVMe with the written system into the device to use.
- Install using microSD card: Write the Armbian system image to the microSD card, insert the microSD card into the device and boot it up, upload the `armbian.img` image file to the microSD card, and use the `dd` command to write the Armbian image to the NVMe, as follows:

```Shell
dd if=armbian.img of=/dev/nvme0n1 bs=1M status=progress
```

#### 8.2.2 Installation method of the FastRhino R66S

Write the Armbian system image to the microSD using tools such as Rufus or balenaEtcher, and then insert the microSD with the written system into the device to use.

#### 8.2.3 Installation method of the FastRhino R68S

- Download the [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) tool and driver, unzip it and install DriverAssistant driver program, and open RKDevTool tool for later use.
- When the R68s is powered off, plug in the USB double-headed cable first, then press and hold the Recovery key and plug in the 12V power supply. Release the Recovery key after two seconds, and the flashing tool will "find a LOADER device".
- Right-click in the blank space of the RKDevTool operation interface and add an item.
- The address is `0x00000000`, the name is `armbian`, and the path is selected on the right side to select the `armbian.img` system file.
- Outside of the armbian line that was added, `deselect other lines` and click `Execute` to write.
- Supplementary explanation: If another system has been written to the eMMC, please erase it first in the advanced features, and then write the Armbian system. If erasing is not possible, write the `MiniLoaderAll.bin` boot file again first, and then enter `MASKROM` to write the Armbian system. MiniLoaderAll.bin boot file settings: Address `0xCCCCCCCC`, Name `Loader`, Path Select the `MiniLoaderAll.bin` file in the RKDevTool flashing tool Image directory.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202970301-d798677b-e875-4971-ac8f-ee58b2a1e686.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970405-cb68cb78-cd0f-43ee-b807-5e594ab73099.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970488-5f18c574-c11f-486f-8fe8-002f3ba2f3f4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970577-87549acf-b98b-441f-bb31-e4fd6608108d.png width="600" />
</div>

#### 8.2.4 Installation method of Beikeyun

This method is reprinted from the tutorial by [milton](https://www.cnblogs.com/milton/p/15391525.html). To flash the device, you need to enter Maskrom mode. Disconnect all connections first, short the CLK and GND (use the TTL GND or the nearby small button GND) these two points, and then connect the USB to the PC, and the MASKROM device will be detected. The positions of the short circuit points are as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202977817-fb12d291-47e2-47e4-88c3-e21f9ae57922.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202977900-50b4770d-8444-42a0-8478-3234043455bd.png width="600" />
</div>

Open the RKDevTool flashing tool and right-click to add an item.

- Address `0xCCCCCCCC`, Name `Boot`, Path [select](../u-boot/rockchip/beikeyun) `rk3328_loader_v1.14.249.bin`.
- Address `0x00000000`, Name `system`, select the `Armbian.img` system to be flashed.

Click execute to write.

#### 8.2.5 Installation method of Chainedbox-L1-Pro

This method is reprinted from the tutorial by [cc747](https://post.smzdm.com/p/a4wkdo7l/). To flash the device, you need to enter Maskrom mode. Make Chainedbox-L1-Pro in a power-off state and unplug all wires. Use a USB double-headed cable, plug one end into the USB 2.0 port of Chainedbox-L1-Pro, and the other end into the computer. Insert a paper clip into the Reset hole and press it without releasing it. Insert the power cord. Wait for a few seconds until "LOADER device found" appears at the bottom of the RKDevTool frame before releasing the paper clip. Switch RKDevTool to "Advanced Functions" and click the "Enter Maskrom" button, and it will prompt "MASKROM device found". Right-click to add an item.

- Address `0xCCCCCCCC`, Name `Boot`, Path [select](../u-boot/rockchip/l1pro) `rk3328_loader.bin`.
- Address `0x00000000`, Name `system`, select the `Armbian.img` system to be flashed.

Click execute to write.

### 8.3 Allwinner Series Installation Method

Login to the Armbian system (default user: root, default password: 1234) → Enter the command:

```shell
armbian-install
```

## 9. Compile Armbian Kernel

Supports compiling the kernel in Ubuntu20.04/22.04, debian11 or Armbian system. Both local compilation and GitHub Actions cloud compilation are supported. For specific methods, please refer to [Kernel Compilation Instructions](../../compile-kernel/README.md).

### 9.1 How to Add Custom Kernel Patches

When there is a directory of common kernel patches (`common-kernel-patches`) in the kernel patch directory [tools/patch](../../compile-kernel/tools/patch), or there is a directory with the same name as the `kernel source code repository` (e.g., [linux-5.15.y](https://github.com/unifreq/linux-5.15.y)), you can use `-p true` to automatically apply the kernel patch. The naming convention for patch directories is as follows:

```shell
~/amlogic-s9xxx-armbian
    └── compile-kernel
        └── tools
            └── patch
                ├── common-kernel-patches  # Fixed directory name: Storing common kernel patches
                ├── linux-5.15.y           # Same as kernel source repository: storing dedicated kernel patches
                ├── linux-6.1.y
                ├── linux-5.10.y-rk35xx
                └── more kernel directory...
```

- When compiling the kernel locally, you can manually create the corresponding directory and add the corresponding custom kernel patches.
- When compiling in GitHub Actions cloud, you can use the `kernel_patch` parameter to specify the directory of the kernel patch in your repository. For example, the usage in the [compile-beta-kernel.yml](https://github.com/ophub/kernel/blob/main/.github/workflows/compile-beta-kernel.yml) file in the [kernel](https://github.com/ophub/kernel) repository.

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

When using the `kernel_patch` parameter to specify a custom kernel patch, please name the patch directory according to the above specifications.

### 9.2 How to create kernel patches

- Obtain from repositories such as [Armbian](https://github.com/armbian/build) and [OpenWrt](https://github.com/openwrt/openwrt), for example [armbian/patch/kernel](https://github.com/armbian/build/tree/main/patch/kernel/archive) and [openwrt/rockchip/patches-6.1](https://github.com/openwrt/openwrt/tree/main/target/linux/rockchip/patches-6.1), [lede/rockchip/patches-5.15](https://github.com/coolsnowwolf/lede/tree/master/target/linux/rockchip/patches-5.15), etc. Generally, patches in these repositories that use the mainline kernel can be used directly.
- Obtain from commits in github.com repositories: add the `.patch` suffix to the corresponding `commit` address to generate the corresponding patch.

Before adding custom kernel patches, it is necessary to compare them with the upstream kernel source repository [unifreq/linux-k.x.y](https://github.com/unifreq) to confirm whether the patches have already been added to avoid conflicts. Tested kernel patches are recommended to be submitted to the series of kernel repositories maintained by unifreq. Every little bit helps and everyone's contribution will make using Armbian and OpenWrt systems on boxes more stable and interesting.

## 10. Update Armbian Kernel

Login to the Armbian system → Enter the command:

```shell
# Run as root user (sudo -i)
# If no parameters are specified, it will be updated to the latest version.
armbian-update
```

| Optional | Default       | Options              | Description                     |
| -------- | ------------- | -------------------- | ------------------------------- |
| -r       | ophub/kernel  | `<owner>/<repo>`     | Set the repository from which to download the kernel from GitHub.com             |
| -u       | Automatic     | `stable`/`flippy`/`dev`/`rk3588` | Set the [tags suffix](https://github.com/ophub/kernel/releases) of the kernel used |
| -k       | Latest version | Kernel version      | Set the [kernel version](https://github.com/ophub/kernel/releases/tag/kernel_stable)   |
| -c       | None          | Custom domain name   | Set the cdn domain name for accelerated access to github.com        |
| -b       | yes           | yes/no               | Automatically backup the currently used kernel when updating the kernel           |
| -m       | no            | yes/no               | Use the mainline u-boot                                                           |
| -s       | None          | None                 | [SOS] Restore eMMC with the system kernel in USB                                    |
| -h       | None          | None                 | View usage help                                                                   |

Example: `armbian-update -k 5.15.50 -u dev`

When specifying the kernel version using the `-k` parameter, you can provide an exact version number, for example: `armbian-update -k 5.15.50`. Alternatively, you can provide a fuzzy specification to indicate the kernel series, for example: `armbian-update -k 5.15`. When using a fuzzy specification, the tool will automatically select the latest version available in the specified series.

When updating the kernel, the currently used kernel will be automatically backed up, and the storage path is in the `/ddbr/backup` directory. The three most recently used versions of the kernel will be kept. If the newly installed kernel is unstable, you can always restore the backup kernel:

```shell
# Enter the backup kernel directory, such as 5.10.125
cd /ddbr/backup/5.10.125
# Run the update kernel command, which will automatically install the kernel in the current directory
armbian-update
```

If there are problems with incomplete updates caused by special reasons, causing the system to fail to start from eMMC, you can start any version of the Armbian system kernel from USB, and run the `armbian-update -s` command to update the system kernel in USB to eMMC, to achieve the purpose of rescue.

If your network access to github.com is not smooth and cannot be updated online, you can manually download the kernel, upload it to any directory of the Armbian system, enter the kernel directory, and run `armbian-update` for local installation. If there is a complete set of kernel files in the current directory, the kernel in the current directory will be used for updates (the 4 kernel files required for updating are `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-xxx.tar.gz`, `modules-xxx.tar.gz`. Other kernel files are not needed, and if they exist at the same time, they will not affect the update. The system can accurately identify the required kernel files). You can freely update the optional kernels supported by the device, such as updating from kernel 5.10.125 to kernel 5.15.50.

If your local network's access to github.com is not smooth, you can add a CDN acceleration service through the `armbian-update -c https://gh...xy.com/` method, please refer to the suitable acceleration CDN domain name for local use. The accelerated domain name can also be fixedly filled in the personalized configuration file `/etc/ophub-release` parameter `GITHUB_CDN='https://gh...xy.com/'`, to avoid inputting it every time.

Custom options set by `-r`/`-u`/`-c`/`-b` parameters can be fixedly filled in the relevant parameters of the personalized configuration file `/etc/ophub-release` to avoid inputting them every time. The corresponding settings are:

```shell
# Customized parameter assignment
-r  :  KERNEL_REPO='ophub/kernel'
-u  :  KERNEL_TAGS='stable'
-c  :  GITHUB_CDN='https://gh...xy.com/'
-b  :  KERNEL_BACKUP='yes'
```

## 11. Install Common Software

Login to the Armbian system → Enter the command:

```shell
armbian-software
```

Use the `armbian-software -u` command to update the software center list locally. According to user feedback on [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), commonly used [software](../armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) are gradually integrated to achieve convenient operations such as one-click installation/update/uninstallation. It includes `docker images`, `desktop software`, `application services`, etc. For more information, please refer to the [instructions](armbian_software.md).

## 12. Common Issues

The following summarizes some common issues that may be encountered in using Armbian.

### 12.1 Correspondence table between dtb and u-boot of each box

The list of supported TV boxes in the `Armbian` system configuration file is located at [/etc/model_database.conf](../armbian-files/common-files/etc/model_database.conf).

### 12.2 LED screen display control instructions

Please refer to the [instructions](led_screen_display_control.md)

### 12.3 How to restore the original Android TV system

Generally use armbian-ddbr backup and restore, or use Amlogic flash tool to restore the original Android TV system.

#### 12.3.1 Backup and Restore with armbian-ddbr

It is recommended that you back up the original Android TV system of the current TV box before installing the Armbian system on a new box, so that it can be used when you need to restore the system. Boot into the Armbian system from `TF/SD/USB`, enter the `armbian-ddbr` command, and then enter `b` according to the prompt to back up the system. The backup file is saved at `/ddbr/BACKUP-arm-64-emmc.img.gz`, please download and save it. When you need to restore the Android TV system, upload the previously backed up file to the same path of the `TF/SD/USB` device, enter the `armbian-ddbr` command, and then enter `r` according to the prompt to restore the system.

#### 12.3.2 Restore with Amlogic flash tool

- In general, if the TV box can be booted from USB by reconnecting the power supply, just reinstall it a few more times until it works.

- If the screen is black and unable to boot from USB after connecting to a monitor, you need to perform a short-circuit initialization of the box. Restore the box to the original Android system and then flash the Armbian system again. First download and install the [amlogic_usb_burning_tool](https://github.com/ophub/kernel/releases/tag/tools) system recovery tool. Prepare a [USB-A to USB-A data cable](https://user-images.githubusercontent.com/68696949/159267576-74ad69a5-b6fc-489d-b1a6-0f8f8ff28634.png) and a [paperclip](https://user-images.githubusercontent.com/68696949/159267790-38cf4681-6827-4cb6-86b2-19c7f1943342.png).

- Taking the x96max+ as an example, confirm the [short-circuit point](https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg) location on the motherboard of the TV box, and download the [Android TV system package](https://github.com/ophub/kernel/releases/tag/tools) for the box. The schematic diagram of the Android TV system and the corresponding short-circuit points for other common devices can also be downloaded and viewed here.
```shell
Operation steps:

1. Open the flash tool USB Burning Tool:
   [File → Import Image]: X96Max_Plus2_20191213-1457_ATV9_davietPDA_v1.5.img
   [Select]: Erase flash
   [Select]: Erase bootloader
   Click the [Start] button
2. Use a [paperclip] to short-circuit the [two short-circuit points] on the TV box motherboard, and at the same time use a [USB-A to USB-A cable] to connect the [box] to the [computer].
3. When you see the [progress bar start moving], remove the paper clip, and no longer short-circuit.
4. When you see the [progress bar reach 100%], the flashing is complete, and the box has been restored to the Android TV system.
   Click the [Stop] button, unplug the [USB-A to USB-A cable] between the [box] and the [computer].
5. If any of the above steps fail, try again until it succeeds.
   If the progress bar does not move, try plugging in the power supply. In general, power supply support is not required, only the USB-A to USB-A cable can meet the flashing requirements.
```

After restoring the factory settings and restoring the box to the Android TV system, the other operations for installing the Armbian system are the same as the requirements when you first installed the system. Just do it again.

### 12.4 Set the box to boot from USB/TF/SD

- Insert the USB/TF/SD with the system into the box.
- Enable developer mode: Settings → About → Version (such as: X96max plus...), quickly click the left mouse button on the version number five times, see the system display the prompt `developer mode enabled`.
- Enable USB debugging: System → Advanced options → Developer options (set `Enable USB debugging` to enable). Enable `ADB` debugging.
- Install ADB tool: Download [adb](https://github.com/ophub/kernel/releases/tag/tools) and extract it. Copy the three files `adb.exe`, `AdbWinApi.dll`, and `AdbWinUsbApi.dll` to the `system32` and `syswow64` folders under the `c://windows/` directory. Then open the `cmd` command panel, use the `adb --version` command, if there is a display, it means it can be used.
- Enter `cmd` command mode. Enter the command `adb connect 192.168.1.137` (the IP address is modified according to your box, you can check it in the router device connected to the box), if the connection is successful, it will display `connected to 192.168.1.137:5555`
- Enter the command `adb shell reboot update`, and the box will restart and boot from the USB/TF/SD you inserted. Access the system's IP address from a browser or SSH to enter the system.

### 12.5 Disable the IR receiver

By default, support for the IR receiver is enabled, but if you use the TV box as a server, you may want to disable the IR kernel module to prevent your box from being shut down mistakenly. To completely disable IR, add the following line:

```shell
blacklist meson_ir
```

to `/etc/modprobe.d/blacklist.conf` and restart.

### 12.6 Bootloader Selection

- Currently, only a few devices such as `T95(s905x)` / `T95Z-Plus(s912)` / `BesTV-R3300L(s905l-b)` need to use the `/bootfs/extlinux/extlinux.conf` file, which has been added by default in the system. If other devices need it, after writing the system to the USB drive, double-click to open the `boot` partition and remove the `.bak` from the `/boot/extlinux/extlinux.conf.bak` file name provided by the system to use it. When writing to eMMC, `armbian-install` will automatically check if the `extlinux.conf` file exists and create one if it does.

- Other devices only need `/boot/uEnv.txt` to start, do not modify the `extlinux.conf.bak` file.

### 12.7 Network Settings

#### 12.7.1 Setting up network with interfaces

The default content of the network configuration file `/etc/network/interfaces` is as follows:

```shell
source /etc/network/interfaces.d/*
# Network is managed by Network manager
auto lo
iface lo inet loopback
```

##### 12.7.1.1 Use DHCP to dynamically allocate IP addresses

```shell
source /etc/network/interfaces.d/*

auto eth0
iface eth0 inet dhcp
```

##### 12.7.1.2 Manually set static IP address

Modify the IP, gateway, and DNS according to your own network situation.

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

##### 12.7.1.3 Establishing a network with OpenWrt in Docker

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

#### 12.7.2 Managing Network with NetworkManager

##### 12.7.2.1 Creating a New Network Connection

Preparations before creating or modifying a network connection.

###### 12.7.2.1.1 Get the Network Interface Name

Check which network interfaces in the device can be used to establish a network connection.

```shell
nmcli device | grep -E "^[e].*|^[w].*|^[D].*|^[T].*" | awk '{printf "%-19s%-19s\n",$1,$2}'
```

After executing the command, the returned content shows the network interface name in the `DEVICE` column and the network interface type in the `TYPE` column.

Where `eth0` = the name of the first wired network card, `eth1` = the name of the second wired network card, and so on, wireless network card is similar.

```shell
DEVICE             TYPE
eth0               ethernet
eth1               ethernet
eth2               ethernet
eth3               ethernet
wlan0              wifi
wlan1              wifi
```

###### 12.7.2.1.2 Get the Existing Network Connection Names

Check which network connections are currently on the device, including both active and inactive connections.

*) When creating a new network connection, it is not recommended to use an existing connection name.

```shell
nmcli connection show | grep -E ".*|^[N].*" | awk '{printf "%-19s%-19s\n", $1,$3}'
```

After executing the command, the returned content shows the existing network connection names in the `NAME` column and the network interface type in the `TYPE` column.

Where `ethernet` = wired network card, `wifi` = wireless network card, `bridge` = bridge.

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

###### 12.7.2.1.3 Creating a Wired Network Connection

Create a new network connection on the network interface `eth0` and activate it immediately (`Dynamic IP Address` - `IPv4 / IPv6`).

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

Create a new network connection on the network interface `eth0` and activate it immediately (`Static IP Address` - `IPv4`).

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

Create a new network connection on the network interface `wlan0` and activate it immediately (`Dynamic IP Address` - `IPv4 / IPv6`).

```shell
# Set ENV
MYCON=ssid                    # New network connection name, it is recommended to use the WiFi SSID to specify the connection name
MYSSID=ssid                   # WiFi SSID is case-sensitive
MYPSWD=passwd                 # WiFi password
MYWSKM=wpa-psk                # Security selection WPA-WPA2 = wpa-psk or WPA3 = sae (check which encryption method in wireless router or AP)
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

##### 12.7.2.2 Modifying WiFi SSID or PASSWD in a Wireless Network Connection

Modify the `WiFi SSID or PASSWD` in the wireless network connection `ssid` and activate it immediately.

```shell
# Set ENV
MYCON=ssid                    # Wireless network connection name
MYSSID=ssid                   # WiFi SSID is case-sensitive
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

##### 12.7.2.3 Modifying Network Address Allocation Method

###### 12.7.2.3.1 Static IP Address - IPv4

Modify the IP address allocation method to `Static IP Address` and activate it immediately on the network connection `ether1`.

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

Modify the IP address allocation method to `DHCP Obtains Dynamic IP Address` and activate it immediately on the network connection `ether1`.

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

##### 12.7.2.4 Modifying Network Connection MAC Address

Modify (clone) the `MAC address` on the network connection `ether1` and activate it immediately to resolve LAN MAC address conflicts.

*Applicable to wired connections / wireless connections

```shell
# Set ENV
MYCON=ether1                  # Network connection name, note to match the network interface type
MYTYPE=ethernet               # Network interface type = Wired NIC / Wireless NIC = ethernet / wifi
MYMAC=12:34:56:78:9A:BC       # New MAC address

# Chg CON
nmcli connection modify ${MYCON} \
${MYTYPE}.cloned-mac-address ${MYMAC}
nmcli connection up ${MYETH}
ip -c -br address
```

* Creating or modifying some network parameters may cause the network connection to be disconnected and reconnected.
* Due to different software and hardware environments (boxes, systems, network devices, etc.), it may take `1-15` seconds for the changes to take effect. If it takes longer, please check the software and hardware environment.

##### 12.7.2.5 How to Disable IPv6

You can disable the IPv6 protocol using the nmcli utility on the command line. Refer to the source [disable-ipv6](https://access.redhat.com/documentation/zh-cn/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/using-networkmanager-to-disable-ipv6-for-a-specific-connection_configuring-and-managing-networking).

First, use the command `nmcli connection show` to view the list of network connections. The result will be as follows:

```shell
NAME                 UUID                                   TYPE       DEVICE
Wired connection 1   8a7e0151-9c66-4e6f-89ee-65bb2d64d366   ethernet   eth0
...
```

Set the ipv6.method parameter of the connection to disabled:

```shell
nmcli connection modify "Wired connection 1" ipv6.method "disabled"
```

Reconnect to the network:

```shell
nmcli connection up "Wired connection 1"
```

Check the network connection status. If there is no inet6 entry displayed, it means IPv6 is disabled on that device:

```shell
ip address show eth0
```

Verify that the file `/proc/sys/net/ipv6/conf/eth0/disable_ipv6` now contains the value `1`:

```shell
# cat /proc/sys/net/ipv6/conf/eth0/disable_ipv6
1
```

#### 12.7.3 How to Enable Wireless

Some devices support wireless usage, the method to enable it is as follows:

```shell
# Install management tool
sudo apt-get install network-manager

# View network devices
sudo nmcli dev

# Enable wireless
sudo nmcli r wifi on

# Scan for wireless networks
sudo nmcli dev wifi

# Connect to a wireless network
sudo nmcli dev wifi connect "wifi_name" password "WiFi_Password"

# Show the list of saved network connections
sudo nmcli connection show

# Disconnect from a connection
sudo nmcli connection down "wifi_name"

# Forget the connection and disable automatic connection
sudo nmcli connection delete "wifi_name"
```

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/230541872-565a655e-2781-4170-8898-0ae096725506.png">
</div>

#### 12.7.4 How to Enable Bluetooth

Some devices support Bluetooth usage, the method to enable it is as follows:

```shell
# Install Bluetooth support
armbian-config >> Network >> BT: Install Bluetooth support

# Reboot system
reboot
```

After the system restarts, check if the Bluetooth driver is working properly. For desktop systems, you can connect to Bluetooth devices in the menu. You can also use the terminal GUI to install it.

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

You can also use commands in the terminal to install it:

```shell
# Check the status of the Bluetooth service
sudo systemctl status bluetooth

# If not started, start the Bluetooth service first
sudo systemctl enable bluetooth
sudo systemctl start bluetooth

# Scan for nearby Bluetooth devices
bluetoothctl scan on

# Enable Bluetooth discovery
bluetoothctl discoverable on

# Pairing with Bluetooth MAC address
bluetoothctl pair 12:34:56:78:90:AB

# View paired Bluetooth devices
bluetoothctl paired-devices

# Connect to a Bluetooth device
bluetoothctl connect 12:34:56:78:90:AB

# Trust the device for easy direct connection in the future
bluetoothctl trust 12:34:56:78:90:AB

# Disconnect Bluetooth device
bluetoothctl disconnect 12:34:56:78:90:AB

# Remove Bluetooth pairing
bluetoothctl remove 12:34:56:78:90:AB

# Block connecting to the device
bluetoothctl block 12:34:56:78:90:AB
```

### 12.8 How to Add Startup Tasks

The system has already added a custom startup task script file, and the path in the Armbian system is the [/etc/custom_service/start_service.sh](../armbian-files/common-files/etc/custom_service/start_service.sh) file. You can customize and add relevant tasks in this script according to your needs.

### 12.9 How to Update Service Scripts in the System

Use the `armbian-sync` command to update all service scripts in the local system to the latest version with one click.

### 12.10 How to Get Partition Information of Android System on eMMC

When writing the Armbian system to the eMMC system, we need to first confirm the partition table of the Android system of the device to ensure that the data is written to the secure area and try not to damage the partition table of the Android system to avoid problems such as the system cannot start. If you write to an unsafe area, it will not start or encounter errors like the following:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/187075834-4ac40263-52ae-4538-a4b1-d6f0d5b9c856.png">
</div>

#### 12.10.1 Get Partition Information

If you are using Armbian released in this repository after 2022.11, you can copy and paste the following command to obtain a URL that records complete partition information (the device itself does not need to be connected to the Internet).

```shell
ampart /dev/mmcblk2 --mode webreport 2>/dev/null
```

*The webreport mode of ampart was introduced in version 1.2 released on 2023.02.03. If you get no output when using the command above, it may be an old version that does not support directly outputting URLs. You can try the following command instead:*

```shell
echo "https://7ji.github.io/ampart-web-reporter/?dsnapshot=$(ampart /dev/mmcblk2 --mode dsnapshot 2>/dev/null | head -n 1)&esnapshot=$(ampart /dev/mmcblk2 --mode esnapshot 2>/dev/null | head -n 1)"
```

The obtained URL will be similar to the following format:

```shell
https://7ji.github.io/ampart-web-reporter/?esnapshot=bootloader:0:4194304:0%20reserved:37748736:67108864:0%20cache:113246208:754974720:2%20env:876609536:8388608:0%20logo:893386752:33554432:1%20recovery:935329792:33554432:1%20rsv:977272832:8388608:1%20tee:994050048:8388608:1%20crypt:1010827264:33554432:1%20misc:1052770304:33554432:1%20instaboot:1094713344:536870912:1%20boot:1639972864:33554432:1%20system:1681915904:1073741824:1%20params:2764046336:67108864:2%20bootfiles:2839543808:754974720:2%20data:3602907136:4131389440:4&dsnapshot=logo::33554432:1%20recovery::33554432:1%20rsv::8388608:1%20tee::8388608:1%20crypt::33554432:1%20misc::33554432:1%20instaboot::536870912:1%20boot::33554432:1%20system::1073741824:1%20cache::536870912:2%20params::67108864:2%20data::-1:4
```

Copy and paste this URL into your browser and you will be able to see the clear and concise DTB partition information and eMMC partition information:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287642-e1b7be27-4d2c-4ac3-9fcc-15e06aebb97e.png">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287654-d1929e21-d2b3-4fb6-bcf0-c454c88e21b9.png">
</div>

#### 12.10.2 Sharing Partition Information

When you need to share partition information with others (such as reporting the situation of a new device since it was added to this repository, or seeking help from others), try to share the URL itself instead of screenshots. If the URL is too long, you can use some free URL shortening tools.

On the one hand, the partition information on the webpage will be dynamically generated every time it is accessed. The annotations for some partitions that can be written and the format of the table may also be updated.

On the other hand, others cannot easily copy partition parameters for calculation from screenshots.

In addition, there is no need to organize the parameters into a table file separately. The layout of the table on the web page has been specially designed so that you can import it into Excel or LibreOffice Calc by simply copying and pasting it.

#### 12.10.3 Interpreting Partition Information

The DTB table is the partition layout that each box **system** in the Android DTB wants to have. This layout generally ends with a `data` partition of automatically filled size, so the layout here is always the same for boxes of the same system (including the same model). The actual partition layout on the box will vary due to the different capacity of eMMC, but it is always determined by the partition layout of DTB (that is, given the known DTB partition layout and accurate eMMC size, the eMMC partition information can be inferred. *Did you notice that the above DTB partition information and eMMC partition information are not from the same box?*)

The eMMC table is the actual eMMC partition layout on the box. Each row represents a storage area, which can be either a partition or a gap between partitions (due to Geniatech's weird decision, there is at least 8M of gap between each partition, intended for other uses, but never used until the latest S905X4, which is a waste of space). In the row corresponding to the partition, the font is black and both the offset and mask columns have values; in the row corresponding to the gap, the font is gray, the offset and mask columns have no values, and the partition name is `gap`.

In the eMMC table, the last column of each storage area indicates whether it can be written to. Green and `yes` indicate that this area can be written to, red and `no` indicate that this area cannot be written to, and yellow with annotations indicate that this area can be written to under certain conditions or only part of it can be written to.

Taking the above table as an example, the `bootloader` partition corresponds to `0+4M` (`0M~4M`) and cannot be written to, the 32M gap after it (`4M~36M`) can be written to, the `reserved` partition corresponds to `36M+64M` (`36M~100M`) and cannot be written to, and the gap after it all the way to the gap before `env` (`100M~836M`) can be written to. The 1M after `env` (`837M to end`) can be written to without needing an Android boot logo, so the writable ranges on the eMMC are:

- 4M~36M
- 100M~836M
- 837M to end

If an Android boot logo is needed, in addition to the above, the 852M + 32M corresponding to the `logo` partition (`852M~884M`) cannot be written to, so the writable ranges on the eMMC are:

- 4M~36M
- 100M~836M
- 837M~852M
- 884M to end

#### 12.10.4 For eMMC Installation

If your device fails to use `armbian-install` with the `-a` parameter (use [ampart](https://github.com/7Ji/ampart) to adjust eMMC partition layout) set to `yes` (the default value), then your box cannot use the optimized layout (that is, adjust the DTB partition information to only `data`, generate eMMC partition information from it, and move all remaining partitions forward so that the space after 117M can be used). You need to modify the corresponding partition information in [armbian-install](../armbian-files/common-files/usr/sbin/armbian-install).

In this file, there are three key parameters that declare the partition layout: `BLANK1`, `BOOT`, and `BLANK2`. `BLANK1` indicates the size from the beginning of eMMC that cannot be used; `BOOT` indicates the size of the partition created after `BLANK1` to store the kernel, DTB, and other files. Its size should not be less than 256M, and `BLANK2` represents the size after `BOOT` that cannot be used. All space after this will be used to create the `ROOT` partition to store data outside the `/boot` mount point of the whole system. All three should be integers and have a unit of MiB (1 MiB = 1024 KiB = 1024^2 Byte).

Going back to the case where the `logo` partition is not needed, we naturally want to use all available space, but the area of `4M~36M` is too small to be used as `BOOT`, so it can only be counted in `BLANK1`. The `100M~836M` area is more than enough to be used as `BOOT`, so all 736M of it can be assigned to `BOOT`. After that, the unallocated area of `836M~837M` can be counted in `BLANK2`. Then the parameters you should use are as follows (the following example will only use `s905x3`. If your SoC is different, you need to modify other corresponding code blocks):

```shell
# Set partition size (Unit: MiB)
elif [[ "${AMLOGIC_SOC}" == "s905x3" ]]; then
    BLANK1="100"
    BOOT="736"
    BLANK2="1"
```

### 12.11 How to make u-boot files

The u-boot file is an important file that boots the system normally.

#### 12.11.1 Extracting bootloader and dtb files

To extract the required files, you need to use the HxD software. You can download it from the [official website](https://mh-nexus.de/en/downloads.php?product=HxD20) or the [backup download link](https://github.com/ophub/kernel/releases/download/tools/HxDSetup.2.5.0.0.zip).

Execute the following commands in the `cmd` panel to extract the relevant files and download them to your local computer.

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

# Download the bootloader, dtb, and gpio files to the mybox folder on the root directory of the C drive of your local computer one by one.
adb pull /data/local/bootloader.bin C:\mybox
adb pull /data/local/mybox.dtb C:\mybox
adb pull /data/local/mybox_gpio.txt C:\mybox
```

#### 12.11.2 Creating the acs.bin file

The most important part of the mainstream u-boot is acs.bin, which is used to initialize memory. The original factory u-boot is located at the first 4MB of the system. Extract the `acs.bin` file using the `bootloader.bin` file obtained earlier.

Open the HxD software, open the exported `bootloader.bin` file, right-click and select `Select Block`, starting position `F200`, length `1000`, select `hexadecimal`.

<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/187056711-1b58ce71-2a7d-4e9b-a976-e5f278edaa53.png">

Copy the selected result, then create a new file, insert paste, ignore the warning, and save as acs.bin file.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056725-0a0e60af-6a21-4a6b-a2d5-f3d46b438a6a.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056827-8419c738-3428-473e-9a95-ab7270170d98.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056852-9f62f16a-f7f1-4c34-a2c2-78358d198f9a.png">
</div>

If the bootloader is locked, this area of code will be garbled and unusable. The normal code should have many `0`s, and there will be several consecutive appearances of `cfg`, with words related to `ddr` in the middle. This type of normal code can be used.

#### 12.11.3 Creating u-boot files

Creating a u-boot file requires two source code libraries: https://github.com/unifreq/amlogic-boot-fip and https://github.com/unifreq/u-boot. Compile two u-boot files for your own box.

In the amlogic-boot-fip source code, only the acs.bin file is different for each model, and all other files are universal.

<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187057209-c4716384-46ef-4922-9710-8da7ae6db1e4.png">

For detailed instructions on how to create u-boot, please refer to the specific instructions in https://github.com/unifreq/u-boot/tree/master/doc/board/amlogic and compile and test for your own device model.

Creating u-boot with the method provided by [unifreq](https://github.com/unifreq) requires the use of the box's acs.bin, dts, and config files. The dts exported by the Android system cannot be directly converted into Armbian format, so you need to write a corresponding dts file yourself. According to the specific hardware differences of your device, such as switches, LEDs, power control, TF cards, SDIO Wi-Fi modules, etc., modify and create similar [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) files in the kernel source code library.

Take creating u-boot for X96Max Plus as an example:

```shell
~/make-uboot
    ├── amlogic-boot-fip
    │   ├── x96max-plus                                     # Create your own directory
    │   │   ├── asc.bin                                     # Create your own source file
    │   │   └── other-copy-files...                         # Copy files from other directories
    │   │
    │   ├── other-source-directories...
    │   └── other-source-files...
    │
    └── u-boot
        ├── configs
        │   └── x96max-plus_defconfig                       # Create your own source file
        ├── arch
        │   └── arm
        │       └── dts
        │           ├── meson-sm1-x96-max-plus-u-boot.dtsi  # Create your own source file
        │           ├── meson-sm1-x96-max-plus.dts          # Create your own source file
        │           └── Makefile                            # Edit
        ├── fip
        │   ├── u-boot.bin                                  # Generated file
        │   └── u-boot.bin.sd.bin                           # Generated file
        ├── u-boot.bin                                      # Generated file
        │
        ├── other-source-directories...
        └── other-source-files...
```

- Download the [amlogic-boot-fip](https://github.com/unifreq/amlogic-boot-fip) source code. Create an [x96max-plus](https://github.com/unifreq/amlogic-boot-fip/tree/master/x96max-plus) directory in the root directory, and except for the `asc.bin` file produced by yourself, files in it can be copied from other directories.
- Download the [u-boot](https://github.com/unifreq/u-boot) source code. Create the corresponding [x96max-plus_defconfig](https://github.com/unifreq/u-boot/blob/master/configs/x96max-plus_defconfig) file and put it in the [configs](https://github.com/unifreq/u-boot/tree/master/configs) directory. Create the corresponding [meson-sm1-x96-max-plus-u-boot.dtsi](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus-u-boot.dtsi) and [meson-sm1-x96-max-plus.dts](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus.dts) files and put them in the [arch/arm/dts](https://github.com/unifreq/u-boot/tree/master/arch/arm/dts) directory. Edit the [Makefile](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/Makefile) file in this directory and add an index for the `meson-sm1-x96-max-plus.dtb` file.
- In the root directory of the u-boot source code, follow the steps in the document https://github.com/unifreq/u-boot/blob/master/doc/board/amlogic/x96max-plus.rst.

There are two types of final generated files: the `u-boot.bin` file in the u-boot root directory is the incomplete version of u-boot used in the `/boot` directory (corresponding to the [overload](../amlogic-u-boot/overload) directory in the repository); the `u-boot.bin` and `u-boot.bin.sd.bin` in the `fip` directory are the complete versions of u-boot files used in the `/usr/lib/u-boot/` directory (corresponding to the [bootloader](../amlogic-u-boot/bootloader/) directory in the repository). The two files differ by 512 bytes, and the larger one is filled with 512 bytes of 0 at the front.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189039426-c127631f-77ca-4fcb-9fb6-4220045d712b.png">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189029320-e43a4cc9-b4b5-4de4-92fe-b17bd29020d0.png">
</div>


💡Note: Before writing to eMMC for testing, please check the bricking method in section 12.3. Be sure to master the position of the shorting point, have the Android system file in the original .img format and have conducted a shorting test to ensure that the bricking method has been mastered before proceeding with the write test.

### 12.12 Memory size recognition error

If the memory size is not recognized correctly (4GB memory recognized as 1-2GB is abnormal, recognized as 3.7GB is normal), you can try to manually copy a `/boot/UBOOT_OVERLOAD` file (note that it is `copy` a copy, do not rename it, after installation and update operations, renaming will make it unable to start). When used on `USB`, save it as `/boot/u-boot.ext`, and when used on `eMMC`, save it as `/boot/u-boot.emmc`.

Do not manually copy u-boot files other than trying to solve memory problems. Adding them incorrectly can cause startup failures and various problems.

### 12.13 How to decompile dtb files

For some new devices that are not on the current supported list (or have variations), you can try to decompile and adjust relevant parameters.

```shell
# Install dependency
sudo apt-get update
sudo apt-get install -y device-tree-compiler

# 1. Decompile command (generate dts source code from dtb file)
dtc -I dtb -O dts -o xxx.dts xxx.dtb

# 2. Compile command (generate dtb file from dts)
dtc -I dts -O dtb -o xxx.dtb xxx.dts
```

### 12.14 How to modify cmdline settings

In Amlogic devices, settings such as addition/modification/deletion can be made in the `/boot/uEnv.txt` file. In Rockchip and Allwinner devices, settings are made in the `/boot/armbianEnv.txt` file (added to the `extraargs` or `extraboardargs` parameter). For devices using `/boot/extlinux/extlinux.conf`, configurations are made in this file. Restart the device after each change for it to take effect.

- For example, the `Home Assistant Supervisor` application only supports the `docker cgroup v1` version, while the docker installed by default is the latest version of v2. If you need to switch to v1 version, you can add the `systemd.unified_cgroup_hierarchy=0` parameter setting in the cmdline, and after restarting, you can switch to the `docker cgroup v1` version.

- By adding the `max_loop=128` setting in cmdline, you can adjust the number of loop mounts allowed.

- By adding `usbcore.usbfs_memory_mb=1024` to cmdline, the USBFS memory buffer can be permanently changed from the default `16 mb` to a larger size (`cat /sys/module/usbcore/parameters/usbfs_memory_mb`), improving the ability to transfer large files via USB.

- By adding `usbcore.usb3_disable=1` to the cmdline, you can disable all USB 3.0 devices.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://user-images.githubusercontent.com/68696949/216220941-47db0183-7b26-4768-81cf-2ee73d59d23e.png">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a600dcad-d817-47eb-b529-4014019915b3">
</div>

### 12.15 How to add support for new devices

To build an Armbian system for a device, you need 4 parts: `device configuration files`, `system files`, `u-boot files`, and `process control files`. The specific steps for adding them are as follows:

#### 12.15.1 Add device configuration files

In the configuration file [/etc/model_database.conf](../armbian-files/common-files/etc/model_database.conf), according to the test support status of the device, add the corresponding configuration information. Among them, the value of `BUILD` that is `yes` is the part of the default built devices, and the corresponding `BOARD` value `must be unique`. These boxes can use the default built Armbian system directly.

The default value of `no` is not packaged. These devices need to download the Armbian system of the same `FAMILY`. After writing to `USB`, you can open the `/boot` partition in the `USB` on your computer, and modify the `FDT dtb name` in the `/boot/uEnv.txt` file to adapt to other devices in the compatibility list.

#### 12.15.2 Add system files

Common files are located in the `build-armbian/armbian-files/common-files` directory, which is common to all platforms.

Platform-specific files are located in the `build-armbian/armbian-files/platform-files/<platform>` directory. [Amlogic](../armbian-files/platform-files/amlogic), [Rockchip](../armbian-files/platform-files/rockchip), and [Allwinner](../armbian-files/platform-files/allwinner) share their respective platform files. The `bootfs` directory contains files on the /boot partition, and the `rootfs` directory contains Armbian system files.

If individual devices have special differentiation requirements, add an independent directory named after `BOARD` in the `build-armbian/armbian-files/different-files` directory, establish the `bootfs` directory as needed to add files related to the system `/boot` partition, and establish the `rootfs` directory as needed to add system files. The naming of each folder should follow the actual path of the `Armbian` system. It is used to add new files or overwrite same-named files added from common files and platform files.

#### 12.15.3 Adding u-boot files

For `Amlogic` series devices, share [bootloader](../u-boot/amlogic/bootloader/) files and [u-boot](../u-boot/amlogic/overload) files. If there are new files, put them in the corresponding directory. The `bootloader` file will be automatically added to the `/usr/lib/u-boot` directory of the Armbian system during system construction, and the `u-boot` file will be automatically added to the `/boot` directory.

For `Rockchip` and `Allwinner` series devices, add an independent [u-boot](../u-boot) directory named after `BOARD` for each device, and place the corresponding series files in this directory.

When building Armbian images, these u-boot files will be written into the corresponding Armbian image files according to the configuration in [/etc/model_database.conf](../armbian-files/common-files/etc/model_database.conf) by the rebuild script.

#### 12.15.4 Adding workflow control files

Add the corresponding `BOARD` option in `armbian_board` in the [yml workflow control file](../../.github/workflows/build-armbian.yml), which supports use in `Actions` on github.com.

### 12.16 How to solve I/O errors when writing to eMMC

Some devices can start Armbian normally from USB/SD/TF, but when writing to eMMC, an I/O write error will be reported, such as the case in [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues/989). The error messages are as follows:

```shell
[  284.338449] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.341544] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.446972] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.450074] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.497746] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.500871] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
```

In this case, you can adjust the working mode speed and frequency of the dtb used to stabilize the read and write support for storage. When using the sdr mode, the frequency is twice the speed, and when using the ddr mode, the frequency is equal to the speed. As follows:

```shell
sd-uhs-sdr12
sd-uhs-sdr25
sd-uhs-sdr50
sd-uhs-ddr50
sd-uhs-sdr104

max-frequency = <208000000>;
```

Taking the code snippet in the [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) file of the kernel source code as an example:

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

In general, lowering the frequency of `&sd_emmc_c` from `max-frequency = <200000000>;` to `max-frequency = <100000000>;` can solve the problem. If it doesn't work, you can continue to lower it to `50000000` for testing and adjust `&sd_emmc_b` to set up `USB/SD/TF`. You can also use `sd-uhs-sdr` for speed limiting. You can modify the dts file and [compile](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel) to obtain test files, or use the method described in section 12.13 to decompile and modify the existing dtb file to generate test files. When modifying the decompiled dtb file, use hexadecimal values. The decimal `200000000` corresponds to hexadecimal `0xbebc200`, the decimal `100000000` corresponds to hexadecimal `0x5f5e100`, the decimal `50000000` corresponds to hexadecimal `0x2faf080`, and the decimal `25000000` corresponds to hexadecimal `0x17d7840`.

In addition to solving the problem through system software, you can also make use of [financial power](https://github.com/ophub/amlogic-s9xxx-armbian/issues/998) and [hands-on ability](https://www.right.com.cn/forum/thread-901586-1-1.html) to solve it.

### 12.17 How to solve the problem of no sound in Bullseye version

Error log information for sound problems:

```shell
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: dpcm_fe_dai_prepare() failed (-22)
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: no backend DAIs enabled for fe.dai-link-0
```

Please refer to the method in [Bullseye NO Sound](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1000) for setting.

```shell
wget https://github.com/ophub/kernel/releases/download/tools/bullseye_g12_sound-khadas-utils-4-2-any.tar.gz
tar -xzf bullseye_g12_sound-khadas-utils-4-2-any.tar.gz -C /

systemctl enable sound.service
systemctl restart sound.service
```

After restarting Armbian for testing, if the sound still doesn't work, it may be because your box uses an old sound output route corresponding to the conf, and you need to comment out the new configuration (mainly for G12B/S922X, not suitable for Old G12A/S905X2 and SM1/S905X3 based on G12A) corresponding to `L137-L142` in `/usr/bin/g12_sound.sh`, and then uncomment the old configuration corresponding to `L130-L134`.

