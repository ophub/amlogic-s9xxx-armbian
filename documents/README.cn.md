# Armbian 构建及使用方法

查看英文说明 | [View English description](README.md)

Github Actions 是 Microsoft 推出的一项服务，它提供了性能配置非常不错的虚拟服务器环境，基于它可以进行构建、测试、打包、部署项目。对于公开仓库可免费无时间限制地使用，且单次编译时间长达 6 个小时，这对于编译 Armbian 来说是够用的（我们一般在3小时左右可以完成一次编译工作）。分享只是为了交流经验，不足的地方请大家理解，请不要在网络上发起各种不好的攻击行为，也不要恶意使用 GitHub Actions。

# 目录

- [Armbian 构建及使用方法](#armbian-构建及使用方法)
- [目录](#目录)
  - [1. 注册自己的 Github 的账户](#1-注册自己的-github-的账户)
  - [2. 设置隐私变量 GITHUB\_TOKEN](#2-设置隐私变量-github_token)
  - [3. Fork 仓库并设置工作流权限](#3-fork-仓库并设置工作流权限)
  - [4. 个性化 Armbian 系统定制文件说明](#4-个性化-armbian-系统定制文件说明)
  - [5. 编译系统](#5-编译系统)
    - [5.1 手动编译](#51-手动编译)
    - [5.2 定时编译](#52-定时编译)
    - [5.3 自定义默认系统配置](#53-自定义默认系统配置)
    - [5.4 使用逻辑卷扩大 Github Actions 编译空间](#54-使用逻辑卷扩大-github-actions-编译空间)
    - [5.5 制作 Armbian Docker 镜像](#55-制作-armbian-docker-镜像)
  - [6. 保存系统](#6-保存系统)
  - [7. 下载系统](#7-下载系统)
  - [8. 安装 Armbian 到 EMMC](#8-安装-armbian-到-emmc)
    - [8.1 Amlogic 系列安装方法](#81-amlogic-系列安装方法)
    - [8.2 Rockchip 系列安装方法](#82-rockchip-系列安装方法)
      - [8.2.1 Radxa-Rock5B 的安装方法](#821-radxa-rock5b-的安装方法)
        - [8.2.1.1 安装系统至 microSD](#8211-安装系统至-microsd)
        - [8.2.1.2 安装系统至 eMMC](#8212-安装系统至-emmc)
        - [8.2.1.3 安装系统至 NVMe](#8213-安装系统至-nvme)
      - [8.2.2 电犀牛 R66S 的安装方法](#822-电犀牛-r66s-的安装方法)
      - [8.2.3 电犀牛 R68S 的安装方法](#823-电犀牛-r68s-的安装方法)
      - [8.2.4 贝壳云的安装方法](#824-贝壳云的安装方法)
      - [8.2.5 我家云的安装方法](#825-我家云的安装方法)
      - [8.2.6 泰山派的安装方法](#826-泰山派的安装方法)
    - [8.3 Allwinner 系列安装方法](#83-allwinner-系列安装方法)
  - [9. 编译 Armbian 内核](#9-编译-armbian-内核)
    - [9.1 如何添加自定义内核补丁](#91-如何添加自定义内核补丁)
    - [9.2 如何制作内核补丁](#92-如何制作内核补丁)
    - [9.3 如何自定义编译驱动模块](#93-如何自定义编译驱动模块)
  - [10. 更新 Armbian 内核](#10-更新-armbian-内核)
  - [11. 安装常用软件](#11-安装常用软件)
  - [12. 常见问题](#12-常见问题)
    - [12.1 每个盒子的 dtb 和 u-boot 对应关系表](#121-每个盒子的-dtb-和-u-boot-对应关系表)
    - [12.2 LED 屏显示控制说明](#122-led-屏显示控制说明)
    - [12.3 如何恢复原安卓 TV 系统](#123-如何恢复原安卓-tv-系统)
      - [12.3.1 使用 armbian-ddbr 备份恢复](#1231-使用-armbian-ddbr-备份恢复)
      - [12.3.2 使用 Amlogic 刷机工具恢复](#1232-使用-amlogic-刷机工具恢复)
    - [12.4 设置盒子从 USB/TF/SD 中启动](#124-设置盒子从-usbtfsd-中启动)
      - [12.4.1 初次安装 Armbian 系统](#1241-初次安装-armbian-系统)
      - [12.4.2 重新安装 Armbian 系统](#1242-重新安装-armbian-系统)
    - [12.5 禁用红外接收器](#125-禁用红外接收器)
    - [12.6 启动引导文件的选择](#126-启动引导文件的选择)
    - [12.7 网络设置](#127-网络设置)
      - [12.7.1 使用 interfaces 设置网络](#1271-使用-interfaces-设置网络)
        - [12.7.1.1 由 DHCP 动态分配 IP 地址](#12711-由-dhcp-动态分配-ip-地址)
        - [12.7.1.2 手动设置静态 IP 地址](#12712-手动设置静态-ip-地址)
        - [12.7.1.3 在 docker 中使用 OpenWrt 建立互通网络](#12713-在-docker-中使用-openwrt-建立互通网络)
      - [12.7.2 使用 NetworkManager 管理网络](#1272-使用-networkmanager-管理网络)
        - [12.7.2.1 新建网络连接](#12721-新建网络连接)
          - [12.7.2.1.1 获取网络接口名称](#127211-获取网络接口名称)
          - [12.7.2.1.2 获取现有网络连接名称](#127212-获取现有网络连接名称)
          - [12.7.2.1.3 新建有线网络连接](#127213-新建有线网络连接)
          - [12.7.2.1.4 新建无线网络连接](#127214-新建无线网络连接)
        - [12.7.2.2 修改无线网络连接中的 WiFi SSID or PASSWD](#12722-修改无线网络连接中的-wifi-ssid-or-passwd)
        - [12.7.2.3 修改网络地址分配方式](#12723-修改网络地址分配方式)
          - [12.7.2.3.1 静态 IP 地址 - IPv4](#127231-静态-ip-地址---ipv4)
          - [12.7.2.3.2 DHCP 获取动态 IP 地址 - IPv4 / IPv6](#127232-dhcp-获取动态-ip-地址---ipv4--ipv6)
        - [12.7.2.4 修改网络连接 MAC 地址](#12724-修改网络连接-mac-地址)
        - [12.7.2.5 如何禁用 IPv6](#12725-如何禁用-ipv6)
      - [12.7.3 如何启用无线](#1273-如何启用无线)
      - [12.7.4 如何启用蓝牙](#1274-如何启用蓝牙)
    - [12.8 如何添加开机启动任务](#128-如何添加开机启动任务)
    - [12.9 如何更新系统中的服务脚本](#129-如何更新系统中的服务脚本)
    - [12.10 如何获取 eMMC 上的安卓系统分区信息](#1210-如何获取-emmc-上的安卓系统分区信息)
      - [12.10.1 获取分区信息](#12101-获取分区信息)
      - [12.10.2 分区信息分享](#12102-分区信息分享)
      - [12.10.3 分区信息解读](#12103-分区信息解读)
      - [12.10.4 用于 eMMC 安装](#12104-用于-emmc-安装)
    - [12.11 如何制作 u-boot 文件](#1211-如何制作-u-boot-文件)
      - [12.11.1 如何制作 Amlogic 设备的 u-boot 文件](#12111-如何制作-amlogic-设备的-u-boot-文件)
        - [12.11.1.1 如何提取 bootloader 和 dtb 文件](#121111-如何提取-bootloader-和-dtb-文件)
        - [12.11.1.2 如何制作 acs.bin 文件](#121112-如何制作-acsbin-文件)
        - [12.11.1.3 如何编译 u-boot 文件](#121113-如何编译-u-boot-文件)
      - [12.11.2 如何制作 Rockchip 设备的 u-boot 文件](#12112-如何制作-rockchip-设备的-u-boot-文件)
        - [12.11.2.1 如何使用 Radxa 的 u-boot 制作脚本](#121121-如何使用-radxa-的-u-boot-制作脚本)
        - [12.11.2.2 如何使用 cm9vdA 的 u-boot 制作脚本](#121122-如何使用-cm9vda-的-u-boot-制作脚本)
    - [12.12 内存大小识别错误](#1212-内存大小识别错误)
    - [12.13 如何反编译 dtb 文件](#1213-如何反编译-dtb-文件)
    - [12.14 如何修改 cmdline 设置](#1214-如何修改-cmdline-设置)
    - [12.15 如何添加新的支持设备](#1215-如何添加新的支持设备)
      - [12.15.1 添加设备配置文件](#12151-添加设备配置文件)
      - [12.15.2 添加系统文件](#12152-添加系统文件)
      - [12.15.3 添加 u-boot 文件](#12153-添加-u-boot-文件)
      - [12.15.4 添加流程控制文件](#12154-添加流程控制文件)
    - [12.16 如何解决写入 eMMC 时 I/O 错误的问题](#1216-如何解决写入-emmc-时-io-错误的问题)
    - [12.17 如何解决 Bullseye 版本没有声音的问题](#1217-如何解决-bullseye-版本没有声音的问题)
    - [12.18 如何编译 boot.scr 文件](#1218-如何编译-bootscr-文件)
    - [12.19 如何开启远程桌面和修改默认端口](#1219-如何开启远程桌面和修改默认端口)

## 1. 注册自己的 Github 的账户

注册自己的账户，以便进行系统个性化定制的继续操作。点击 github.com 网站右上角的 `Sign up` 按钮，根据提示注册自己的账户。

## 2. 设置隐私变量 GITHUB_TOKEN

根据 [GitHub 文档](https://docs.github.com/zh/actions/security-guides/automatic-token-authentication#about-the-github_token-secret)，在每个工作流作业开始时，GitHub 会自动创建唯一的 GITHUB_TOKEN 机密以在工作流中使用。可以使用 `${{ secrets.GITHUB_TOKEN }}` 在工作流作业中进行身份验证。

## 3. Fork 仓库并设置工作流权限

现在可以 Fork 仓库了，打开仓库 https://github.com/ophub/amlogic-s9xxx-armbian ，点击右上的 Fork 按钮，复制一份仓库代码到自己的账户下，稍等几秒钟，提示 Fork 完成后，到自己的账户下访问自己仓库里的 amlogic-s9xxx-armbian 。在右上角的 `Settings` > `Actions` > `General` > `Workflow permissions` 下选择 `Read and write permissions` 并保存。图示如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/167585338-841d3b05-8d98-4d73-ba72-475aad4a95a9.png width="300" />
</div>

## 4. 个性化 Armbian 系统定制文件说明

系统编译的流程在 [.github/workflows/build-armbian.yml](../.github/workflows/build-armbian.yml) 文件里控制，在 workflows 目录下还有其他 .yml 文件，实现其他不同的功能。编译系统时采用了 Armbian 官方的当前代码进行实时编译，相关参数可以查阅官方文档。

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

## 5. 编译系统

系统编译的方式很多，可以设置定时编译，手动编译，或者设置一些特定事件来触发编译。我们先从简单的操作开始。

### 5.1 手动编译

在自己仓库的导航栏中，点击 Actions 按钮，再依次点击 Build armbian > Run workflow > Run workflow ，开始编译，等待大约 3 个小时，全部流程都结束后就完成编译了。图示如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163203938-e7762b09-e6b8-4cf5-b1f1-9c67c1a29953.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204044-9c7a7429-47ee-4fce-b7dd-e217bebf6133.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204127-6b16b558-7e78-4c22-a28a-7b37b5a34fa3.png width="300" />
</div>

### 5.2 定时编译

在 [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) 文件里，使用 Cron 设置定时编译，5 个不同位置分别代表的意思为 分钟 (0 - 59) / 小时 (0 - 23) / 日期 (1 - 31) / 月份 (1 - 12) / 星期几 (0 - 6)(星期日 - 星期六)。通过修改不同位置的数值来设定时间。系统默认使用 UTC 标准时间，请根据你所在国家时区的不同进行换算。

```yaml
schedule:
  - cron: '0 17 * * *'
```

### 5.3 自定义默认系统配置

默认系统的配置信息记录在 [model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) 文件里，其中的 `BOARD` 名字要求唯一。

其中 `BUILD` 的值是 `yes` 的是默认打包的部分盒子的系统，这些盒子可以直接使用。默认值是 `no` 的没有打包，这些没有打包的盒子使用时需要下载相同 `FAMILY` 的打包好的系统（推荐下载 `5.15/5.4` 内核的系统），在写入 `USB` 后，可以在电脑上打开 `USB 中的 boot 分区`，修改 `/boot/uEnv.txt` 文件中 `FDT 的 dtb 名称`，适配列表中的其他盒子。

在本地编译时通过 `-b` 参数指定，在 github.com 的 Actions 里编译时通过 `armbian_board` 参数指定。使用 `-b all` 代表打包 `BUILD` 是 `yes` 的全部设备。使用指定 `BOARD` 参数打包时，无论 `BUILD` 是 `yes` 或者 `no` 均可打包，例如：`-b r68s_s905x3-tx3_s905l3a-cm311`

### 5.4 使用逻辑卷扩大 Github Actions 编译空间

Github Actions 编译空间默认是 84G，除去系统和必要软件包外，可用空间在 50G 左右，当编译全部固件时会遇到空间不足的问题，可以使用逻辑卷扩大编译空间至 110G 左右。参考 [.github/workflows/build-armbian.yml](../.github/workflows/build-armbian.yml) 文件里的方法，使用下面的命令创建逻辑卷。并在编译时使用逻辑卷的路径。

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

### 5.5 制作 Armbian Docker 镜像

Armbian 系统 [Docker](https://hub.docker.com/u/ophub) 镜像的制作方法可以参考 [armbian_docker](../compile-kernel/tools/script/docker) 制作脚本。

## 6. 保存系统

系统保存的设置也在 [.github/workflows/build-armbian.yml](../../.github/workflows/build-armbian.yml) 文件里控制。我们将编译好的系统通过脚本自动上传到 github 官方提供的 Releases 里面。

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

## 7. 下载系统

从仓库首页右下角的 Release 版块进入，选择和自己盒子型号对应的系统。图示如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163204798-0d98524c-73df-4876-8912-fcae2845fbba.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204879-4898babf-fa00-4e63-89ea-235129e2ce1d.png width="300" />
</div>

## 8. 安装 Armbian 到 EMMC

Amlogic, Rockchip 和 Allwinner 的安装方法不同。不同的设备具有不同的存储，有的设备使用外置 microSD 卡，有的带有 eMMC，有的支持使用 NVMe 等多种存储介质，根据设备不同，分别介绍其安装方法。首先在 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 里下载自己设备的 Armbian 系统，解压缩成 .img 格式备用。根据自己的设备，使用下面小结中不同的安装方法。

当安装完成后，将 Armbian 设备接入`路由器`，设备开机`2分钟`后，到路由器里查看设备名称为 Armbian 的 `IP`，使用 `SSH` 工具连接进行管理设置。默认用户名为 `root`，默认密码为 `1234`，默认端口为 `22`

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972715-addcd695-970a-43d6-8a34-24a9c4bc80a2.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972773-fc9e9ef9-69a7-4279-8329-6fad1cf2f5b9.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202972818-b72e18cd-17d1-4f9f-a0fa-b6c22eef041d.png width="600" />
</div>

### 8.1 Amlogic 系列安装方法

登录 Armbian 系统 (默认用户: root, 默认密码: 1234) → 输入命令：

```shell
armbian-install
```

| 可选参数  | 默认值   | 选项     | 说明                |
| -------  | ------- | ------  | -----------------   |
| -m       | no      | yes/no  | 使用 Mainline u-boot |
| -a       | yes     | yes/no  | 使用 [ampart](https://github.com/7Ji/ampart) 分区表调整工具 |
| -l       | no      | yes/no  | List. 显示全部设备列表 |

举例: `armbian-install -m yes -a no`

### 8.2 Rockchip 系列安装方法

每个设备的安装方法不同，分别介绍如下。

#### 8.2.1 Radxa-Rock5B 的安装方法

Radxa-Rock5B 有 microSD/eMMC/NVMe 等多种存储介质可以选择，相应的安装方法也不同。下载 [rk3588_spl_loader_v1.08.111.bin 和 spi_image.img](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/rock5b) 文件备用。下载 [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/Radxa_rock5b_RKDevTool_Release_v2.96__DriverAssitant_v5.1.1.tar.gz) 工具及驱动备用。下载 [Rufus](https://rufus.ie/) 或者 [balenaEtcher](https://www.balena.io/etcher/) 写盘工具备用。

##### 8.2.1.1 安装系统至 microSD

使用 Rufus 或者 balenaEtcher 等工具将 Armbian 系统镜像写入 microSD 里，然后把写好系统的 microSD 插入设备即可使用。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972996-300f223b-f6f6-48af-86ca-bdc842e5017d.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202973216-85b1a21b-0763-4a36-8c57-84490e071fdd.png width="600" />
</div>

##### 8.2.1.2 安装系统至 eMMC

使用 microSD 卡安装：将 Armbian 系统镜像写入 microSD 卡，将 microSD 卡插入设备并启动，上传 `armbian.img` 镜像文件到 microSD 卡，使用 `dd` 命令将 Armbian 镜像写入 NVMe 中，命令如下：

```Shell
dd if=armbian.img  of=/dev/mmcblk1  bs=1M status=progress
```

- 使用 USB 转 eMMC 读卡器安装：将 eMMC 模块与电脑连接，使用 Rufus 或者 balenaEtcher 等工具将 Armbian 系统镜像写入 eMMC 里，然后把写好系统的 eMMC 插入设备即可使用。
- 使用 Maskrom 模式安装：关闭开发板电源。按住金色按钮。将 USB-A 转 C 型电缆插入 ROCK 5B C 型端口，另一端插入 PC。松开金色按钮。检查 USB 设备提示找到一个 MASKROM 设备。右键单击列表的空白区域，然后选择加载 `rock-5b-emmc.cfg` 配置文件（配置文件和 RKDevTool 在同一个目录下）。将 `rk3588_spl_loader_v1.08.111.bin` 和 `Armbian.img` 按下图所示设置，选择写入即可。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954901-c829d74d-c75a-4fd3-9bd0-aa3cdf2b77b4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954998-c08514e2-8760-4c0f-b5f7-0d30be635aa5.png width="600" />
</div>

##### 8.2.1.3 安装系统至 NVMe

ROCK-5B 在主板上有一个 SPI 闪存，将引导加载程序安装到 SPI 闪存可以支持 SoC maskrom 模式不直接支持的其他启动介质（如 SATA、USB3 或 NVMe）。使用 NVMe 需要先写入 SPI 文件。方法如下：

关闭开发板电源。删除可启动设备，如MicroSD卡，eMMC模块等。按住金色（或某些开发板修订版上的银色）按钮。将 USB-A 转 C 型电缆插入 ROCK-5B C 型端口，另一端插入 PC。松开金色按钮。检查 USB 设备找到一个 MASKROM 设备。在列表框中右键选择加载配置，然后在资源管理文件夹中选择配置文件（配置文件和 RKDevTool 在同一个目录下），根据下图选择 `rk3588_spl_loader_v1.08.111.bin` 和 `spi_image.img` 文件，点击写入即可，如下图所示：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961420-8316c96c-2796-43ed-b5ed-2fa5bfa1ddff.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961447-49c0941a-e233-4b2a-b96b-b47636ce3cf2.png width="600" />
</div>

- 使用读卡器安装：将 M.2 NVMe SSD 插入 M.2 NVMe SSD 到 USB3.0 读卡器，以连接到主机。使用 Rufus 或者 balenaEtcher 等工具将 Armbian 系统镜像写入 NVMe 里，然后把写好系统的 NVMe 插入设备即可使用。
- 使用 microSD 卡安装：将 Armbian 系统镜像写入 microSD 卡，将 microSD 卡插入设备并启动，上传 `armbian.img` 镜像文件到 microSD 卡，使用 `dd` 命令将 Armbian 镜像写入 NVMe 中，命令如下：

```Shell
dd if=armbian.img  of=/dev/nvme0n1  bs=1M status=progress
```

#### 8.2.2 电犀牛 R66S 的安装方法

使用 Rufus 或者 balenaEtcher 等工具将 Armbian 系统镜像写入 microSD 里，然后把写好系统的 microSD 插入设备即可使用。

#### 8.2.3 电犀牛 R68S 的安装方法

- 下载 [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) 工具及驱动，解压并安装 DriverAssitant 驱动程序，打开 RKDevTool 工具备用。
- R68s 在关机状态下，先插入 USB 双公头线，然后按住 Recovery 键并插上 12V 电源，两秒之后松开 Recovery 键，刷机工具会`发现一个 LOADER 设备`。
- 在 RKDevTool 工具操作界面的空白处点右键，添加项。
- 地址是 `0x00000000`，名字是 `armbian`，路径点击右侧选择 `armbian.img` 系统文件。
- 选择添加的 armbian 一行外，`取消其他行的选择`，点击`执行`写入即可。
- 补充说明：如果 eMMC 中写入了其他系统，请先在高级功能里擦除，再写入 Armbian 系统。如果无法擦除，先重新写入一次 `MiniLoaderAll.bin` 引导文件，然后再次进入 `MASKROM` 写入 Armbian 系统。MiniLoaderAll.bin 引导文件设置：地址 `0xCCCCCCCC`, 名字 `Loader`, 路径选择 RKDevTool 刷机工具 Image 目录下的 `MiniLoaderAll.bin` 文件。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202970301-d798677b-e875-4971-ac8f-ee58b2a1e686.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970405-cb68cb78-cd0f-43ee-b807-5e594ab73099.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970488-5f18c574-c11f-486f-8fe8-002f3ba2f3f4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202970577-87549acf-b98b-441f-bb31-e4fd6608108d.png width="600" />
</div>

#### 8.2.4 贝壳云的安装方法

方法转载自 [milton](https://www.cnblogs.com/milton/p/15391525.html) 的教程。刷机需要进入 Maskrom 模式。先断开所有连接，通过短接 CLK 和 GND（使用 TTL 的 GND, 或者旁边小按钮的 GND 均可）这两个触点，然后将 USB 连接到 PC 就会检测到 MASKROM 设备了。短接点位置如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202977817-fb12d291-47e2-47e4-88c3-e21f9ae57922.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202977900-50b4770d-8444-42a0-8478-3234043455bd.png width="600" />
</div>

打开 RKDevTool 刷机工具，右键添加项。

- 地址 `0xCCCCCCCC`, 名字 `Boot`, 路径[选择](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/beikeyun) `rk3328_loader_v1.14.249.bin`。
- 地址 `0x00000000`, 名字 `system`, 路径选择要刷的 `Armbian.img` 系统。
- 勾选`强制按地址写入`，点`执行`，等右侧下载面板显示进度完成即可。

#### 8.2.5 我家云的安装方法

方法转载自 [cc747](https://post.smzdm.com/p/a4wkdo7l/) 的教程。刷机需要进入 Maskrom 模式。使我家云处于断电状态，拔掉所有线。用 USB 双公头线，一头插入我家云的 USB2.0 接口，一头插入电脑。用回形针插进 Reset 孔，并按压住不松开。插入电源线。等待几秒钟，直到 RKDevTool 框的下方出现`发现一个LOADER设备`后才松开回形针。将 RKDevTool 切换到`高级功能`点击`进入Maskrom`按钮，提示`发现一个MASKROM设备`。右键添加项。

- 地址 `0xCCCCCCCC`, 名字 `Boot`, 路径[选择](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/chainedbox) `rk3328_loader_v1.14.249.bin`。
- 地址 `0x00000000`, 名字 `system`, 路径选择要刷的 `Armbian.img` 系统。
- 勾选`强制按地址写入`，点`执行`，等右侧下载面板显示进度完成即可。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a6d2d8c0-35c5-44ba-be35-fd2e2758729b width="600" /><br />
<img src=https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/13aab016-1b93-4ff1-b1ef-c202bd357068 width="600" />
</div>

#### 8.2.6 泰山派的安装方法
- 下载 [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/FastRhino_r68s_RKDevTool_Release_v2.86___DriverAssitant_v5.1.1.tar.gz) 工具及驱动，解压并安装 DriverAssitant 驱动程序，打开 RKDevTool 工具。(注意，请使用2.86版本工具而不是2.92，2.92版本刷入时会闪退)
- 泰山派关机状态下按住 Recovery 键后插入type-c数据线，待 RKDevTool 提示`发现一个 LOADER 设备`后松开 Recovery 键。右键添加项。
- 地址 `0x00000000`, 名字 `system`, 路径选择要刷的 `Armbian.img` 系统。
- 点击执行，等待进度条完成即可


### 8.3 Allwinner 系列安装方法

登录 Armbian 系统 (默认用户: root, 默认密码: 1234) → 输入命令：

```shell
armbian-install
```

## 9. 编译 Armbian 内核

支持在 Ubuntu20.04/22.04，debian11 或 Armbian 系统中编译内核。支持本地编译，也支持使用 GitHub Actions 云编译，具体方法详见 [内核编译说明](../../compile-kernel/README.cn.md)。

### 9.1 如何添加自定义内核补丁

当内核补丁目录 [tools/patch](../../compile-kernel/tools/patch) 中有通用内核补丁目录（`common-kernel-patches`），或者有 `与内核源码库同名` 的目录时（例如 [linux-5.15.y](https://github.com/unifreq/linux-5.15.y)），可以使用 `-p true` 自动应用内核补丁。补丁目录的命名规范如下：

```shell
~/amlogic-s9xxx-armbian
    └── compile-kernel
        └── tools
            └── patch
                ├── common-kernel-patches  # 固定目录名：存放各版本都通用的内核补丁
                ├── linux-5.15.y           # 与内核源码库同名：存放专用补丁
                ├── linux-6.1.y
                ├── linux-5.10.y-rk35xx
                └── more kernel directory...
```

- 在本地编译内核时，可以手动创建相应目录，添加对应的自定义内核补丁。
- 在 GitHub Actions 云编译时，可以使用 `kernel_patch` 参数指定内核补丁在你仓库中的目录，例如 [kernel](https://github.com/ophub/kernel) 仓库中 [compile-beta-kernel.yml](https://github.com/ophub/kernel/blob/main/.github/workflows/compile-beta-kernel.yml) 的使用方法：

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

当使用 `kernel_patch` 参数指定自定义内核补丁时，补丁目录请参照上述规范进行命名。

### 9.2 如何制作内核补丁

- 从 [Armbian](https://github.com/armbian/build) 和 [OpenWrt](https://github.com/openwrt/openwrt) 等仓库中获得：例如 [armbian/patch/kernel](https://github.com/armbian/build/tree/main/patch/kernel/archive) 和 [openwrt/rockchip/patches-6.1](https://github.com/openwrt/openwrt/tree/main/target/linux/rockchip/patches-6.1)，[lede/rockchip/patches-5.15](https://github.com/coolsnowwolf/lede/tree/master/target/linux/rockchip/patches-5.15) 等等，这些使用主线内核的仓库中的补丁一般可以直接使用。
- 从 github.com 仓库的 commits 中获得：在相应的 `commit` 地址后添加 `.patch` 后缀即可生成对应的补丁。

在添加自定义内核补丁前，需要先和上游的内核源码仓库 [unifreq/linux-k.x.y](https://github.com/unifreq) 进行比较，确认此补丁是否已经添加，避免造成冲突。通过测试的内核补丁，建议向 unifreq 大佬维护的系列内核仓库进行提交。每人一小步，世界一大步，大家的贡献会让我们在盒子里使用 Armbian 和 OpenWrt 系统时更加稳定和有趣。

### 9.3 如何自定义编译驱动模块

在 linux 主线内核里，有些驱动尚未支持，可以自定义编译驱动模块。请选择支持在主线内核里使用的驱动，安卓驱动一般不支持主线内核，无法编译。举例如下：

```shell
# 第一步，更新最新内核
# 由于早期的 header 文件不全，所以需要更新到最新的内核。
# 各内核版本要求不低于 5.4.280, 5.10.222, 5.15.163, 6.1.100, 6.6.41。
armbian-sync
armbian-update -k 6.1


# 第二步，安装编译工具
mkdir -p /usr/local/toolchain
cd /usr/local/toolchain
# 下载编译工具
wget https://github.com/ophub/kernel/releases/download/dev/arm-gnu-toolchain-13.3.rel1-aarch64-aarch64-none-elf.tar.xz
# 解压
tar -Jxf arm-gnu-toolchain-13.3.rel1-aarch64-aarch64-none-elf.tar.xz
# 安装其他编译依赖包（可选项，可根据错误提示手动安装缺少项）
armbian-kernel -u


# 第三步，下载驱动，编译
# 下载驱动源码
cd ~/
git clone https://github.com/jwrdegoede/rtl8189ES_linux
cd rtl8189ES_linux
# 设置编译环境
gun_file="arm-gnu-toolchain-13.3.rel1-aarch64-aarch64-none-elf.tar.xz"
toolchain_path="/usr/local/toolchain"
toolchain_name="gcc"
export CROSS_COMPILE="${toolchain_path}/${gun_file//.tar.xz/}/bin/aarch64-none-linux-gnu-"
export CC="${CROSS_COMPILE}gcc"
export LD="${CROSS_COMPILE}ld.bfd"
export ARCH="arm64"
export KSRC=/usr/lib/modules/$(uname -r)/build
# 根据源码的实际路径设置 M 变量
export M="/root/rtl8189ES_linux"
# 编译驱动
make


# 第四步，安装驱动
sudo cp -f 8189es.ko /lib/modules/$(uname -r)/kernel/drivers/net/wireless/
# 更新模块依赖关系
sudo depmod -a
# 加载驱动模块
sudo modprobe 8189es
# 检查驱动是否加载成功
lsmod | grep 8189es
# 可以看到成功加载驱动
8189es               1843200  0
cfg80211              917504  2 8189es,brcmfmac
```

图示如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/1a89cbe6-df38-4862-8d11-9d977e0f4191">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/1a1d0bb9-44d4-4de5-9907-47e5f20747a7">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/user-attachments/assets/d1bd2eff-4c57-4e91-a870-08b0f8b1fe16">
</div>

## 10. 更新 Armbian 内核

登录 Armbian 系统 → 输入命令：

```shell
# 使用 root 用户运行 (sudo -i)
# 如果不指定参数，将更新为最新版本。
armbian-update
```

| 可选参数  | 默认值        | 选项           | 说明                              |
| -------- | ------------ | ------------- | -------------------------------- |
| -r       | ophub/kernel | `<owner>/<repo>` | 设置从 github.com 下载内核的仓库  |
| -u       | 自动化        | stable/flippy/dev/rk3588/rk35xx/h6 | 设置使用的内核的 [tags 后缀](https://github.com/ophub/kernel/releases) |
| -k       | 最新版        | 内核版本       | 设置[内核版本](https://github.com/ophub/kernel/releases/tag/kernel_stable)  |
| -b       | yes          | yes/no        | 更新内核时自动备份当前系统使用的内核    |
| -m       | no           | yes/no        | 使用主线 u-boot                    |
| -s       | 无           | 无/磁盘名称     | [SOS] 恢复 eMMC/NVMe/sdX 等磁盘中的系统内核 |
| -h       | 无           | 无             | 查看使用帮助                       |

举例: `armbian-update -k 5.15.50 -u dev`

通过 `-k` 参数指定内核版本号时，可以准确指定具体版本号，例如：`armbian-update -k 5.15.50`，也可以模糊指定到内核系列，例如：`armbian-update -k 5.15`，当模糊指定时将自动使用指定系列的最新版本。

更新内核时会自动备份当前系统使用的内核，存储路径在 `/ddbr/backup` 目录里，保留最近使用过的 3 个版本的内核，如果新安装的内核不稳定，可以随时恢复回备份的内核：
```shell
# 进入备份的内核目录，如 6.6.12
cd /ddbr/backup/6.6.12
# 执行更新内核命令，会自动安装当前目录下的内核
armbian-update
```

[SOS]：因特殊原因导致的更新不完整等问题，造成系统无法从 eMMC/NVMe/sdX 启动时，可以从 USB 等其他磁盘启动任意内核版本的 Armbian 系统，然后运行 `armbian-update -s` 命令可以把 USB 中的系统内核更新至 eMMC/NVMe/sdX 中，实现救援的目的。不指定磁盘参数时，默认将从 USB 设备恢复 eMMC/NVMe/sdX 中的内核，如果设备有多个磁盘，可以准确指定需要恢复的磁盘名称，举例如下：

```shell
# 恢复 eMMC 中的内核
armbian-update -s mmcblk1
# 恢复 NVMe 中的内核
armbian-update -s nvme0n1
# 恢复移动存储设备中的内核
armbian-update -s sda
# 磁盘名称可以简写为 mmcblk0/mmcblk1/nvme0n1/nvme1n1/sda/sdb/sdc 等，也可以使用完整的名称，如 /dev/sda
armbian-update -s /dev/sda
# 当设备只有 eMMC/NVMe/sdX 中的一个内置存储时，可以省略磁盘名称参数
armbian-update -s
```

如果你访问 github.com 的网络不通畅，无法在线下载更新时，可以手动下载内核，上传至 Armbian 系统的任意目录，并进入内核目录，执行 `armbian-update` 进行本地安装。如果当前目录下有成套的内核文件，将使用当前目录的内核进行更新（更新需要的 4 个内核文件是 `header-xxx.tar.gz`, `boot-xxx.tar.gz`, `dtb-xxx.tar.gz`, `modules-xxx.tar.gz`。其他内核文件不需要，如果同时存在也不影响更新，系统可以准确识别需要的内核文件）。在设备支持的可选内核里可以自由更新，如从 6.6.12 内核更新为 5.15.50 内核。

通过 `-r`/`-u`/`-b` 等参数设置的自定义选项，可以固定填写到个性化配置文件 `/etc/ophub-release` 的相关参数里，避免每次输入。对应设置为：

```shell
# 自定义修改参数的赋值
-r  :  KERNEL_REPO='ophub/kernel'
-u  :  KERNEL_TAGS='stable'
-b  :  KERNEL_BACKUP='yes'
```

## 11. 安装常用软件

登录 Armbian 系统 → 输入命令：

```shell
armbian-software
```

使用 `armbian-software -u` 命令可以更新本地的软件中心列表。根据用户在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中的需求反馈，逐步整合常用[软件](../build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf)，实现一键安装/更新/卸载等快捷操作。包括 `docker 镜像`、`桌面软件`、`应用服务` 等。详见更多[说明](armbian_software.md)。

根据你所在的国家或地区，使用 `armbian-apt` 命令选择合适的软件源，提高软件的下载速度。例如，选择中国的清华大学源：

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

## 12. 常见问题

在 Armbian 的使用中，一些可能遇到的常见问题汇总如下。

### 12.1 每个盒子的 dtb 和 u-boot 对应关系表

支持的电视盒子列表在 `Armbian` 系统中配置文件的位置为 [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf)。

### 12.2 LED 屏显示控制说明

请查阅[说明](led_screen_display_control.md)

### 12.3 如何恢复原安卓 TV 系统

通常使用 `armbian-ddbr` 对设备的安卓 TV 系统进行备份和恢复。

除此之外也可以通过线刷的方法，将安卓系统刷入 eMMC 中，安卓系统的下载镜像可在 [Tools](https://github.com/ophub/kernel/releases/tag/tools) 中查找。

#### 12.3.1 使用 armbian-ddbr 备份恢复

建议您在全新的盒子里安装 Armbian 系统前，先对当前盒子自带的原安卓 TV 系统进行备份，以便在需要恢复系统时使用。请从 `TF/SD/USB` 启动 Armbian 系统，输入 `armbian-ddbr` 命令，然后根据提示输入 `b` 进行系统备份，备份文件的存放路径为 `/ddbr/BACKUP-arm-64-emmc.img.gz` ，请下载保存。在需要恢复安卓 TV 系统时，将之前备份的文件上传至 `TF/SD/USB` 设备的相同路径下，输入 `armbian-ddbr` 命令，然后根据提示输入 `r` 进行系统恢复。

#### 12.3.2 使用 Amlogic 刷机工具恢复

- 一般情况下，重新插入电源，如果可以从 USB 中启动，只要重新安装即可，多试几次。

- 如果接入显示器后，屏幕是黑屏状态，无法从 USB 启动，就需要进行盒子的短接初始化了。先将盒子恢复到原来的安卓系统，再重新刷入 Armbian 系统。首先下载 [amlogic_usb_burning_tool](https://github.com/ophub/kernel/releases/tag/tools) 系统恢复工具并安装好。准备一条 [USB 双公头数据线](https://user-images.githubusercontent.com/68696949/159267576-74ad69a5-b6fc-489d-b1a6-0f8f8ff28634.png)，准备一个 [曲别针](https://user-images.githubusercontent.com/68696949/159267790-38cf4681-6827-4cb6-86b2-19c7f1943342.png)。

- 以 x96max+ 为例，在盒子的主板上确认 [短接点](https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg) 的位置，下载盒子的 [Android TV 系统包](https://github.com/ophub/kernel/releases/tag/tools)。其他常见设备的安卓 TV 系统系统及对应的短接点示意图也可以在此[下载查看](https://github.com/ophub/kernel/releases/tag/tools)。

```shell
操作方法：

1. 打开刷机软件 USB Burning Tool:
   [ 文件 → 导入系统包 ]: X96Max_Plus2_20191213-1457_ATV9_davietPDA_v1.5.img
   [ 选择 ]：擦除 flash
   [ 选择 ]：擦除 bootloader
   点击 [ 开始 ] 按钮
2. 使用 [ 曲别针 ] 将盒子主板上的 [ 两个短接点进行短接连接 ]，
   并同时使用 [ USB 双公头数据线 ] 将 [ 盒子 ] 与 [ 电脑 ] 进行连接。
3. 当看到 [ 进度条开始走动 ] 后，拿走曲别针，不再短接。
4. 当看到 [ 进度条 100% ], 则刷机完成，盒子已经恢复成 Android TV 系统。
   点击 [ 停止 ] 按钮, 拔掉 [ 盒子 ] 和 [ 电脑 ] 之间的 [ USB 双公头数据线] 。
5. 如果以上某个步骤失败，就再来一次，直至成功。
   如果进度条没有走动，可以尝试插入电源。通长情况下不用电源支持供电，只 USB 双公头的供电即可满足刷机要求。
```

当完成恢复出厂设置，盒子已经恢复成 Android TV 系统，其他安装 Armbian 系统的操作，就和你之前第一次安装系统时的要求一样了，再来一遍即可。

### 12.4 设置盒子从 USB/TF/SD 中启动

根据自己盒子的情况，分别使用初次安装和重新安装 Armbian 系统的两种方法。

#### 12.4.1 初次安装 Armbian 系统

- 把刷好系统的 USB/TF/SD 插入盒子。
- 开启开发者模式: 设置 → 关于本机 → 版本号 (如: X96max plus...), 在版本号上快速连击 5 次鼠标左键, 看到系统显示 `开启开发者模式` 的提示。
- 开启 USB 调试模式: 系统 → 高级选选 → 开发者选项 (设置 `开启USB调试` 为启用)。启用 `ADB` 调试。
- 安装 ADB 工具：下载 [adb](https://github.com/ophub/kernel/releases/tag/tools) 并解压，将 `adb.exe`，`AdbWinApi.dll`，`AdbWinUsbApi.dll` 三个文件拷⻉到 `c://windows/` 目录下的 `system32` 和 `syswow64` 两个文件夹内，然后打开 `cmd` 命令面板，使用 `adb --version` 命令，如果有显示就表示可以使用了。
- 进入 `cmd` 命令模式。输入 `adb connect 192.168.1.137` 命令（其中的 ip 根据你的盒子修改，可以到盒子所接入的路由器设备里查看），如果链接成功会显示 `connected to 192.168.1.137:5555`
- 输入 `adb shell reboot update` 命令，盒子将重启并从你插入的 USB/TF/SD 启动，从浏览器访问系统的 IP 地址，或者 SSH 访问即可进入系统。

#### 12.4.2 重新安装 Armbian 系统

- 正常情况下，直接把刷写好 Armbian 的 U 盘插入 USB 即可直接从 U 盘中启动。USB 启动比 eMMC 具有优先启动权。
- 个别设备可能出现无法从 U 盘启动的现象，可以先把 eMMC 里 Armbian 系统 `/boot` 目录下的 `boot.scr` 文件改个名字，例如 `boot.scr.bak`，然后再插入 U 盘启动，这样就可以从 U 盘启动了。

### 12.5 禁用红外接收器

默认情况下启用对红外接收器的支持，但如果您将电视盒用作服务器，那么您可能希望禁用 IR 内核模块以防止错误地关闭您的盒子。 要完全禁用 IR，请添加以下行：

```shell
blacklist meson_ir
```

至 `/etc/modprobe.d/blacklist.conf` 并重启。

### 12.6 启动引导文件的选择

- 目前已知的设备中，只有 `T95(s905x)` / `T95Z-Plus(s912)` / `BesTV-R3300L(s905l-b)` 等少数设备需要使用 `/bootfs/extlinux/extlinux.conf` 文件，已经在系统里默认添加了。其他设备如果需要，可以将系统写入 USB 后，双击打开 `boot` 分区，将系统自带的 `/boot/extlinux/extlinux.conf.bak` 文件名称中的 `.bak` 删除即可使用。当写入 eMMC 时 `armbian-install` 会自动检查，如果存在 `extlinux.conf` 文件，会自动创建。

- 其他设备只需要 `/boot/uEnv.txt` 即可启动，不要修改 `extlinux.conf.bak` 文件。

### 12.7 网络设置

#### 12.7.1 使用 interfaces 设置网络

网络配置文件 `/etc/network/interfaces` 的默认内容如下：

```shell
source /etc/network/interfaces.d/*
# Network is managed by Network manager
auto lo
iface lo inet loopback
```

##### 12.7.1.1 由 DHCP 动态分配 IP 地址

```shell
source /etc/network/interfaces.d/*

auto eth0
iface eth0 inet dhcp
```

##### 12.7.1.2 手动设置静态 IP 地址

其中的 IP 和网关和 DNS 根据自己的网络情况修改。

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

##### 12.7.1.3 在 docker 中使用 OpenWrt 建立互通网络

其中的 MAC 地址根据自己的需要修改。

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

#### 12.7.2 使用 NetworkManager 管理网络

##### 12.7.2.1 新建网络连接

新建或修改网络连接前的准备工作

###### 12.7.2.1.1 获取网络接口名称

查看设备中有哪些网络接口可以用来建立网络连接。

```shell
nmcli device | grep -E "^[e].*|^[w].*|^[D].*|^[T].*" | awk '{printf "%-19s%-19s\n",$1,$2}'
```

执行命令后返回内容, `DEVICE` 列显示网络接口名称, `TYPE` 列显示网络接口类型。

其中 `eth0` = 第1块有线网卡的名称, `eth1` = 第2块有线网卡的名称, 以此类推, 无线网卡同理。

```shell
DEVICE             TYPE
eth0               ethernet
eth1               ethernet
eth2               ethernet
eth3               ethernet
wlan0              wifi
wlan1              wifi
```

###### 12.7.2.1.2 获取现有网络连接名称

查看设备现有哪些网络连接, 包含使用中和未使用的连接。在新建网络连接时, 不建议使用已经存在的连接名称。

```shell
nmcli connection show | grep -E ".*|^[N].*" | awk '{printf "%-19s%-19s\n", $1,$3}'
```

执行命令后返回内容, `NAME` 列显示现有网络连接名称, `TYPE` 列显示网络接口类型。

其中 `ethernet` = 有线网卡, `wifi` = 无线网卡, `bridge` = 网桥

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

###### 12.7.2.1.3 新建有线网络连接

在网络接口 `eth0` 上新建网络连接并立即生效 (`动态 IP 地址` - `IPv4 / IPv6`)。

```shell
# Set ENV
MYCON=ether1                  # 新建网络连接名称
MYETH=eth0                    # 网络接口名称 = eth0 / eth1 / eht2 / eth3
IPV6AGM=stable-privacy        # IPv6 地址状态模式 = stable-privacy / eui64

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

在网络接口 `eth0` 上新建网络连接并立即生效 (`静态 IP 地址` - `IPv4`)。

```shell
# Set ENV
MYCON=ether1                  # 网络连接名称
MYETH=eth0                    # 网络接口名称 = eth0 / eth1 / eht2 / eth3
IP=192.168.67.167/24          # HOST IP 地址, 其中 24 是子网掩码 对应 255.255.255.0
GW=192.168.67.1               # 网关
DNS=119.29.29.29,223.5.5.5    # DNS 服务器地址

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

###### 12.7.2.1.4 新建无线网络连接

在网络接口 `wlan0` 上新建网络连接并立即生效 (`动态 IP 地址` - `IPv4 / IPv6`)。

```shell
# Set ENV
MYCON=ssid                    # 新建网络连接名称, 建议使用 WiFi SSID 来指定连接名称
MYSSID=ssid                   # WiFi SSID 区分大小写
MYPSWD=passwd                 # WiFi 密码
MYWSKM=wpa-psk                # 安全性选择 WPA-WPA2 = wpa-psk or WPA3 = sae （无线路由器或 AP 中查看是哪一种加密方式）
MYWLAN=wlan0                  # 网络接口名称 = wlan0 / wlan1
IPV6AGM=stable-privacy        # IPv6 地址状态模式 = stable-privacy / eui64

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

##### 12.7.2.2 修改无线网络连接中的 WiFi SSID or PASSWD

修改无线网络连接 `ssid` 中的 `WiFi SSID or PASSWD` 并立即生效。

```shell
# Set ENV
MYCON=ssid                    # 无线网络连接名称
MYSSID=ssid                   # WiFi SSID 区分大小写
MYPSWD=passwd                 # WiFi 密码
MYWSKM=wpa-psk                # 安全性选择 WPA-WPA2 = wpa-psk or WPA3 = sae

# Add WLAN
nmcli connection modify $MYCON \
wifi.ssid $MYSSID \
wifi-sec.key-mgmt $MYWSKM \
wifi-sec.psk $MYPSWD
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.3 修改网络地址分配方式

###### 12.7.2.3.1 静态 IP 地址 - IPv4

在网络连接 `ether1` 上修改 IP 地址分配方式为 `静态 IP 地址` 并立即生效。

*适用 有线连接 / 无线连接

```shell
# Set ENV
MYCON=ether1                  # 网络连接名称
IP=192.168.67.167/24          # HOST IP 地址, 其中 24 是子网掩码 对应 255.255.255.0
GW=192.168.67.1               # 网关
DNS=119.29.29.29,223.5.5.5    # DNS 服务器地址

# Chg CON
nmcli connection modify $MYCON \
ipv4.method manual \
ipv4.addresses $IP \
ipv4.gateway $GW \
ipv4.dns $DNS
nmcli connection up $MYCON
ip -c -br address
```

###### 12.7.2.3.2 DHCP 获取动态 IP 地址 - IPv4 / IPv6

在网络连接 `ether1` 上修改 IP 地址分配方式为 `DHCP 获取动态 IP 地址` 并立即生效。

*适用 有线连接 / 无线连接

```shell
# Set ENV
MYCON=ether1                  # 网络连接名称

# Chg CON
nmcli connection modify $MYCON \
ipv4.method auto \
ipv6.method auto
nmcli connection up $MYCON
ip -c -br address
```

##### 12.7.2.4 修改网络连接 MAC 地址

在网络连接 `ether1` 上修改(克隆) `MAC 地址`并立即生效, 以解决局域网 MAC 地址冲突问题。

*适用 有线连接 / 无线连接

```shell
# Set ENV
MYCON=ether1                  # 网络连接名称, 注意匹配网络接口类型
MYTYPE=ethernet               # 网络接口类型 = 有线网卡 / 无线网卡 = ethernet / wifi
MYMAC=12:34:56:78:9A:BC       # 新的 MAC 地址

# Chg CON
nmcli connection modify ${MYCON} \
${MYTYPE}.cloned-mac-address ${MYMAC}
nmcli connection up ${MYETH}
ip -c -br address
```

* 新建或修改部分网络参数, 网络连接可能会被断开, 并重新连接网络。
* 由于软硬件环境不同（盒子, 系统, 网络设备等）, 生效所需时间 `1-15` 秒左右, 更长时间未生效的建议检查软硬件环境。

##### 12.7.2.5 如何禁用 IPv6

您可以使用 `nmcli` 实用程序在命令行中禁用 `IPv6` 协议，参考来源 [disable-ipv6](https://access.redhat.com/documentation/zh-cn/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/using-networkmanager-to-disable-ipv6-for-a-specific-connection_configuring-and-managing-networking)。

第一步，先使用 `nmcli connection show` 命令查看网络连接列表，返回结果如下：

```shell
NAME                 UUID                                   TYPE       DEVICE
Wired connection 1   8a7e0151-9c66-4e6f-89ee-65bb2d64d366   ethernet   eth0
...
```

第二步，将连接的 ipv6.method 参数设为 disabled ：

```shell
nmcli connection modify "Wired connection 1" ipv6.method "disabled"
```

第三步，重新连接网络：

```shell
nmcli connection up "Wired connection 1"
```

第四步，查看网络连接状态，如果没有显示 inet6 条目，则 IPv6 在该设备上被禁用：

```shell
ip address show eth0
```

第五步，验证 `/proc/sys/net/ipv6/conf/eth0/disable_ipv6` 文件现在是否包含值 `1`

```shell
# cat /proc/sys/net/ipv6/conf/eth0/disable_ipv6
1
```

#### 12.7.3 如何启用无线

有的设备支持使用无线，启用方法如下：

```shell
# 安装管理工具
sudo apt-get install network-manager

# 查看网络设备
sudo nmcli dev

# 启用无线
sudo nmcli r wifi on

# 扫描无线
sudo nmcli dev wifi

# 连接无线
sudo nmcli dev wifi connect "wifi名称" password "wifi密码"

# 显示已保存的网络连接列表
sudo nmcli connection show

# 断开连接
sudo nmcli connection down "wifi名称"

# 忘记连接并取消自动连接
sudo nmcli connection delete "wifi名称"
```

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/230541872-565a655e-2781-4170-8898-0ae096725506.png">
</div>

#### 12.7.4 如何启用蓝牙

有的设备支持使用蓝牙，启用方法如下：

```shell
# 安装蓝牙支持
armbian-config >> Network >> BT: Install Bluetooth support

# 重启系统
reboot
```

系统重启后，查看蓝牙驱动是否正常。桌面系统的可以在菜单里连接蓝牙设备。也可以使用终端图形界面安装。

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
# 连接蓝牙设备
armbian-config >> Network >> BT: Discover and connect Bluetooth devices
```

也可以在终端中使用命令安装：
```shell
# 查看蓝牙服务运行状态
sudo systemctl status bluetooth

# 如果未启动，先开启蓝牙服务
sudo systemctl enable bluetooth
sudo systemctl start bluetooth

# 扫描附近的蓝牙设备
bluetoothctl scan on

# 启用蓝牙发现
bluetoothctl discoverable on

# 进行蓝牙 MAC 地址配对
bluetoothctl pair 12:34:56:78:90:AB

# 查看配对好的蓝牙设备
bluetoothctl paired-devices

# 连接蓝牙设备
bluetoothctl connect 12:34:56:78:90:AB

# 信任设备，方便以后直接连接
bluetoothctl trust 12:34:56:78:90:AB

# 断开蓝牙设备
bluetoothctl disconnect 12:34:56:78:90:AB

# 解除蓝牙配对
bluetoothctl remove 12:34:56:78:90:AB

# 阻止连接设备
bluetoothctl block 12:34:56:78:90:AB
```

### 12.8 如何添加开机启动任务

系统中已经添加了自定义开机启动任务脚本文件，在 Armbian 系统中的路径是 [/etc/custom_service/start_service.sh](../build-armbian/armbian-files/common-files/etc/custom_service/start_service.sh) 文件，可以根据个人需求在该脚本中自定义添加相关任务。

### 12.9 如何更新系统中的服务脚本

使用 `armbian-sync` 命令可以一键将本地系统中的全部服务脚本更新到最新版本。

如果 `armbian-sync` 更新失败，说明这个命令的版本过旧，可以使用下面的方法更新这个命令：

```shell
wget https://raw.githubusercontent.com/ophub/amlogic-s9xxx-armbian/main/build-armbian/armbian-files/common-files/usr/sbin/armbian-sync -O /usr/sbin/armbian-sync

chmod +x /usr/sbin/armbian-sync

armbian-sync
```

### 12.10 如何获取 eMMC 上的安卓系统分区信息

我们将 Armbian 系统写入 eMMC 系统时，需要首先确认设备的安卓系统分区表，确保将数据写入至安全区域，尽量不要破坏安卓系统分区表，以免造成系统无法启动等问题。如果写入了不安全的区域，会无法启动，或出现类似下面的错误：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/187075834-4ac40263-52ae-4538-a4b1-d6f0d5b9c856.png">
</div>

#### 12.10.1 获取分区信息

如果你使用的是 2022.11 之后本仓库中发布的 Armbian，你可以复制粘贴以下命令来获得一个记录完整分区信息的网址（设备本身并不需要联网）

```shell
ampart /dev/mmcblk2 --mode webreport 2>/dev/null
```

*ampart 的 webreport 模式为 2023.02.03 发布的 v1.2 版本引入的，如果你使用上面的命令无输出，则可能为不支持直接输出网址的旧版，你可以转而使用下面这条命令：*

```shell
echo "https://7ji.github.io/ampart-web-reporter/?dsnapshot=$(ampart /dev/mmcblk2 --mode dsnapshot 2>/dev/null | head -n 1)&esnapshot=$(ampart /dev/mmcblk2 --mode esnapshot 2>/dev/null | head -n 1)"
```

得到的网址将会类似于下面这样：

```shell
https://7ji.github.io/ampart-web-reporter/?esnapshot=bootloader:0:4194304:0%20reserved:37748736:67108864:0%20cache:113246208:754974720:2%20env:876609536:8388608:0%20logo:893386752:33554432:1%20recovery:935329792:33554432:1%20rsv:977272832:8388608:1%20tee:994050048:8388608:1%20crypt:1010827264:33554432:1%20misc:1052770304:33554432:1%20instaboot:1094713344:536870912:1%20boot:1639972864:33554432:1%20system:1681915904:1073741824:1%20params:2764046336:67108864:2%20bootfiles:2839543808:754974720:2%20data:3602907136:4131389440:4&dsnapshot=logo::33554432:1%20recovery::33554432:1%20rsv::8388608:1%20tee::8388608:1%20crypt::33554432:1%20misc::33554432:1%20instaboot::536870912:1%20boot::33554432:1%20system::1073741824:1%20cache::536870912:2%20params::67108864:2%20data::-1:4
```

将这个网址复制到你的浏览器打开，即可看到格式清晰明了的 DTB 分区信息和 eMMC 分区信息：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287642-e1b7be27-4d2c-4ac3-9fcc-15e06aebb97e.png">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287654-d1929e21-d2b3-4fb6-bcf0-c454c88e21b9.png">
</div>

#### 12.10.2 分区信息分享

当你需要分享分区信息给其他人时（比如，发布到本仓库以来汇报某一新设备的情况，或者寻求他人的帮助），尽量分享网址本身，而不是截图。如果介意网址太长，可以借用一些免费的短网址工具。

- 一方面，网页上的分区信息在每次访问时都会动态生成，对于某些分区是否能写入的标注，以及表格的格式等都可能会更新。
- 另一方面，从截图中其他人也不能方便地复制分区参数做计算等。

另外，也不需要额外地将参数整理到表格文件，网页上表格的布局已经特意设计为仅需复制粘贴就可以导入到 Excel 或者 LibreOffice Calc 中。

#### 12.10.3 分区信息解读

DTB 表是安卓 DTB 中记录的每个盒子**系统**希望的分区布局，这一布局里一般会以一个大小为自动填充的 `data` 分区为结尾，所以同系统（也必然包括同型号）的盒子，这里的布局必然是相同的。盒子上实际的分区布局会因为 eMMC 的容量不同而各有差别，但总是由 DTB 的分区布局所决定的（即已知 DTB 分区布局 +eMMC 准确大小，必然可推知 eMMC 分区情况。 *上面的 DTB 分区信息和 eMMC 分区信息并非来自同一个盒子，你看出来了吗？*）。

eMMC 表是盒子上实际的 eMMC 分区布局。其中每一行表示一块存储区域，这一存储区域既可能是一个分区，也可能是分区间的空隙（因为晶晨的诡异决策，每个分区之间都至少有 8M 的空隙，计划留作他用，结果到最新的 S905X4 都没有用上，十分浪费空间）。对应分区的行中，字体为黑色，且偏移和掩码栏均有数值；对应空隙的行中，字体为灰色，偏移和掩码栏没有数值，且分区名为 `gap` 。

eMMC 表中，每一块存储区域的最后一栏为可写入的情况，绿色且 `yes` 表示这一区域可以写入，红色且 `no` 表示这一区域绝对不可以写入，黄色且有标注则表示某前提的下可以写入，或者只有部分可以写入。

以上表为例，`bootloader` 分区对应的 `0+4M` (`0M~4M`)绝对不可写入，其后的 `32M` 空隙（`4M~36M`）可以写入，`reserved` 分区对应的 `36M+64M` (`36M~100M`)绝对不可写入，其后的空隙一直到 `env` 前的空隙（`100M~836M`）都可以写入，`env` 的1M往后（`837M一直到结尾`）在不需要安卓启动 logo 的情况下都可以写入，则 eMMC 上所有可写入的范围为：

- 4M~36M
- 100M~836M
- 837M~结尾

在需要安卓启动 logo 的情况下，额外的，`logo`分区对应的 852M + 32M (`852M~884M`)不能写入，则 eMMC 上所有的可写入范围为：

- 4M~36M
- 100M~836M
- 837M~852M
- 884M~结尾

#### 12.10.4 用于 eMMC 安装

如果你的设备在使用 `armbian-install` 且 `-a` 参数（使用 [ampart](https://github.com/7Ji/ampart) 调整 eMMC 分区布局）为 `yes`（默认值）的情况下失败，则你的盒子不能使用最优化的布局（即把 DTB 分区信息调整为只有 `data` ，再由此生成 eMMC 分区信息，然后将所有还存在的分区均向前挪动，如此一来，117M 向后的空间便均可使用），你需要在 [armbian-install](../build-armbian/armbian-files/common-files/usr/sbin/armbian-install) 中修改对应的分区信息。

此文件中，声明分区布局的关键参数有三个：`BLANK1`, `BOOT`, `BLANK2`。其中 `BLANK1` 表示从 eMMC 开头算起的不能使用的大小；`BOOT` 表示在 `BLANK1` 以后创建的用来存放内核、DTB 等的分区的大小，最好不要小于 256M，`BLANK2` 表示 `BOOT` 以后不能使用的大小；在此之后的空间会全部用来创建 `ROOT` 分区，储存整个系统中 `/boot` 挂载点以外的数据。三者均应为整数，且单位为MiB (1 MiB = 1024 KiB = 1024^2 Byte)

讨论上一段中不需要 `logo` 分区的情况，我们自然希望将所有能使用的空间全部使用，但是 `4M~36M` 的区域由于太小，不能用作 `BOOT`，所以只能将它算在不能用的 `BLANK1` 里面。而 `100M~836M` 的区域，用作 `BOOT` 绰绰有余，则可以将这 736M 全部分配给 `BOOT`。此后再有 `836M~837M` 的不能使用区域，便算给 `BLANK2` ，那么应该使用的参数就应该如下（下文仅以 `s905x3` 为例，若你的 SoC 为其他，需要修改其他的对应代码块）：

```shell
# Set partition size (Unit: MiB)
elif [[ "${AMLOGIC_SOC}" == "s905x3" ]]; then
    BLANK1="100"
    BOOT="736"
    BLANK2="1"
```

### 12.11 如何制作 u-boot 文件

u-boot 文件是引导系统正常启动的重要文件。Amlogic，Allwinner 和 Rockchip 设备在获取源码和编译流程上略有不同。

#### 12.11.1 如何制作 Amlogic 设备的 u-boot 文件

由于 Amlogic 系列的设备厂商大多数都是闭源的，所以我们需要从设备上提取 u-boot 相关文件，然后再进行编译。这里介绍的方法来自 [unifreq](https://github.com/unifreq) 大佬分享的制作教程。

##### 12.11.1.1 如何提取 bootloader 和 dtb 文件

提取需要使用 HxD 软件。可以从 [官网下载链接](https://mh-nexus.de/en/downloads.php?product=HxD20) 或 [备份下载链接](https://github.com/ophub/kernel/releases/download/tools/HxDSetup.2.5.0.0.zip) 获取安装。

在 `cmd` 面板中依次执行以下命令提取相关文件，并下载到本地电脑。

```shell
# 使用 adb 工具进入盒子
adb connect 192.168.1.111
adb shell

# 导出 bootloader 命令
dd if=/dev/block/bootloader of=/data/local/bootloader.bin

# 导出 dtb 命令
cat /dev/dtb >/data/local/mybox.dtb

# 导出 gpio 命令
cat /sys/kernel/debug/gpio >/data/local/mybox_gpio.txt

# 依次把 bootloader、dtb 和 gpio 文件都下载到本地电脑C盘根目录下的 mybox 文件夹
adb pull /data/local/bootloader.bin C:\mybox
adb pull /data/local/mybox.dtb C:\mybox
adb pull /data/local/mybox_gpio.txt C:\mybox
```

##### 12.11.1.2 如何制作 acs.bin 文件

主线 u-boot 最重要的是 acs.bin，用于初始化内存的部分，原厂 u-boot 位于系统最前面的 4MB 位置。使用刚才获得的 `bootloader.bin` 文件提取 `acs.bin` 文件。

打开 HxD 软件，打开上面导出的 `bootloader.bin` 文件，`右键 - 选择范围`，起始位置 `F200`，长度 `1000`，选`十六进制`。

<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/187056711-1b58ce71-2a7d-4e9b-a976-e5f278edaa53.png">

复制选择的结果，然后新建文件，插入式粘贴，警告忽略，另存为 acs.bin 文件。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056725-0a0e60af-6a21-4a6b-a2d5-f3d46b438a6a.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056827-8419c738-3428-473e-9a95-ab7270170d98.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056852-9f62f16a-f7f1-4c34-a2c2-78358d198f9a.png">
</div>

如果是锁了 bootloader 的话这个区域的代码是是乱码就没用了。正常的应该像上图中这样有很多 `0` ，有 `cfg` 会连续出现几次，中间会出现 `ddr` 相关的字样，这种正常代码就是可以使用的。

##### 12.11.1.3 如何编译 u-boot 文件

制作 u-boot 需要 https://github.com/unifreq/amlogic-boot-fip 和 https://github.com/unifreq/u-boot 这两个源码库，编译自己盒子的两个 u-boot 文件。

在 amlogic-boot-fip 源码里面每个机型只有 acs.bin 这个文件是不同的，其它的文件都可以通用。

<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187057209-c4716384-46ef-4922-9710-8da7ae6db1e4.png">

制作 u-boot 的方法详见 https://github.com/unifreq/u-boot/tree/master/doc/board/amlogic 里的具体说明，选择自己设备的型号进行编译测试。

根据 [unifreq](https://github.com/unifreq) 的方法制作 u-boot 需要用到盒子的 acs.bin，dts 和 config 文件。其中安卓系统导出来的 dts 不能直接转换成 Armbian 的格式，需要自己编写一个对应的 dts 文件。根据自己设备具体硬件上的区别部分，比如开关、led、电源控制、tf卡、sdio wifi模块等，使用内核源码库中相似的 [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) 文件进行修改制作。

以制作 X96Max Plus 的 u-boot 为例：

```shell
~/make-uboot
    ├── amlogic-boot-fip
    │   ├── x96max-plus                                     # 自己创建目录
    │   │   ├── asc.bin                                     # 自己制作源文件
    │   │   └── other-copy-files...                         # 复制其他目录的文件
    │   │
    │   ├── other-source-directories...
    │   └── other-source-files...
    │
    └── u-boot
        ├── configs
        │   └── x96max-plus_defconfig                       # 自己制作源文件
        ├── arch
        │   └── arm
        │       └── dts
        │           ├── meson-sm1-x96-max-plus-u-boot.dtsi  # 自己制作源文件
        │           ├── meson-sm1-x96-max-plus.dts          # 自己制作源文件
        │           └── Makefile                            # 编辑
        ├── fip
        │   ├── u-boot.bin                                  # 生成文件
        │   └── u-boot.bin.sd.bin                           # 生成文件
        ├── u-boot.bin                                      # 生成文件
        │
        ├── other-source-directories...
        └── other-source-files...
```

- 下载 [amlogic-boot-fip](https://github.com/unifreq/amlogic-boot-fip) 源码。在根目录创建 [x96max-plus](https://github.com/unifreq/amlogic-boot-fip/tree/master/x96max-plus) 目录，里面的文件除了自己制作的 `asc.bin` 文件外，其他文件可以从其他目录下复制。
- 下载 [u-boot](https://github.com/unifreq/u-boot) 源码。制作对应的 [x96max-plus_defconfig](https://github.com/unifreq/u-boot/blob/master/configs/x96max-plus_defconfig) 文件放入 [configs](https://github.com/unifreq/u-boot/tree/master/configs) 目录。制作对应的 [meson-sm1-x96-max-plus-u-boot.dtsi](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus-u-boot.dtsi) 和 [meson-sm1-x96-max-plus.dts](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus.dts) 文件放入 [arch/arm/dts](https://github.com/unifreq/u-boot/tree/master/arch/arm/dts) 目录，并编辑此目录中的 [Makefile](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/Makefile) 文件，添加 `meson-sm1-x96-max-plus.dtb` 文件的索引。
- 进入 u-boot 源码目录根目录下，根据文档 https://github.com/unifreq/u-boot/blob/master/doc/board/amlogic/x96max-plus.rst 中的步骤操作。

最终生成的文件有两类：在 u-boot 根目录下的 `u-boot.bin` 文件是 `/boot` 目录下使用的不完整版 u-boot（对应仓库中的 [overload](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) 目录）；在 `fip` 目录下的 `u-boot.bin` 和 `u-boot.bin.sd.bin` 是 `/usr/lib/u-boot/` 目录下使用的完整版 u-boot 文件（对应仓库中的 [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) 目录），完整版的两个文件相差 512 字节，大的那个是填充了 512 字节的 0 在前面。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189039426-c127631f-77ca-4fcb-9fb6-4220045d712b.png">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189029320-e43a4cc9-b4b5-4de4-92fe-b17bd29020d0.png">
</div>

💡提示：在写入 eMMC 进行测试前，请先查看 12.3 的救砖方法。务必掌握短接点位置，有原厂 .img 格式的安卓系统文件，并进行过短接刷机测试，确保救砖方法都已经掌握的情况下再进行写入测试。

#### 12.11.2 如何制作 Rockchip 设备的 u-boot 文件

由于 Rockchip 设备的大部分厂商都开放了他们的 u-boot 源码，所以可以比较方便地从厂商的源码库中获取到相关的 u-boot 源码，然后进行编译。同时一些开源大佬们也分享了很多更易使用的 u-boot 编译脚本，下面以几个实例介绍几种编译方法。

##### 12.11.2.1 如何使用 Radxa 的 u-boot 制作脚本

以编译 [Rock5b(rk3588)](https://wiki.radxa.com/Rock5/guide/build-u-boot-on-5b) 为例。

```shell
# 01.安装依赖
sudo apt-get update
sudo apt-get install -y git device-tree-compiler libncurses5 libncurses5-dev build-essential libssl-dev mtools bc python dosfstools flex bison

# 02.克隆源码
mkdir ~/rk3588-sdk && cd ~/rk3588-sdk
git clone -b stable-5.10-rock5 https://github.com/radxa/u-boot.git
git clone -b master https://github.com/radxa/rkbin.git
git clone -b debian https://github.com/radxa/build.git

# 源码说明：
# ~/rk3588-sdk/build/：Radxa 辅助脚本文件和用于构建 U-Boot、Linux 内核和根文件系统的配置文件。
# ~/rk3588-sdk/rkbin/：预构建的 Rockchip 二进制文件，包括第一阶段加载程序和 ATF（Arm Trustzone固件）。
# ~/rk3588-sdk/u-boot/：用于启动操作系统（如 Linux 或 Android）的第二阶段引导加载程序。

# 03.编译 u-boot （For ROCK 5B）
cd ~/rk3588-sdk
./build/mk-uboot.sh rk3588-rock-5b

# 04.构建成功后，将放置在 ~/rk3588-sdk/out/u-boot 目录
~/rk3588-sdk/out/u-boot
├── idbloader.img
├── rk3588_spl_loader_v1.08.111.bin
├── spi
│   └── spi_image.img
└── u-boot.itb
```

通过在 [radxa/build](https://github.com/radxa/build) 源码的 `board_configs.sh` 和 `mk-uboot.sh` 里添加更多选项，可以编译其他设备的 u-boot 文件，例如我编译 [Beelink-IPC-R(rk3588)](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415#issuecomment-1508234307) 设备的使用方法。

##### 12.11.2.2 如何使用 cm9vdA 的 u-boot 制作脚本

cm9vdA 在他的 [cm9vdA/build-linux](https://github.com/cm9vdA/build-linux) 开源项目里提供了编译 u-boot 和 kernel 的脚本和使用方法，我在一些 Rockchip 设备的 u-boot 编译中使用了他的项目并进行了过程记录，摘录部分以供参考。

- 编译 Lenovo-Leez-P710(rk3399) 设备的 u-boot：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609#issuecomment-1681494735)
- 编译 DLFR100(rk3399) 设备的 u-boot：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522#issuecomment-1622919423)
- 编译 ZYSJ(rk3399) 设备的 u-boot：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380#issuecomment-1539325464)


### 12.12 内存大小识别错误

如果内存大小识别不正确（4G内存识别为1-2G是不正常，识别为3.7G是正常），可以尝试手动复制一份 `/boot/UBOOT_OVERLOAD` 文件（注意是`复制`一份，`不要改名`，改名后安装与更新等操作后将无法启动），在 `USB` 中使用时另存为 `/boot/u-boot.ext`，在 `eMMC` 中使用时另存为 `/boot/u-boot.emmc`。

除了想尝试解决内存的问题外，不要手动复制 u-boot 文件，添加不正确会导致无法启动以及出现各种问题。

### 12.13 如何反编译 dtb 文件

有些新设备不在目前的支持列表（或有变异体），可以通过反编译，调整相关参数进行尝试。

```shell
# 安装依赖
sudo apt-get update
sudo apt-get install -y device-tree-compiler

# 1. 反编译命令（使用 dtb 文件生成 dts 源码）
dtc -I dtb -O dts -o xxx.dts xxx.dtb

# 2. 编译命令（使用 dts 编译生成 dtb 文件）
dtc -I dts -O dtb -o xxx.dtb xxx.dts

# 3.保存数据并重启
sync && reboot

# 4.[自选动作]根据需求进行测试
# 例如在解决 12.16 中介绍的问题时，重新安装测试
armbian-install
```

### 12.14 如何修改 cmdline 设置

在 Amlogic 设备中，可以在 `/boot/uEnv.txt` 文件中进行添加/修改/删除等设置。在 Rockchip 和 Allwinner 设备中在 `/boot/armbianEnv.txt` 文件中进行设置（添加至 `extraargs` 或 `extraboardargs` 参数里）。使用 `/boot/extlinux/extlinux.conf` 的设备在这个文件里配置。每次更改后要重启才能生效。

- 比如 `Home Assistant Supervisor` 应用只支持 `docker cgroup v1` 版本，而目前 docker 默认安装的都是最新的 v2 版本。如需切换至 v1 版本，可以在 cmdline 中添加 `systemd.unified_cgroup_hierarchy=0` 参数设置，重启后就可以切换至 `docker cgroup v1` 版本。

- 通过在 cmdline 中添加 `max_loop=128` 设置，可以调整允许的 loop 挂载数量。

- 通过在 cmdline 中添加 `usbcore.usbfs_memory_mb=1024` 设置，可以永久将 USBFS 内存缓冲区从默认的 `16 mb` 改为更大（`cat /sys/module/usbcore/parameters/usbfs_memory_mb`），提升 USB 传输大文件的能力。

- 通过在 cmdline 中添加 `usbcore.usb3_disable=1` 设置，可以禁用 USB 3.0 的所有设备。

- 通过在 cmdline 中添加 `extraargs=video=HDMI-A-1:1920x1080@60` 设置，可以将视频显示模式强制为 1080p。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://user-images.githubusercontent.com/68696949/216220941-47db0183-7b26-4768-81cf-2ee73d59d23e.png">
</div>

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="700" alt="image" src="https://github.com/ophub/amlogic-s9xxx-armbian/assets/68696949/a600dcad-d817-47eb-b529-4014019915b3">
</div>

### 12.15 如何添加新的支持设备

为一个设备构建 Armbian 系统，需要用到 `设备配置文件`、`系统文件`、`u-boot 文件`、`流程控制文件` 共 4 部分，具体添加方法介绍如下：

#### 12.15.1 添加设备配置文件

在配置文件 [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) 里面，根据设备的测试支持情况，添加对应的配置信息。其中 `BUILD` 的值是 `yes` 的是默认构建的部分设备，对应的 `BOARD` 值 `必须唯一`，这些盒子可以直接使用默认构建的 Armbian 系统。

默认值是 `no` 的没有打包，这些设备使用时需要下载相同 `FAMILY` 的 Armbian 系统，在写入 `USB` 后，可以在电脑上打开 `USB 中的 boot 分区`，修改 `/boot/uEnv.txt` 文件中 `FDT 的 dtb 名称`，适配列表中的其他设备。

#### 12.15.2 添加系统文件

通用文件放在：`build-armbian/armbian-files/common-files` 目录下，各平台通用。

平台文件分别放在 `build-armbian/armbian-files/platform-files/<platform>` 目录下，[Amlogic](../build-armbian/armbian-files/platform-files/amlogic)，[Rockchip](../build-armbian/armbian-files/platform-files/rockchip) 和 [Allwinner](../build-armbian/armbian-files/platform-files/allwinner) 分别共用各自平台的文件，其中 `bootfs` 目录下是 /boot 分区的文件，`rootfs` 目录下的是 Armbian 系统文件。

如果个别设备有特殊差异化设置需求，在 `build-armbian/armbian-files/different-files` 目录下添加以 `BOARD` 命名的独立目录，根据需要建立 `bootfs` 目录添加系统 `/boot` 分区下的相关文件，根据需要建立 `rootfs` 目录添加系统文件。各文件夹命名以 `Armbian` 系统中的实际路径为准。用于添加新文件，或覆盖从通用文件和平台文件中添加的同名文件。

#### 12.15.3 添加 u-boot 文件

`Amlogic` 系列的设备，共用 [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) 文件和 [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) 文件，如果有新增的文件，分别放入对应的目录。其中的 `bootloader` 文件在系统构建时会自动添加至 Armbian 系统的 `/usr/lib/u-boot` 目录，`u-boot` 文件会自动添加至 `/boot` 目录。

`Rockchip` 和 `Allwinner` 系列的设备，为每个设备添加以 `BOARD` 命名的独立 [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot) 文件目录，对应的系列文件放在此目录中。

构建 Armbian 镜像时，这些 u-boot 文件将根据 [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) 中的配置，由 rebuild 脚本写入对应的 Armbian 镜像文件中。

#### 12.15.4 添加流程控制文件

在 [yml 工作流控制文件](../../.github/workflows/build-armbian.yml) 的 `armbian_board` 中添加对应的 `BOARD` 选项，支持在 github.com 的 `Actions` 中进行使用。

### 12.16 如何解决写入 eMMC 时 I/O 错误的问题

有些设备可以从 USB/SD/TF 正常启动 Armbian 使用，但是写入 eMMC 时会报 I/O 写入错误，例如 [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues/989) 中的案例，报错内容如下：

```shell
[  284.338449] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.341544] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.446972] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.450074] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.497746] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.500871] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
```

这种情况下可以调整所使用的 dtb 的工作模式速度和频率，来稳定对存储的读写支持。使用 sdr 模式时，频率是速度的 2 倍，使用 ddr 模式时，频率等于速度。如下：

```shell
sd-uhs-sdr12
sd-uhs-sdr25
sd-uhs-sdr50
sd-uhs-ddr50
sd-uhs-sdr104

max-frequency = <208000000>;
```

以内核源码的 [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) 文件中的代码片段举例：

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

一般情况下，把 `&sd_emmc_c` 的频率由 `max-frequency = <200000000>;` 下调为 `max-frequency = <100000000>;` 即可解决问题。如果不行可继续下调到 `50000000` 进行测试，并通过调整 `&sd_emmc_b` 来对 `USB/SD/TF` 进行设置，也可以使用 `sd-uhs-sdr` 进行限速。你可以通过修改 dts 文件并 [编译](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel) 得到测试文件，也可以通过 `12.13 节` 中介绍的方法对已有的 dtb 文件进行反编译修改生成测试文件。反编译 dtb 文件修改时使用十六进制的值，其中十进制的 `200000000` 对应的十六进制为 `0xbebc200`，十进制的 `100000000` 对应的十六进制为 `0x5f5e100`，十进制的 `50000000` 对应的十六进制为 `0x2faf080`，十进制的 `25000000` 对应的十六进制为 `0x17d7840`。

除了通过系统软件层来解决，还可以发挥 [钞能力](https://github.com/ophub/amlogic-s9xxx-armbian/issues/998) 和 [动手能力](https://www.right.com.cn/forum/thread-901586-1-1.html) 解决。

### 12.17 如何解决 Bullseye 版本没有声音的问题

声音问题的错误日志信息：

```shell
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: dpcm_fe_dai_prepare() failed (-22)
Mar 29 15:47:18 armbian-ct2000 kernel:  fe.dai-link-0: ASoC: no backend DAIs enabled for fe.dai-link-0
```

请参考 [Bullseye NO Sound](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1000) 中的方法进行设置。

```shell
curl -fsSOL https://github.com/ophub/kernel/releases/download/tools/bullseye_g12_sound-khadas-utils-4-2-any.tar.gz
tar -xzf bullseye_g12_sound-khadas-utils-4-2-any.tar.gz -C /

systemctl enable sound.service
systemctl restart sound.service
```

重启 Armbian 测试。如果声音仍然不工作，可能是因为你的盒子用的是旧的 conf 对应的声音输出路线，需要在 /usr/bin/g12_sound.sh 里面注释掉 `L137-L142` 对应的新配置（主要是给 G12B 用的，也就是 S922X，旧的 G12A/S905X2 之前，以及基于 G12A 的 SM1/S905X3 大部分用不来），然后取消 `L130-L134` 对应的旧配置的注释。

### 12.18 如何编译 boot.scr 文件

在 Armbian 系统中的 `/boot` 目录下，`boot.scr` 是用于引导系统的文件。`boot.scr` 是 `boot.cmd` 的编译文件。`boot.cmd` 是 `boot.scr` 的源码文件。可以通过修改 `boot.cmd` 文件来修改 `boot.scr.` 文件，然后通过 `mkimage` 命令编译成 `boot.scr` 文件。

这两个文件一般不需要修改，如果有调整需求，可以参考以下方法。

```shell
# 安装依赖
sudo apt-get update
sudo apt-get install -y u-boot-tools

# 编辑 boot.cmd 文件
cd /boot
copy /boot/boot.cmd /boot/boot.cmd.bak
copy /boot/boot.scr /boot/boot.scr.bak
nano boot.cmd

# 编译命令
mkimage -C none -A arm -T script -d boot.cmd boot.scr

# 重启测试
sync
reboot

# 补充说明
# 在 Amlogic 设备中，在 USB 中使用的是 /boot/boot.scr 文件，写入 eMMC 时使用的是 /boot/boot-emmc.scr 文件。
```

### 12.19 如何开启远程桌面和修改默认端口

在软件中心 `armbian-software` 里选择 `201` 可以安装桌面，在安装桌面时会询问是否开启远程桌面，输入 `y` 即可开启。远程桌面的默认端口是 `3389`，可以根据需要自定义使用其他端口：

```shell
sudo nano /etc/xrdp/xrdp.ini
# 修改为自定义端口，例如 5000
port=5000
```

