# 编译内核

查看英文说明 | [View English description](README.md)

根据需要，编译自定义内核。此内核可用于 [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian) 和 [OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt) 系统，在以 [unifreq](https://github.com/unifreq/openwrt_packit) 标准制作的同类系统中可通用。

## 本地编译命令说明

| 参数 | 含义 | 说明 |
| ---- | ---- | ---- |
| -d | Defaults | 使用默认配置 |
| -k | Kernel | 指定 [kernel](https://cdn.kernel.org/pub/linux/kernel/v5.x/) 名称，如 `-k 5.4.170` . 多个内核使用 `_` 进行连接，如 `-k 5.15.13_5.4.170` |
| -a | AutoKernel | 设置是否自动采用同系列最新版本内核。当为 `true` 时，将自动查找在 `-k` 中指定的内核如 `5.4.170` 的 `5.4` 同系列是否有更新的版本，如有 `5.4.170` 之后的最新版本时，将自动更换为最新版。设置为 `false` 时将编译指定版本内核。默认值：`true` |
| -n | CustomName | 设置内核自定义签名。默认值为 `-meson64-dev` ，生成的内核名称为 `5.4.170-meson64-dev` 。设置自定义签名时请勿包含空格。 |
| -r | Repo | 指定编译内核的源代码仓库。默认为 `unifreq` 。可选择 `kernel.org` 的源码和 `github.com` 的内核源代码仓库。例如 `-r kernel.org` 或 `-r unifreq` 或 `-r chewitt/linux@amlogic-5.4.y` 。当使用 `github.com` 的内核源代码仓库时，可设置参数格式为 `owner/repo@branch` 三项组合，参数中的所有者名称 `owner` 为必选参数，内核源代码仓库名称 `/repo` 和 仓库的分支名称 `@branch` 为可选参数。当仅指定所有者名称 `owner` 参数时，将自动匹配所有者的名称为 `linux-5.x.y` 格式且分支为 `main` 的内核源代码仓库。如果仓库名称或分支名称不同，请使用组合方式指定，如 `owner@branch` 或 `owner/repo` 或 `owner/repo@branch` |

- `sudo ./recompile -d -k 5.4.170` : 使用默认配置，并通过 `-k` 进行指定需要编译的内核版本，多个版本同时编译时使用 `_` 进行连接。
- `sudo ./recompile -d -k 5.4.170 -a true` : 使用默认配置，并通过 `-a` 参数设置编译内核时，是否自动升级到同系列最新内核。
- `sudo ./recompile -d -k 5.4.170 -n -leifeng` : 使用默认配置，并通过 `-n` 参数设置内核自定义签名。
- `sudo ./recompile -d -k 5.4.170 -r kernel.org` : 使用默认配置，并通过 `-r` 参数设置内核源代码仓库。
- `sudo ./recompile -d -k 5.15.13_5.4.170 -a true -n -leifeng -r kernel.org` : 使用默认配置，并通过多个参数进行设置。

💡提示：推荐使用 `unifreq` 的 [.config](https://github.com/unifreq/arm64-kernel-configs) 模板和源码编译 [5.4](https://github.com/unifreq/linux-5.4.y) / [5.10](https://github.com/unifreq/linux-5.10.y) / [5.15](https://github.com/unifreq/linux-5.15.y) 等内核的 `最新版本` 。`其他系列或历史版本` 可以使用 [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/) 编译。你也可以使用其他内核源代码仓库。

- ### 本地编译

1. 安装必要的软件包（脚本仅在 Ubuntu-20.04-x86_64 下做过测试）

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/ubuntu-2004-server)
```

2. 克隆仓库到本地 `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

3. 首先在 `~/amlogic-s9xxx-armbian/compile-kernel` 目录下创建 `kernel` 目录，用于存放编译的内核源码。如采用 [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/) 的源码进行编译，请下载对应的内核如 `linux-5.4.170.tar.xz` 并解压到对应的 `compile-kernel/kernel/linux-5.4.170` 目录下；如采用 [unifreq](https://github.com/unifreq) 的源码进行编译，请克隆指定内核系列的源码如 `git clone --depth 1 https://github.com/unifreq/linux-5.4.y compile-kernel/kernel/linux-5.4.y` 到对应的目录下。完成后进入对应的内核如 `compile-kernel/kernel/linux-5.4.170` 的目录下，复制对应的内核系列的 [.config](tools/config) 模板到当前内核目录（如复制 config-5.4.170 文件，并重命名为 `.config`），并运行个性化配置选择命令 `make menuconfig` 进行自定义选择，完成后保存，会在内核目录下生成自定义的内核 `.config` 配置文件。

4. 进入 `~/amlogic-s9xxx-armbian` 根目录，然后运行 `sudo ./recompile -d -k 5.4.170` 等指定参数命令即可编译内核。打包好的内核文件保存在 `compile-kernel/output` 目录里。

- ### 使用 GitHub Action 进行编译

1. 关于 Workflows 文件的配置在 [.yml](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/.github/workflows) 文件里。

2. 在 [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 ***`Compile the armbian kernel`*** ，点击 ***`Run workflow`*** 按钮即可编译。

- ### 仅单独引入 GitHub Action 编译内核

可以在你的 github.com 仓库中通过 Actions 调用本仓库的内核编译脚本，制作个性化内核。详见使用模板 [compile-kernel.yml](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/.github/workflows/compile-kernel.yml) 。代码如下:

```yaml
- name: Compile the kernel for Amlogic s9xxx
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.15.13_5.4.170
    kernel_auto: true
    kernel_sign: -meson64-dev
```

- GitHub Action 输入参数说明

| 参数                   | 默认值                  | 说明                                            |
|------------------------|------------------------|------------------------------------------------|
| build_target         | kernel                 | 固定参数 `kernel`，设置编译目标为内核。 |
| kernel_repo | unifreq | 指定编译内核的源代码仓库。默认为 `unifreq` 。可选择 `kernel.org` 的源码和 `github.com` 的内核源代码仓库。例如 `kernel.org` 或 `unifreq` 或 `chewitt/linux@amlogic-5.4.y` 。当使用 `github.com` 的内核源代码仓库时，可设置参数格式为 `owner/repo@branch` 三项组合，参数中的所有者名称 `owner` 为必选参数，内核源代码仓库名称 `/repo` 和 仓库的分支名称 `@branch` 为可选参数。当仅指定所有者名称 `owner` 参数时，将自动匹配所有者的名称为 `linux-5.x.y` 格式且分支为 `main` 的内核源代码仓库。如果仓库名称或分支名称不同，请使用组合方式指定，如 `owner@branch` 或 `owner/repo` 或 `owner/repo@branch` |
| kernel_version | 5.15.13_5.4.170 | 指定 [kernel](https://cdn.kernel.org/pub/linux/kernel/v5.x/) 名称，如 `5.4.170` . 多个内核使用 `_` 进行连接，如 `5.15.13_5.4.170` |
| kernel_auto | true | 设置是否自动采用同系列最新版本内核。当为 `true` 时，将自动查找在指定的内核如 `5.4.170` 的 `5.4` 同系列是否有更新的版本，如有 `5.4.170` 之后的最新版本时，将自动更换为最新版。设置为 `false` 时将编译指定版本内核。默认值：`true` |
| kernel_sign | -meson64-dev | 设置内核自定义签名。默认值为 `-meson64-dev` ，生成的内核名称为 `5.4.170-meson64-dev` 。设置自定义签名时请勿包含空格。 |

- GitHub Action 输出变量说明

| 参数                               | 默认值                        | 说明                       |
|-----------------------------------|------------------------------|---------------------------|
| ${{ env.PACKAGED_OUTPUTTAGS }}    | 5.15.13_5.4.170              | 编译好的内核的名称           |
| ${{ env.PACKAGED_OUTPUTPATH }}    | ${PWD}/compile-kernel/output | 编译完成的内核所在文件夹的路径  |
| ${{ env.PACKAGED_OUTPUTDATE }}    | 2021.04.13.1058              | 编译日期                    |
| ${{ env.PACKAGED_STATUS }}        | success / failure            | 编译状态。成功 / 失败        |

## 其他说明

1. 内核编译文件检查的优先级：如果 `compile-kernel/kernel` 目录下有指定内核的文件夹如 `linux-5.4.170` 时，将使用本地源码进行编译；当没有指定内核的文件夹，但有指定内核的压缩文件如 linux-5.4.170.tar.xz 时，将自动解压并进行编译；当本地没有指定内核时，将自动从服务器下载并编译。

2. 如果本地的内核目录如 `compile-kernel/kernel/linux-5.4.170` 中没有 [.config](tools/config) 文件，将自动从 unifreq 分享的模板中复制相同内核系列的配置文件。

3. 内核编译完成后，将会按照 `unifreq` 分享的内核文件的组织方式自动打包成 6 个内核文件，并存放在 `compile-kernel/output` 目录下。这些内核文件会自动从当前内核编译的系统中自动清除。可以将这 6 个内核文件上传到 `Armbian` 系统任意目录，如 `/opt/5.4.170` 目录下，并在此内核目录内执行 `armbian-update` 命令进行内核安装。内核中的 `headers` 文件默认安装在 `/use/local/include` 目录下。

4. 如果当前系统中已经安装了相同名称的内核如 `5.4.170-meson64-dev` ，将会自动停止编译，因为打包时会删除本地相同名称的内核文件，这么做会造成系统瘫痪。

5. 在内核测试时，请在 `USB/TF` 设备上进行测试，请不要贸然写入 `EMMC` 分区，避免成砖；在没有熟练地掌握救砖方法之前，请不要进行自定义内核测试；请不要在正式生产环境中测试自定义内核。

