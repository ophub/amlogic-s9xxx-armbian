# Kernel Compilation and Usage Instructions

View Chinese description | [查看中文说明](README.cn.md)

This kernel can be used for `Armbian` and `OpenWrt` systems. For example, it can be used in the [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt), [flippy-openwrt-actions](https://github.com/ophub/flippy-openwrt-actions), and [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) projects. It can be integrated when compiling firmware or installed to use in an existing system.

You can adjust the kernel's configuration as needed, such as adding drivers and patches. You can also compile personalized signature kernels with special significance according to your mood, such as `5.10.95-happy-new-year`, `5.10.96-beijing-winter-olympics`, `5.10.99-valentines-day`, and so on.

## Kernel Repository

Pre-compiled kernels are available in the [Releases](https://github.com/ophub/kernel/releases) of [ophub/kernel](https://github.com/ophub/kernel).

## Local Compilation

- ### Running on Ubuntu system

1. Clone the repository to local `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install necessary software packages (the script has only been tested under Ubuntu-20.04/22.04(Supported by both X86 and Arm)):

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-22.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2204-build-armbian-depends)
```

3. Go to the `~/amlogic-s9xxx-armbian` root directory, and then run `sudo ./recompile -k 5.15.100` or other specified parameter commands to compile the kernel. The script will automatically download and install the compilation environment and kernel source code, and make all settings. The packaged kernel files are saved in the `compile-kernel/output` directory.


- ### Running on the Armbian system

1. Update local compilation environment and configuration files: `armbian-kernel -u`

2. Compile the kernel: Run `armbian-kernel -k 5.15.100` or other specified parameter commands to compile the kernel. The script will automatically download, install the compilation environment and the kernel source code, and make all settings. The packaged kernel files are saved in the `/opt/kernel/compile-kernel/output` directory.

- ### Local Compilation Parameter Explanation

| Parameter | Meaning      | Description |
| --------- | ----------- | ----------- |
| -r        | Repository  | Specifies the source code repository for compiling the kernel. You can choose the kernel source code repository from `github.com`, such as `-r unifreq`. The parameter format can be a combination of three items `owner/repo@branch`. The owner's name `owner` is a mandatory parameter, the kernel source code repository name `/repo` and the branch name of the repository `@branch` are optional parameters. When only specifying the owner's name `owner`, it will automatically match the kernel source code repository with the name format `linux-5.x.y` and the branch `main` of the owner. If the repository name or branch name is different, please specify it in combination, such as `owner@branch` or `owner/repo` or `owner/repo@branch`. Default value: `unifreq` |
| -k        | Kernel      | Specifies the kernel name, such as `-k 5.15.100`. Multiple kernels are connected with `_`, such as `-k 5.15.100_5.15.50` |
| -a        | AutoKernel  | Sets whether to automatically adopt the latest version of the same series of kernels. When it is `true`, it will automatically search whether there is a newer version of the same series of kernels specified in `-k`, such as `5.15.100`. If there is a latest version after `5.15.100`, it will automatically switch to the latest version. When set to `false`, it will compile the specified version of the kernel. Default value: `true` |
| -m        | MakePackage | Sets the package list for making the kernel. When set to `all`, it will make all the files of `Image, modules, dtbs`. When the setting value is `dtbs`, only 3 dtbs files will be produced. Default value: `all` |
| -p        | AutoPatch   | Sets whether to use custom kernel patches. When set to `true`, it will use the kernel patches in the [tools/patch](tools/patch) directory. For detailed instructions, refer to [how to add kernel patches](../documents/README.md#9-compiling-armbian-kernel). Default value: `false` |
| -n        | CustomName  | Sets the custom signature of the kernel. When set to `-ophub`, the generated kernel name is `5.15.100-ophub`. Please do not include spaces when setting custom signatures. Default value: `-ophub` |
| -t        | Toolchain   | Sets the toolchain for compiling the kernel. Options: `clang / gcc / gcc-<version>`. Default value: `gcc` |
| -c        | Compress    | Set the compression format used for initrd in the kernel. Options: `xz / gzip / zstd / lzma`. Default value: `xz` |


- `sudo ./recompile`: Compile the kernel using the default configuration.
- `sudo ./recompile -k 5.15.100`: Use the default configuration and specify the kernel version to be compiled through `-k`. Multiple versions are connected using `_` for simultaneous compilation.
- `sudo ./recompile -k 5.15.100 -a true`: Use the default configuration and set whether to automatically upgrade to the latest kernel of the same series during kernel compilation through the `-a` parameter.
- `sudo ./recompile -k 5.15.100 -n -ophub`: Use the default configuration and set the kernel custom signature through the `-n` parameter.
- `sudo ./recompile -k 5.15.100 -m dtbs`: Use the default configuration and specify only the creation of dtbs files through the `-m` parameter.
- `sudo ./recompile -k 5.15.100_6.1.10 -a true -n -ophub`: Use the default configuration and set multiple parameters.

💡 Tip: We recommend using the kernel source code from repositories such as [linux-6.1.y](https://github.com/unifreq/linux-6.1.y), [linux-5.15.y](https://github.com/unifreq/linux-5.15.y), [linux-5.10.y](https://github.com/unifreq/linux-5.10.y) and [linux-5.4.y](https://github.com/unifreq/linux-5.4.y) of `unifreq` for compilation, who added drivers and patches for related boxes. It is recommended to use the template in [tools/config](tools/config), which has been pre-configured according to the related boxes and can be customized based on this.

## Compile Kernel Using GitHub Actions

1. In the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Compile the kernel`*** and click the ***`Run workflow`*** button to compile.

2. See the use of the template [compile-kernel.yml](../.github/workflows/compile-kernel.yml). The code is as follows:

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 6.1.y_5.15.y
    kernel_auto: true
    kernel_sign: -yourname
```

💡 Note: If you `fork` the repository and made modifications, you need to change the Actions `username` to your own repository when using it, for example:

```yaml
uses: YOUR-REPO/amlogic-s9xxx-armbian@main
```

- ### GitHub Action Input Parameters

These parameters correspond to the `local compilation commands`. Please refer to the above explanation.

| Parameter        | Default Value | Description                                                     |
|------------------|---------------|-----------------------------------------------------------------|
| build_target     | kernel        | Fixed parameter `kernel`, set the compilation target to the kernel.|
| kernel_source    | unifreq       | Specifies the source code repository for compiling the kernel. Default value is `unifreq`. Refer to `-r` for functionality. |
| kernel_version   | 6.1.y_5.15.y  | Specifies the kernel name, such as `5.15.100`. Refer to `-k` for functionality. |
| kernel_auto      | true          | Sets whether to automatically adopt the latest version of the same series kernel. Default value is `true`. Refer to `-a` for functionality. |
| kernel_package   | all           | Sets the package list for making the kernel. Default value is `all`. Refer to `-m` for functionality. |
| kernel_sign      | -ophub        | Sets the kernel custom signature. Default value is `-ophub`. Refer to `-n` for functionality. |
| kernel_toolchain | gcc           | Sets the toolchain for compiling the kernel. Default value is `gcc`. Refer to `-t` for functionality. |
| kernel_config    | false         | By default, use the configuration template in the [tools/config](tools/config) directory. You can set the directory for storing the kernel configuration file in your repository, such as `kernel/config_path`. The configuration templates stored in this directory must be named after the main version of the kernel, such as `config-5.10`, `config-5.15`, etc. |
| kernel_patch     | false         | Sets the directory for custom kernel patches. |
| auto_patch       | false         | Sets whether to use custom kernel patches. Default value is `false`. Refer to `-p` for functionality. |
| compress_format  | xz            | Set the compression format used for initrd in the kernel. Default value is `xz`. Refer to `-c` for functionality. |

- ### GitHub Action Output Variables

To upload to `Releases`, you need to set `Workflow read/write permissions` for repository. For more details, see [Usage Instructions](../documents/README.md#2-set-up-private-variable-github_token).

| Parameter                        | Default Value   | Description                            |
|----------------------------------|-----------------|----------------------------------------|
| ${{ env.PACKAGED_OUTPUTTAGS }}   | 6.1.y_5.15.y    | The name of the compiled kernel.       |
| ${{ env.PACKAGED_OUTPUTPATH }}   | compile-kernel/output | The path of the directory where the compiled kernel is stored. |
| ${{ env.PACKAGED_OUTPUTDATE }}   | 04.13.1058      | The compilation date (month.day.hourminute). |
| ${{ env.PACKAGED_STATUS }}       | success         | Compilation status: success / failure. |

## Kernel Usage Instructions

This kernel can be used for `Armbian` and `OpenWrt` systems. For example, the project of ophub.

### Use in Armbian System

The following describes separately how to integrate it when compiling Armbian firmware, and how to install it for use in an existing system.

- #### Using the Kernel to Compile Armbian Firmware

Compiling Armbian firmware supports localization operations, and it also supports online compilation using Actions on github.com. The method of use when localizing compilation can be found in: [Local Packaging](../README.md#local-packaging), and the method of use when using Actions for online compilation can be found in: [Using GitHub Actions for Compilation](../README.md#using-github-actions-for-compilation).

- #### Installing the Kernel in an Existing Armbian System

You can use the `armbian-update` command to install the compiled kernel into an existing Armbian system. For specific operation methods, please refer to: [Update Armbian Kernel](../README.md#update-armbian-kernel).

- #### Custom Compile Driver Module

In the Linux mainline kernel, some drivers are not yet supported, so you can customize and compile driver modules. For specific instructions, please refer to: [Compile Driver Module](../documents/README.md#93-how-to-customize-compilation-of-driver-modules)

### Use in OpenWrt System

The following describes separately how to integrate it when compiling firmware in the OpenWrt system and how to install it for use in an existing system.

- #### Using the Kernel to Compile OpenWrt Firmware

Compiling OpenWrt firmware supports localization operations, and it also supports online compilation using Actions on github.com. The method of use when localizing compilation can be found in: [Local Packaging](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#local-packaging), and the method of use when using Actions for online compilation can be found in: [Use GitHub Actions for Compilation](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#use-gitHub-actions-for-compilation).

- #### Installing the Kernel in an Existing OpenWrt System

You can use the [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic) plugin to install the compiled kernel into an existing OpenWrt system. For specific operation methods, please refer to: [Update OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#update-openwrt).

