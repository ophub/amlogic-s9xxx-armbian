# Armbian 构建及使用方法

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

GitHub Actions 是 Microsoft 推出的一项服务，提供高性能的虚拟服务器环境，可用于构建、测试、打包和部署项目。公开仓库可免费无时间限制地使用，单次编译时间长达 6 小时，足以满足 Armbian 的编译需求（通常 3 小时左右即可完成编译）。本项目仅供技术交流，不足之处敬请谅解。请勿发起网络攻击或恶意使用 GitHub Actions。

# 目录

- [Armbian 构建及使用方法](#armbian-构建及使用方法)
- [目录](#目录)
  - [1. 注册自己的 Github 的账户](#1-注册自己的-github-的账户)
  - [2. 设置隐私变量 GITHUB\_TOKEN 等](#2-设置隐私变量-github_token-等)
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
    - [8.4 Docker 版本的 Armbian 安装方法](#84-docker-版本的-armbian-安装方法)
      - [8.4.1 安装 Docker 运行环境](#841-安装-docker-运行环境)
      - [8.4.2 设置 macvlan 网络](#842-设置-macvlan-网络)
      - [8.4.3 运行 Armbian Docker 容器](#843-运行-armbian-docker-容器)
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
          - [12.7.2.4.1 方法一：使用 `nmcli` 命令修改 MAC 地址](#127241-方法一使用-nmcli-命令修改-mac-地址)
          - [12.7.2.4.2 方法二：通过配置文件修改 MAC 地址](#127242-方法二通过配置文件修改-mac-地址)
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
        - [12.15.3.1 查看分区信息布局情况](#121531-查看分区信息布局情况)
        - [12.15.3.2 备份关键分区](#121532-备份关键分区)
        - [12.15.3.3 添加特殊分区写入文件](#121533-添加特殊分区写入文件)
      - [12.15.4 添加流程控制文件](#12154-添加流程控制文件)
    - [12.16 如何解决写入 eMMC 时 I/O 错误的问题](#1216-如何解决写入-emmc-时-io-错误的问题)
    - [12.17 如何解决 Bullseye 版本没有声音的问题](#1217-如何解决-bullseye-版本没有声音的问题)
    - [12.18 如何编译 boot.scr 文件](#1218-如何编译-bootscr-文件)
    - [12.19 如何开启远程桌面和修改默认端口](#1219-如何开启远程桌面和修改默认端口)
    - [12.20 TCP 拥塞控制优化方案](#1220-tcp-拥塞控制优化方案)

## 1. 注册自己的 Github 的账户

注册账户以进行后续的系统定制操作。点击 github.com 网站右上角的 `Sign up` 按钮，根据提示完成注册。

## 2. 设置隐私变量 GITHUB_TOKEN 等

根据 [GitHub 文档](https://docs.github.com/zh/actions/security-guides/automatic-token-authentication#about-the-github_token-secret)，在每个工作流作业开始时，GitHub 会自动创建唯一的 `GITHUB_TOKEN` 机密以在工作流中使用。可以使用 `${{ secrets.GITHUB_TOKEN }}` 在工作流作业中进行身份验证。

在 Actions 中制作 [Armbian Docker](../.github/workflows/build-armbian-arm64-docker-image.yml) 镜像并推送到 Docker Hub 时，需要设置 `DOCKERHUB_USERNAME` 和 `DOCKERHUB_PASSWORD` 两个隐私变量。在自己的仓库页面，点击右上角的 `Settings` > `Secrets and variables` > `Actions` > `Repository secrets` > `New repository secret` 按钮，添加如下两个隐私变量：

- 变量名 `DOCKERHUB_USERNAME`：值是登录 Docker Hub 的用户名
- 变量名 `DOCKERHUB_PASSWORD`：值是登录 Docker Hub 的密码

## 3. Fork 仓库并设置工作流权限

打开仓库 https://github.com/ophub/amlogic-s9xxx-armbian ，点击右上角的 Fork 按钮，将仓库代码复制到自己的账户下。Fork 完成后，访问自己账户中的 amlogic-s9xxx-armbian 仓库，在右上角的 `Settings` > `Actions` > `General` > `Workflow permissions` 下选择 `Read and write permissions` 并保存。操作示意如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/167585338-841d3b05-8d98-4d73-ba72-475aad4a95a9.png width="300" />
</div>

## 4. 个性化 Armbian 系统定制文件说明

系统编译流程由 [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) 文件控制，workflows 目录下的其他 .yml 文件用于实现不同功能。编译时采用 Armbian 官方的最新代码进行实时编译，相关参数请参阅官方文档。

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

使用 `ophub` 打包 Armbian 时，可通过 `armbian_files` 参数将自定义文件添加或覆盖至 ophub 的 [common-files](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/build-armbian/armbian-files/common-files) 目录。目录结构必须与 Armbian 根目录保持一致，以确保文件被正确覆盖到固件中（例如：默认配置文件应存放于 `etc/default/` 子目录下）。示例如下：

```yaml
- name: Rebuild Armbian
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img.gz
    armbian_files: files
    ...
```

## 5. 编译系统

系统编译支持多种方式，包括手动编译、定时编译以及特定事件触发编译。

### 5.1 手动编译

在仓库导航栏中点击 Actions，依次点击 Build armbian > Run workflow > Run workflow 即可开始编译。全部流程约需 3 小时。操作示意如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163203938-e7762b09-e6b8-4cf5-b1f1-9c67c1a29953.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204044-9c7a7429-47ee-4fce-b7dd-e217bebf6133.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204127-6b16b558-7e78-4c22-a28a-7b37b5a34fa3.png width="300" />
</div>

### 5.2 定时编译

在 [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) 文件里，使用 Cron 设置定时编译，5 个不同位置分别代表的意思为 分钟 (0 - 59) / 小时 (0 - 23) / 日期 (1 - 31) / 月份 (1 - 12) / 星期几 (0 - 6)(星期日 - 星期六)。通过修改不同位置的数值来设定时间。系统默认使用 UTC 标准时间，请根据你所在国家时区的不同进行换算。

```yaml
schedule:
  - cron: '0 17 * * *'
```

### 5.3 自定义默认系统配置

默认系统配置信息记录在 [model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) 文件中，`BOARD` 名称必须唯一。

其中 `BUILD` 值为 `yes` 的是默认构建的设备，这些设备可直接使用对应系统。默认值为 `no` 的设备未包含在默认构建中，使用时需下载相同 `FAMILY` 的已构建系统。写入 `USB` 后，在电脑上打开 `USB 中的 boot 分区`，修改 `/boot/uEnv.txt` 文件中的 `FDT dtb 名称`，即可适配列表中的其他设备。

本地编译时通过 `-b` 参数指定，GitHub Actions 编译时通过 `armbian_board` 参数指定。`-b all` 表示构建所有 `BUILD` 为 `yes` 的设备。指定 `BOARD` 参数时，无论 `BUILD` 值为 `yes` 或 `no` 均可构建，例如：`-b r68s_s905x3-tx3_s905l3a-cm311`

### 5.4 使用逻辑卷扩大 Github Actions 编译空间

GitHub Actions 默认编译空间为 84G，去除系统和必要软件包后可用空间约 50G。编译全部固件时可能遇到空间不足的问题，可通过逻辑卷将编译空间扩大至约 110G。参照 [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) 文件中的方法，使用以下命令创建逻辑卷，并在编译时使用逻辑卷路径。

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

Armbian 系统 [Docker](https://hub.docker.com/u/ophub) 镜像的制作方法请参考 [armbian_docker](../compile-kernel/tools/script/docker) 制作脚本。

## 6. 保存系统

系统保存设置同样在 [.github/workflows/build-armbian-arm64-server-image.yml](../.github/workflows/build-armbian-arm64-server-image.yml) 文件中控制。编译完成的系统将通过脚本自动上传至 GitHub 官方的 Releases。

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

从仓库首页右下角的 Release 版块进入，选择与自身设备型号对应的系统。操作示意如下：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/163204798-0d98524c-73df-4876-8912-fcae2845fbba.png width="300" />
<img src=https://user-images.githubusercontent.com/68696949/163204879-4898babf-fa00-4e63-89ea-235129e2ce1d.png width="300" />
</div>

## 8. 安装 Armbian 到 EMMC

Amlogic、Rockchip 和 Allwinner 的安装方法各不相同。不同设备支持不同的存储介质，包括外置 microSD 卡、eMMC 和 NVMe 等，以下按设备类型分别介绍安装方法。首先从 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 下载对应设备的 Armbian 系统，解压为 .img 格式备用，然后根据设备类型参照以下相应章节进行安装。

安装完成后，将设备接入`路由器`，开机约 `2 分钟`后，在路由器中查找设备名称为 Armbian 的 `IP` 地址，使用 `SSH` 工具连接即可进行管理配置。默认用户名为 `root`，默认密码为 `1234`，默认端口为 `22`

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

各设备的安装方法如下。

#### 8.2.1 Radxa-Rock5B 的安装方法

Radxa-Rock5B 支持 microSD/eMMC/NVMe 等多种存储介质，安装方法因介质而异。下载 [rk3588_spl_loader_v1.08.111.bin 和 spi_image.img](https://github.com/ophub/u-boot/tree/main/u-boot/rockchip/rock5b) 文件备用。下载 [RKDevTool](https://github.com/ophub/kernel/releases/download/tools/Radxa_rock5b_RKDevTool_Release_v2.96__DriverAssitant_v5.1.1.tar.gz) 工具及驱动备用。下载 [Rufus](https://rufus.ie/) 或者 [balenaEtcher](https://www.balena.io/etcher/) 写盘工具备用。

##### 8.2.1.1 安装系统至 microSD

使用 Rufus 或 balenaEtcher 等工具将 Armbian 系统镜像写入 microSD，然后将 microSD 插入设备即可使用。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202972996-300f223b-f6f6-48af-86ca-bdc842e5017d.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202973216-85b1a21b-0763-4a36-8c57-84490e071fdd.png width="600" />
</div>

##### 8.2.1.2 安装系统至 eMMC

使用 microSD 卡安装：将 Armbian 系统镜像写入 microSD 卡，将 microSD 卡插入设备并启动，上传 `armbian.img` 镜像文件到 microSD 卡，使用 `dd` 命令将 Armbian 镜像写入 NVMe，命令如下：

```Shell
dd if=armbian.img  of=/dev/mmcblk1  bs=1M status=progress
```

- 使用 USB 转 eMMC 读卡器安装：将 eMMC 模块与电脑连接，使用 Rufus 或 balenaEtcher 等工具将 Armbian 系统镜像写入 eMMC，然后将 eMMC 插入设备即可使用。
- 使用 Maskrom 模式安装：关闭开发板电源。按住金色按钮。将 USB-A 转 C 型电缆插入 ROCK 5B C 型端口，另一端插入 PC。松开金色按钮。检查 USB 设备提示找到一个 MASKROM 设备。右键单击列表的空白区域，然后选择加载 `rock-5b-emmc.cfg` 配置文件（配置文件和 RKDevTool 在同一个目录下）。将 `rk3588_spl_loader_v1.08.111.bin` 和 `Armbian.img` 按下图所示设置，选择写入即可。

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954901-c829d74d-c75a-4fd3-9bd0-aa3cdf2b77b4.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954998-c08514e2-8760-4c0f-b5f7-0d30be635aa5.png width="600" />
</div>

##### 8.2.1.3 安装系统至 NVMe

ROCK-5B 主板配备 SPI 闪存，将引导加载程序写入 SPI 闪存后，可支持 SoC maskrom 模式不直接支持的启动介质（如 SATA、USB3 或 NVMe）。使用 NVMe 启动前需先写入 SPI 文件，方法如下：

关闭开发板电源，移除 MicroSD 卡、eMMC 模块等可启动设备。按住金色（或部分修订版上的银色）按钮，将 USB-A 转 Type-C 线缆插入 ROCK-5B 的 Type-C 端口，另一端插入 PC。松开按钮，检查 USB 设备是否发现 MASKROM 设备。在列表框中右键选择加载配置，在资源管理器中选择配置文件（配置文件和 RKDevTool 在同一目录下），根据下图选择 `rk3588_spl_loader_v1.08.111.bin` 和 `spi_image.img` 文件，点击写入即可，如下图所示：

<div style="width:100%;margin-top:40px;margin:5px;">
<img src=https://user-images.githubusercontent.com/68696949/202954823-3d3b1509-eedc-4192-91eb-017269c7f896.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961420-8316c96c-2796-43ed-b5ed-2fa5bfa1ddff.png width="600" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202954956-6646b7cf-6354-4dc8-b076-c3323cb89a43.png width="200" /><br />
<img src=https://user-images.githubusercontent.com/68696949/202961447-49c0941a-e233-4b2a-b96b-b47636ce3cf2.png width="600" />
</div>

- 使用读卡器安装：将 M.2 NVMe SSD 插入 M.2 NVMe SSD 转 USB3.0 读卡器以连接到主机。使用 Rufus 或 balenaEtcher 等工具将 Armbian 系统镜像写入 NVMe，然后将 NVMe 插入设备即可使用。
- 使用 microSD 卡安装：将 Armbian 系统镜像写入 microSD 卡，将 microSD 卡插入设备并启动，上传 `armbian.img` 镜像文件到 microSD 卡，使用 `dd` 命令将 Armbian 镜像写入 NVMe 中，命令如下：

```Shell
dd if=armbian.img  of=/dev/nvme0n1  bs=1M status=progress
```

#### 8.2.2 电犀牛 R66S 的安装方法

使用 Rufus 或 balenaEtcher 等工具将 Armbian 系统镜像写入 microSD，然后将 microSD 插入设备即可使用。

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

方法转载自 [cc747](https://post.smzdm.com/p/a4wkdo7l/) 的教程。刷机需要进入 Maskrom 模式。确保设备处于断电状态，拔掉所有线缆。用 USB 双公头线，一头插入我家云的 USB2.0 接口，一头插入电脑。用回形针插入 Reset 孔并按住不松开。插入电源线，等待数秒，直到 RKDevTool 下方出现`发现一个LOADER设备`后松开回形针。将 RKDevTool 切换到`高级功能`点击`进入Maskrom`按钮，提示`发现一个MASKROM设备`。右键添加项。

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

### 8.4 Docker 版本的 Armbian 安装方法

可以在 Ubuntu/Debian/Armbian 系统中使用 Docker 版本的 Armbian 镜像。这些镜像托管在 [Docker Hub](https://hub.docker.com/r/ophub) 上，可以直接下载使用。

提供了四个不同内核版本的 Armbian Docker 镜像：`armbian-trixie`，`armbian-bookworm`，`armbian-noble`，`armbian-resolute`。每个版本均提供 `arm64` 和 `amd64` 架构，可根据需要选择。

其中 armbian-trixie 基于 debian13，armbian-bookworm 基于 debian12，armbian-noble 基于 ubuntu24.04，armbian-resolute 基于 ubuntu26.04。

arm64 版本适用于 Amlogic/Rockchip/Allwinner 等平台架构的设备，amd64 版本适用于 x86_64 架构的电脑和服务器。

#### 8.4.1 安装 Docker 运行环境

```shell
curl -fsSL https://get.docker.com | sh
sudo usermod -aG docker $USER
sudo newgrp docker
```

#### 8.4.2 设置 macvlan 网络

```shell
# 查看已有的 docker 网络是否包含 macvlan 网络
docker network ls

# 如果没有 macvlan 网络，则创建 macvlan 网络
# 其中的网段、网关和网卡名称根据自己的实际网络修改
docker network create -d macvlan \
    --subnet=10.1.1.0/24 \
    --gateway=10.1.1.1 \
    -o parent=eth0 \
    macvlan
```

#### 8.4.3 运行 Armbian Docker 容器

以 `armbian-trixie:arm64` 镜像为例，说明如何运行 Armbian 容器。

```shell
# 以后台方式运行 Armbian 容器
# 其中的容器名称，IP 地址，镜像版本等根据自己的实际情况修改
docker run -itd --name=armbian-trixie \
    --privileged \
    --network macvlan \
    --ip 10.1.1.15 \
    --hostname=armbian-trixie \
    -e TZ=Asia/Shanghai \
    --restart unless-stopped \
    ophub/armbian-trixie:arm64

# 查看 Armbian 容器日志
docker logs -f armbian-trixie

# 进入 Armbian 容器
docker exec -it armbian-trixie bash

# 退出 Armbian 容器
exit

# 停止并删除 Armbian 容器
docker rm -f armbian-trixie
```

## 9. 编译 Armbian 内核

支持在 Ubuntu、Debian 或 Armbian 系统中编译内核，同时支持本地编译和 GitHub Actions 云编译，具体方法详见 [内核编译说明](../../compile-kernel/README.cn.md)。

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
- 在 GitHub Actions 云编译时，可通过 `kernel_patch` 参数指定内核补丁在仓库中的目录，例如 [kernel](https://github.com/ophub/kernel) 仓库中 [compile-beta-kernel.yml](https://github.com/ophub/kernel/blob/main/.github/workflows/compile-beta-kernel.yml) 的使用方法：

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

- 从 [Armbian](https://github.com/armbian/build) 和 [OpenWrt](https://github.com/openwrt/openwrt) 等仓库获取：例如 [armbian/patch/kernel](https://github.com/armbian/build/tree/main/patch/kernel/archive) 和 [openwrt/rockchip/patches-6.1](https://github.com/openwrt/openwrt/tree/main/target/linux/rockchip/patches-6.1)，[lede/rockchip/patches-5.15](https://github.com/coolsnowwolf/lede/tree/master/target/linux/rockchip/patches-5.15) 等，这些基于主线内核的仓库中的补丁通常可直接使用。
- 从 github.com 仓库的 commits 中获取：在对应的 `commit` 地址后添加 `.patch` 后缀即可生成补丁。

添加自定义内核补丁前，需先与上游内核源码仓库 [unifreq/linux-k.x.y](https://github.com/unifreq) 进行比对，确认该补丁尚未被合入，以避免冲突。通过测试的内核补丁，建议向 unifreq 维护的系列内核仓库提交。社区的每一份贡献，都将使 Armbian 和 OpenWrt 系统更加稳定可靠。

### 9.3 如何自定义编译驱动模块

Linux 主线内核中部分驱动尚未内置，可自行编译驱动模块。请选择支持主线内核的驱动，安卓驱动通常不兼容主线内核，无法编译。示例如下：

```shell
# 第一步，更新最新内核
# 由于早期的 header 文件不全，所以需要更新到最新的内核。
# 各内核版本要求不低于 5.10.222, 5.15.163, 6.1.100, 6.6.41。
armbian-sync
armbian-update -k 6.1


# 第二步，安装编译工具
mkdir -p /usr/local/toolchain
cd /usr/local/toolchain
# 下载编译工具
wget https://github.com/ophub/kernel/releases/download/dev/arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz
# 解压
tar -Jxf arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz
# 安装其他编译依赖包（可选项，可根据错误提示手动安装缺少项）
armbian-kernel -u


# 第三步，下载驱动，编译
# 下载驱动源码
cd ~/
git clone https://github.com/jwrdegoede/rtl8189ES_linux
cd rtl8189ES_linux
# 设置编译环境
gun_file="arm-gnu-toolchain-14.3.rel1-aarch64-aarch64-none-linux-gnu.tar.xz"
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
| -u       | 自动化        | stable/flippy/beta/rk3588/rk35xx/h6 | 设置使用的内核的 [tags 后缀](https://github.com/ophub/kernel/releases) |
| -k       | 最新版        | 内核版本       | 设置[内核版本](https://github.com/ophub/kernel/releases/tag/kernel_stable)  |
| -b       | yes          | yes/no        | 更新内核时自动备份当前系统使用的内核    |
| -d       | deb          | tar/deb       | 设置优先使用的内核包格式。若指定格式不存在，脚本将自动尝试另一种格式。如需编译自定义驱动推荐选择 `deb` 格式。 |
| -m       | no           | yes/no        | 使用主线 u-boot                    |
| -s       | 无           | 无/磁盘名称     | [SOS] 恢复 eMMC/NVMe/sdX 等磁盘中的系统内核 |
| -h       | 无           | 无             | 查看使用帮助                       |

举例: `armbian-update -k 5.15 -u stable -d deb`

通过 `-k` 参数指定内核版本号时，既可精确指定版本号（如 `armbian-update -k 5.15.50`），也可指定内核系列（如 `armbian-update -k 5.15`），指定系列时将自动使用该系列的最新版本。

更新内核时将自动备份当前使用的内核至 `/ddbr/backup` 目录，保留最近 3 个版本。若新内核不稳定，可随时恢复至备份版本：
```shell
# 进入备份的内核目录，如 6.6.12
cd /ddbr/backup/6.6.12
# 执行更新内核命令，会自动安装当前目录下的内核
armbian-update
```

[SOS]：当因异常情况导致更新不完整、系统无法从 eMMC/NVMe/sdX 启动时，可从 USB 等其他磁盘启动任意版本的 Armbian 系统，然后运行 `armbian-update -s` 命令将 USB 中的系统内核恢复至 eMMC/NVMe/sdX，实现系统救援。不指定磁盘参数时，默认从 USB 设备恢复 eMMC/NVMe/sdX 中的内核。若设备有多个磁盘，可精确指定目标磁盘名称，示例如下：

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

如果您因网络问题无法访问 github.com 进行在线更新，可以手动下载内核文件并上传至 Armbian 系统的任意目录。随后进入该目录，运行 `armbian-update` 命令即可执行本地安装。若当前目录下存在完整的内核文件集，系统将优先使用本地文件进行更新。内核支持 `tar` 和 `deb` 两种格式，各自所需的 4 个核心文件如下：

- `tar` 格式更新所需的 4 个文件为：`header-xxx.tar.gz`，`boot-xxx.tar.gz`，`dtb-xxx.tar.gz`，`modules-xxx.tar.gz`。
- `deb` 格式更新所需的 4 个文件为：`linux-image-xxx.deb`，`linux-dtb-xxx.deb`，`linux-headers-xxx.deb`，`linux-libc-dev-xxx.deb`。
- 无需移除其他无关的内核文件，即使它们同时存在也不会影响更新，系统能够精准识别所需文件。在设备支持的内核范围内，您可以自由切换版本，例如从 6.6.12 内核变更为 5.15.50 内核。

通过 `-r`/`-u`/`-b`/`-d` 等参数设置的自定义选项，可固化至配置文件 `/etc/ophub-release` 的相关参数中，无需每次手动输入。对应设置为：

```shell
# 自定义修改参数的赋值
-r  :  KERNEL_REPO='ophub/kernel'
-u  :  KERNEL_TAGS='stable'
-b  :  KERNEL_BACKUP='yes'
-d  :  DOWNLOAD_TYPE='deb'
```

## 11. 安装常用软件

登录 Armbian 系统 → 输入命令：

```shell
armbian-software
```

使用 `armbian-software -u` 命令可更新本地软件中心列表。根据用户在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中的反馈，逐步整合常用[软件](../build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf)，支持一键安装、更新和卸载。包括 `docker 镜像`、`桌面软件`、`应用服务` 等。详见更多[说明](armbian_software.md)。

根据所在国家或地区，使用 `armbian-apt` 命令选择合适的软件源以提高下载速度。例如，选择中国的清华大学源：

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

以下汇总了使用 Armbian 过程中可能遇到的常见问题。

### 12.1 每个盒子的 dtb 和 u-boot 对应关系表

支持的设备列表位于 Armbian 系统的配置文件 [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf)。

### 12.2 LED 屏显示控制说明

请查阅[说明](led_screen_display_control.md)

### 12.3 如何恢复原安卓 TV 系统

通常使用 `armbian-ddbr` 对设备的安卓 TV 系统进行备份和恢复。

此外，也可通过线刷方式将安卓系统写入 eMMC。安卓系统镜像可在 [Tools](https://github.com/ophub/kernel/releases/tag/tools) 中查找。

#### 12.3.1 使用 armbian-ddbr 备份恢复

建议在安装 Armbian 系统前，先备份设备原有的安卓 TV 系统，以便后续需要时恢复。从 `TF/SD/USB` 启动 Armbian 系统后，输入 `armbian-ddbr` 命令，根据提示输入 `b` 执行备份，备份文件存放路径为 `/ddbr/BACKUP-arm-64-emmc.img.gz`，请妥善保存。需要恢复时，将备份文件上传至 `TF/SD/USB` 设备的相同路径，输入 `armbian-ddbr` 命令，根据提示输入 `r` 执行恢复。

#### 12.3.2 使用 Amlogic 刷机工具恢复

- 通常情况下，重新接入电源后若能从 USB 启动，重新安装即可，可多次尝试。

- 若接入显示器后屏幕黑屏且无法从 USB 启动，则需对设备进行短接初始化。先将设备恢复至原安卓系统，再重新刷入 Armbian。首先下载并安装 [amlogic_usb_burning_tool](https://github.com/ophub/kernel/releases/tag/tools) 系统恢复工具。准备一条 [USB 双公头数据线](https://user-images.githubusercontent.com/68696949/159267576-74ad69a5-b6fc-489d-b1a6-0f8f8ff28634.png)，准备一个 [曲别针](https://user-images.githubusercontent.com/68696949/159267790-38cf4681-6827-4cb6-86b2-19c7f1943342.png)。

- 以 x96max+ 为例，确认主板上[短接点](https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg)的位置，下载设备的 [Android TV 系统包](https://github.com/ophub/kernel/releases/tag/tools)。其他常见设备的安卓 TV 系统及对应短接点示意图也可在此[下载查看](https://github.com/ophub/kernel/releases/tag/tools)。

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
   如果进度条没有走动，可以尝试插入电源。通常情况下不用电源支持供电，只 USB 双公头的供电即可满足刷机要求。
```

恢复出厂设置完成后，设备已还原为 Android TV 系统。后续安装 Armbian 的操作与首次安装相同，按原流程操作即可。

### 12.4 设置盒子从 USB/TF/SD 中启动

根据实际情况，选择初次安装或重新安装 Armbian 系统的对应方法。

#### 12.4.1 初次安装 Armbian 系统

- 将写好系统的 USB/TF/SD 插入设备。
- 开启开发者模式: 设置 → 关于本机 → 版本号 (如: X96max plus...), 快速点击版本号 5 次，直到系统显示 `开启开发者模式` 提示。
- 开启 USB 调试模式: 系统 → 高级选项 → 开发者选项 (设置 `开启USB调试` 为启用)。启用 `ADB` 调试。
- 安装 ADB 工具：下载 [adb](https://github.com/ophub/kernel/releases/tag/tools) 并解压，将 `adb.exe`，`AdbWinApi.dll`，`AdbWinUsbApi.dll` 三个文件复制到 `c://windows/` 目录下的 `system32` 和 `syswow64` 文件夹中。打开 `cmd` 命令面板，执行 `adb --version` 命令，若有输出则表示安装成功。
- 进入 `cmd` 命令模式，输入 `adb connect 192.168.1.137` 命令（其中 IP 根据设备实际情况修改，可在路由器管理界面查看），连接成功后将显示 `connected to 192.168.1.137:5555`
- 输入 `adb shell reboot update` 命令，设备将重启并从 USB/TF/SD 启动。通过浏览器访问系统 IP 地址或使用 SSH 连接即可进入系统。

#### 12.4.2 重新安装 Armbian 系统

- 正常情况下，将写好 Armbian 的 U 盘插入 USB 即可直接启动，USB 启动优先于 eMMC。
- 若个别设备无法从 U 盘启动，可将 eMMC 中 Armbian 系统 `/boot` 目录下的 `boot.scr` 文件重命名（如 `boot.scr.bak`），再插入 U 盘即可正常启动。

### 12.5 禁用红外接收器

系统默认启用红外接收器支持。若将设备用作服务器，建议禁用 IR 内核模块以防止误触关机。完全禁用 IR 的方法：添加以下内容

```shell
blacklist meson_ir
```

至 `/etc/modprobe.d/blacklist.conf` 并重启。

### 12.6 启动引导文件的选择

- 目前已知设备中，仅 `T95(s905x)` / `T95Z-Plus(s912)` / `BesTV-R3300L(s905l-b)` 等少数设备需要使用 `/bootfs/extlinux/extlinux.conf` 文件，系统已默认添加。其他设备如有需要，将系统写入 USB 后打开 `boot` 分区，将系统自带的 `/boot/extlinux/extlinux.conf.bak` 文件名称中的 `.bak` 删除即可使用。写入 eMMC 时，`armbian-install` 会自动检查并在 `extlinux.conf` 文件存在时自动创建。

- 其他设备仅需 `/boot/uEnv.txt` 即可启动，请勿修改 `extlinux.conf.bak` 文件。

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

查看设备中可用的网络接口。

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

查看设备现有的网络连接（包含已启用和未启用的连接）。新建连接时，建议使用不同于现有连接的名称。

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

在网络连接上修改 `MAC 地址` 以解决局域网 MAC 地址冲突问题。

###### 12.7.2.4.1 方法一：使用 `nmcli` 命令修改 MAC 地址

```shell
# 使用 nmcli connection show 命令查看网络连接名称
nmcli connection show
# 返回的结果中包含网口名字，例如'Wired connection 1'
NAME                UUID                                  TYPE      DEVICE
Wired connection 1  24d63dc7-c46f-3bf1-912f-1c33eb94338b  ethernet  eth0
lo                  35ca24e5-bdc0-4658-8ac8-435ee22e07f3  loopback  lo
Wired connection 2  59660b21-b460-30e0-8cb3-89b886556955  ethernet  --

# 设置变量
MYCON='Wired connection 1'    # 网络连接名称, 注意匹配网络接口类型
MYTYPE='802-3-ethernet'       # 网络接口类型 = 有线网卡        / 无线网卡
                              #            = 802-3-ethernet / 802-11-wireless
MYMAC='12:34:56:78:9A:BC'     # 设置新的 MAC 地址

# 执行修改
nmcli connection modify "${MYCON}" ${MYTYPE}.cloned-mac-address ${MYMAC}
nmcli connection up "${MYCON}"
ip -c a show "${MYCON}"
```

* 新建或修改部分网络参数, 网络连接可能会被断开, 并重新连接网络。
* 由于软硬件环境不同（盒子, 系统, 网络设备等）, 生效所需时间 `1-15` 秒左右, 更长时间未生效的建议检查软硬件环境。

###### 12.7.2.4.2 方法二：通过配置文件修改 MAC 地址

添加 Mac 地址覆盖配置文件。

```shell
sudo mkdir -p /etc/systemd/network/
sudo nano /etc/systemd/network/10-mac-override.link
```

添加以下内容：

```shell
[Match]
# 设置需要修改 MAC 地址的网络接口名称
OriginalName=eth0

[Link]
# 设置符合规范的 MAC 地址
MACAddress=02:55:66:77:88:99
```

重启后生效。

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

也可以通过修改 `/etc/sysctl.conf` 文件来禁用 IPv6：

```shell
# Disable IPv6 by default
net.ipv6.conf.all.disable_ipv6 = 1
net.ipv6.conf.default.disable_ipv6 = 1
net.ipv6.conf.lo.disable_ipv6 = 1
```

#### 12.7.3 如何启用无线

部分设备支持无线网络，启用方法如下：

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

部分设备支持蓝牙，启用方法如下：

```shell
# 安装蓝牙支持
armbian-config >> Network >> BT: Install Bluetooth support

# 重启系统
reboot
```

系统重启后，检查蓝牙驱动是否正常加载。桌面系统可通过菜单连接蓝牙设备，也可通过终端命令操作。

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

系统已内置自定义开机启动任务脚本，路径为 [/etc/custom_service/start_service.sh](../build-armbian/armbian-files/common-files/etc/custom_service/start_service.sh)，可根据个人需求在该脚本中添加自定义任务。

### 12.9 如何更新系统中的服务脚本

使用 `armbian-sync` 命令可一键将本地系统中的全部服务脚本更新至最新版本。

若 `armbian-sync` 更新失败，可能因命令版本过旧，可通过以下方法手动更新：

```shell
wget https://raw.githubusercontent.com/ophub/amlogic-s9xxx-armbian/main/build-armbian/armbian-files/common-files/usr/sbin/armbian-sync -O /usr/sbin/armbian-sync

chmod +x /usr/sbin/armbian-sync

armbian-sync
```

### 12.10 如何获取 eMMC 上的安卓系统分区信息

将 Armbian 系统写入 eMMC 时，需先确认设备的安卓系统分区表，确保数据写入安全区域，避免破坏原分区表导致系统无法启动。若写入了不安全的区域，可能无法启动或出现如下错误：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/68696949/187075834-4ac40263-52ae-4538-a4b1-d6f0d5b9c856.png">
</div>

#### 12.10.1 获取分区信息

若使用 2022.11 之后本仓库发布的 Armbian，可复制粘贴以下命令获取包含完整分区信息的网址（设备本身无需联网）

```shell
ampart /dev/mmcblk2 --mode webreport 2>/dev/null
```

*ampart 的 webreport 模式为 2023.02.03 发布的 v1.2 版本引入的，如果你使用上面的命令无输出，则可能为不支持直接输出网址的旧版，你可以转而使用下面这条命令：*

```shell
echo "https://7ji.github.io/ampart-web-reporter/?dsnapshot=$(ampart /dev/mmcblk2 --mode dsnapshot 2>/dev/null | head -n 1)&esnapshot=$(ampart /dev/mmcblk2 --mode esnapshot 2>/dev/null | head -n 1)"
```

生成的网址格式类似如下：

```shell
https://7ji.github.io/ampart-web-reporter/?esnapshot=bootloader:0:4194304:0%20reserved:37748736:67108864:0%20cache:113246208:754974720:2%20env:876609536:8388608:0%20logo:893386752:33554432:1%20recovery:935329792:33554432:1%20rsv:977272832:8388608:1%20tee:994050048:8388608:1%20crypt:1010827264:33554432:1%20misc:1052770304:33554432:1%20instaboot:1094713344:536870912:1%20boot:1639972864:33554432:1%20system:1681915904:1073741824:1%20params:2764046336:67108864:2%20bootfiles:2839543808:754974720:2%20data:3602907136:4131389440:4&dsnapshot=logo::33554432:1%20recovery::33554432:1%20rsv::8388608:1%20tee::8388608:1%20crypt::33554432:1%20misc::33554432:1%20instaboot::536870912:1%20boot::33554432:1%20system::1073741824:1%20cache::536870912:2%20params::67108864:2%20data::-1:4
```

将此网址复制到浏览器打开，即可查看格式清晰的 DTB 分区信息和 eMMC 分区信息：

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287642-e1b7be27-4d2c-4ac3-9fcc-15e06aebb97e.png">
<img width="800" alt="image" src="https://user-images.githubusercontent.com/24390674/216287654-d1929e21-d2b3-4fb6-bcf0-c454c88e21b9.png">
</div>

#### 12.10.2 分区信息分享

需要向他人分享分区信息时（如向本仓库汇报新设备情况或寻求帮助），请优先分享网址而非截图。若网址过长，可使用免费的短链接服务。

- 一方面，网页上的分区信息每次访问时均会动态生成，分区可写入的标注及表格格式等可能随时更新。
- 另一方面，截图中的分区参数无法被方便地复制用于计算。

此外，无需额外将参数整理为表格文件，网页表格布局已专门设计为可直接复制粘贴导入 Excel 或 LibreOffice Calc。

#### 12.10.3 分区信息解读

DTB 表记录了安卓 DTB 中每台设备**系统**期望的分区布局，通常以一个大小为自动填充的 `data` 分区结尾。因此，相同系统（必然包括同型号）的设备，DTB 分区布局一定相同。设备上实际的分区布局因 eMMC 容量不同而有所差异，但始终由 DTB 分区布局决定（即已知 DTB 分区布局 +eMMC 准确大小，必然可推知 eMMC 分区情况。 *上面的 DTB 分区信息和 eMMC 分区信息并非来自同一个盒子，你看出来了吗？*）。

eMMC 表是设备上实际的 eMMC 分区布局。其中每一行表示一块存储区域，该区域可能是一个分区，也可能是分区间的间隙（由于晶晨的设计决策，每个分区之间至少有 8M 的间隙，原计划留作他用，但直至 S905X4 仍未使用，造成空间浪费）。对应分区的行中，字体为黑色，且偏移和掩码栏均有数值；对应空隙的行中，字体为灰色，偏移和掩码栏没有数值，且分区名为 `gap` 。

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

若设备在使用 `armbian-install` 且 `-a` 参数（使用 [ampart](https://github.com/7Ji/ampart) 调整 eMMC 分区布局）为 `yes`（默认值）时失败，则该设备无法使用最优分区布局（即把 DTB 分区信息调整为只有 `data` ，再由此生成 eMMC 分区信息，然后将所有还存在的分区均向前挪动，如此一来，117M 向后的空间便均可使用），你需要在 [armbian-install](../build-armbian/armbian-files/common-files/usr/sbin/armbian-install) 中修改对应的分区信息。

此文件中，声明分区布局的关键参数有三个：`BLANK1`, `BOOT`, `BLANK2`。其中 `BLANK1` 表示从 eMMC 开头算起的不可使用的大小；`BOOT` 表示 `BLANK1` 之后用于存放内核、DTB 等文件的分区大小，建议不小于 256M；`BLANK2` 表示 `BOOT` 之后不可使用的大小；在此之后的空间会全部用来创建 `ROOT` 分区，储存整个系统中 `/boot` 挂载点以外的数据。三者均应为整数，且单位为MiB (1 MiB = 1024 KiB = 1024^2 Byte)

以上述不需要 `logo` 分区的情况为例，为充分利用所有可用空间，由于 `4M~36M` 区域过小无法用作 `BOOT`，只能将其计入 `BLANK1`。`100M~836M` 区域足以用作 `BOOT`，可将其 736M 全部分配给 `BOOT`。其后 `836M~837M` 的不可用区域计入 `BLANK2`，相应参数配置如下（以 `s905x3` 为例，其他 SoC 需修改对应代码块）：

```shell
# Set partition size (Unit: MiB)
elif [[ "${AMLOGIC_SOC}" == "s905x3" ]]; then
    BLANK1="100"
    BOOT="736"
    BLANK2="1"
```

### 12.11 如何制作 u-boot 文件

u-boot 是引导系统启动的关键文件。Amlogic、Allwinner 和 Rockchip 设备在源码获取和编译流程上略有差异。

#### 12.11.1 如何制作 Amlogic 设备的 u-boot 文件

由于 Amlogic 系列设备厂商大多不开源，需先从设备上提取 u-boot 相关文件，再进行编译。以下方法来自 [unifreq](https://github.com/unifreq) 分享的制作教程。

##### 12.11.1.1 如何提取 bootloader 和 dtb 文件

提取过程需使用 HxD 软件，可从 [官网下载链接](https://mh-nexus.de/en/downloads.php?product=HxD20) 或 [备份下载链接](https://github.com/ophub/kernel/releases/download/tools/HxDSetup.2.5.0.0.zip) 获取并安装。

在 `cmd` 面板中依次执行以下命令提取相关文件并下载至本地。

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

主线 u-boot 中最关键的是 acs.bin 文件，用于内存初始化，原厂 u-boot 位于系统起始 4MB 区域。使用前面获取的 `bootloader.bin` 提取 `acs.bin` 文件。

打开 HxD 软件，打开上面导出的 `bootloader.bin` 文件，`右键 - 选择范围`，起始位置 `F200`，长度 `1000`，选`十六进制`。

<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/187056711-1b58ce71-2a7d-4e9b-a976-e5f278edaa53.png">

复制选择的结果，然后新建文件，插入式粘贴，警告忽略，另存为 acs.bin 文件。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056725-0a0e60af-6a21-4a6b-a2d5-f3d46b438a6a.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056827-8419c738-3428-473e-9a95-ab7270170d98.png">
<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187056852-9f62f16a-f7f1-4c34-a2c2-78358d198f9a.png">
</div>

若 bootloader 已锁定，则该区域的代码将显示为乱码，无法使用。正常情况下，该区域应包含大量 `0`，`cfg` 会连续出现数次，并夹杂 `ddr` 相关字样，此类代码为有效可用数据。

##### 12.11.1.3 如何编译 u-boot 文件

制作 u-boot 需要 https://github.com/unifreq/amlogic-boot-fip 和 https://github.com/unifreq/u-boot 两个源码仓库，用于编译设备所需的 u-boot 文件。

在 amlogic-boot-fip 源码中，各机型仅 acs.bin 文件不同，其余文件通用。

<img width="600" alt="image" src="https://user-images.githubusercontent.com/68696949/187057209-c4716384-46ef-4922-9710-8da7ae6db1e4.png">

制作方法详见 https://github.com/unifreq/u-boot/tree/master/doc/board/amlogic 中的说明，选择对应设备型号进行编译测试。

根据 [unifreq](https://github.com/unifreq) 的方法制作 u-boot，需要使用设备的 acs.bin、dts 和 config 文件。其中安卓系统导出的 dts 无法直接转换为 Armbian 格式，需自行编写对应的 dts 文件。根据设备的具体硬件差异（如开关、LED、电源控制、TF 卡、SDIO WiFi 模块等），参照内核源码库中相似的 [dts](https://github.com/unifreq/linux-5.15.y/tree/main/arch/arm64/boot/dts/amlogic) 文件进行修改。

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

- 下载 [amlogic-boot-fip](https://github.com/unifreq/amlogic-boot-fip) 源码。在根目录创建 [x96max-plus](https://github.com/unifreq/amlogic-boot-fip/tree/master/x96max-plus) 目录，其中除自行制作的 `asc.bin` 文件外，其余文件可从其他目录复制。
- 下载 [u-boot](https://github.com/unifreq/u-boot) 源码。制作对应的 [x96max-plus_defconfig](https://github.com/unifreq/u-boot/blob/master/configs/x96max-plus_defconfig) 文件放入 [configs](https://github.com/unifreq/u-boot/tree/master/configs) 目录。制作对应的 [meson-sm1-x96-max-plus-u-boot.dtsi](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus-u-boot.dtsi) 和 [meson-sm1-x96-max-plus.dts](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/meson-sm1-x96-max-plus.dts) 文件放入 [arch/arm/dts](https://github.com/unifreq/u-boot/tree/master/arch/arm/dts) 目录，并编辑此目录中的 [Makefile](https://github.com/unifreq/u-boot/blob/master/arch/arm/dts/Makefile) 文件，添加 `meson-sm1-x96-max-plus.dtb` 文件的索引。
- 进入 u-boot 源码根目录，按照文档 https://github.com/unifreq/u-boot/blob/master/doc/board/amlogic/x96max-plus.rst 中的步骤操作。

最终生成两类文件：u-boot 根目录下的 `u-boot.bin` 为 `/boot` 目录使用的精简版 u-boot（对应仓库中的 [overload](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) 目录）；在 `fip` 目录下的 `u-boot.bin` 和 `u-boot.bin.sd.bin` 是 `/usr/lib/u-boot/` 目录下使用的完整版 u-boot 文件（对应仓库中的 [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) 目录），完整版两个文件相差 512 字节，较大的文件在头部填充了 512 字节的 0。

<div style="width:100%;margin-top:40px;margin:5px;">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189039426-c127631f-77ca-4fcb-9fb6-4220045d712b.png">
<img width="400" alt="image" src="https://user-images.githubusercontent.com/68696949/189029320-e43a4cc9-b4b5-4de4-92fe-b17bd29020d0.png">
</div>

💡提示：写入 eMMC 测试前，请先阅读 12.3 节的救砖方法。务必确认已掌握短接点位置、备有原厂 .img 格式的安卓系统文件，并已成功测试短接刷机流程后，方可进行写入测试。

#### 12.11.2 如何制作 Rockchip 设备的 u-boot 文件

Rockchip 设备的大部分厂商已开源 u-boot 源码，可在厂商源码仓库中获取并编译。同时，一些开源贡献者也分享了便捷的 u-boot 编译脚本，以下通过实例介绍几种编译方法。

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

通过在 [radxa/build](https://github.com/radxa/build) 源码的 `board_configs.sh` 和 `mk-uboot.sh` 中添加更多选项，可编译其他设备的 u-boot 文件。例如编译 [Beelink-IPC-R(rk3588)](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415#issuecomment-1508234307) 设备的方法。

##### 12.11.2.2 如何使用 cm9vdA 的 u-boot 制作脚本

cm9vdA 的开源项目 [cm9vdA/build-linux](https://github.com/cm9vdA/build-linux) 提供了编译 u-boot 和 kernel 的脚本及使用方法。以下为部分 Rockchip 设备 u-boot 编译的过程记录，摘录供参考。

- 编译 Lenovo-Leez-P710(rk3399) 设备的 u-boot：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609#issuecomment-1681494735)
- 编译 DLFR100(rk3399) 设备的 u-boot：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522#issuecomment-1622919423)
- 编译 ZYSJ(rk3399) 设备的 u-boot：[Link](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380#issuecomment-1539325464)


### 12.12 内存大小识别错误

若内存大小识别不正确（4G 内存识别为 1-2G 属异常，识别为 3.7G 属正常），可尝试手动复制 `/boot/UBOOT_OVERLOAD` 文件（注意是`复制`而非`改名`，改名后安装或更新操作将导致无法启动）：USB 中使用时保存为 `/boot/u-boot.ext`，eMMC 中使用时保存为 `/boot/u-boot.emmc`。

除排查内存识别问题外，请勿手动复制 u-boot 文件，错误操作可能导致无法启动或其他问题。

### 12.13 如何反编译 dtb 文件

部分新设备不在当前支持列表中（或存在硬件变体），可通过反编译 dtb 文件调整相关参数进行适配尝试。

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

Amlogic 设备在 `/boot/uEnv.txt` 文件中配置。Rockchip 和 Allwinner 设备在 `/boot/armbianEnv.txt` 文件中配置（添加至 `extraargs` 或 `extraboardargs` 参数）。使用 `/boot/extlinux/extlinux.conf` 的设备在该文件中配置。每次修改后需重启生效。

- 例如 `Home Assistant Supervisor` 仅支持 `docker cgroup v1`，而当前 docker 默认安装 v2 版本。如需切换至 v1，可在 cmdline 中添加 `systemd.unified_cgroup_hierarchy=0` 参数，重启后即可切换至 `docker cgroup v1`。

- 通过在 cmdline 中添加 `mmc_core.max_freq=50000000` 设置，可以限制 eMMC 最大频率为 `50MHz`。某些 S905L2 盒子 eMMC 在高频（HS200/HS400, 100MHz+）下不稳定，降频可解决启动失败/随机崩溃，读写错误和不稳定等问题。

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

为设备构建 Armbian 系统需要 `设备配置文件`、`系统文件`、`u-boot 文件`和`流程控制文件` 四部分，具体添加方法如下：

#### 12.15.1 添加设备配置文件

在配置文件 [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) 中，根据设备的测试结果添加对应配置信息。其中 `BUILD` 值为 `yes` 的是默认构建的设备，对应的 `BOARD` 值`必须唯一`，这些设备可直接使用默认构建的 Armbian 系统。

默认值为 `no` 的设备未包含在默认构建中，使用时需下载相同 `FAMILY` 的 Armbian 系统。写入 `USB` 后，在电脑上打开 `USB 中的 boot 分区`，修改 `/boot/uEnv.txt` 文件中的 `FDT dtb 名称`，即可适配列表中的其他设备。

#### 12.15.2 添加系统文件

通用文件放在：`build-armbian/armbian-files/common-files` 目录下，各平台通用。

平台文件分别放在 `build-armbian/armbian-files/platform-files/<platform>` 目录下，[Amlogic](../build-armbian/armbian-files/platform-files/amlogic)、[Rockchip](../build-armbian/armbian-files/platform-files/rockchip) 和 [Allwinner](../build-armbian/armbian-files/platform-files/allwinner) 各自共享平台文件。其中 `bootfs` 目录存放 /boot 分区文件，`rootfs` 目录存放 Armbian 系统文件。

若个别设备有特殊配置需求，在 `build-armbian/armbian-files/different-files` 目录下创建以 `BOARD` 命名的独立目录，按需建立 `bootfs` 目录添加 `/boot` 分区文件，或建立 `rootfs` 目录添加系统文件。目录结构以 Armbian 系统中的实际路径为准，可用于添加新文件或覆盖通用文件和平台文件中的同名文件。

#### 12.15.3 添加 u-boot 文件

`Amlogic` 系列的设备，共用 [bootloader](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader) 文件和 [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/overload) 文件，新增文件分别放入对应目录。`bootloader` 文件在系统构建时将自动添加至 Armbian 系统的 `/usr/lib/u-boot` 目录，`u-boot` 文件自动添加至 `/boot` 目录。

`Rockchip` 和 `Allwinner` 系列的设备，为每个设备添加以 `BOARD` 命名的独立 [u-boot](https://github.com/ophub/u-boot/tree/main/u-boot) 文件目录，对应的系列文件放在此目录中。

构建 Armbian 镜像时，这些 u-boot 文件将根据 [/etc/model_database.conf](../build-armbian/armbian-files/common-files/etc/model_database.conf) 中的配置，由 rebuild 脚本写入对应的 Armbian 镜像文件中。

能使用标准 U-Boot 文件的设备，建议优先直接使用。部分设备可能无法编译或获取适用的 U-Boot，若这类设备已能正常运行 Ubuntu 等 Linux 系统，可尝试保留引导相关的关键分区来安装 Armbian 或 OpenWrt。通常需要保留的关键分区包括 `bootloader`、`reserved` 和 `env`。

这些分区可备份后，在制作新的 Armbian 或 OpenWrt 镜像时写回相应位置。制作完成的包含原系统引导分区的镜像，可直接使用 `dd` 命令写入 eMMC，也可使用相应系统的内置工具安装，例如 Armbian 的 `armbian-install` 命令，或 OpenWrt 的 `晶晨宝盒` 插件。

目前采用此方法的设备有 [oes(a311d)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2666)，[oes-plus(s922x)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3029)，[oec-turbo(rk3566)](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2736)，以下以 `oes(a311d)` 设备为例说明具体操作流程。

##### 12.15.3.1 查看分区信息布局情况

```shell
ampart /dev/mmcblk2

# 得到的分区信息如下：
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

##### 12.15.3.2 备份关键分区

```shell
dd if=/dev/mmcblk2 of=bootloader.bin bs=1M count=4 skip=0
dd if=/dev/mmcblk2 of=reserved.bin bs=1M count=8 skip=36
dd if=/dev/mmcblk2 of=env.bin bs=1M count=1 skip=628
```

备份的文件放在 [u-boot](https://github.com/ophub/u-boot) 仓库对应的目录 [u-boot/amlogic/bootloader/a311d-oes](https://github.com/ophub/u-boot/tree/main/u-boot/amlogic/bootloader/a311d-oes) 里面。

注意到 reserved 分区为 64MB，但仅备份了 8MB。原因在于 `oes(a311d)` 设备上 `reserved` 分区的前 8MB 为关键数据，后续 56MB 为空白，无需备份。具体查看方法：

```shell
# 首先，备份 reserved 分区完整的 64MB 大小的文件：
dd if=/dev/mmcblk2 of=reserved_64M.bin bs=1M count=64 skip=36

# 然后，把我们备份出来的 reserved_64M.bin 文件的前 8MB 提取出来：
dd if=reserved_64M.bin of=reserved_first_8M.bin bs=1M count=8

# 接下来，查看十六进制内容：
hexdump -C reserved_first_8M.bin | less

# 现在，我们查看返回结果最后几行的内容：
0071ffd0  4c 5e a8 1f fc 5b 5b 98  ae ef b0 97 0c 3b e8 c2  |L^...[[......;..|
0071ffe0  c8 e0 b2 74 3d 67 d5 3d  24 7b 63 b7 c7 73 f5 d8  |...t=g.=${c..s..|
0071fff0  a1 b8 38 a7 57 d6 b4 b5  e8 1c ba c0 07 0f f5 79  |..8.W..........y|
00720000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00800000
```

分析输出结果，最后一个包含非零数据的行的地址是 `0071fff0`。从地址 `00720000` 开始，所有内容都变成了 `00`（零）。hexdump 工具使用 （`*`） 来表示重复的行，这意味着从 `00720000` 一直到 `00800000` (即8MB的末尾) 都是零。把效数据地址 `0x00720000` 换算成十进制是 `7,471,104` 字节，也就是 `7471104 / 1024 / 1024 = 7.125 MB` 大小，因此取整备份 8MB 即可。env 分区同理，仅前 80KB 为有效数据，其余为空白，因此仅备份 1MB。

##### 12.15.3.3 添加特殊分区写入文件

具体实现细节请参考文件 [/etc/armbian-board-release.conf](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/build-armbian/armbian-files/different-files/a311d-oes/rootfs/etc/armbian-board-release.conf) 中定义的 `write_board_bootloader` 函数。该函数在镜像重构（rebuild）过程中被调用。此外，该配置文件也是功能强大的设备定制中心。您不仅可以通过 `skip_mb="700"` 等参数来精确控制镜像分区的布局与大小，还可以添加自定义脚本，以实现对内核或其他系统文件的特殊处理。所有针对特定设备的高级定制操作均集中在此文件中统一管理，确保配置清晰高效。

#### 12.15.4 添加流程控制文件

在 [yml 工作流控制文件](../.github/workflows/build-armbian-arm64-server-image.yml) 的 `armbian_board` 中添加对应的 `BOARD` 选项，使其支持在 GitHub Actions 中使用。

### 12.16 如何解决写入 eMMC 时 I/O 错误的问题

部分设备可从 USB/SD/TF 正常启动 Armbian，但写入 eMMC 时会出现 I/O 写入错误，例如 [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues/989) 中的案例，报错内容如下：

```shell
[  284.338449] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.341544] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.446972] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.450074] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
[  284.497746] I/O error, dev mmcblk2, sector 0 op 0x1:(WRITE) flags 0x800 phys_seg 1 prio class 2
[  284.500871] Buffer I/O error on dev mmcblk2, logical block 0, lost async page write
```

此时可调整 dtb 中的工作模式速度和频率以稳定存储读写。SDR 模式下频率为速度的 2 倍，DDR 模式下频率等于速度。可用模式如下：

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

通常将 `&sd_emmc_c` 的频率从 `max-frequency = <200000000>;` 下调至 `max-frequency = <100000000>;` 即可解决。若无效可继续下调至 `50000000` 进行测试，同时可通过调整 `&sd_emmc_b` 配置 `USB/SD/TF`，或使用 `sd-uhs-sdr` 限速。可通过修改 dts 文件并[编译](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel)生成测试文件，也可参照 `12.13 节` 的方法对现有 dtb 文件进行反编译修改。反编译 dtb 文件时使用十六进制值，其中十进制的 `200000000` 对应的十六进制为 `0xbebc200`，十进制的 `100000000` 对应的十六进制为 `0x5f5e100`，十进制的 `50000000` 对应的十六进制为 `0x2faf080`，十进制的 `25000000` 对应的十六进制为 `0x17d7840`。也可以参照 `12.14 节` 的 cmdline 设置方法，在 cmdline 中添加 `mmc_core.max_freq=50000000` 参数来限制 eMMC 的最大频率来解决读写错误和不稳定等问题。

除软件层面的优化外，还可通过[硬件升级](https://github.com/ophub/amlogic-s9xxx-armbian/issues/998)和[动手改造](https://www.right.com.cn/forum/thread-901586-1-1.html)解决。

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

重启 Armbian 进行测试。若声音仍无输出，可能是设备使用了旧版 conf 对应的音频输出路径，需在 /usr/bin/g12_sound.sh 中注释掉 `L137-L142` 的新配置（该配置主要适用于 G12B/S922X，旧版 G12A/S905X2 及基于 G12A 的 SM1/S905X3 大多不兼容），并取消 `L130-L134` 旧配置的注释。

### 12.18 如何编译 boot.scr 文件

Armbian 系统 `/boot` 目录下，`boot.scr` 是系统引导文件，由 `boot.cmd` 源码文件编译生成。修改 `boot.cmd` 后，通过 `mkimage` 命令重新编译即可生成新的 `boot.scr`。

通常无需修改这两个文件，如有调整需求可参考以下方法。

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

在软件中心 `armbian-software` 中选择 `201` 安装桌面。安装过程中会询问是否开启远程桌面，输入 `y` 即可启用。远程桌面默认端口为 `3389`，可根据需要自定义端口：

```shell
sudo nano /etc/xrdp/xrdp.ini
# 修改为自定义端口，例如 5000
port=5000
```

### 12.20 TCP 拥塞控制优化方案

针对不同性能的设备，建议采用差异化的网络栈配置以获得最佳体验。请根据您的设备实际情况，编辑配置文件 `/etc/sysctl.conf` 并修改以下两行：
- 千兆设备（高性能/现代架构）：建议使用 `fq + bbr` 组合，以最大化吞吐量并提升抗丢包能力。
- 百兆设备（低性能/老旧架构）：建议使用 `fq_codel + cubic` 组合，以降低 CPU 负载并确保低延迟稳定性。

```shell
# 方案 A：千兆设备/高性能推荐 (Gigabit/High Performance)
net.core.default_qdisc = fq
net.ipv4.tcp_congestion_control = bbr

# 方案 B：百兆设备/低性能推荐 (100M/Low Performance)
# net.core.default_qdisc = fq_codel
# net.ipv4.tcp_congestion_control = cubic
```
