# 内核编译和使用说明

查看英文说明 | [View English description](README.md)

此内核可用于 `Armbian` 和 `OpenWrt` 系统。例如 [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt), [flippy-openwrt-actions](https://github.com/ophub/flippy-openwrt-actions) 和 [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) 等项目。可以在编译固件时集成，也可以安装到已有的系统中使用。

你可以根据需要对内核的配置进行调整，如添加驱动和补丁。也可以根据心情编译具有特殊意义的个性化签名内核，如 `5.10.95-happy-new-year`, `5.10.96-beijing-winter-olympics`, `5.10.99-valentines-day` 等等。

## 内核存储库

在 [ophub/kernel](https://github.com/ophub/kernel) 的 [Releases](https://github.com/ophub/kernel/releases) 里有已经编译好的内核。

## 本地编译

- ### 在 Ubuntu 系统下运行

1. 克隆仓库到本地 `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. 安装必要的软件包（如 Ubuntu 24.04）

进入 `~/amlogic-s9xxx-armbian` 根目录，然后运行安装命令:

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-24.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2404-build-armbian-depends)
```

3. 进入 `~/amlogic-s9xxx-armbian` 根目录，然后运行 `sudo ./recompile -k 5.15.100` 等指定参数命令即可编译内核。脚本会自动下载安装编译环境和内核源码并做好全部设置。编译好的内核文件保存在当前源码的 `compile-kernel/output` 目录，或使用 `-h` 参数设置的目录里。

- ### 在 Armbian 系统下运行

可以在 [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 系统中编译内核，也可以在 Ubuntu/Debian 等系统中通过 [Docker](https://hub.docker.com/u/ophub) 容器运行 Armbian 系统编译内核。Armbian 系统的 Docker 镜像的制作方法可以参考 [armbian_docker](./tools/script/docker) 制作脚本。

1. 更新本地编译环境和配置文件：`armbian-kernel -u`

2. 编译内核：运行 `armbian-kernel -k 5.15.100` 等指定参数命令即可编译内核。脚本会自动下载安装编译环境和内核源码并做好全部设置。打包好的内核文件保存在 `/opt/kernel/compile-kernel/output` 目录里。

- ### 本地编译参数说明

| 参数    | 含义        | 说明                             |
| ------ | ----------- | ------------------------------- |
| -r     | Repository  | 指定编译内核的源代码仓库。可选择 `github.com` 的内核源代码仓库。例如 `-r unifreq` 等。可设置参数格式为 `owner/repo@branch` 三项组合，参数中的所有者名称 `owner` 为必选参数，内核源代码仓库名称 `/repo` 和 仓库的分支名称 `@branch` 为可选参数。当仅指定所有者名称 `owner` 参数时，将自动匹配所有者的名称为 `linux-5.x.y` 格式且分支为 `main` 的内核源代码仓库。如果仓库名称或分支名称不同，请使用组合方式指定，如 `owner@branch` 或 `owner/repo` 或 `owner/repo@branch`。默认值：`unifreq` |
| -k     | Kernel      | 指定 kernel 名称，如 `-k 5.15.100`。多个内核使用 `_` 进行连接，如 `-k 5.15.100_5.15.50`。使用 `-k all` 代表编译全部主线内核，当前等价于 `-k 5.10.y_5.15.y_6.1.y_6.6.y_6.12.y_6.18.y`，内核列表会随着上游内核源码仓库 [unifreq](https://github.com/unifreq) 的维护情况动态调整。 |
| -a     | AutoKernel  | 设置是否自动采用同系列最新版本内核。当为 `true` 时，将自动查找在 `-k` 中指定的内核如 `5.15.100` 的同系列是否有更新的版本，如有 `5.15.100` 之后的最新版本时，将自动更换为最新版。设置为 `false` 时将编译指定版本内核。默认值：`true` |
| -m     | MakePackage | 设置制作内核的包列表。当设置为 `all` ，将制作 `Image, modules, dtbs` 的全部文件。当设置值为 `dtbs` 时仅制作 3 个 dtbs 文件。默认值：`all` |
| -f     | configFlavor | 设置从内核仓库 [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) 里下载指定配置文件 `config-*` 文件到本地 [tools/config](tools/config) 目录。当本地已经有内核版本对应的配置文件并且未设置下载时将忽略。可选项是内核仓库里存在的[目录名](https://github.com/ophub/kernel/tree/main/kernel-config/release)，例如：[ `stable` / `rk3588` / `rk35xx` / `h6` ]。默认值：`stable` |
| -p     | AutoPatch   | 设置是否使用自定义内核补丁。当设置为 `true` 时将使用 [tools/patch](tools/patch) 目录下的内核补丁，详细说明参考[内核补丁添加方法](../documents/README.cn.md#9-编译-armbian-内核)。默认值：`false` |
| -n     | CustomName  | 设置内核自定义签名。当设置为 `-ophub` ，生成的内核名称为 `5.15.100-ophub` 。设置自定义签名时请勿包含空格。默认值：`-ophub` |
| -t     | Toolchain   | 设置编译内核的工具链。可选项：`clang / gcc / gcc-<version>`。默认值：`gcc` |
| -z     | CompressFormat | 设置内核中 initrd 使用的压缩格式。可选项：`xz / gzip / zstd / lzma`。默认值：`xz` |
| -d     | DeleteSource | 设置内核编译结束后是否删除内核源码。可选项：`true / false`。默认值：`false` |
| -s     | SilentLog   | 设置内核编译时是否使用静默模式减少日志输出。可选项：`true / false`。默认值：`false` |
| -l     | EnableLog   | 设置是否将内核编译过程记录到日志文件：`/var/log/kernel_compile_*.log`。可选项：`true / false`。默认值：`false` |
| -c     | CcacheClear | 设置是否在编译前清除 ccache。可选项：`true / false`。默认值：`false` |
| -h     | DockerHostpath | 设置内核编译时 Docker 容器在宿主机的挂载路径。默认使用当前目录。 |
| -i     | DockerImage | 设置编译内核的 Docker 容器镜像。默认值：`ophub/armbian-trixie:arm64` |

- `sudo ./recompile` : 使用默认配置编译内核。
- `sudo ./recompile -k 5.15.100` : 使用默认配置，并通过 `-k` 进行指定需要编译的内核版本，多个版本同时编译时使用 `_` 进行连接。
- `sudo ./recompile -k 5.15.100 -f stable` : 使用默认配置，并通过 `-k 5.15.100` 参数指定需要编译的内核版本，通过 `-f stable` 参数指定从内核仓库下载 `stable` 目录下的配置文件。
- `sudo ./recompile -k 5.15.100 -a true` : 使用默认配置，并通过 `-a` 参数设置编译内核时，是否自动升级到同系列最新内核。
- `sudo ./recompile -k 5.15.100 -n -ophub` : 使用默认配置，并通过 `-n` 参数设置内核自定义签名。
- `sudo ./recompile -k 5.15.100 -m dtbs` : 使用默认配置，并通过 `-m` 参数指定仅制作 dtbs 文件。
- `sudo ./recompile -k 5.15.100_6.1.10 -a true -n -ophub` : 使用默认配置，并通过多个参数进行设置。

💡 提示：推荐使用 `unifreq` 的 [linux-6.1.y](https://github.com/unifreq/linux-6.1.y), [linux-5.15.y](https://github.com/unifreq/linux-5.15.y), [linux-5.10.y](https://github.com/unifreq/linux-5.10.y) 等仓库的内核源代码进行编译，他针对相关盒子添加了驱动和补丁。推荐使用 [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) 中的模板，已经根据相关盒子进行了预配置，可以在此基础上进行个性化定制。

## 使用 GitHub Actions 编译内核

1. 在 [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 **_`Compile the kernel`_** ，点击 **_`Run workflow`_** 按钮即可编译。

2. 详见使用模板 [compile-kernel-via-docker.yml](../.github/workflows/compile-kernel-via-docker.yml) 。代码如下:

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 6.12.y_6.18.y
    kernel_auto: true
    kernel_sign: -yourname
```

💡 注意: 如果你 `fork` 仓库并进行了修改，使用时须将 Actions 的 `用户名` 改成你自己的仓库，例如：

```yaml
uses: YOUR-REPO/amlogic-s9xxx-armbian@main
```

- ### GitHub Action 输入参数说明

相关参数与`本地编译命令`相对应，请参考上面的说明。

| 参数               | 默认值            | 说明                                                      |
|-------------------|------------------|-----------------------------------------------------------|
| build_target      | kernel           | 固定参数 `kernel`，设置编译目标为内核。                        |
| kernel_source     | unifreq          | 指定编译内核的源代码仓库。默认值为 `unifreq` 。功能参考 `-r`      |
| kernel_version    | 6.12.y_6.18.y    | 指定 kernel 名称，如 `5.15.100`。功能参考 `-k`                |
| kernel_auto       | true             | 设置是否自动采用同系列最新版本内核。默认值为 `true`。功能参考 `-a`  |
| kernel_package    | all              | 设置制作内核的包列表。默认值为 `all`。功能参考 `-m`             |
| kernel_sign       | -ophub           | 设置内核自定义签名。默认值为 `-ophub`。功能参考 `-n`             |
| kernel_toolchain  | gcc              | 设置编译内核的工具链。默认值为 `gcc`。功能参考 `-t`             |
| config_flavor     | stable           | 设置从内核仓库 [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) 里下载指定配置文件 `config-*` 文件到本地 [tools/config](tools/config) 目录。当本地已经有内核版本对应的配置文件，并且未设置此参数进行指定配置文件下载时将忽略下载。可选项是内核仓库里存在的[目录名](https://github.com/ophub/kernel/tree/main/kernel-config/release)，例如：[ `stable` / `rk3588` / `rk35xx` / `h6` ]。默认值：`stable`。如果同时设置了 `kernel_config` 参数时，最终采用的是 `config_flavor` 的配置文件。这 2 个参数是二选一，你可以通过不同参数选择使用 [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) 的内核配置文件，或者使用你仓库的配置文件。功能参考 `-f` |
| kernel_config     | false            | 默认使用 [tools/config](tools/config) 目录下的配置模板。你可以设置编译内核的配置文件在你仓库中的存放目录，如 `kernel/config_path` 。该目录下存储的配置模板必须以内核的主版本命名，如`config-5.10`、`config-5.15`等。 |
| kernel_patch      | false            | 设置自定义内核补丁文件在你仓库中的目录。如果设置此参数，在编译前会自动从你仓库指定的目录下载内核补丁文件。未设置则忽略。 |
| auto_patch        | false            | 设置是否使用自定义内核补丁。默认值为 `false`。功能参考 `-p` |
| compress_format   | xz               | 设置内核中 initrd 使用的压缩格式。默认值为 `xz`。功能参考 `-z` |
| delete_source     | false            | 设置内核编译结束后是否删除内核源码。默认值为 `false`。功能参考 `-d` |
| silent_log        | false            | 设置内核编译时是否使用静默模式减少日志输出。默认值为 `false`。功能参考 `-s` |
| enable_log        | false            | 设置是否将内核编译过程记录到日志文件：`/var/log/kernel_compile_*.log`。默认值：`false`，功能参考 `-l` |
| ccache_clear      | false            | 设置是否在编译前清除 ccache。默认值为 `false`。功能参考 `-c` |
| docker_hostpath   | .                | 设置内核编译时 Docker 容器在宿主机的挂载路径。默认使用当前目录。功能参考 `-h` |
| docker_image      | ophub/armbian-trixie:arm64 | 设置编译内核的 Docker 容器镜像。功能参考 `-i` |

- ### GitHub Action 输出变量说明

上传到 `Releases` 需要给仓库设置 `Workflow 读写权限`，详见[使用说明](../documents/README.cn.md#2-设置隐私变量-github_token)。

| 参数                            | 默认值                 | 说明                           |
| ------------------------------ | --------------------- | ------------------------------ |
| ${{ env.PACKAGED_OUTPUTTAGS }} | 6.12.y_6.18.y         | 编译好的内核的名称                |
| ${{ env.PACKAGED_OUTPUTPATH }} | compile-kernel/output | 编译完成的内核所在文件夹的路径      |
| ${{ env.PACKAGED_OUTPUTDATE }} | 04.13.1058            | 编译日期（月.日.时分）            |
| ${{ env.PACKAGED_STATUS }}     | success               | 编译状态：success / failure     |

## 内核使用说明

此内核可用于 `Armbian` 和 `OpenWrt` 系统。以 ophub 的项目进行举例说明。

### 在 Armbian 系统中使用

下面分别对在编译 Armbian 固件时集成，以及如何安装到已有的系统中使用进行说明。

- #### 使用内核编译 Armbian 固件

编译 Armbian 固件支持本地化操作，也支持使用 github.com 的 Actions 在线编译。其中本地化编译时的使用方法详见：[本地化打包](../README.cn.md#本地化打包), 使用 Actions 在线编译的方法详见：[使用 GitHub Actions 进行编译](../README.cn.md#使用-github-actions-进行编译)

- #### 将内核安装到已有的 Armbian 系统

可以使用 `armbian-update` 命令将编译好的内核安装到已有的 Armbian 系统中，具体操作方法详见：[更新 Armbian 内核](../README.cn.md#更新-armbian-内核)

- #### 自定义编译驱动模块

在 linux 主线内核里，有些驱动尚未支持，可以自定义编译驱动模块。具体操作方法详见：[编译驱动模块](../documents/README.cn.md#93-如何自定义编译驱动模块)

### 在 OpenWrt 系统中使用

下面分别对在 OpenWrt 系统中编译固件时集成和安装到已有的系统中使用进行介绍。

- #### 使用内核编译 OpenWrt 固件

编译 OpenWrt 固件支持本地化操作，也支持使用 github.com 的 Actions 在线编译。其中本地化编译时的使用方法详见：[本地化打包](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.cn.md#本地化打包), 使用 Actions 在线编译的方法详见：[使用 Github Actions 进行编译](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.cn.md#使用-github-actions-进行编译)

- #### 将内核安装到已有的 OpenWrt 系统

可以使用 [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic/blob/main/README.cn.md) 插件将编译好的内核安装到已有的 OpenWrt 系统中，具体操作方法详见：[升级 OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.cn.md#升级-openwrt)
