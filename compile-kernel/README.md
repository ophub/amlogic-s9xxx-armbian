# Kernel Compilation and Usage Guide

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

The compiled kernel is compatible with both `Armbian` and `OpenWrt` systems. It can be used in projects such as [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt), [flippy-openwrt-actions](https://github.com/ophub/flippy-openwrt-actions), and [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit). The kernel can be integrated during firmware compilation or installed into an existing system.

You can adjust the kernel configuration as needed—for example, adding drivers and patches. You can also compile personalized kernels with custom signatures, such as `5.10.95-happy-new-year`, `5.10.96-beijing-winter-olympics`, `5.10.99-valentines-day`, and so on.

## Kernel Repository

Pre-compiled kernels are available in the [Releases](https://github.com/ophub/kernel/releases) of [ophub/kernel](https://github.com/ophub/kernel).

## Local Compilation

- ### Running on Ubuntu System

1. Clone the repository to local: `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install the required packages (for Ubuntu 24.04):

Navigate to the `~/amlogic-s9xxx-armbian` root directory, then run the installation command:

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-24.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2404-build-armbian-depends)
```

3. Navigate to the `~/amlogic-s9xxx-armbian` root directory, then run `sudo ./recompile -k 5.15.100` or other specified parameters to compile the kernel. The script automatically downloads and installs the build environment and kernel source code, and completes all necessary configuration. The compiled kernel files are saved in the `compile-kernel/output` directory within the source tree, or in the directory specified by the `-h` option.

- ### Running on Armbian System

You can compile the kernel directly in an [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian/releases) system, or run the Armbian system within a [Docker](https://hub.docker.com/u/ophub) container on Ubuntu/Debian to compile the kernel. For instructions on creating the Armbian Docker image, refer to the [armbian_docker](./tools/script/docker) build script.

1. Update the local build environment and configuration files: `armbian-kernel -u`

2. Compile the kernel: Run `armbian-kernel -k 5.15.100` or other specified parameters to compile the kernel. The script automatically downloads and installs the build environment and kernel source code, and completes all necessary configuration. The compiled kernel files are saved in the `/opt/kernel/compile-kernel/output` directory.

- ### Local Compilation Parameter Reference

| Parameter | Meaning      | Description |
| --------- | ----------- | ----------- |
| -r        | Repository  | Specifies the kernel source code repository for compilation. You can choose a kernel source code repository from `github.com`, e.g., `-r unifreq`. The parameter format supports a combination of three components: `owner/repo@branch`. The owner name `owner` is required; the repository name `/repo` and branch name `@branch` are optional. When only the owner name is specified, the script automatically matches a repository named `linux-5.x.y` on the `main` branch under that owner. If the repository name or branch name differs, specify them in combination, e.g., `owner@branch`, `owner/repo`, or `owner/repo@branch`. Default: `unifreq` |
| -k        | Kernel      | Specifies the kernel version, e.g., `-k 5.15.100`. Multiple kernels are separated with `_`, e.g., `-k 5.15.100_5.15.50`. Using `-k all` compiles all mainline kernels, currently equivalent to `-k 5.10.y_5.15.y_6.1.y_6.6.y_6.12.y_6.18.y`. The kernel list is dynamically adjusted based on the maintenance status of the upstream source repository [unifreq](https://github.com/unifreq). |
| -a        | AutoKernel  | Sets whether to automatically adopt the latest version within the same kernel series. When set to `true`, the script checks if a newer version exists in the same series as the kernel specified via `-k` (e.g., `5.15.100`), and automatically switches to the latest version if available. When set to `false`, the specified kernel version is compiled as-is. Default: `true` |
| -m        | MakePackage | Sets the package list for building the kernel. When set to `all`, all files including `Image, modules, dtbs` are built. When set to `dtbs`, only the 3 DTB files are generated. Default: `all` |
| -f        | configFlavor | Specifies which configuration file `config-*` to download from the kernel repository [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) to the local [tools/config](tools/config)￼ directory. If a configuration file matching the kernel version already exists locally and this parameter is not set, the download is skipped. Available options correspond to the [directory names](https://github.com/ophub/kernel/tree/main/kernel-config/release) in the kernel repository, e.g., `stable` / `rk3588` / `rk35xx` / `h6`. Default: `stable`. |
| -p        | AutoPatch   | Sets whether to apply custom kernel patches. When set to `true`, patches from the [tools/patch](tools/patch) directory are applied. For details, refer to [how to add kernel patches](../documents/README.md#9-compiling-armbian-kernel). Default: `false` |
| -n        | CustomName  | Sets the custom signature appended to the kernel version. For example, setting `-ophub` produces a kernel named `5.15.100-ophub`. Do not include spaces in the custom signature. Default: `-ophub` |
| -t        | Toolchain   | Sets the toolchain for kernel compilation. Options: `clang / gcc / gcc-<version>`. Default: `gcc` |
| -z        | CompressFormat | Sets the compression format for initrd in the kernel. Options: `xz / gzip / zstd / lzma`. Default: `xz` |
| -d        | DeleteSource | Sets whether to delete the kernel source code after compilation. Options: `true / false`. Default: `false` |
| -s        | SilentLog   | Sets whether to enable silent mode to reduce log output during compilation. Options: `true / false`. Default: `false` |
| -l        | EnableLog   | Sets whether to log the kernel compilation process to a file: `/var/log/kernel_compile_*.log`. Options: `true / false`. Default: `false` |
| -c        | CcacheClear | Sets whether to clear the ccache before compilation. Options: `true / false`. Default: `false` |
| -h     | DockerHostpath | Sets the host mount path for kernel compilation in Docker. Default: current directory. |
| -i     | DockerImage | Sets the Docker container image used for kernel compilation. Default: `ophub/armbian-trixie:arm64` |

- `sudo ./recompile`: Compile the kernel using the default configuration.
- `sudo ./recompile -k 5.15.100`: Use the default configuration and specify the kernel version to be compiled through `-k`. Multiple versions are connected using `_` for simultaneous compilation.
- `sudo ./recompile -k 5.15.100 -f stable` : Use the default configuration and specify the kernel version to be compiled through `-k 5.15.100`, and specify the configuration files to be downloaded from the `stable` directory of the kernel repository through `-f stable`.
- `sudo ./recompile -k 5.15.100 -a true`: Use the default configuration and set whether to automatically upgrade to the latest kernel of the same series during kernel compilation through the `-a` parameter.
- `sudo ./recompile -k 5.15.100 -n -ophub`: Use the default configuration and set the kernel custom signature through the `-n` parameter.
- `sudo ./recompile -k 5.15.100 -m dtbs`: Use the default configuration and specify only the creation of dtbs files through the `-m` parameter.
- `sudo ./recompile -k 5.15.100_6.1.10 -a true -n -ophub`: Use the default configuration and set multiple parameters.

💡 Tip: We recommend using the kernel source code from repositories such as [linux-6.1.y](https://github.com/unifreq/linux-6.1.y), [linux-5.15.y](https://github.com/unifreq/linux-5.15.y), [linux-5.10.y](https://github.com/unifreq/linux-5.10.y) of `unifreq` for compilation, who added drivers and patches for related boxes. It is recommended to use the template in [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release), which has been pre-configured according to the related boxes and can be customized based on this.

## Compile Kernel Using GitHub Actions

1. In the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select **_`Compile the kernel`_** and click the **_`Run workflow`_** button to compile.

2. See the use of the template [compile-kernel-via-docker.yml](../.github/workflows/compile-kernel-via-docker.yml). The code is as follows:

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 6.12.y_6.18.y
    kernel_auto: true
    kernel_sign: -yourname
```

’ Note: If you `fork` the repository and make modifications, you must change the Actions `username` to your own repository when using it, for example:

```yaml
uses: YOUR-REPO/amlogic-s9xxx-armbian@main
```

- ### GitHub Action Input Parameters

These parameters correspond to the `local compilation commands`. Refer to the descriptions above.

| Parameter        | Default Value | Description                                                     |
|------------------|---------------|-----------------------------------------------------------------|
| build_target     | kernel        | Fixed parameter `kernel`. Sets the compilation target to the kernel. |
| kernel_source    | unifreq       | Specifies the kernel source code repository for compilation. Default: `unifreq`. Refer to `-r` for details. |
| kernel_version   | 6.12.y_6.18.y | Specifies the kernel version, e.g., `5.15.100`. Refer to `-k` for details. |
| kernel_auto      | true          | Sets whether to automatically adopt the latest version within the same kernel series. Default: `true`. Refer to `-a` for details. |
| kernel_package   | all           | Sets the package list for building the kernel. Default: `all`. Refer to `-m` for details. |
| kernel_sign      | -ophub        | Sets the custom signature for the kernel. Default: `-ophub`. Refer to `-n` for details. |
| kernel_toolchain | gcc           | Sets the toolchain for kernel compilation. Default: `gcc`. Refer to `-t` for details. |
| config_flavor    | stable        | Specifies which configuration file `config-*` to download from the kernel repository [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release) to the local [tools/config](tools/config)￼ directory. If a configuration file matching the kernel version already exists locally and this parameter is not set, the download is skipped. Available options correspond to the [directory names](https://github.com/ophub/kernel/tree/main/kernel-config/release) in the kernel repository, e.g., `stable` / `rk3588` / `rk35xx` / `h6`. Default: `stable`. If `kernel_config` is also set, the configuration file specified by `config_flavor` takes precedence. These two parameters are mutually exclusive: use one to select kernel configuration files from [ophub/kernel](https://github.com/ophub/kernel/tree/main/kernel-config/release), or the other to use files from your own repository. Refer to `-f` for details. |
| kernel_config    | false         | By default, uses the configuration templates in the [tools/config](tools/config) directory. You can specify the directory in your repository containing kernel configuration files, e.g., `kernel/config_path`. The configuration files in that directory must follow the naming convention based on major kernel version, e.g., `config-5.10`, `config-5.15`, etc. |
| kernel_patch     | false         | Sets the directory in your repository containing custom kernel patch files. If specified, patch files are automatically downloaded from the designated directory before compilation. If not set, this step is skipped. |
| auto_patch       | false         | Sets whether to apply custom kernel patches. Default: `false`. Refer to `-p` for details. |
| compress_format  | xz            | Sets the compression format for initrd in the kernel. Default: `xz`. Refer to `-z` for details. |
| delete_source    | false         | Sets whether to delete the kernel source code after compilation. Default: `false`. Refer to `-d` for details. |
| silent_log       | false         | Sets whether to enable silent mode to reduce log output during compilation. Default: `false`. Refer to `-s` for details. |
| enable_log       | false         | Sets whether to log the kernel compilation process to a file: `/var/log/kernel_compile_*.log`. Default: `false`. Refer to `-l` for details. |
| ccache_clear     | false         | Sets whether to clear the ccache before compilation. Default: `false`. Refer to `-c` for details. |
| docker_hostpath  | .             | Sets the host mount path for kernel compilation in Docker. Defaults to the current working directory. Refer to `-h` for details. |
| docker_image     | ophub/armbian-trixie:arm64 | Sets the Docker container image used for kernel compilation. Refer to `-i` for details. |

- ### GitHub Action Output Variables

To upload to `Releases`, you need to set `Workflow read/write permissions` for the repository. For details, see [Usage Instructions](../documents/README.md#2-set-up-private-variable-github_token).

| Parameter                        | Default Value   | Description                            |
|----------------------------------|-----------------|----------------------------------------|
| ${{ env.PACKAGED_OUTPUTTAGS }}   | 6.12.y_6.18.y   | The name of the compiled kernel.       |
| ${{ env.PACKAGED_OUTPUTPATH }}   | compile-kernel/output | The directory path where the compiled kernel files are stored. |
| ${{ env.PACKAGED_OUTPUTDATE }}   | 04.13.1058      | Compilation date (month.day.hourminute). |
| ${{ env.PACKAGED_STATUS }}       | success         | Compilation status: success / failure. |

## Kernel Usage Guide

The compiled kernel supports both `Armbian` and `OpenWrt` systems. The following examples use ophub's projects.

### Using in Armbian

The following sections describe how to integrate the kernel during Armbian firmware compilation and how to install it in an existing system.

- #### Compiling Armbian Firmware with the Kernel

Armbian firmware compilation supports both local and online workflows via GitHub Actions. For local compilation, see [Local Packaging](../README.md#local-packaging). For online compilation via Actions, see [Using GitHub Actions for Compilation](../README.md#using-github-actions-for-compilation).

- #### Installing the Kernel in an Existing Armbian System

Use the `armbian-update` command to install the compiled kernel into an existing Armbian system. For detailed instructions, see [Update Armbian Kernel](../README.md#update-armbian-kernel).

- #### Compiling Custom Driver Modules

Some drivers are not yet included in the Linux mainline kernel. You can compile custom driver modules as needed. For instructions, see [Compile Driver Module](../documents/README.md#93-how-to-customize-compilation-of-driver-modules).

### Using in OpenWrt

The following sections describe how to integrate the kernel during OpenWrt firmware compilation and how to install it in an existing system.

- #### Compiling OpenWrt Firmware with the Kernel

OpenWrt firmware compilation supports both local and online workflows via GitHub Actions. For local compilation, see [Local Packaging](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#local-packaging). For online compilation via Actions, see [Use GitHub Actions for Compilation](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#use-gitHub-actions-for-compilation).

- #### Installing the Kernel in an Existing OpenWrt System

Use the [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic) plugin to install the compiled kernel into an existing OpenWrt system. For detailed instructions, see [Update OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#update-openwrt).
