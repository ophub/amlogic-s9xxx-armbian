# Armbian Build and Usage Guide

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

GitHub Actions is a CI/CD service from Microsoft that provides high-performance virtual server environments for building, testing, packaging, and deploying projects. Public repositories can use it free of charge with no time limit, and each build can run for up to 6 hours — more than sufficient for compiling Armbian (typically completed within about 3 hours). This project is shared for educational and experience-exchange purposes. Please do not initiate any harmful network attacks or misuse GitHub Actions.

# Table of Contents

- [Armbian Build and Usage Guide](#armbian-build-and-usage-guide)
- [Table of Contents](#table-of-contents)
  - [1. Register your own Github account](#1-register-your-own-github-account)
  - [2. Set up private variable GITHUB\_TOKEN etc](#2-set-up-private-variable-github_token-etc)
  - [3. Fork the repository and set Workflow permissions](#3-fork-the-repository-and-set-workflow-permissions)
  - [4. Customization instructions for personalized Armbian system files](#4-customization-instructions-for-personalized-armbian-system-files)
  - [5. Compile the system](#5-compile-the-system)
    - [5.1 Manual Compilation](#51-manual-compilation)
    - [5.2 Scheduled Compilation](#52-scheduled-compilation)
    - [5.3 Customizing Default System Configuration](#53-customizing-default-system-configuration)
    - [5.4 Expanding Github Actions Compilation Space Using Logical Volumes](#54-expanding-github-actions-compilation-space-using-logical-volumes)
    - [5.5 Build Armbian Docker image](#55-build-armbian-docker-image)
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
    - [8.4 Installation Method for the Docker Version of Armbian](#84-installation-method-for-the-docker-version-of-armbian)
      - [8.4.1 Install Docker Runtime Environment](#841-install-docker-runtime-environment)
      - [8.4.2 Configure macvlan Network](#842-configure-macvlan-network)
      - [8.4.3 Run Armbian Docker Container](#843-run-armbian-docker-container)
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
          - [12.7.2.4.1 Method 1: Use the `nmcli` command to change the MAC address](#127241-method-1-use-the-nmcli-command-to-change-the-mac-address)
          - [12.7.2.4.2 Method 2: Modify the MAC address via a configuration file](#127242-method-2-modify-the-mac-address-via-a-configuration-file)
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
        - [12.15.3.1 Check the Partition Layout](#121531-check-the-partition-layout)
        - [12.15.3.2 Back Up Key Partitions](#121532-back-up-key-partitions)
        - [12.15.3.3 Add a Special Partition Writing File](#121533-add-a-special-partition-writing-file)
      - [12.15.4 Add Process Control Files](#12154-add-process-control-files)
    - [12.16 How to Resolve the Issue of I/O Errors While Writing to eMMC](#1216-how-to-resolve-the-issue-of-io-errors-while-writing-to-emmc)
    - [12.17 How to Solve the Issue of No Sound in the Bullseye Version](#1217-how-to-solve-the-issue-of-no-sound-in-the-bullseye-version)
    - [12.18 How to build the boot.scr file](#1218-how-to-build-the-bootscr-file)
    - [12.19 How to Enable Remote Desktop and Modify the Default Port](#1219-how-to-enable-remote-desktop-and-modify-the-default-port)
    - [12.20 TCP Congestion Control Optimization Guide](#1220-tcp-congestion-control-optimization-guide)

## 1. Register your own Github account

Register an account to proceed with system customization. Click the `Sign up` button in the upper right corner of github.com and follow the prompts to complete registration.

## 2. Set up private variable GITHUB_TOKEN etc

According to the [GitHub Docs](https://docs.github.com/en/actions/security-guides/automatic-token-authentication), GitHub automatically creates a unique `GITHUB_TOKEN` secret at the start of every workflow job for use within the workflow. The `{{ secrets.GITHUB_TOKEN }}` can be used for authentication within the workflow job.

When building an [Armbian Docker](../.github/workflows/build-armbian-arm64-docker-image.yml) image in Actions and pushing it to Docker Hub, you need to set two secrets: `DOCKERHUB_USERNAME` and `DOCKERHUB_PASSWORD`. On your repository's page, click Settings in the top right corner, then navigate to `Settings` > `Secrets and variables` > `Actions` > `Repository secrets` > `New repository secret`, add the following two secrets:

- Secret name `DOCKERHUB_USERNAME`: The value is your username for logging into Docker Hub.
- Secret name `DOCKERHUB_PASSWORD`: The value is your password for logging into Docker Hub.

## 3. Fork the repository and set Workflow permissions

Fork the repository by opening https://github.com/ophub/amlogic-s9xxx-armbian and clicking the Fork button in the upper right to copy the repository to your account. Once the fork is complete, navigate to your copy. Then go to `Settings` > `Actions` > `General` > `Workflow permissions` in the left navigation bar and save. The illustration is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/167585338-841d3b05-8d98-4d73-ba72-475aad4a95a9.png width="300" />
</div>

## 4. Customization instructions for personalized Armbian system files

The system compilation process is controlled in the [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) file. Additional .yml files in the workflows directory handle other functions. The system is compiled using Armbian's current official source code. Refer to the official documentation for parameter details.

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

When building Armbian with `ophub`, the `armbian_files` parameter allows you to add or overwrite custom files in the [common-files](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/build-armbian/armbian-files/common-files) directory. The directory structure must strictly mirror the Armbian root filesystem to ensure files are correctly overlaid into the firmware (e.g., default configuration files should be placed under the `etc/default/` subdirectory). Example configuration:

```yaml
- name: Rebuild Armbian
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img.gz
    armbian_files: files
    ...
```

## 5. Compile the system

The system can be compiled manually, on a schedule, or triggered by specific events. The following sections cover each method.

### 5.1 Manual Compilation

In your repository's navigation bar, click the Actions button, then select Build armbian > Run workflow > Run workflow to start compilation. The process takes approximately 3 hours and is complete once all steps finish. The illustration is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163203938-e7762b09-e6b8-4cf5-b1f1-9c67c1a29953.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204044-9c7a7429-47ee-4fce-b7dd-e217bebf6133.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204127-6b16b558-7e78-4c22-a28a-7b37b5a34fa3.png width="300" />
</div>

### 5.2 Scheduled Compilation

In the [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) file, use Cron to configure scheduled compilation. The five fields represent minutes (0–59) / hours (0–23) / day of month (1–31) / month (1–12) / day of week (0–6, Sunday–Saturday). Modify values as needed. The system uses UTC by default; convert to your local time zone accordingly.

```yaml
schedule:
  - cron: '0 17 * * *'
```

### 5.3 Customizing Default System Configuration

The default system configuration information is recorded in the [model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) file, in which the `BOARD` name must be unique.

Devices with `BUILD` set to `yes` are included in the default build and can be used directly. Devices set to `no` are not packaged by default. To use an unpackaged device, download the Armbian system with the same `FAMILY`, write it to `USB`, then open the USB boot partition on your computer and modify the `FDT dtb name` in `/boot/uEnv.txt` to match your device.

For local compilation, use the `-b` parameter; for GitHub Actions, use the `armbian_board` parameter. `-b all` packages all devices with `BUILD` set to `yes`. Specifying a `BOARD` explicitly will package it regardless of its `BUILD` value. For example: `-b r68s_s905x3-tx3_s905l3a-cm311`.

### 5.4 Expanding Github Actions Compilation Space Using Logical Volumes

GitHub Actions provides 84GB of compile space by default, with approximately 50GB available after accounting for the system and required packages. When compiling all firmware, this may be insufficient. Use logical volumes to expand the compile space to approximately 110GB. Refer to the [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) file and use the commands below to create a logical volume. Then use the logical volume path during compilation.

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

### 5.5 Build Armbian Docker image

To create a [Docker](https://hub.docker.com/u/ophub) image of the Armbian system, refer to the [armbian_docker](../compile-kernel/tools/script/docker) build script.

## 6. Saving the System

System storage is also configured in the [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) file. The compiled system is automatically uploaded to GitHub Releases via script.

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

Navigate to the Releases section on the repository homepage and select the system image matching your device model. The illustration is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163204798-0d98524c-73df-4876-8912-fcae2845fbba.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204879-4898babf-fa00-4e63-89ea-235129e2ce1d.png width="300" />
</div>

## 8. Installing Armbian to EMMC

Installation methods vary across Amlogic, Rockchip, and Allwinner platforms. Devices differ in storage options — some use external microSD cards, some have eMMC, and some support NVMe or other media. Each platform's installation method is described separately below. First, download the appropriate Armbian system image for your device from [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) and decompress it to .img format. Depending on your device, follow the corresponding installation method.

After installation, connect the device to your `router` and allow `2 minutes` for it to boot. Then find the device's `IP` address (listed as Armbian) in your router's admin panel and connect via `SSH`. The default username is `root`, the default password is `1234`, and the default port is `22`.

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

Installation methods differ by device and are described individually below.

#### 8.2.1 Installation Method for Radxa-Rock5B

Radxa-Rock5B supports multiple storage media (microSD/eMMC/NVMe), each with a different installation method. Download the [rk3588_spl_loader_v1.08.111.bin and spi_image.img](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/rock5b) files. Download the [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/Radxa_rock5b_RKDevTool_Release_v2.96__DriverAssitant_v5.1.1.tar.gz) tool and driver. Download [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/) for writing disk images.

##### 8.2.1.1 Install the System to MicroSD

Use Rufus, balenaEtcher, or a similar tool to write the Armbian system image to a microSD card, then insert it into the device.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972996-300f223b-f6f6-48af-86ca-bdc842e5017d.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202973216-85b1a21b-0763-4a36-8c57-84490e071fdd.png width="600" />
</div>

##### 8.2.1.2 Install the System to eMMC

- Install via microSD card: Write the Armbian system image to a microSD card and boot the device from it. Upload the `armbian.img` file to the microSD card, then use `dd` to write it to eMMC:

```Shell
dd if=armbian.img  of=/dev/mmcblk1  bs=1M status=progress
```

- Install via USB-to-eMMC card reader: Connect the eMMC module to your computer, use Rufus or balenaEtcher to write the Armbian system image to eMMC, then insert the eMMC module into the device.
- Install via Maskrom mode: Power off the development board. Hold the gold button, then connect a USB-A to Type-C cable between the ROCK 5B Type-C port and your PC. Release the gold button. Verify that a MASKROM device is detected. Right-click in the blank area of the list and load the `rock-5b-emmc.cfg` configuration file (the configuration file and RKDevTool are in the same directory). Set `rk3588_spl_loader_v1.08.111.bin` and `Armbian.img` as shown below and begin writing.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954901-c829d74d-c75a-4fd3-9bd0-aa3cdf2b77b4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954998-c08514e2-8760-4c0f-b5f7-0d30be635aa5.png width="600" />
</div>

##### 8.2.1.3 Installation of the system on NVMe

The ROCK-5B has an SPI flash memory on its motherboard. Installing the bootloader on this SPI flash memory can support other boot media (such as SATA, USB3, or NVMe) that are not directly supported by the SoC maskrom mode. To use NVMe, you must first write the SPI file. The method is as follows:

Power off the development board and remove all bootable devices (MicroSD cards, eMMC modules, etc.). Press and hold the golden button, then connect a USB-A to Type-C cable between the ROCK-5B and your PC. Release the golden button. Verify that a MASKROM device is detected. Right-click in the list to load the configuration file from the resource folder (the configuration file and RKDevTool are in the same directory), select `rk3588_spl_loader_v1.08.111.bin` and `spi_image.img` files according to the figure below, and click write:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961420-8316c96c-2796-43ed-b5ed-2fa5bfa1ddff.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961447-49c0941a-e233-4b2a-b96b-b47636ce3cf2.png width="600" />
</div>

- Install via card reader: Insert the M.2 NVMe SSD into an M.2 NVMe-to-USB 3.0 card reader and connect it to your computer. Use Rufus or balenaEtcher to write the Armbian system image to the NVMe drive, then install it in the device.
- Install via microSD card: Write the Armbian system image to a microSD card and boot the device from it. Upload the `armbian.img` file to the microSD card, then use `dd` to write it to NVMe:

```Shell
dd if=armbian.img  of=/dev/nvme0n1  bs=1M status=progress
```

#### 8.2.2 Installation method for FastRhino R66S

Use Rufus or balenaEtcher to write the Armbian system image to a microSD card, then insert it into the device.

#### 8.2.3 Installation method for FastRhino R68S

- Download the [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) tool and driver. Unzip, install the DriverAssistant driver, and open RKDevTool.
- With R68s powered off, connect the USB male-to-male cable, press and hold the Recovery key, then plug in the 12V power supply. Release the Recovery key after two seconds — the flashing tool should `discover a LOADER device`.
- Right-click in the blank area of the RKDevTool interface to add an item.
- Set address to `0x00000000`, name to `armbian`, and path to the `armbian.img` system file.
- Select the armbian entry, `deselect all other entries`, and click `execute` to write.
- Note: If another system exists on eMMC, erase it first via Advanced Features before writing Armbian. If erasure fails, reflash the `MiniLoaderAll.bin` bootloader first, then enter `MASKROM` mode to write Armbian. MiniLoaderAll.bin settings: address `0xCCCCCCCC`, name `Loader`, path to the `MiniLoaderAll.bin` file in the Image directory of RKDevTool.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202970301-d798677b-e875-4971-ac8f-ee58b2a1e686.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970405-cb68cb78-cd0f-43ee-b807-5e594ab73099.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970488-5f18c574-c11f-486f-8fe8-002f3ba2f3f4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970577-87549acf-b98b-441f-bb31-e4fd6608108d.png width="600" />
</div>

#### 8.2.4 Installation method for Beikeyun

This method is based on [milton](https://www.cnblogs.com/milton/p/15391525.html)'s tutorial. Flashing requires entering Maskrom mode. Disconnect all connections, short the CLK and GND contacts (using the TTL GND or the GND near the small button), then connect via USB to your PC. A MASKROM device should be detected. The short-circuit point location is shown below:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202977817-fb12d291-47e2-47e4-88c3-e21f9ae57922.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202977900-50b4770d-8444-42a0-8478-3234043455bd.png width="600" />
</div>

Open RKDevTool and right-click to add an item.

- Address `0xCCCCCCCC`, name `Boot`, path [select](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/beikeyun) `rk3328_loader_v1.14.249.bin`.
- Address `0x00000000`, name `system`, path select the `Armbian.img` system to be flashed.
- Check the "Force Write by Address" option, click "Execute," and wait for the progress to complete on the right-hand download panel.

#### 8.2.5 Installation method for Chainedbox-L1-Pro

This method is based on [cc747](https://post.smzdm.com/p/a4wkdo7l/)'s tutorial. Power off the Chainedbox-L1-Pro and unplug all cables. Connect a USB male-to-male cable between the USB 2.0 port on the Chainedbox-L1-Pro and your computer. Insert a paperclip into the Reset hole and hold it down, then plug in the power cord. Wait until `discovered a LOADER device` appears at the bottom of RKDevTool, then release the paperclip. Switch to `Advanced Features` and click `Enter Maskrom` until `Found a MASKROM device` appears. Right-click to add an item.

- Address `0xCCCCCCCC`, name `Boot`, path [select](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/chainedbox) `rk3328_loader_v1.14.249.bin`.
- Address `0x00000000`, name `system`, path select the `Armbian.img` system to be flashed.
- Check the "Force Write by Address" option, click "Execute," and wait for the progress to complete on the right-hand download panel.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a6d2d8c0-35c5-44ba-be35-fd2e2758729b width="600" /><br />
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/13aab016-1b93-4ff1-b1ef-c202bd357068 width="600" />
</div>

#### 8.2.6 Installation method for lckfb-tspi
- Download the [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) tool and driver. Unzip, install the DriverAssistant driver, and open RKDevTool. (Note: use version 2.86 instead of 2.92, as version 2.92 crashes during flashing.)
- With the tspi powered off, press and hold the Recovery key, then insert the Type-C data cable. Release the Recovery key after RKDevTool displays `A LOADER device was found`. Right-click to add an item.
- Address `0x00000000`, name `system`, path select the `Armbian.img` system to be flashed.
- Click Execute and wait for the progress to complete.


### 8.3 Allwinner Series Installation Method

Log in to the Armbian system (default user: root, default password: 1234) → Enter the command:

```shell
armbian-install
```

### 8.4 Installation Method for the Docker Version of Armbian

You can use Docker versions of Armbian images on Ubuntu/Debian/Armbian systems. These images are hosted on [Docker Hub](https://hub.docker.com/r/ophub) and can be downloaded directly for use.

Four Armbian Docker images with different base versions are provided: `armbian-trixie`, `armbian-bookworm`, `armbian-noble`, and `armbian-resolute`. Each version has both `arm64` and `amd64` builds, allowing you to choose the appropriate version based on your needs.

Among them, armbian-trixie is based on Debian 13, armbian-bookworm is based on Debian 12, armbian-noble is based on Ubuntu 24.04, and armbian-resolute is based on Ubuntu 26.04.

The arm64 version is suitable for devices with platform architectures such as Amlogic/Rockchip/Allwinner, while the amd64 version is suitable for computers and servers with x86_64 architecture.

#### 8.4.1 Install Docker Runtime Environment

```shell
curl -fsSL https://get.docker.com | sh
sudo usermod -aG docker $USER
sudo newgrp docker
```

#### 8.4.2 Configure macvlan Network

```shell
# Check if existing docker networks include a macvlan network
docker network ls

# If there is no macvlan network, create one
# Modify the subnet, gateway, and interface name according to your actual network
docker network create -d macvlan \
    --subnet=10.1.1.0/24 \
    --gateway=10.1.1.1 \
    -o parent=eth0 \
    macvlan
```

#### 8.4.3 Run Armbian Docker Container

Here, the `armbian-trixie:arm64` image is used as an example to demonstrate how to run an Armbian container.

```shell
# Run the Armbian container in detached mode
# Modify the container name, IP address, image version, etc., according to your actual situation
docker run -itd --name=armbian-trixie \
    --privileged \
    --network macvlan \
    --ip 10.1.1.15 \
    --hostname=armbian-trixie \
    -e TZ=Asia/Shanghai \
    --restart unless-stopped \
    ophub/armbian-trixie:arm64

# View Armbian container logs
docker logs -f armbian-trixie

# Enter the Armbian container
docker exec -it armbian-trixie bash

# Exit the Armbian container
exit

# Stop and remove the Armbian container
docker rm -f armbian-trixie
```

## 9. Compiling Armbian Kernel

Kernel compilation is supported on Ubuntu, Debian, and Armbian systems, both locally and via GitHub Actions. For details, refer to the [Kernel Compilation Instructions](../../compile-kernel).

### 9.1 How to Add Custom Kernel Patches

When the kernel patch directory [tools/patch](../../compile-kernel/tools/patch) contains a common patch directory (`common-kernel-patches`) or a directory matching the kernel source repository name (for example, [linux-5.15.y](https://github.com/unifreq/linux-5.15.y)), use `-p true` to automatically apply kernel patches. The directory naming convention is as follows:

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

- For local kernel compilation, manually create the corresponding directory and add your custom kernel patches.
- For GitHub Actions cloud compilation, use the `kernel_patch` parameter to specify the kernel patch directory in your repository. For example, see the usage in [compile-beta-kernel.yml](https://github.com/ophub/kernel/blob/main/.github/workflows/compile-beta-kernel.yml) in the [kernel](https://github.com/ophub/kernel) repository:

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

When specifying a custom kernel patch via `kernel_patch`, follow the naming convention described above.

### 9.2 How to Make Kernel Patches

- Obtained from repositories such as [Armbian](https://github.com/armbian/build) and [OpenWrt](https://github.com/openwrt/openwrt): for example, [armbian/patch/kernel](https://github.com/armbian/build/tree/main/patch/kernel/archive) and [openwrt/rockchip/patches-6.1](https://github.com/openwrt/openwrt/tree/main/target/linux/rockchip/patches-6.1), [lede/rockchip/patches-5.15](https://github.com/coolsnowwolf/lede/tree/master/target/linux/rockchip/patches-5.15) etc. Patches from these mainline kernel repositories can generally be used directly.
- Obtained from GitHub commit URLs: Appending `.patch` to a commit URL generates the corresponding patch file.

Before adding a custom kernel patch, compare it against the upstream kernel source repository [unifreq/linux-k.x.y](https://github.com/unifreq) to verify it has not already been applied and to avoid conflicts. Tested kernel patches are encouraged to be submitted to the kernel repositories maintained by unifreq. Your contributions help make Armbian and OpenWrt more stable and functional for everyone.


### 9.3 How to Customize Compilation of Driver Modules

Some drivers are not yet included in the mainline Linux kernel but can be compiled as custom modules. Only drivers compatible with the mainline kernel can be compiled; Android-specific drivers are generally unsupported. For example:

```shell
# Step 1: Update to the latest kernel
# Due to incomplete header files in earlier versions, it is necessary to update to the latest kernel version.
# The requirement for each kernel version is not lower than 5.10.222, 5.15.163, 6.1.100, 6.6.41.
armbian-sync
armbian-update -k 6.1


# Step 2: Install compilation tools
mkdir -p /usr/local/toolchain
cd /usr/local/toolchain
# Download the compilation tools
wget https://github.com/ophub/kernel/releases/download/dev/arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz
# Extract
tar -Jxf arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz
# Install additional compilation dependencies (optional; you can manually install missing components based on errors).
armbian-kernel -u


# Step 3: Download and compile the driver
# Download driver source code
cd ~/
git clone https://github.com/jwrdegoede/rtl8189ES_linux
cd rtl8189ES_linux
# Set up the compilation environment
gun_file="arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz"
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
| -u | Automatic | stable/flippy/beta/rk3588/rk35xx/h6 | Set the suffix of the used kernel's [tags](https://github.com/ophub/kernel/releases) |
| -k | Latest Version | Kernel Version | Set the [Kernel Version](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| -b | yes | yes/no | Automatically backup the kernel currently in use when updating the kernel |
| -d | deb | tar/deb | Set the preferred kernel package format. If unavailable, the script will automatically try the alternative. `deb` is recommended for compiling custom drivers. |
| -m | no | yes/no | Use the mainline u-boot |
| -s | None | None/DiskName | [SOS] Restore the system kernel in eMMC/NVMe/sdX and other disks |
| -h | None | None | View the usage help |

Example: `armbian-update -k 5.15 -u stable -d deb`

The `-k` parameter accepts either an exact version number (e.g., `armbian-update -k 5.15.50`) or a version series (e.g., `armbian-update -k 5.15`), which automatically selects the latest version in that series.

During kernel updates, the current kernel is automatically backed up to `/ddbr/backup`, retaining the three most recent versions. If a newly installed kernel is unstable, restore a backed-up kernel at any time:
```shell
# Enter the backup kernel directory, such as 6.6.12
cd /ddbr/backup/6.6.12
# Execute the kernel update command, which will automatically install the kernel in the current directory
armbian-update
```

[SOS]: If an incomplete update or other issue prevents the system from booting from eMMC/NVMe/sdX, boot Armbian from another disk (e.g., USB) with any kernel version, then run `armbian-update -s` to restore the kernel to eMMC/NVMe/sdX. Without a disk parameter, the kernel is restored from the USB device to eMMC/NVMe/sdX by default. For systems with multiple disks, specify the target disk name explicitly. Examples:

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

If network issues prevent downloading from github.com, manually download the kernel files, upload them to any directory on your Armbian system, navigate to that directory, and run `armbian-update` for local installation. The system automatically prioritizes local kernel files when a complete set is detected in the current directory. The kernel is available in `tar` and `deb` formats. Each requires 4 files:

- The 4 required files for `tar` format updates are: `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-xxx.tar.gz`, and `modules-xxx.tar.gz`.
- The 4 required files for `deb` format updates are: `linux-image-xxx.deb`, `linux-dtb-xxx.deb`, `linux-headers-xxx.deb`, and `linux-libc-dev-xxx.deb`.
- Additional kernel files are optional and do not affect the update process, as the system accurately identifies the required files. Any supported kernel version can be installed, e.g., switching from 6.6.12 to 5.15.50.

Custom options set via `-r`/`-u`/`-b`/`-d` can be saved permanently in `/etc/ophub-release` to avoid re-entering them each time. The corresponding settings are:

```shell
# Assign values to custom parameters
-r  :  KERNEL_REPO='ophub/kernel'
-u  :  KERNEL_TAGS='stable'
-b  :  KERNEL_BACKUP='yes'
-d  :  DOWNLOAD_TYPE='deb'
```

## 11. Installing Common Software

Log in to the Armbian system → Enter the command:

```shell
armbian-software
```

Use `armbian-software -u` to update the local software center list. Based on community feedback in [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), commonly used [software](../build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) is gradually integrated with one-click installation, update, and uninstallation support. This includes `docker images`, `desktop software`, `application services`, etc. See more [instructions](armbian_software.md).

Use `armbian-apt` to select an appropriate software mirror for your region, improving download speeds. For example, select the `mirrors.tuna.tsinghua.edu.cn` source in China:


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

This section covers common issues encountered when using Armbian.

### 12.1 dtb and u-boot Correspondence Table for Each Box

The supported TV box list is located in the [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) configuration file in the `Armbian` system.

### 12.2 Instructions for LED Screen Display Control

Please refer to the [instructions](led_screen_display_control.md).

### 12.3 How to Restore the Original Android TV System

Back up and restore the Android TV system using `armbian-ddbr`.

Alternatively, flash the Android system to eMMC via USB cable. Android system images are available in [Tools](https://github.com/ophub/kernel/releases/tag/tools).

#### 12.3.1 Backup and Restore Using Armbian-ddbr

Before installing Armbian on a new device, back up the original Android TV system in case restoration is needed later. Boot Armbian from `TF/SD/USB`, run `armbian-ddbr`, and enter `b` when prompted to create a backup. The backup is saved to `/ddbr/BACKUP-arm-64-emmc.img.gz` — download and keep it in a safe location. To restore, upload the backup file to the same path on the `TF/SD/USB` device, run `armbian-ddbr`, and enter `r` when prompted.


#### 12.3.2 Recovering using Amlogic Flashing Tool

- If the device can still boot from USB after reinserting the power supply, simply reinstall. Retry multiple times if necessary.

- If the screen remains black and the device cannot boot from USB, a short-circuit recovery is required. First restore the original Android system, then reinstall Armbian. Download and install the [amlogic_usb_burning_tool](https://github.com/ophub/kernel/releases/tag/tools) recovery tool. Prepare a [USB male-to-male cable](https://user-images.githubusercontent.com/68696949/159267576-74ad69a5-b6fc-489d-b1a6-0f8f8ff28634.png) and a [paperclip](https://user-images.githubusercontent.com/68696949/159267790-38cf4681-6827-4cb6-86b2-19c7f1943342.png).

- Using x96max+ as an example, locate the [short-circuit point](https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg) on the motherboard and download the [Android TV system package](https://github.com/ophub/kernel/releases/tag/tools). Android TV system images and short-circuit point diagrams for other common devices are also [available here](https://github.com/ophub/kernel/releases/tag/tools).

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

Once factory settings are restored, follow the same steps as the initial Armbian installation.

### 12.4 Setting the box to boot from USB/TF/SD

Depending on your situation, use either the initial installation or reinstallation method.

#### 12.4.1 Initial Installation of Armbian System

- Insert the USB/TF/SD with the installed system into the box.
- Enable developer mode: Settings → About device → Version number (e.g., X96max plus...), tap the version number 5 times rapidly until `You are now a developer` appears.
- Enable USB debugging: System → Advanced options → Developer options → enable `USB debugging` and `ADB` debugging.
- Install ADB: Download [adb](https://github.com/ophub/kernel/releases/tag/tools) and extract it. Copy `adb.exe`, `AdbWinApi.dll`, and `AdbWinUsbApi.dll` to `c:\windows\system32` and `c:\windows\syswow64`. Verify the installation by running `adb --version` in a command prompt.
- Open a command prompt and run `adb connect 192.168.1.137` (replace with your device's IP, found in your router's admin panel). A successful connection displays `connected to 192.168.1.137:5555`.
- Run `adb shell reboot update` — the device will reboot from the inserted USB/TF/SD. Access the system via its IP address in a browser or through SSH.

#### 12.4.2 Reinstallation of Armbian System

- In normal situations, simply insert the USB drive with Armbian and boot from it. USB takes boot priority over eMMC.
- If the device does not boot from USB, rename the `boot.scr` file in the eMMC's `/boot` directory (e.g., to `boot.scr.bak`), then insert the USB drive and boot again.

### 12.5 Disable Infrared Receiver

Infrared receiver support is enabled by default. If using the device as a server, disable the IR kernel module to prevent accidental shutdowns. To disable IR completely, add the following line:

```shell
blacklist meson_ir
```

to `/etc/modprobe.d/blacklist.conf` and restart.

### 12.6 Boot file selection

- Among known devices, only `T95(s905x)` / `T95Z-Plus(s912)` / `BesTV-R3300L(s905l-b)` and a few others require the `/bootfs/extlinux/extlinux.conf` file, which is included by default in the system. If other devices require it, write the system to USB, open the `boot` partition, and rename `/boot/extlinux/extlinux.conf.bak` by removing the `.bak` extension. When installing to eMMC, `armbian-install` automatically detects and creates the `extlinux.conf` file if needed.

- All other devices boot using `/boot/uEnv.txt` only — do not modify the `extlinux.conf.bak` file.

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

Adjust the IP, gateway, and DNS values to match your network configuration.

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

Modify the MAC address as needed.

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

Prerequisites for creating or modifying a network connection.

###### 12.7.2.1.1 Get Network Interface Name

Check available network interfaces.

```shell
nmcli device | grep -E "^[e].*|^[w].*|^[D].*|^[T].*" | awk '{printf "%-19s%-19s\n",$1,$2}'
```

The `DEVICE` column displays the network interface name, and the `TYPE` column displays the network interface type.

`eth0` is the first Ethernet interface, `eth1` is the second, and so on. Wireless interfaces follow the same convention.

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

List existing network connections on the device, both active and inactive. Avoid reusing existing connection names when creating new ones.

```shell
nmcli connection show | grep -E ".*|^[N].*" | awk '{printf "%-19s%-19s\n", $1,$3}'
```

The `NAME` column displays the name of the existing network connection, and the `TYPE` column displays the network interface type.

`ethernet` = wired, `wifi` = wireless, `bridge` = bridge

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

Create a new network connection on interface `eth0` with immediate activation (`Dynamic IP Address` - `IPv4 / IPv6`).

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

Create a new network connection on interface `eth0` with immediate activation (`Static IP Address` - `IPv4`).

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

Create a network connection on the `wlan0` interface with immediate activation (`Dynamic IP address` - `IPv4 / IPv6`).

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

Update the `WiFi SSID or password` in the wireless network connection `ssid` with immediate activation.

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

Change the IP allocation method to `Static IP address` for network connection `ether1` with immediate activation.

*Applicable to both wired and wireless connections

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

Change the IP allocation method to `DHCP` for network connection `ether1` with immediate activation.

*Applicable to both wired and wireless connections

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

Change the MAC address on a network connection to resolve MAC address conflicts.

###### 12.7.2.4.1 Method 1: Use the `nmcli` command to change the MAC address

```shell
# Use the `nmcli connection show` command to view the network interface name
nmcli connection show
# The returned result contains the network interface name, such as 'Wired connection 1'
NAME                UUID                                  TYPE      DEVICE
Wired connection 1  24d63dc7-c46f-3bf1-912f-1c33eb94338b  ethernet  eth0
lo                  35ca24e5-bdc0-4658-8ac8-435ee22e07f3  loopback  lo
Wired connection 2  59660b21-b460-30e0-8cb3-89b886556955  ethernet  --

# Set ENV
MYCON='Wired connection 1'    # Network connection name, note to match the network interface type
MYTYPE='802-3-ethernet'       # Network interface type = Wired network card / Wireless network card
                              #                        = 802-3-ethernet     / 802-11-wireless
MYMAC='12:34:56:78:9A:BC'     # Set new MAC address

# Chg CON
nmcli connection modify "${MYCON}" ${MYTYPE}.cloned-mac-address ${MYMAC}
nmcli connection up "${MYCON}"
ip -c a show "${MYCON}"
```

* Modifying certain network parameters may temporarily disconnect and reconnect the network.
* Depending on the hardware and software environment, changes take `1–15` seconds to apply. If changes do not take effect within this period, check your hardware and software configuration.

###### 12.7.2.4.2 Method 2: Modify the MAC address via a configuration file

Create a MAC address override configuration file.

```shell
sudo mkdir -p /etc/systemd/network/
sudo nano /etc/systemd/network/10-mac-override.link
```

Add the following content:

```shell
[Match]
# Specify the network interface name whose MAC address needs to be changed
OriginalName=eth0

[Link]
# Set a valid MAC address
MACAddress=02:55:66:77:88:99
```

Changes take effect after reboot.

##### 12.7.2.5 How to Disable IPv6

Use `nmcli` to disable `IPv6` from the command line. Please refer to [disable-ipv6](https://access.redhat.com/documentation/zh-cn/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/using-networkmanager-to-disable-ipv6-for-a-specific-connection_configuring-and-managing-networking) for the source.

Step 1: View the network connection list:

```shell
NAME                 UUID                                   TYPE       DEVICE
Wired connection 1   8a7e0151-9c66-4e6f-89ee-65bb2d64d366   ethernet   eth0
...
```

Step 2: Set the connection's ipv6.method parameter to disabled:

```shell
nmcli connection modify "Wired connection 1" ipv6.method "disabled"
```

Step 3: Reconnect to the network:

```shell
nmcli connection up "Wired connection 1"
```

Step 4: Verify the connection status. If no inet6 entry appears, IPv6 is disabled:

```shell
ip address show eth0
```

Step 5: Confirm that `/proc/sys/net/ipv6/conf/eth0/disable_ipv6` contains `1`:

```shell
# cat /proc/sys/net/ipv6/conf/eth0/disable_ipv6
1
```

Alternatively, disable IPv6 system-wide by adding the following to `/etc/sysctl.conf`:

```shell
# Disable IPv6 by default
net.ipv6.conf.all.disable_ipv6 = 1
net.ipv6.conf.default.disable_ipv6 = 1
net.ipv6.conf.lo.disable_ipv6 = 1
```

#### 12.7.3 How to Enable Wireless

Some devices support wireless networking. Enable it as follows:

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

Some devices support Bluetooth. Enable it as follows:

```shell
# Install Bluetooth support
armbian-config >> Network >> BT: Install Bluetooth support

# Reboot the system
reboot
```

After rebooting, verify the Bluetooth driver is functioning. Desktop environments can connect Bluetooth devices via the menu. Terminal-based configuration is also available.

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

Bluetooth can also be managed via terminal commands:

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

A startup task script is included in the system. In Armbian, the path is [/etc/custom_service/start_service.sh](../build-armbian/armbian-files/common-files/etc/custom_service/start_service.sh). Add your custom startup tasks to this script as needed.

### 12.9 How to Update Service Scripts in the System

Run `armbian-sync` to update all local service scripts to the latest version.

If `armbian-sync` fails, the installed version may be outdated. Update it manually:

```shell
wget https://raw.githubusercontent.com/ophub/amlogic-s9xxx-armbian/main/build-armbian/armbian-files/common-files/usr/sbin/armbian-sync -O /usr/sbin/armbian-sync

chmod +x /usr/sbin/armbian-sync

armbian-sync
```

### 12.10 How to Get Android System Partition Information on eMMC

Before writing Armbian to eMMC, confirm the device's Android partition table to ensure data is written to safe areas. Damaging the partition table may prevent the system from booting. Writing to unsafe areas can cause boot failures or errors like the one shown below:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/187075834-4ac40263-52ae-4538-a4b1-d6f0d5b9c856.png">
</div>

#### 12.10.1 Obtaining Partition Information

For Armbian versions released from this repository after November 2022, run the following command to generate a URL containing complete partition information (no internet connection required on the device):

```shell
ampart /dev/mmcblk2 --mode webreport 2>/dev/null
```

*The webreport mode was introduced in ampart v1.2 (released 2023.02.03). If the above command produces no output, your version may be older. Use this alternative command instead:*

```shell
echo "https://7ji.github.io/ampart-web-reporter/?dsnapshot=$(ampart /dev/mmcblk2 --mode dsnapshot 2>/dev/null | head -n 1)&esnapshot=$(ampart /dev/mmcblk2 --mode esnapshot 2>/dev/null | head -n 1)"
```

The generated URL will look similar to this:

```shell
https://7ji.github.io/ampart-web-reporter/?esnapshot=bootloader:0:4194304:0%20reserved:37748736:67108864:0%20cache:113246208:754974720:2%20env:876609536:8388608:0%20logo:893386752:33554432:1%20recovery:935329792:33554432:1%20rsv:977272832:8388608:1%20tee:994050048:8388608:1%20crypt:1010827264:33554432:1%20misc:1052770304:33554432:1%20instaboot:1094713344:536870912:1%20boot:1639972864:33554432:1%20system:1681915904:1073741824:1%20params:2764046336:67108864:2%20bootfiles:2839543808:754974720:2%20data:3602907136:4131389440:4&dsnapshot=logo::33554432:1%20recovery::33554432:1%20rsv::8388608:1%20tee::8388608:1%20crypt::33554432:1%20misc::33554432:1%20instaboot::536870912:1%20boot::33554432:1%20system::1073741824:1%20cache::536870912:2%20params::67108864:2%20data::-1:4
```

Open this URL in a browser to view the DTB and eMMC partition information:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287642-e1b7be27-4d2c-4ac3-9fcc-15e06aebb97e.png">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287654-d1929e21-d2b3-4fb6-bcf0-c454c88e21b9.png">
</div>

#### 12.10.2 Sharing Partition Information

When sharing partition information (e.g., reporting a new device or seeking help), share the URL rather than a screenshot. Use a URL shortener if the link is too long.

- The partition information on the webpage is dynamically generated on each visit, so annotations and table formatting may be updated over time.
- Additionally, partition parameters cannot be easily copied from screenshots for analysis.

The webpage's table layout is designed for easy copy-paste into Excel or LibreOffice Calc, eliminating the need to manually organize parameters.

#### 12.10.3 Interpreting Partition Information

The DTB table records the desired partition layout defined in the Android DTB. This layout typically ends with a `data` partition that fills the remaining space, so devices with the same system (and model) share identical DTB layouts. The actual partition layout may vary due to different eMMC capacities, but is always derived from the DTB partition layout (i.e., given the DTB layout and exact eMMC size, the eMMC partition layout can be determined. *Did you notice that the DTB and eMMC partition examples above come from different devices?*).

The eMMC table shows the actual partition layout on the device. Each row represents a storage area — either a partition or an inter-partition gap (Amlogic reserves at least 8MB between partitions, originally intended for other purposes but unused even in the latest S905X4, effectively wasting space). Partition rows appear in black with offset and mask values. Gap rows appear in grey with no offset or mask values and are labeled `gap`.

The last column indicates write safety: green/`yes` = writable, red/`no` = non-writable, yellow with a label = conditionally or partially writable.

Using the above table as an example, the `0+4M` (`0M~4M`) area of the `bootloader` partition is non-writable, the `32M` gap (`4M~36M`) after it is writable, the `36M+64M` (`36M~100M`) `reserved` partition is non-writable, and the gap from there to the gap before `env` (`100M~836M`) is fully writable. From 1M after `env` (`837M to the end`), the area is writable if the Android boot logo is not needed. The writable ranges on eMMC are:

- 4M~36M
- 100M~836M
- 837M~end

If the Android boot logo is needed, the 852M + 32M (`852M~884M`) `logo` partition is additionally non-writable, reducing the writable ranges to:

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

The u-boot file is a crucial component for system startup. The process of obtaining source code and the compilation workflow varies slightly across Amlogic, Allwinner, and Rockchip platforms.

#### 12.11.1 How to build the u-boot file for Amlogic devices

Since most Amlogic device manufacturers keep their source code closed, u-boot related files must be extracted from the device before compilation. The method presented here is derived from the tutorial shared by [unifreq](https://github.com/unifreq).

##### 12.11.1.1 How to extract the bootloader and dtb files

Extraction requires the HxD software. Download it from the [official site](https://mh-nexus.de/en/downloads.php?product=HxD20) or the [backup link](https://github.com/ophub/kernel/releases/download/tools/HxDSetup.2.5.0.0.zip).

Run the following commands in the `cmd` panel to extract the relevant files and download them to your local computer.

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

The most important part of the mainline u-boot is the acs.bin, which initializes memory. The original factory u-boot is located at the beginning of the system at the 4MB position. Use the `bootloader.bin` file obtained earlier to extract the `acs.bin` file.

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

Building u-boot requires the source repositories https://github.com/unifreq/amlogic-boot-fip and https://github.com/unifreq/u-boot to compile two u-boot files for your device.

Within the amlogic-boot-fip source code, the only file that varies by device model is acs.bin, all other files are universal.

<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187057209-c4716384-46ef-4922-9710-8da7ae6db1e4.png">

For build instructions, see the device-specific documentation at https://github.com/unifreq/u-boot/tree/master/doc/board/amlogic.

Building u-boot using [unifreq](https://github.com/unifreq)'s method requires the device's acs.bin, dts, and config files. The dts exported from the Android system cannot be directly converted to Armbian format, so a corresponding dts file must be written manually. Based on your device's specific hardware differences (switches, LEDs, power control, TF card, SDIO wifi module, etc.), modify and create a [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) file from similar ones in the kernel source repository.

For example, building u-boot for X96Max Plus:

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

Two types of files are generated: the `u-boot.bin` in the u-boot root directory is an incomplete u-boot for the `/boot` directory (corresponds to the [overload](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) directory); the `u-boot.bin` and `u-boot.bin.sd.bin` in the `fip` directory are complete u-boot files for `/usr/lib/u-boot/` (corresponds to the [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) directory). The two complete versions differ by 512 bytes — the larger one has 512 bytes of zeros prepended.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189039426-c127631f-77ca-4fcb-9fb6-4220045d712b.png">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189029320-e43a4cc9-b4b5-4de4-92fe-b17bd29020d0.png">
</div>

💡 Tip: Before writing to eMMC for testing, review section 12.3 for recovery methods. Ensure you know the short-circuit point location, have the original .img format Android system file, and have verified the short-circuit flashing process. Master all recovery methods before proceeding.


#### 12.11.2 How to build the u-boot file for Rockchip devices

Since most Rockchip device manufacturers have open-sourced their u-boot code, the relevant source code can be obtained directly from their repositories. Additionally, some open-source contributors have shared user-friendly u-boot compilation scripts. Below are several examples illustrating different compilation methods.

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

By adding more options in `board_configs.sh` and `mk-uboot.sh` within the [radxa/build](https://github.com/radxa/build) source code, u-boot files for other devices can also be compiled. For example, see the instructions for compiling the [Beelink-IPC-R(rk3588)](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415#issuecomment-1508234307) device.


##### 12.11.2.2 How to use cm9vdA's u-boot building script

cm9vdA provides scripts and instructions for compiling u-boot and the kernel in his open-source project [cm9vdA/build-linux](https://github.com/cm9vdA/build-linux). The following are documented compilation processes for various Rockchip devices:

- Build u-boot for Lenovo-Leez-P710 (rk3399) device: [Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609#issuecomment-1681494735)
- Build u-boot for DLFR100 (rk3399) device: [Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522#issuecomment-1622919423)
- Build u-boot for ZYSJ (rk3399) device: [Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380#issuecomment-1539325464)


### 12.12 Error in Memory Size Recognition

If the memory size is recognized incorrectly (4GB recognized as 1–2GB is abnormal; 3.7GB is normal), try manually copying a `/boot/UBOOT_OVERLOAD` file (note: "copy", not "rename" — renaming will prevent the system from booting after installation and updates). When used from `USB`, save it as `/boot/u-boot.ext`; when used from `eMMC`, save it as `/boot/u-boot.emmc`.

Do not manually copy the u-boot file for any other purpose. Incorrect u-boot files will cause boot failures and various other issues.

### 12.13 How to Decompile dtb Files

Some new devices are not currently supported (or have hardware variants). Try adjusting related parameters by decompiling the dtb file.

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

In Amlogic devices, add/modify/delete settings in `/boot/uEnv.txt`. In Rockchip and Allwinner devices, configure in `/boot/armbianEnv.txt` (via the `extraargs` or `extraboardargs` parameters). Devices using `/boot/extlinux/extlinux.conf` configure in that file. A reboot is required after each change.

- For instance, `Home Assistant Supervisor` only supports `docker cgroup v1`, while the default Docker installation uses v2. To switch to v1, add `systemd.unified_cgroup_hierarchy=0` to cmdline and reboot.

- Adding `max_loop=128` to cmdline adjusts the maximum number of loop mounts.

- Adding `usbcore.usbfs_memory_mb=1024` to cmdline increases the USBFS memory buffer from the default `16 mb` (`cat /sys/module/usbcore/parameters/usbfs_memory_mb`), improving large USB file transfers.

- Adding `usbcore.usb3_disable=1` to cmdline disables all USB 3.0 devices.

- Adding `extraargs=video=HDMI-A-1:1920x1080@60` to cmdline forces the display to 1080p.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://user-images.githubusercontent.com/68696949/216220941-47db0183-7b26-4768-81cf-2ee73d59d23e.png">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a600dcad-d817-47eb-b529-4014019915b3">
</div>

### 12.15 How to Add New Supported Devices

Building an Armbian system for a device requires a `device configuration file`, `system files`, `u-boot files`, and `process control files`. The methods for adding each are described below:

#### 12.15.1 Add Device Configuration File

In the configuration file [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf), add device configuration based on its test support status. Devices with `BUILD` set to `yes` are built by default; the corresponding `BOARD` value `must be unique`. These devices can use the default Armbian system directly.

Devices set to `no` are not packaged by default. To use them, download the Armbian system with the same `FAMILY`, write it to `USB`, open the USB boot partition on your computer, and modify the `FDT dtb name` in `/boot/uEnv.txt` to match your device.

#### 12.15.2 Add System Files

Common files are placed in `build-armbian/armbian-files/common-files`, shared across all platforms.

Platform-specific files are placed in `build-armbian/armbian-files/platform-files/<platform>`: [Amlogic](../build-armbian/armbian-files/platform-files/amlogic), [Rockchip](../build-armbian/armbian-files/platform-files/rockchip), and [Allwinner](../build-armbian/armbian-files/platform-files/allwinner). The `bootfs` directory contains /boot partition files, and the `rootfs` directory contains Armbian system files.

For devices with special configuration requirements, add an independent directory named after the `BOARD` in `build-armbian/armbian-files/different-files`. Create `bootfs` and `rootfs` subdirectories as needed to add files under the system's `/boot` partition or other system paths. All folder names correspond to actual paths in the Armbian system. These files can add new files or override files from common and platform directories.

#### 12.15.3 Add u-boot Files

`Amlogic` series devices, share [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) files and [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) files. If there are new files, put them in the corresponding directory. The `bootloader` files will automatically be added to the Armbian system's `/usr/lib/u-boot` directory during system construction, and `u-boot` files will be automatically added to the `/boot` directory.

`Rockchip` and `Allwinner` series devices, add an independent [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot) file directory named after `BOARD` for each device, and the corresponding series files are placed in this directory.

During the Armbian image construction, these u-boot files will be written into the corresponding Armbian image files by the rebuild script according to the configuration in [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf).

For devices that can use standard U-Boot files, using them directly is strongly recommended. However, some devices may lack a suitable U-Boot. If such a device can already run other Linux systems like Ubuntu, you can try preserving key boot-related partitions to install Armbian or OpenWrt. The critical partitions that typically need preservation include `bootloader`, `reserved`, and `env`.

These partitions can be backed up and written back to their corresponding locations when creating a new Armbian or OpenWrt image. The resulting image, containing the original system's boot partitions, can be written to eMMC directly via the `dd` command or through the built-in installation tools (e.g., Armbian's `armbian-install` or OpenWrt's `Amlogic Service` plugin).

Currently, devices using this method include [oes(a311d)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2666), [oes-plus(s922x)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3029), and [oec-turbo(rk3566)](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2736). The following uses the `oes(a311d)` device as an example to detail the procedure.

##### 12.15.3.1 Check the Partition Layout

```shell
ampart /dev/mmcblk2

# The partition information obtained is as follows:
# ===================================================================================
# IO seek EPT: Seeking to 37748736
# EPT report: 20 partitions in the table:
# ===================================================================================
# ID| name            |          offset|(   human)|            size|(   human)| masks
# -----------------------------------------------------------------------------------
#  0: bootloader                      0 (   0.00B)           400000 (   4.00M)      0
#     (GAP)                                                 2000000 (  32.00M)
#  1: reserved                  2400000 (  36.00M)          4000000 (  64.00M)      0
#     (GAP)                                                  800000 (   8.00M)
#  2: cache                     6c00000 ( 108.00M)         20000000 ( 512.00M)      2
#     (GAP)                                                  800000 (   8.00M)
#  3: env                      27400000 ( 628.00M)           800000 (   8.00M)      0
#     (GAP)                                                  800000 (   8.00M)
#  ... other partitions ...
# ===================================================================================
```

##### 12.15.3.2 Back Up Key Partitions

```shell
dd if=/dev/mmcblk2 of=bootloader.bin bs=1M count=4 skip=0
dd if=/dev/mmcblk2 of=reserved.bin bs=1M count=8 skip=36
dd if=/dev/mmcblk2 of=env.bin bs=1M count=1 skip=628
```
Place the backed-up files in the corresponding directory of the [u-boot](https://github.com/ophub/u-boot) repository: [u-boot/amlogic/bootloader/a311d-oes](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader/a311d-oes).

You might have noticed that the reserved partition is 64MB in size, yet only 8MB was backed up. On the oes(a311d) device, only the first 8MB contains critical data; the remaining 56MB is empty. Here is how to verify this:

```shell
# First, back up the complete 64MB file of the reserved partition:
dd if=/dev/mmcblk2 of=reserved_64M.bin bs=1M count=64 skip=36

# Then, extract the first 8MB from the backed-up file reserved_64M.bin:
dd if=reserved_64M.bin of=reserved_first_8M.bin bs=1M count=8

# Next, check the hexadecimal contents:
hexdump -C reserved_first_8M.bin | less

# Now, examine the last few lines of the output:
0071ffd0  4c 5e a8 1f fc 5b 5b 98  ae ef b0 97 0c 3b e8 c2  |L^...[[......;..|
0071ffe0  c8 e0 b2 74 3d 67 d5 3d  24 7b 63 b7 c7 73 f5 d8  |...t=g.=${c..s..|
0071fff0  a1 b8 38 a7 57 d6 b4 b5  e8 1c ba c0 07 0f f5 79  |..8.W..........y|
00720000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00800000
```

Analyzing the output, the address of the last line containing non-zero data is `0071fff0`. Starting from address `00720000`, all content is `00` (zero). The hexdump utility uses an asterisk (`*`) to indicate repeated lines, meaning everything from `00720000` to the end of the file at `00800000` (the 8MB mark) is zero. Converting `0x00720000` to decimal gives 7,471,104 bytes (7.125 MB). Therefore, rounding up to 8MB for the backup is sufficient. Similarly, only the first 80KB of the env partition contains valid data, so 1MB is adequate.

##### 12.15.3.3 Add a Special Partition Writing File

For implementation details, refer to the `write_board_bootloader` function in [/etc/armbian-board-release.conf](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/build-armbian/armbian-files/different-files/a311d-oes/rootfs/etc/armbian-board-release.conf). This function is called during the image rebuild process. This configuration file also serves as a powerful device customization hub — it allows precise control over image partition layout and size via parameters like `skip_mb="700"`, and supports custom scripts for kernel or system file operations. All advanced, device-specific customizations are centrally managed in this file.

#### 12.15.4 Add Process Control Files

Add the corresponding `BOARD` option to `armbian_board` in the [yml workflow control file](../.github/workflows/build-armbian-arm64-server-image.yml), which supports use in `Actions` on github.com.

### 12.16 How to Resolve the Issue of I/O Errors While Writing to eMMC

Some devices can boot Armbian normally from USB/SD/TF but report I/O write errors when writing to eMMC, such as the case in [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues/989):

```shell
[  284.338449] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.341544] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.446972] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.450074] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.497746] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.500871] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
```

In such cases, adjust the working mode speed and frequency of the dtb to stabilize storage read/write support. In SDR mode, the frequency is twice the speed; in DDR mode, the frequency equals the speed. For example:

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

Generally, reducing the frequency of `&sd_emmc_c` from `max-frequency = <200000000>;` to `max-frequency = <100000000>;` resolves the issue. If not, try `50000000`. Adjust `&sd_emmc_b` for `USB/SD/TF` settings, and use `sd-uhs-sdr` to limit speed. Modify the dts file and [compile](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel) to get a test file, or use the decompilation method in `Section 12.13` to modify an existing dtb file. When modifying decompiled dtb files, use hexadecimal values: decimal `200000000` = hex `0xbebc200`, `100000000` = `0x5f5e100`, `50000000` = `0x2faf080`, `25000000` = `0x17d7840`.

In addition to software-layer solutions, this issue can also be resolved through [hardware upgrades](https://github.com/ophub/amlogic-s9xxx-armbian/issues/998) or [physical modifications](https://www.right.com.cn/forum/thread-901586-1-1.html).

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

Restart Armbian for testing. If the sound still doesn't work, your device may be using an older audio output configuration. Comment out the new configuration at `L137-L142` in /usr/bin/g12_sound.sh (primarily for G12B / S922X; the older G12A/S905X2 and most SM1/S905X3 based on G12A cannot use it), and uncomment the old configuration at `L130-L134`.

### 12.18 How to build the boot.scr file

In the Armbian system, the `boot.scr` file in `/boot` is the compiled boot script. `boot.scr` is generated from the `boot.cmd` source file. Modify `boot.cmd` to make changes, then compile it into `boot.scr` using the mkimage command.

Normally, these files do not need modification. If adjustments are necessary, follow the steps below.

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

In the software center `armbian-software`, selecting `201` installs a desktop environment. During installation, you will be prompted to enable remote desktop — enter `y` to enable. The default remote desktop port is `3389`. To use a custom port:

```shell
sudo nano /etc/xrdp/xrdp.ini
# Change to a custom port, for example 5000
port=5000
```

### 12.20 TCP Congestion Control Optimization Guide

Different network stack configurations are recommended based on device performance to ensure the best user experience. Please configure your device according to its hardware capabilities by editing `/etc/sysctl.conf` and modifying (or adding) the following lines:

- Gigabit Devices (High Performance/Modern Arch): The `fq + bbr` combination is recommended to maximize throughput and improve packet loss resilience.
- 100Mbps Devices (Low Performance/Legacy Arch): The `fq_codel + cubic` combination is suggested to minimize CPU load and maintain low-latency stability.

```shell
# Option A: Recommended for Gigabit/High Performance Devices
net.core.default_qdisc = fq
net.ipv4.tcp_congestion_control = bbr

# Option B: Recommended for 100Mbps/Low Performance Devices
# net.core.default_qdisc = fq_codel
# net.ipv4.tcp_congestion_control = cubic
```
