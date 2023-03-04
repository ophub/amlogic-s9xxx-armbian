# How to build and use Armbian

View Chinese description  |  [查看中文说明](README.cn.md)

`GitHub Actions` is a service launched by `Microsoft`. It provides a virtual server environment with very good performance configuration. Based on it, projects can be built, tested, packaged, and deployed. The public repository can be used for free without time limit, and the single compilation time is up to `6 hours`, which is enough for compiling  `Armbian` (we can usually complete a compilation in about `3 hours`). Sharing is only for the exchange of experience. Please understand the deficiencies. Please do not initiate various bad attacks on the Internet, and do not maliciously use it.

# Tutorial directory

- [How to build and use Armbian](#how-to-build-and-use-armbian)
- [Tutorial directory](#tutorial-directory)
  - [1. Register your own GitHub account](#1-register-your-own-github-account)
  - [2. Set the privacy variable GitHub\_TOKEN](#2-set-the-privacy-variable-github_token)
  - [3. Fork repository and set GH\_TOKEN](#3-fork-repository-and-set-gh_token)
  - [4. Personalized Armbian firmware customization file description](#4-personalized-armbian-firmware-customization-file-description)
  - [5. Compile the firmware](#5-compile-the-firmware)
    - [5.1 Manual compilation](#51-manual-compilation)
    - [5.2 Compile at the agreed time](#52-compile-at-the-agreed-time)
    - [5.3 Customize the default firmware configuration](#53-customize-the-default-firmware-configuration)
  - [6. Save the firmware](#6-save-the-firmware)
  - [7. Download the firmware](#7-download-the-firmware)
  - [8. Install Armbian to EMMC](#8-install-armbian-to-emmc)
    - [8.1 Amlogic Series Installation Method](#81-amlogic-series-installation-method)
    - [8.2 Rockchip Series Installation Method](#82-rockchip-series-installation-method)
      - [8.2.1 Installation method of Radxa-Rock5B](#821-installation-method-of-radxa-rock5b)
        - [8.2.1.1 Installing the system to microSD](#8211-installing-the-system-to-microsd)
        - [8.2.1.2 Installing the system to eMMC](#8212-installing-the-system-to-emmc)
        - [8.2.1.3 Installing the system to NVMe](#8213-installing-the-system-to-nvme)
      - [8.2.2 Installation method of FastRhino-R66S](#822-installation-method-of-fastrhino-r66s)
      - [8.2.3 Installation method of FastRhino-R68S](#823-installation-method-of-fastrhino-r68s)
      - [8.2.4 Installation method of BeikeYun](#824-installation-method-of-beikeyun)
      - [8.2.5 Installation method of l1pro](#825-installation-method-of-l1pro)
    - [8.3 Allwinner Series Installation Method](#83-allwinner-series-installation-method)
  - [9. Compile the Armbian kernel](#9-compile-the-armbian-kernel)
  - [10. Update Armbian Kernel](#10-update-armbian-kernel)
  - [11. Install common software](#11-install-common-software)
  - [12. common problem](#12-common-problem)
    - [12.1 dtb and u-boot correspondence table for each box](#121-dtb-and-u-boot-correspondence-table-for-each-box)
    - [12.2 LED screen display control instructions](#122-led-screen-display-control-instructions)
    - [12.3 How to restore the original Android TV system](#123-how-to-restore-the-original-android-tv-system)
      - [12.3.1 Restoring using armbian-ddbr backup](#1231-restoring-using-armbian-ddbr-backup)
      - [12.3.2 Restoring with Amlogic usb burning tool](#1232-restoring-with-amlogic-usb-burning-tool)
    - [12.4 Set the box to boot from USB/TF/SD](#124-set-the-box-to-boot-from-usbtfsd)
    - [12.5 Disable infrared receiver](#125-disable-infrared-receiver)
    - [12.6 Selection of bootstrap file](#126-selection-of-bootstrap-file)
    - [12.7 Network settings](#127-network-settings)
      - [12.7.1 Dynamic IP address assignment by DHCP](#1271-dynamic-ip-address-assignment-by-dhcp)
      - [12.7.2 Manually set a static IP address](#1272-manually-set-a-static-ip-address)
      - [12.7.3 Use OpenWrt in docker to establish interworking network](#1273-use-openwrt-in-docker-to-establish-interworking-network)
    - [12.8 How to add startup tasks](#128-how-to-add-startup-tasks)
    - [12.9 How to update service scripts in the system](#129-how-to-update-service-scripts-in-the-system)
    - [12.10 How to obtain Android partition info on eMMC](#1210-how-to-obtain-android-partition-info-on-emmc)
      - [12.10.1 To obain the partition info](#12101-to-obain-the-partition-info)
      - [12.10.2 To share the partition info](#12102-to-share-the-partition-info)
      - [12.10.3 To understand the partition info](#12103-to-understand-the-partition-info)
      - [12.10.4 To be used on eMMC installation](#12104-to-be-used-on-emmc-installation)
    - [12.11 How to make u-boot file](#1211-how-to-make-u-boot-file)
      - [12.11.1 Extract the bootloader and dtb files](#12111-extract-the-bootloader-and-dtb-files)
      - [12.11.2 Make the acs.bin file](#12112-make-the-acsbin-file)
      - [12.11.3 Make the u-boot file](#12113-make-the-u-boot-file)
    - [12.12 Memory size recognition error](#1212-memory-size-recognition-error)
    - [12.13 How to decompile a dtb file](#1213-how-to-decompile-a-dtb-file)
    - [12.14 How to modify cmdline settings](#1214-how-to-modify-cmdline-settings)
    - [12.15 How to add new supported devices](#1215-how-to-add-new-supported-devices)
      - [12.15.1 Add device profile](#12151-add-device-profile)
      - [12.15.2 Add system files](#12152-add-system-files)
      - [12.15.3 Add u-boot files](#12153-add-u-boot-files)
      - [12.15.4 Add process control files](#12154-add-process-control-files)
    - [12.16 12.16 How to fix I/O errors when writing to eMMC](#1216-1216-how-to-fix-io-errors-when-writing-to-emmc)
    - [12.17 How to fix the Bullseye version with no sound](#1217-how-to-fix-the-bullseye-version-with-no-sound)

## 1. Register your own GitHub account

Register your own account, so that you can continue to customize the firmware. Click the `Sign up` button in the upper right corner of the `github.com` website and follow the prompts to `register your account`.

## 2. Set the privacy variable GitHub_TOKEN

Set the GitHub privacy variable `GitHub_TOKEN`. After the firmware is compiled, we need to upload the firmware to `GitHub Releases`. We set this variable according to the official requirements of GitHub. The method is as follows:
`Personal center`: `Settings` > `Developer settings` > `Personal access tokens` > `Generate new token` ( Name: `GitHub_TOKEN`, Select: `public_repo` ). `Other options` can be selected according to your needs. Submit and save, copy the `Encrypted KEY Value` generated by the system, and `save it` to your computer's notepad first. This value will be used in the next step. The icons are as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/109418474-85032b00-7a03-11eb-85a2-759b0320cc2a.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418479-8b91a280-7a03-11eb-8383-9d970f4fffb6.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418483-90565680-7a03-11eb-8320-0df1174b0267.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418493-9815fb00-7a03-11eb-862e-deca4a976374.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418485-93514700-7a03-11eb-848d-36de784a4438.jpg width="300" />
</div>

## 3. Fork repository and set GH_TOKEN

Now you can `Fork` the `repository`, open the repository [https://github.com/ophub/amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), click the `Fork` button on the `upper right`, Will copy a copy of the repository code to your account, `wait a few seconds`, and prompt the Fork to complete Later, go to your account to access `amlogic-s9xxx-armbian` in `your repository`. In the upper right corner of `Settings` > `Secrets` > `Actions` > `New repostiory secret` (Name: `GH_TOKEN`, Value: `Fill in the value of GitHub_TOKEN` just now), `save it`. And select `Read and write permissions` under `Actions` > `General` > `Workflow permissions` in the left nav and save. The icons are as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/109418568-0eb2f880-7a04-11eb-81c9-194e32382998.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163203032-f044c63f-d113-4076-bf94-41f86c7dd0ce.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/109418573-15417000-7a04-11eb-97a7-93973d7479c2.jpg width="300" />
<img src=https://user-images.githubusercontent.com/68696949/167579714-fdb331f3-5198-406f-b850-13da0024b245.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/167585338-841d3b05-8d98-4d73-ba72-475aad4a95a9.png width="300" />
</div>

## 4. Personalized Armbian firmware customization file description

The firmware compilation process is controlled in the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file. There are other `yml files` in the `workflows` directory to achieve other different functions. When compiling the firmware, the Armbian official current code is used for real-time compilation, and the relevant parameters can be found in the official documentation.

```yaml
- name: Compile Armbian [ ${{ env.ARMBIAN_BOARD }} ]
  id: compile
  run: |
    cd build/
    sudo ./compile.sh RELEASE=${{ env.ARMBIAN_RELEASE }} BOARD=odroidn2 BRANCH=current BUILD_ONLY=default HOST=armbian EXPERT=yes \
                      BUILD_DESKTOP=no BUILD_MINIMAL=no KERNEL_CONFIGURE=no CLEAN_LEVEL="make,debs" COMPRESS_OUTPUTIMAGE="sha"
    echo "build_tag=Armbian_${{ env.ARMBIAN_RELEASE }}_$(date +"%m.%d.%H%M")" >> ${GITHUB_OUTPUT}
    echo "status=success" >> ${GITHUB_OUTPUT}
```

## 5. Compile the firmware

There are many ways to compile firmware, you can set timed compilation, manual compilation, or set some specific events to trigger compilation. Let's start with simple operations.

### 5.1 Manual compilation

In the `navigation bar of your repository`, click the `Actions` button, and then click `Build armbian` > `Run workflow` > `Run workflow` to start the compilation, wait about `3 hours`, and complete the compilation after all the processes are over. The icons are as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163203938-e7762b09-e6b8-4cf5-b1f1-9c67c1a29953.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204044-9c7a7429-47ee-4fce-b7dd-e217bebf6133.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204127-6b16b558-7e78-4c22-a28a-7b37b5a34fa3.png width="300" />
</div>

### 5.2 Compile at the agreed time

In the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file, use `cron` to set the timing compilation. The 5 different positions represent min (0 - 59) / hour (0 - 23) / day of month (1 - 31) / month (1 - 12) / day of week (0 - 6)(Sunday - Saturday). Set the time by modifying the values of different positions. The system uses `UTC standard time` by default, please convert it according to the time zone of your country.

```yaml
schedule:
  - cron: '0 17 * * *'
```

### 5.3 Customize the default firmware configuration

The configuration information of the default firmware is recorded in the [model_database.conf](../armbian-files/common-files/etc/model_database.conf) file, the `BUILD` value of the firmware to be compiled is set to `yes`, and the `BOARD` name must be unique.

It is specified by the `-b` parameter when compiling `locally`, and specified by the `armbian_board` parameter when compiling in `Actions` of github.com.

In general, you only need to compile the general firmware. Other boxes in the same family can refer to the configuration file information table and use it by modifying the `dtb` value in `/boot/uEnv.txt`.

## 6. Save the firmware

The settings saved by the firmware are also controlled in the [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) file. We will automatically upload the compiled firmware to the `Releases` officially provided by `GitHub` through scripts.

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

## 7. Download the firmware

Enter from the GitHub `Releases` section at the bottom right corner of the `Repository homepage`, and select the firmware corresponding to the model of your `Amlogic s9xxx TV Boxes`. The icons are as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163204798-0d98524c-73df-4876-8912-fcae2845fbba.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204879-4898babf-fa00-4e63-89ea-235129e2ce1d.png width="300" />
</div>

## 8. Install Armbian to EMMC

Amlogic, Rockchip and Allwinner have different installation methods. Different devices have different storage. Some devices use external TF cards, some have eMMC, and some support the use of NVMe and other storage media. According to different devices, their installation methods are introduced respectively. Use the different installation methods in the following summary according to your own devices.

When the installation is complete, Connect the Armbian device to the `router`. After the device is powered on for `2 minutes`, check the `IP` address of the device named `Armbian` in the router, and use the `SSH` tool to connect for management settings. The default user name is `root`, the default password is `1234`, and the default port is `22`

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972715-addcd695-970a-43d6-8a34-24a9c4bc80a2.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972773-fc9e9ef9-69a7-4279-8329-6fad1cf2f5b9.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972818-b72e18cd-17d1-4f9f-a0fa-b6c22eef041d.png width="600" />
</div>

### 8.1 Amlogic Series Installation Method

Login in to armbian (default user: root, default password: 1234) → input command:

```yaml
armbian-install
```

| Optional  | Default  | Value    | Description           |
| --------- | -------  | -------- | --------------------  |
| -m        | no       | yes/no   | Use Mainline u-boot   |
| -a        | yes      | yes/no   | Use [ampart](https://github.com/7Ji/ampart) tool |
| -l        | no       | yes/no   | List show all         |

Example: `armbian-install -m yes -a no`

### 8.2 Rockchip Series Installation Method

The installation method of each device is different, which are introduced as follows.

#### 8.2.1 Installation method of Radxa-Rock5B

Radxa-Rock5B has a variety of storage media to choose from, including microSD/eMMC/NVMe, and the corresponding installation methods are different. Download [rk3588_spl_loader_v1.08.111.bin and spi_image.img files](../u-boot/rockchip/rock5b) for backup. Download [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/Radxa_rock5b_RKDevTool_Release_v2.96__DriverAssitant_v5.1.1.tar.gz) tool and drive backup. Download [Rufus](https://rufus.ie/) Or [balenaEtcher](https://www.balena.io/etcher/) disk writing tools for backup.

##### 8.2.1.1 Installing the system to microSD

Using Rufus or balenaEtcher and other tools to write the Armbian system image into the microSD, and then insert the microSD with the firmware into the device for use.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972996-300f223b-f6f6-48af-86ca-bdc842e5017d.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202973216-85b1a21b-0763-4a36-8c57-84490e071fdd.png width="600" />
</div>

##### 8.2.1.2 Installing the system to eMMC

- Install using USB to eMMC card reader: connect the eMMC module to the computer, and use Rufus Or balenaEtcher and other tools to write the image of the Armbian system into the eMMC, and then insert the eMMC with the firmware into the device for use.
- Install using Maskrom mode: Power off the board. Press the golden button and hold it. Plug the USB-A to Type-C cable to ROCK 5B Type-C port, the other side to PC. Release the golded button. Check usb device is in Found One MASKROM Device. Right-click in the blank area of the list and select Load configuration file(The configuration file and RKDevTool are in the same directory). Select `rk3588_spl_loader_v1.08.111.bin` and `Armbian.img` as shown in the figure below and write them in.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954901-c829d74d-c75a-4fd3-9bd0-aa3cdf2b77b4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954998-c08514e2-8760-4c0f-b5f7-0d30be635aa5.png width="600" />
</div>

##### 8.2.1.3 Installing the system to NVMe

ROCK-5B has one SPI Flash on the board, it's useful to install the bootloader to the SPI flash for a back up booting and support other booting media that the SoC maskrom mode doesn't direct support, such as SATA, USB 3 or NVMe. Using NVMe requires writing SPI files first. The method is as follows:

Power off the board. Remove bootable device like MicroSD card, eMMC module, etc. Press the golden (or silver on some board revisions) button and hold it. Plug the USB-A to Type-C cable to ROCK-5B Type-C port, the other side to PC. Release the golded button. Check usb device is Found One MASKROM Device. Right-click in the list box and select Load Config,Then select the configuration file in the resource management folder（The configuration file and RKDevTool are in the same directory）, click the right last columns in the `Loader` row to select `rk3588_spl_loader_v1.08.111.bin`, click the right last columns in the `spi` row to select `spi_image.img`. Finally, click the `Run` button, and you will see the content in the red box on the right. When the progress reaches 100%, the download is completed. As shown in the figure below:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961420-8316c96c-2796-43ed-b5ed-2fa5bfa1ddff.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961447-49c0941a-e233-4b2a-b96b-b47636ce3cf2.png width="600" />
</div>

- Install with a card reader: insert the M.2 NVMe SSD into the M.2 NVMe SSD to the USB3.0 card reader to connect to the host. Use tools such as Rufus or balenaEtcher to write the Armbian system image to NVMe, and then insert the NVMe with the firmware into the device for use.
- Installation with microSD card: write the image of the Armbian system into the microSD card, insert the microSD card into the device, start it, and upload the Armbian image file is to the microSD card, and then use the `dd` command to write the `Armbian.img` to NVMe. The command is as follows:

```Shell
dd if=armbian.img  of=/dev/nvme0n1  bs=1M status=progress
```

#### 8.2.2 Installation method of FastRhino-R66S

Use tools such as Rufus or balenaEtcher to write the Armbian system image into the microSD, and then insert the microSD with the firmware into the device for use.

#### 8.2.3 Installation method of FastRhino-R68S

- Download the [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) tool and driver, decompress and install the DriverAssistant driver, and open the RKDevTool tool for standby.
- When the R68s is powered off, first insert the USB dual male cable, then press and hold the Recovery key and plug in the 12V power supply. After two seconds, release the Recovery key, and the brushing tool will `find a LOADER device`.
- Right click the blank space of the RKDevTool tool operation interface to add an item.
- The address is `0x00000000` and the name is `armbian`. Click the path on the right to select `armbian.img` system files.
- Select an additional armbian line, `deselect other lines`, and click `Run` Write.
- Note: If other systems are written to eMMC, please first erase them in the advanced functions, and then write them to the Armian system. If it cannot be erased, first rewrite the `MiniLoaderAll.bin` boot file, and then enter `MASKROM` again to write to the Armbian system. MiniLoaderAll.bin boot file settings: address `0xCCCCCCCC`, name `Loader`, path selection `MiniLoaderAll.bin` file under the Image directory of RKDevTool tool.

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202970301-d798677b-e875-4971-ac8f-ee58b2a1e686.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970405-cb68cb78-cd0f-43ee-b807-5e594ab73099.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970488-5f18c574-c11f-486f-8fe8-002f3ba2f3f4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970577-87549acf-b98b-441f-bb31-e4fd6608108d.png width="600" />
</div>

#### 8.2.4 Installation method of BeikeYun

Method reproduced from milton's [tutorial](https://www.cnblogs.com/milton/p/15391525.html). Flashing needs to enter Maskrom mode. Disconnect all connections first, short the two contacts of CLK and GND (use the GND of TTL, or the GND of the small button next to it), and then connect the USB to the PC to detect the MASKROM device. The position of the short-circuit point is as follows:

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202977817-fb12d291-47e2-47e4-88c3-e21f9ae57922.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202977900-50b4770d-8444-42a0-8478-3234043455bd.png width="600" />
</div>

Open the RKDevTool flashing tool, right-click to add an item.

- Address `0xCCCCCCCC`, name `Boot`, path [select](../u-boot/rockchip/beikeyun) `rk3328_loader_v1.14.249.bin`.
- Address `0x00000000`, name `system`, path select `Armbian.img` firmware.

Click to execute the write.

#### 8.2.5 Installation method of l1pro

Method reproduced from cc747's [tutorial](https://post.smzdm.com/p/a4wkdo7l/). Flashing needs to enter Maskrom mode. Make l1pro power off and unplug all cables. Use a USB double-male cable, plug one end into the USB2.0 port of l1pro, and plug the other end into the computer. Insert a paper clip into the Reset hole and press it down firmly. Plug in the power cord. Wait for a few seconds until `Found a LOADER device` appears at the bottom of the RKDevTool box before releasing the paperclip. Switch RKDevTool to `Advanced Functions` and click the `Enter Maskrom` button, prompting `Found a MASKROM device`.

Right click to add item.

- Address `0xCCCCCCCC`, name `Boot`, path [select](../u-boot/rockchip/l1pro) `rk3328_loader.bin`.
- Address `0x00000000`, name `system`, path to select the `Armbian.img` firmware to flash.

Click to execute the write.

### 8.3 Allwinner Series Installation Method

Login in to armbian (default user: root, default password: 1234) → input command:

```yaml
armbian-install
```

## 9. Compile the Armbian kernel

Supports compiling the kernel in Ubuntu20.04/22.04 or Armbian system. It supports local compilation and cloud compilation using GitHub Actions. For details, see [Kernel Compilation Instructions](../../compile-kernel).

## 10. Update Armbian Kernel

Login in to armbian → input command:

```yaml
# Run as root user (sudo -i)
# If no other parameters are specified, the following update command will update to the latest version of the current kernel of the same series.
armbian-update
```

| Optional  | Default     | Value       | Description                   |
| -------   | -------     | ----------  | ---------------------------   |
| -k        | auto latest | [kernel name](https://github.com/ophub/kernel/tree/main/pub/stable)  | Set the kernel name |
| -v        | stable      | stable/dev  | Set the kernel version branch |
| -m        | no          | yes/no      | Use Mainline u-boot           |

Example: `armbian-update -k 5.15.50 -v dev -m yes`

If there is a set of kernel files in the current directory, it will be updated with the kernel in the current directory (The 4 kernel files required for the update are `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-xxx.tar.gz`, `modules-xxx.tar.gz`. Other kernel files are not required. If they exist at the same time, it will not affect the update. The system can accurately identify the required kernel files). If there is no kernel file in the current directory, it will query and download the latest kernel of the same series from the server for update. The optional kernel supported by the device can be freely updated, such as from 5.10.125 kernel to 5.15.50 kernel.

## 11. Install common software

Login in to armbian → input command:

```yaml
armbian-software
```

Use the `armbian-software -u` command to update the local software center list. According to the user's demand feedback in the [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), gradually integrate commonly used [software](../armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) to achieve one-click install/update/uninstall and other shortcut operations. Including `docker images`, `desktop software`, `application services`, etc. See more [Description](armbian_software.md).

## 12. common problem

In the use of Armbian, some common problems that may be encountered are summarized below.

### 12.1 dtb and u-boot correspondence table for each box

Please refer to [Description](amlogic_model_database.md)

### 12.2 LED screen display control instructions

Please refer to [Description](led_screen_display_control.md)

### 12.3 How to restore the original Android TV system

Usually use armbian-ddbr backup to restore, or use Amlogic usb burning tool to restore the original Android TV system.

#### 12.3.1 Restoring using armbian-ddbr backup

It is recommended that you make a backup of the original Android TV system that comes with the current box before installing the Armbian system in a new box, so that you can use it when you need to restore the system. Please boot the Armbian system from `TF/SD/USB`, enter the `armbian-ddbr` command, and then enter `b` according to the prompts to backup the system. The backup file is stored in the path `/ddbr/BACKUP-arm-64-emmc. img.gz` , please download and save. When you need to restore the Android TV system, upload the previously backed up files to the same path of the `TF/SD/USB` device, enter the `armbian-ddbr` command, and then enter `r` according to the prompt to restore the system.

#### 12.3.2 Restoring with Amlogic usb burning tool

- Under normal circumstances, re-insert the USB hard disk and install it again.

- If you cannot start the Armbian system from the USB hard disk again, connect the Amlogic s9xxx TV Boxes to the computer monitor. If the screen is completely black and there is nothing, you need to restore the Amlogic s9xxx TV Boxes to factory settings first, and then reinstall it. First download the [amlogic_usb_burning_tool](https://github.com/ophub/kernel/releases/tag/tools) system recovery tool and install it. Prepare a [USB dual male data cable](https://user-images.githubusercontent.com/68696949/159267576-74ad69a5-b6fc-489d-b1a6-0f8f8ff28634.png), Prepare a [paper clip](https://user-images.githubusercontent.com/68696949/159267790-38cf4681-6827-4cb6-86b2-19c7f1943342.png).

- Take x96max+ as an example. Find the two [short-circuit points](https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg) on the motherboard, Download the [Android TV firmware](https://github.com/ophub/kernel/releases/tag/tools). The Android TV system firmware of other common devices and the corresponding short circuit diagrams can also be [downloaded and viewed here](https://github.com/ophub/kernel/releases/tag/tools).

```
Operation method:

1. Open the USB Burning Tool:
   [ File → Import image ]: X96Max_Plus2_20191213-1457_ATV9_davietPDA_v1.5.img
   [ Check ]：Erase flash
   [ Check ]：Erase bootloader
   Click the [ Start ] button
2. Use a [ paper clip ] to connect the [ two shorting points ] on the main board of the box,
   and use a [ USB dual male data cable ] to connect the [ box ] to the [ computer ] at the same time.
3. Loosen the short contact after seeing the [ progress bar moving ].
4. After the [ progress bar is 100% ], the restoration of the original Android TV system is completed.
   Click [ stop ], unplug the [ USB male-to-male data cable ] and [ power ].
5. If the progress bar is interrupted, repeat the above steps until it succeeds.
   If the progress bar does not respond after the short-circuit, plug in the [ power ] supply after the short-circuit.
   Generally, there is no need to plug in the power supply.
```

When the factory reset is completed, the box has been restored to the Android TV system, and other operations to install the Armbian system are the same as the requirements when you installed the system for the first time before, just do it again.

### 12.4 Set the box to boot from USB/TF/SD

- Write the firmware to USB/TF/SD, insert it into the box after writing.
- Open the developer mode: Settings → About this machine → Version number (for example: X96max plus...), click on the version number for 5 times in quick succession, See the prompt of `Enable Developer Mode` displayed by the system.
- Turn on USB debugging: System → Advanced options → Developer options again (after entering, confirm that the status is on, and the `USB debugging` status in the list is also on). Enable `ADB` debugging.
- Install ADB tools: Download [adb](https://github.com/ophub/kernel/releases/tag/tools) and unzip it, copy the three files `adb.exe`, `AdbWinApi.dll`, and `AdbWinUsbApi.dll` to the two files `system32` and `syswow64` under the directory of `c://windows/` Folder, then open the `cmd` command panel, use `adb --version` command, if it is displayed, it is ready to use.
- Enter the `cmd` command mode. Enter the `adb connect 192.168.1.137` command (the ip is modified according to your box, and you can check it in the router device connected to the box), If the link is successful, it will display `connected to 192.168.1.137:5555`
- Enter the `adb shell reboot update` command, the box will restart and boot from the USB/TF/SD you inserted, access the firmware IP address from a browser, or SSH to enter the firmware.

### 12.5 Disable infrared receiver

Support for the infrared receiver is enabled by default but if you are using your TV box as a server then you may wish to disable the IR kernel module to prevent switching your TV box off by mistake. To completely disable IR, add the line:

```yaml
blacklist meson_ir
```

to `/etc/modprobe.d/blacklist.conf` and reboot.

### 12.6 Selection of bootstrap file

- Of the currently known devices, only a few devices such as `T95(s905x)` / `T95Z-Plus(s912)` / `BesTV-R3300L(s905l-b)` need to use the `/bootfs/extlinux/extlinux.conf` file, it has been added by default in the firmware. For other devices, if necessary, you can write the Armbian firmware to the USB, double-click to open the `boot` partition, and delete the `.bak` in the `/boot/extlinux/extlinux.conf.bak` file name that comes with the firmware to use it. `armbian-install` automatically checks when writing to eMMC and creates an `extlinux.conf` file if it exists.

- Other devices only need `/boot/uEnv.txt` to boot, do not modify the `extlinux.conf.bak` file.

### 12.7 Network settings

The content of the network configuration file [/etc/network/interfaces](../armbian-files/common-files/etc/network/interfaces) is as follows:

```yaml
source /etc/network/interfaces.d/*

# Network is managed by Network manager
# You can choose one of the following two IP setting methods:
# Use # to disable another setting method


# 01. Enable dynamic DHCP to assign IP
auto eth0
iface eth0 inet dhcp
        hwaddress ether 12:34:56:78:9A:BC


# 02. Enable static IP settings(IP is modified according to the actual)
#auto eth0
#allow-hotplug eth0
#iface eth0 inet static
#address 192.168.1.100
#netmask 255.255.255.0
#gateway 192.168.1.6
#dns-nameservers 192.168.1.6


# 03. Docker install OpenWrt and communicate with each other
#allow-hotplug eth0
#no-auto-down eth0
#auto eth0
#iface eth0 inet manual
#
#auto macvlan
#iface macvlan inet dhcp
#        hwaddress ether 12:34:56:78:9a:bc
#        pre-up ip link add macvlan link eth0 type macvlan mode bridge
#        post-down ip link del macvlan link eth0 type macvlan mode bridge
#
#auto lo
#iface lo inet loopback
```

By default, the DHCP dynamic IP allocation strategy (method 1) is used, and the IP is automatically allocated by the network router connected to Armbian. If you want to change to static IP, you can disable or delete the setting method 1, and enable the static IP setting of method 2.

#### 12.7.1 Dynamic IP address assignment by DHCP

```yaml
source /etc/network/interfaces.d/*

auto eth0
iface eth0 inet dhcp
```

#### 12.7.2 Manually set a static IP address

The IP, gateway and DNS are modified according to your own network conditions.

```yaml
source /etc/network/interfaces.d/*

auto eth0
allow-hotplug eth0
iface eth0 inet static
address 192.168.1.100
netmask 255.255.255.0
gateway 192.168.1.1
dns-nameservers 192.168.1.1
```

#### 12.7.3 Use OpenWrt in docker to establish interworking network

The MAC address in it can be modified according to your needs.

```yaml
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

### 12.8 How to add startup tasks

A custom startup task script file has been added to the system, and the path in the Armbian system is [/etc/custom_service/start_service.sh](../armbian-files/common-files/etc/custom_service/start_service.sh) file, you can customize and add related tasks in this script according to your personal needs.

### 12.9 How to update service scripts in the system

Use the `armbian-sync` command to update all service scripts on the local system to the latest version.


### 12.10 How to obtain Android partition info on eMMC

When writing the Armbian system onto eMMC where Android system resides, you need to confirm the Android system partition table of the device beforehand, to ensure that the data is written to a safe area, and try not to damage the Android system partition table, so as to avoid problems such as the system not being able to boot. If you write to an unsafe area, you will either not be able to start, or get an error similar to the following:

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/187075834-4ac40263-52ae-4538-a4b1-d6f0d5b9c856.png">
</div>

#### 12.10.1 To obain the partition info

If you're using Armbian released in this repo after Nov.2022, you can copy&paste the following command to get a URL that records the whole partition info (the device itself does not need to be online)

```
ampart /dev/mmcblk2 --mode webreport 2>/dev/null
```

*The webreport mode of ampart was added in the v1.2 version released on Feb 3rd 2022, if the above command outputs nothing, then the built-in ampart in your installation is probably older than this. You could use the following command instead:*

```
echo "https://7ji.github.io/ampart-web-reporter/?dsnapshot=$(ampart /dev/mmcblk2 --mode dsnapshot 2>/dev/null | head -n 1)&esnapshot=$(ampart /dev/mmcblk2 --mode esnapshot 2>/dev/null | head -n 1)"
```

The URL should look like this：

```
https://7ji.github.io/ampart-web-reporter/?esnapshot=bootloader:0:4194304:0%20reserved:37748736:67108864:0%20cache:113246208:754974720:2%20env:876609536:8388608:0%20logo:893386752:33554432:1%20recovery:935329792:33554432:1%20rsv:977272832:8388608:1%20tee:994050048:8388608:1%20crypt:1010827264:33554432:1%20misc:1052770304:33554432:1%20instaboot:1094713344:536870912:1%20boot:1639972864:33554432:1%20system:1681915904:1073741824:1%20params:2764046336:67108864:2%20bootfiles:2839543808:754974720:2%20data:3602907136:4131389440:4&dsnapshot=logo::33554432:1%20recovery::33554432:1%20rsv::8388608:1%20tee::8388608:1%20crypt::33554432:1%20misc::33554432:1%20instaboot::536870912:1%20boot::33554432:1%20system::1073741824:1%20cache::536870912:2%20params::67108864:2%20data::-1:4
```

Copy the URL to your browser to open it, and you will see well-formatted DTB partition info and eMMC partition info：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287642-e1b7be27-4d2c-4ac3-9fcc-15e06aebb97e.png">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287654-d1929e21-d2b3-4fb6-bcf0-c454c88e21b9.png">
</div>

#### 12.10.2 To share the partition info

When sharing the partition info to others (e.g. to post it to this repo to get a new device supported, or to get help from others), always prefer the URL itself than a screenshot of the opened webpage. If a long URL annoys you, you could always use some free URL shortener

 - On one hand, the partition infos on the webpage are actually generated on each access, so notes about whether some partitions could be written to and the format of the table could change
 - On the other hand, it's very inconvenient to get the number from a screenshot to do some calculations

Addtionally, you do not need to prepare/post a spreadsheet file, either. The layout on the webpage is designed speciially so any one could just copy&paste to Excel or LibreOffice Calc if they really need a spreadsheet.

#### 12.10.3 To understand the partition info

The DTB table represents the partition layout the **firmware** on the box wants, recorded in the Android DTB, usually ends with a auto-fill `data` partition, so the layout here could be same across all of the boxes that share the same firmware (therefore certainly the same model). The actual partition layout, though, will be different depending on the capacity of the eMMC, but it's always decided by the DTB partition layout (so you could always deduct the eMMC partition layout from a certain DTB partition layout and the capacity. *The above DTB partition info and eMMC partition info did not come from the same box, could you notice why?*)

The eMMC table represents the actual eMMC partition layout, in which in row represents an area, each of the area could be either a partition, or a gap between partitions (Amlogic's quirks again, they decided to leave at least a 8M gap between partitions for possible usage in the future, yet never use it, even on their newest S905X4). The lines that represent partitions have black text color and have values in their offset and masks columns; the lines that represent gaps have grey text color and have no values in their offset and masks columns.

The last column in each row of the eMMC table marks the writable status of the area. A green `yes` means you can safely write to it; a red `no` means you should never write to it; a yellow note means writable on some conditions, or only partially writable.

Take the above table for example, the `bootloader` partition's 0+4M (`0M~4M`) is not writable; the 32M gap (`4M~36M`) after it is writable; the `reserved` partition's `36M+64M` (`36M~100M`) is not writable; then all until the `env` partition is writable (`100M~836M`); the `env` partition's 1M afterwards is writable (`837M~end`). Then all possible writable areas on eMMC are:

- 4M~36M
- 100M~836M
- 837M~end

If Android logo is still needed, then `logo` partition's 852M+32M (852M~884M) is not writable. Then all possible writable areas on eMMC are：

- 4M~36M
- 100M~836M
- 837M~852M
- 884M~end

#### 12.10.4 To be used on eMMC installation

If eMMC installation on the device using `armbian-install` with `-a yes` fails (which will use [ampart](https://github.com/7Ji/ampart) to adjust the eMMC partition layout automatically), then the optimal eMMC partition layout could not be applied on your box (i.e. the DTB partitions will be reduced to auto-filling `data` only, then eMMC partition layout will be generated from this, then still existing partitions will be moved as to the beginning of eMMC as possible, so all area after 117M became writable), you will need to modify the partition info in [armbian-install](../armbian-files/common-files/usr/sbin/armbian-install).

There are 3 key arguments about the partition layout in the file: `BLANK1`, `BOOT`, `BLANK2`. In which, `BLANK1` stands for the area that can't be used from the beginning of the eMMC; `BOOT` stands for the area that could be used to store the kernel, dtbs, etc after `BLANK1`, better not smaller than 256M; `BLANK2` stands for the area that can't be used after `BOOT`; All areas after `BOOT` will be used to create a `ROOT` partition to store all of the data outside of `/boot`. All of the three arguments should be integer, and the unit isMiB (1 MiB = 1024 KiB = 1024^2 Byte)

Consider the above situation where we don't need a `logo` partition. We surely want to utilize all of the space, yet `4M~36M` is too small to be used for `BOOT` so we have to include it in the unusable `BLANK1`. The `100M~836M` then is well enough for `BOOT` so it could take all of it. The unusable `836M~837M` then goes to `BLANK2`. The arguments then should be like the following (an example for `s905x3`, adapt it to other SoCs when editting):

```shell
# Set partition size (Unit: MiB)
elif [[ "${AMLOGIC_SOC}" == "s905x3" ]]; then
    BLANK1="100"
    BOOT="736"
    BLANK2="1"
```

### 12.11 How to make u-boot file

The u-boot file is an important file to guide the system to start normally.

#### 12.11.1 Extract the bootloader and dtb files

Extraction requires the use of HxD software. You can download it from [Official website download link](https://mh-nexus.de/en/downloads.php?product=HxD20) or [Backup download link](https://github.com/ophub/kernel/releases/download/tools/HxDSetup.2.5.0.0.zip) to get the installation.

Execute the following commands in sequence in the `cmd` panel to extract the relevant files and download them to the local computer.

```shell
# use the adb tool to enter the box
adb connect 192.168.1.111
adb shell

# export bootloader command
dd if=/dev/block/bootloader of=/data/local/bootloader.bin

# export dtb command
cat /dev/dtb >/data/local/mybox.dtb

# export gpio command
cat /sys/kernel/debug/gpio >/data/local/mybox_gpio.txt

# Download the bootloader, dtb and gpio files to your local computer
adb pull /data/local/bootloader.bin C:\mybox
adb pull /data/local/mybox.dtb C:\mybox
adb pull /data/local/mybox_gpio.txt C:\mybox
```

#### 12.11.2 Make the acs.bin file

The most important part of the mainline u-boot is acs.bin, which is used to initialize the memory. The original u-boot is located in the first 4MB of the firmware. Extract the `acs.bin` file using the `bootloader.bin` file you just obtained.

Open HxD software, open the `bootloader.bin` file exported above, `right click - select range`, start position `F200`, length `1000`, select `hexadecimal`.

<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/187056711-1b58ce71-2a7d-4e9b-a976-e5f278edaa53.png">

Copy the selected result, then create a new file, paste insert, ignore warnings, save as acs.bin file.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056725-0a0e60af-6a21-4a6b-a2d5-f3d46b438a6a.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056827-8419c738-3428-473e-9a95-ab7270170d98.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056852-9f62f16a-f7f1-4c34-a2c2-78358d198f9a.png">
</div>

If the bootloader is locked, the code in this area is garbled and useless. Normally, there should be a lot of `0` as shown in the above picture, and `cfg` will appear several times in a row, and the words related to `ddr` will appear in the middle. This kind of normal code can be used.

#### 12.11.3 Make the u-boot file

To make u-boot, you need https://github.com/unifreq/amlogic-boot-fip and https://github.com/unifreq/u-boot two source libraries, and compile the two u-boot files of your own box .

In the amlogic-boot-fip source code, only the acs.bin file is different for each model, and other files can be used in common.

<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187057209-c4716384-46ef-4922-9710-8da7ae6db1e4.png">

For the method of making u-boot, please refer to the specific instructions in https://github.com/unifreq/u-boot/tree/master/doc/board/amlogic, and select the model of your own device to compile and test.

To make u-boot according to the method of [unifreq](https://github.com/unifreq), you need to use the acs.bin, dts and config files of the box. The dts exported from the Android system cannot be directly converted into the Armbian format, and you need to write a corresponding dts file yourself. According to the different parts of the specific hardware of your own equipment, such as switches, LEDs, power control, tf cards, sdio wifi modules, etc., use the similar [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) file in the kernel source library for modification and production.

Take the u-boot for X96Max Plus as an example:

```shell
~/make-uboot
    ├── amlogic-boot-fip
    │   ├── x96max-plus                                     # Create your own directory
    │   │   ├── asc.bin                                     # Make your own source files
    │   │   └── other-copy-files...                         # Copy files from other directories
    │   │
    │   ├── other-source-directories...
    │   └── other-source-files...
    │
    └── u-boot
        ├── configs
        │   └── x96max-plus_defconfig                       # Make your own source files
        ├── arch
        │   └── arm
        │       └── dts
        │           ├── meson-sm1-x96-max-plus-u-boot.dtsi  # Make your own source files
        │           ├── meson-sm1-x96-max-plus.dts          # Make your own source files
        │           └── Makefile                            # Edit
        ├── fip
        │   ├── u-boot.bin                                  # Generated file
        │   └── u-boot.bin.sd.bin                           # Generated file
        ├── u-boot.bin                                      # Generated file
        │
        ├── other-source-directories...
        └── other-source-files...
```

- Download the [amlogic-boot-fip](https://github.com/unifreq/amlogic-boot-fip) source code. Create the [x96max-plus](https://github.com/unifreq/amlogic-boot-fip/tree/master/x96max-plus) directory in the root directory. Except for the `asc.bin` file made by yourself, other files can be copied from other directories.
- Download [u-boot](https://github.com/unifreq/u-boot) source code. Make the corresponding [x96max-plus_defconfig](https://github.com/unifreq/u-boot/blob/master/configs/x96max-plus_defconfig) file and put it in the [configs](https://github.com/unifreq/u-boot/tree/master/configs) directory. Make the corresponding [meson-sm1-x96-max-plus-u-boot.dtsi](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus-u-boot.dtsi) and [meson-sm1-x96-max-plus.dts](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus.dts) file and put it in the [arch/arm/dts](https://github.com/unifreq/u-boot/tree/master/arch/arm/dts) directory, and edit the [Makefile](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/Makefile) file in this directory to add the index of the `meson-sm1-x96-max-plus.dtb` file.
- Go to the root directory of the u-boot source code directory and follow the steps in the document https://github.com/unifreq/u-boot/blob/master/doc/board/amlogic/x96max-plus.rst

There are two types of final generated files: the `u-boot.bin` file in the u-boot root directory is the incomplete version of u-boot used in the `/boot` directory (corresponding to [overload](../amlogic-u-boot/overload) directory); `u-boot.bin` and `u-boot.bin.sd.bin` in the `fip` directory are the full version u-boot files used in the `/usr/lib/u-boot/` directory (corresponding to [bootloader](../amlogic-u-boot/bootloader/) directory), the difference between the two files of the full version is 512 bytes, and the larger one is filled with 512 bytes of 0 in front.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189039426-c127631f-77ca-4fcb-9fb6-4220045d712b.png">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189029320-e43a4cc9-b4b5-4de4-92fe-b17bd29020d0.png">
</div>

💡Tip: Before writing to eMMC for testing, please check the Brick Rescue Method in 12.3. Be sure to master the position of the short contact, have the original Android system file in .img format, and perform a short-circuit flash test to ensure that the brick-rescue method has been mastered before writing the test.

### 12.12 Memory size recognition error

If the memory size is incorrectly recognized (1-2G is not normal for 4G memory, and 3.7G is normal), you can try to manually copy a `/boot/UBOOT_OVERLOAD` file (note that it is a `copy`, `not renaming`, after renaming It will not boot after install and update operations), save as `/boot/u-boot.ext` when using in `USB`, and save as `/boot/u-boot.emmc` when using in `eMMC`.

Don't copy the u-boot file manually, except to try to solve the memory problem, adding it incorrectly will result in failure to boot and various problems.

### 12.13 How to decompile a dtb file

Some new devices are not in the current support list (or have variants), you can try by decompiling and adjusting related parameters.

```shell
# Install dependencies
sudo apt-get update
sudo apt-get install -y device-tree-compiler

# 1. Decompile command (use dtb file to generate dts source code)
dtc -I dtb -O dts -o xxx.dts xxx.dtb

# 2. Compile command (use dts to compile to generate dtb file)
dtc -I dts -O dtb -o xxx.dtb xxx.dts
```

### 12.14 How to modify cmdline settings

In Amlogic devices, settings such as adding/modifying/deleting can be done in the `/boot/uEnv.txt` file. On Rockchip devices this is set in the `/boot/armbianEnv.txt` file. A reboot is required after each change to take effect.

For example, the `Home Assistant Supervisor` application only supports the `docker cgroup v1` version, and currently docker installs the latest v2 version by default. If you need to switch to v1 version, you can add `systemd.unified_cgroup_hierarchy=0` parameter setting in cmdline, and you can switch to `docker cgroup v1` version after restarting.

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://user-images.githubusercontent.com/68696949/216220941-47db0183-7b26-4768-81cf-2ee73d59d23e.png">
</div>

### 12.15 How to add new supported devices

To build an Armbian system for a device, you need to use four parts: `device configuration file`, `system files`, `u-boot files`, and `process control files`. The specific adding methods are as follows:

#### 12.15.1 Add device profile

In the configuration file [/etc/model_database.conf](../armbian-files/common-files/etc/model_database.conf), add the corresponding configuration information according to the test support of the device. Where the value of `BUILD` is `yes`, it is part of the devices built by default, and the corresponding `BOARD` value `must be unique`, these boxes can directly use the Armbian system built by default.

The default value is `no` and there is no packaging. When using these devices, you need to download the same `FAMILY` Armbian system. After writing to `USB`, you can open the `boot partition in USB` on the computer and modify `FDT dtb name` in `/boot/uEnv.txt` file, other devices in the adaptation list.

#### 12.15.2 Add system files

The common files are placed in the directory `build-armbian/armbian-files/common-files`, common to all platforms.

The platform files are placed in the `build-armbian/armbian-files/platform-files/<platform>` directory, [Amlogic](../armbian-files/platform-files/amlogic), [Rockchip](../ armbian-files/platform-files/rockchip) and [Allwinner](../armbian-files/platform-files/allwinner) respectively share the files of their respective platforms, where the `bootfs` directory is the file of the /boot partition, The `rootfs` directory contains the Armbian system files.

If individual devices have special differential setting requirements, add an independent directory named `BOARD` under the `build-armbian/armbian-files/different-files` directory, and create the `bootfs` directory to add the relevant files under the system `/boot` partition as needed, Create a `rootfs` directory to add system files as needed. The name of each folder is based on the actual path in the `Armian` system. It is used to add new files or overwrite files with the same name added from the general files and platform files.

#### 12.15.3 Add u-boot files

`Amlogic` series devices share the [bootloader](../u-boot/amlogic/bootloader/) files and [u-boot](../u-boot/amlogic/overload) files. If there is a new file, respectively into the corresponding directory. The `bootloader` files will be automatically added to the `/usr/lib/u-boot` directory of the Armbian system when the system is built, and the `u-boot` files will be automatically added to the `/boot` directory.

For `Rockchip` and `Allwinner` series devices, add an independent [u-boot](../u-boot) file directory named after `BOARD` for each device. The corresponding series of files are placed in this directory.

When building an Armenian image, these u-boot files will be written into the corresponding Armbian image file by the rebuild script according to the configuration in [/etc/model_database.conf](../armbian-files/common-files/etc/model_database.conf).

#### 12.15.4 Add process control files

Add the corresponding `BOARD` option to `armbian_board` in [yml workflow control file](../../.github/workflows/build-armbian.yml), which supports the use in `Actions` of github.com.

### 12.16 12.16 How to fix I/O errors when writing to eMMC

Some devices can start Armbian normally from USB/SD/TF, but will report I/O error when writing to eMMC, such as [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues/989), the error content is as follows:

```shell
[  284.338449] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.341544] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.446972] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.450074] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.497746] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.500871] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
```

In this case, you can adjust the working mode speed and frequency of the dtb used to stabilize the read and write support for storage. When using sdr mode, the frequency is 2 times the speed, and when using ddr mode, the frequency is equal to the speed. as follows:

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

Generally, the problem can be solved by reducing the frequency of `&sd_emmc_c` from `max-frequency = <200000000>;` to `max-frequency = <100000000>;`. If it doesn’t work, you can continue to lower it to `50000000` for testing, and adjust `&sd_emmc_b` to set `USB/SD/TF`, or use `sd-uhs-sdr` to limit the speed. You can get the test file by modifying the dts file and [compiling](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel) it, or you can decompile and modify the existing dtb file to generate the test file by the method introduced in `Chapter 12.13`. The decompiled dtb file is modified using hexadecimal values, where the hexadecimal value of `200000000` in decimal is `0xbebc200`, the hexadecimal value of `100000000` in decimal is `0x5f5e100`, the hexadecimal value of `50000000` in decimal is `0x2faf080`, and the hexadecimal value of `25000000` in decimal is `0x17d7840`.

In addition to solving problems through the system software layer, you can also use [money ability](https://github.com/ophub/amlogic-s9xxx-armbian/issues/998) and [hands-on ability](https://www.right.com.cn/forum/thread-901586-1-1.html) to solve.

### 12.17 How to fix the Bullseye version with no sound

Please refer to the method in [Bullseye NO Sound](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1000) to set.

