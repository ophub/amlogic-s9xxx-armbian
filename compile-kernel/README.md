# Kernel Compilation and Usage Instructions

View Chinese description | [查看中文说明](README.cn.md)

This kernel can be used for `Armbian` and `OpenWrt` systems, such as [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt), [flippy-openwrt-actions](https://github.com/ophub/flippy-openwrt-actions) and [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) projects. It can be integrated into firmware compilation or installed into an existing system.

You can adjust the configuration of the kernel according to your needs, by adding drivers and patches. You can also compile personalized signature kernels with special meanings, such as `5.10.95-happy-new-year`, `5.10.96-beijing-winter-olympics`, `5.10.99-valentines-day`, etc.

## Kernel Repository

Pre-compiled kernels are available in the [Releases](https://github.com/ophub/kernel/releases) section of [ophub/kernel](https://github.com/ophub/kernel).

## Local Compilation

- ### Running on Ubuntu System

1. Clone the repository to your local machine: `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install necessary packages (the script has only been tested on x86_64 Ubuntu-20.04/22.04)

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-22.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2204-build-armbian-depends)
```

3. Enter the root directory of `~/amlogic-s9xxx-armbian` and run the command with specified parameters, such as `sudo ./recompile -k 5.15.100`, to compile the kernel. The script will automatically download and install the compilation environment and kernel source code and set everything up. The packaged kernel files will be saved in the `compile-kernel/output` directory.

- ### Running on Armbian System

1. Update local compilation environment and configuration file: `armbian-kernel -u`

2. Compile the kernel: Run the command with specified parameters, such as `armbian-kernel -k 5.15.100`, to compile the kernel. The script will automatically download and install the compilation environment and kernel source code and set everything up. The packaged kernel files will be saved in the `/opt/kernel/compile-kernel/output` directory.

- ### Description of local compilation parameters

| Parameter | Meaning     | Description                    |
| --------- | ----------- | ------------------------------ |
| -r        | Repository  | Specifies the source code repository of the kernel to be compiled. You can choose the kernel source code repository on `github.com`, for example `-r unifreq`, etc. The parameter format can be a combination of three items: `owner/repo@branch`. The owner name `owner` is a required parameter, and the repository name `/repo` and branch name `@branch` are optional parameters. When only the owner name `owner` parameter is specified, the kernel source code repository will automatically match the repository with the format of `linux-5.x.y` owned by the owner and with the branch `main`. If the repository name or branch name is different, please use a combination of `owner@branch` or `owner/repo` or `owner/repo@branch` to specify. Default value: `unifreq`. |
| -k        | Kernel      | Specifies the kernel name, such as `-k 5.15.100`. Multiple kernels can be connected using `_`, such as `-k 5.15.100_5.15.50`. |
| -a        | AutoKernel  | Sets whether to automatically use the latest version of the same series kernel. When set to `true`, it will automatically search for updated versions of the same series as the kernel specified in `-k`, such as `5.15.100`. If there is a newer version after `5.15.100`, it will be automatically replaced with the latest version. When set to `false`, it will compile the specified version of the kernel. Default value: `true`. |
| -m        | MakePackage | Sets the package list for compiling the kernel. When set to `all`, it will make all files of `Image, modules, dtbs`. When set to `dtbs`, it will only make three dtbs files. Default value: `all`. |
| -p        | AutoPatch   | Set whether to use custom kernel patches. When set to `true`, the kernel patches in the [patch](tool/patch) directory will be used. For detailed instructions, please refer to [README.md](../build-armbian/documents/README.md#9-compile-armbian-kernel). Default value: `false`. |
| -n        | CustomName  | Sets the custom signature of the kernel. When set to `-ophub`, the generated kernel name will be `5.15.100-ophub`. Please do not include spaces when setting the custom signature. Default value: `-ophub`. |
| -t        | Toolchain   | Sets the toolchain for compiling the kernel. Options are: `clang / gcc`. Default value: `gcc`. |

- `sudo ./recompile`: Compiles the kernel with default configurations.
- `sudo ./recompile -k 5.15.100`: Compiles the kernel with default configuration and specifies the kernel version to be compiled using `-k`. Multiple versions can be compiled simultaneously by connecting them with `_`.
- `sudo ./recompile -k 5.15.100 -a true`: Compiles the kernel with default configuration and sets whether to automatically upgrade to the latest kernel of the same series during compilation through the `-a` parameter.
- `sudo ./recompile -k 5.15.100 -n -ophub`: Compiles the kernel with default configuration and sets a custom signature for the kernel through the `-n` parameter.
- `sudo ./recompile -k 5.15.100 -m dtbs`: Compiles only dtbs files with default configuration, specified by the `-m` parameter.
- `sudo ./recompile -k 5.15.100_6.1.10 -a true -n -ophub`: Compiles the kernel with multiple parameters set using the default configuration.

💡Note: It is recommended to use the kernel source code from `unifreq`'s repositories such as [linux-6.1.y](https://github.com/unifreq/linux-6.1.y), [linux-5.15.y](https://github.com/unifreq/linux-5.15.y), [linux-5.10.y](https://github.com/unifreq/linux-5.10.y) and [linux-5.4.y](https://github.com/unifreq/linux-5.4.y) for compiling, which adds drivers and patches for related boxes. It is recommended to use the templates in [tools/config](tools/config), which are pre-configured according to related boxes and can be customized based on them.

## Compiling Kernel using GitHub Actions

1. Select ***`Compile the kernel`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page and click on the ***`Run workflow`*** button to compile.

2. See the template [compile-kernel.yml](../.github/workflows/compile-kernel.yml) for details. The code is as follows:

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.15.1_6.1.1
    kernel_auto: true
    kernel_sign: -yourname
```

💡Note: If you `fork` the repository and make changes, you need to change the `username` of the Actions to your own repository, and follow steps 2-3 in the instructions [Add TOKEN](../build-armbian/documents/README.md#2-set-up-privacy-variable-github_token), for example:

```yaml
uses: YOUR-REPO/amlogic-s9xxx-armbian@main
```

- ### Input Parameters for GitHub Action

Related parameters correspond to the `Local Compilation Command`, please refer to the above instructions.

| Parameter        | Defaults         | Description                 |
| ---------------- | ---------------- | --------------------------- |
| build_target     | kernel           | Set the fixed parameter `kernel` to compile the kernel as the compilation target. |
| kernel_source    | unifreq          | Specifies the source code repository of the kernel to be compiled. The default value is `unifreq`. This function is the same as `-r`. |
| kernel_version   | 5.15.1_6.1.1     | Specifies the kernel name, such as `5.15.100`. This function is the same as `-k`. |
| kernel_auto      | true             | Sets whether to automatically use the latest version of the same series kernel. The default value is `true`. This function is the same as `-a`. |
| kernel_package   | all              | Sets the package list for compiling the kernel. The default value is `all`. This function is the same as `-m`. |
| kernel_sign      | -ophub           | Sets the custom signature of the kernel. The default value is `-ophub`. This function is the same as `-n`. |
| kernel_toolchain | gcc              | Sets the toolchain for compiling the kernel. The default value is `gcc`. This function is the same as `-t`. |
| kernel_config    | None             | By default, the configuration templates in [tools/config](tools/config) directory are used. You can set the directory of the configuration file for compiling the kernel in your repository, such as `kernel/config_path`. The configuration templates stored in this directory must be named after the main version of the kernel, such as `config-5.10`, `config-5.15`, etc. |
| kernel_patch     | no               | Set the directory for custom kernel patches. |
| auto_patch       | false            | Set whether to use custom kernel patches. The default value is `false`. This function is the same as `-p`. |

- ### Output Variables for GitHub Action

Uploading to `Releases` requires adding `GITHUB_TOKEN` and `GH_TOKEN` to the repository and setting `Workflow Read and Write Permissions`. See [instructions](../build-armbian/documents/README.md#2-set-up-privacy-variable-github_token) for details.

| Parameter                         | For example              | Description                         |
|-----------------------------------|--------------------------|-------------------------------------|
| ${{ env.PACKAGED_OUTPUTTAGS }}    | 5.15.1_6.1.1             | The name of the compiled kernel.    |
| ${{ env.PACKAGED_OUTPUTPATH }}    | compile-kernel/output    | The path where the compiled kernel is saved. |
| ${{ env.PACKAGED_OUTPUTDATE }}    | 04.13.1058               | The compilation date (Month.Day.HourMinute). |
| ${{ env.PACKAGED_STATUS }}        | success                  | The compilation status: success / failure.   |

## Instructions for Using the Kernel

This kernel can be used on `Armbian` and `OpenWrt` systems. Taking ophub's project as an example, the following will explain how to integrate it when compiling Armbian firmware and how to install and use it in existing systems.

### Using the Kernel to Compile Armbian Firmware

Compiling Armbian firmware supports localization operations and can also be compiled online using `github.com`'s Actions. The usage method for local compilation is detailed in: [Local Packaging](../README.md#local-build-instructions), and the method for compiling online using Actions is detailed in: [Compiling using GitHub Actions](../README.md#github-actions-input-parameter-description).

### Installing the Kernel in an Existing Armbian System

You can use the `armbian-update` command to install the compiled kernel into an existing Armbian system. The specific operation method is detailed in: [Updating the Armbian kernel](../README.md#updating-the-armbian-kernel)

### Using the Kernel in an OpenWrt System

The following describes how to integrate the kernel when compiling firmware and how to install and use it in an existing OpenWrt system.

- #### Using the Kernel to Compile OpenWrt Firmware

Compiling OpenWrt firmware supports localization operations and can also be compiled online using `github.com`'s Actions. The usage method for local compilation is detailed in: [Local Packaging](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#local-packaging-parameters), and the method for compiling online using Actions is detailed in: [Compiling using Actions](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#compiling-with-github-actions).

- #### Installing the Kernel in an Existing OpenWrt System

You can use the [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic/blob/main/README.md) plugin to install the compiled kernel into an existing OpenWrt system. The specific operation method is detailed in: [Update OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/README.md#update-openwrt).

