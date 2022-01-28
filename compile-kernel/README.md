# Compile the kernel

View Chinese description  |  [查看中文说明](README.cn.md)

Compile a custom kernel as needed. This kernel can be used in [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian) and [OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt) systems. Commonly used in systems made with the [unifreq](https://github.com/unifreq/openwrt_packit) standard.

## Local compile command description

| Parameter | Meaning | Description |
| ---- | ---- | ---- |
| -d | Defaults | Compile all kernels with default configuration. |
| -k | Kernel | Specify [kernel](https://cdn.kernel.org/pub/linux/kernel/v5.x/) name, such as `-k 5.4.170`. Multiple kernels use `_` to connect, such as `- k 5.15.13_5.4.170` |
| -a | AutoKernel | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find whether there is a newer version of the kernel specified in `-k` such as `5.4.170` of the `5.4` series. If there is the latest version after `5.4.170`, it will be automatically replaced with the latest version . When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| -n | CustomName | Set the kernel custom signature. The default value is `-meson64-dev` and the generated kernel is `5.4.170-meson64-dev`. Do not include spaces when setting a custom signature. |
| -r | Repo | Specifies the source code repository for the compiled kernel. Defaults to `unifreq` . You can choose the source code of `kernel.org` and the kernel source of `github.com` code repository. For example `-r kernel.org` or `-r unifreq`, etc., When using the kernel source code repository of `github.com`, the parameter format can be set to the three-item combination of `owner/repo@branch` , The owner name `owner` in the parameters is a required parameter, the kernel source code repository name `/repo` and the repository branch name `@branch` are optional parameters. When only the owner name `owner` parameter is specified, it will automatically match kernel source code repositories whose owner's name is in `linux-5.x.y` format and branch is `main`. If the repository name or branch name is different, use a combination, such as `owner@branch` or `owner/repo` or `owner/repo@branch` |

- `sudo ./recompile -d -k 5.4.170`: Use the default configuration, and use the `-k` parameter to specify the kernel version to be compiled, and use `_` to link when multiple versions are compiled at the same time.
- `sudo ./recompile -d -k 5.4.170 -a true`: Use the default configuration, and use the `-a` parameter to set whether to automatically upgrade to the latest kernel of the same series when compiling the kernel.
- `sudo ./recompile -d -k 5.4.170 -n -xiaoming`: Use the default configuration, and use the `-n` parameter to set the kernel custom signature.
- `sudo ./recompile -d -k 5.4.170 -r kernel.org`: Use the default configuration, and set the kernel source code repository through the `-r` parameter.
- `sudo ./recompile -d -k 5.15.13_5.4.170 -a true -n -xiaoming -r kernel.org`: Use the default configuration, and set through multiple parameters.

💡Tip: Recommended Use the [.config](https://github.com/unifreq/arm64-kernel-configs) template and source code of `unifreq` to compile the `latest version` of [5.4](https://github.com/unifreq/linux-5.4.y) / [5.10](https://github.com/unifreq/linux-5.10.y) / [5.15](https://github.com/unifreq/linux-5.15.y), etc. `Other series or historical versions` can be compiled with [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/). You can also use other kernel source code repositories.

- ### Local compilation

1. Install the necessary packages (The script has only been tested on Ubuntu-20.04-x86_64)

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/ubuntu-2004-server)
```

2. Clone the repository to local: `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

3. Enter the root directory of `~/amlogic-s9xxx-armbian`, and then run `sudo ./recompile -d -k 5.4.170` and other specified parameter commands to compile the kernel. The script will automatically download and install the compilation environment and kernel source code and make all settings. The packaged kernel file is stored in the `compile-kernel/output` directory.

- ### Compile with GitHub Action

1. The configuration of the Workflows file is in the [.yml](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/.github/workflows) file.

2. Select ***`Compile Armbian For Amlogic`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

- ### Only import GitHub Action to compile the kernel

You can call the kernel compilation script of this repository through Actions in your github.com repository to make a personalized kernel. For details, see Using Templates [compile-kernel.yml](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/.github/workflows/compile-kernel.yml). The code is as follows:

```yaml
- name: Compile the kernel for Amlogic s9xxx
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.15.13_5.4.170
    kernel_auto: true
    kernel_sign: -meson64-dev
```

- GitHub Action Input parameter description

The relevant parameters correspond to the `local compilation commands`, please refer to the above description.

| Parameter | Meaning | Description |
| ---- | ---- | ---- |
| build_target   | kernel   | Fixed parameter `kernel`, set the compilation target to the kernel. |
| kernel_repo    | unifreq  | Specifies the source code repository for the compiled kernel. The default is `unifreq` . Function reference `-r` |
| kernel_version | 5.15.13_5.4.170 | Specify [kernel](https://cdn.kernel.org/pub/linux/kernel/v5.x/) name, such as `5.4.170`. Function reference `-k` |
| kernel_auto | true | Set whether to automatically adopt the latest kernel version of the same series. The default value is `true`. Function reference `-a` |
| kernel_sign | -meson64-dev | Set the kernel custom signature. The default is `-meson64-dev`. Function reference `-n` |
| kernel_config | null | The default uses the configuration templates in the [compile-kernel/tools/config](tools/config) directory. You can set the directory where the compiled kernel configuration files are stored in your repository, such as `kernel/config_path` . The kernel configuration templates of each series stored in this directory must start with the name of `config-5.x`. For example, templates for compiling `5.4` series kernels can be named with various names starting with `config-5.4`, such as `config-5.4`, `config-5.4.174` or `config-5.4.174-xiaoming`, etc., When there are multiple files starting with `config-5.4`, the file with the highest version number will be used. Default: `compile-kernel/tools/config` |

- GitHub Action Output variable description

| Parameter                         | For example                  | Description               |
|-----------------------------------|------------------------------|---------------------------|
| ${{ env.PACKAGED_OUTPUTTAGS }}    | 5.15.13_5.4.170              | The name of the compiled kernel   |
| ${{ env.PACKAGED_OUTPUTPATH }}    | compile-kernel/output        | kernel files storage path  |
| ${{ env.PACKAGED_OUTPUTDATE }}    | 2021.04.13.1058              | compile date                    |
| ${{ env.PACKAGED_STATUS }}        | success                      | Compile status. success / failure |

## Other instructions

1. After the kernel is compiled, it will be stored in the `compile-kernel/output` directory. These kernel files will be automatically cleared from the system compiled with the current kernel. You can upload these kernel files to any directory of `Armbian` system, such as `/opt/5.4.170` directory, and execute the `armbian-update` command in this kernel directory to install the kernel.

2. Please perform a custom kernel test on the `USB/SD/TF` device first, and then install it in the official environment after debugging.

