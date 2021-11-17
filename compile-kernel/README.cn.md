# 编译 Armbian 内核

查看英文说明 | [View English description](README.md)

根据需要，为自己的 Armbian 系统编译自定义内核。

## 编译命令说明

- `sudo ./recompile -k 5.4.150` : 通过 -k 进行指定需要编译的内核版本，多个版本同时编译时使用 `_` 进行连接。
- `sudo ./recompile -k 5.4.150 -a true` : 通过 -a 参数设置编译内核时，是否自动升级到同系列最新内核。
- `sudo ./recompile -k 5.4.150 -n leifeng` : 通过 -n 参数设置内核自定义签名。
- `sudo ./recompile -k 5.4.150 -r kernel.org` : 通过 -r 参数设置编译源码的下载站。
- `sudo ./recompile -k 5.10.70_5.4.150 -a true -n leifeng -r kernel.org` : 通过多个参数进行设置。

| 参数 | 含义 | 说明 |
| ---- | ---- | ---- |
| -k | Kernel | 指定 [kernel](https://cdn.kernel.org/pub/linux/kernel/v5.x/) 名称，如 `-k 5.4.150` . 多个内核使用 `_` 进行连接，如 `-k 5.10.70_5.4.150` |
| -a | AutoKernel | 设置是否自动采用同系列最新版本内核。当为 `true` 时，将自动查找在 `-k` 中指定的内核如 `5.4.150` 的 `5.4` 同系列是否有更新的版本，如有 `5.4.150` 之后的最新版本时，将自动更换为最新版。设置为 `false` 时将编译指定版本内核。默认值：`true` |
| -n | CustomName | 设置内核自定义签名。默认值为 `-meson64-beta` ，生成的内核名称为 `5.4.150-meson64-beta` 。设置自定义签名时请勿包含空格。 |
| -r | Repo | 指定内核编译源码的下载站。可选项为 [kernel.org](https://www.kernel.org/) 和 [flippy](https://github.com/unifreq) ，默认为 `kernel.org` |

💡提示：当前已开通使用 `kernel.org` 的源码进行编译，使用 `flippy` 的源码进行编译正在调试中。

- ### 使用 GitHub Action 进行编译

1. 关于 Workflows 文件的配置在 [.yml](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/.github/workflows) 文件里。

2. 在 [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 ***`Compile the armbian kernel`*** ，点击 ***`Run workflow`*** 按钮即可编译。

- ### 本地编译

1. 请在你的盒子中安装 Armbian 系统，并安装以下依赖环境。

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/armbian-kernel-server)
```

2. 克隆仓库到本地 `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

3. 到 [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/) 下载内核并解压到 `compile-kernel/kernle` 目录下。进入内核如 `compile-kernel/kernle/linux-5.4.150` 的目录下，运行个性化配置选择命令 `make menuconfig` 进行选择，完成后保存即可生成自定义内核的 `.config` 配置文件。

4. 进入 `~/amlogic-s9xxx-armbian` 根目录，然后运行 `sudo ./recompile -k 5.4.150` 命令即可编译内核。打包好的内核文件保存在 `compile-kernel/output` 目录里。

## 其他说明

1. 如果 `compile-kernel/kernle` 目录下有指定内核的文件夹如 `linux-5.4.150` 时，将使用本地源码进行编译；当没有指定内核的文件夹，但有指定内核的压缩文件如 linux-5.4.150.tar.xz 时，将自动解压并进行编译；当本地没有指定内核时，将自动从服务器下载并编译。

2. 如果本地的内核目录如 `compile-kernel/kernle/linux-5.4.150` 中没有 [.config](tools/config) 文件，将自动从 flippy 分享的模板中复制相同内核系列的配置文件。

3. 如果在 `x86_64` 的环境下使用 `Ubuntu` 等系统进行内核编译，由于交叉编译 `Armbian` 内核时无法生成 [uInitrd](tools/uInitrd) 文件，在打包内核文件时，将自动使用仓库中的相同内核系列的文件进行替代，这在使用中可能会不稳定。在内核测试时，请在 `USB/TF` 设备上进行测试，不要贸然写入 `EMMC` 分区，避免成砖。

4. 内核编译完成后，将会按照 flippy 分享的内核文件的组织方式自动打包成 5 个内核文件，并存放在 `compile-kernel/output` 目录下。这些内核文件会自动从当前内核编译的 Armbian 系统中清除。如果你想在当前系统安装，可进入对应的内核目录如 `compile-kernel/output/5.4.150` 下，执行指定内核安装命令如 `armbian-update 5.4.150` 进行安装。

