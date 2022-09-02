# Kernel compilation and usage instructions

View Chinese description  |  [查看中文说明](README.cn.md)

This kernel can be used in `Armbian` and `OpenWrt` systems. For example [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt), [flippy-openwrt-actions](https://github.com/ophub/flippy-openwrt-actions) and [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit). It can be integrated when compiling firmware, or it can be installed into an existing system for use.

You can adjust the configuration of the kernel as needed, such as adding drivers and patches. It is also possible to compile personalized signature kernels with special meanings according to mood, such as `5.10.95-happy-new-year`, `5.10.96-beijing-winter-olympics`, `5.10.99-valentines-day` and so on.

## Local compilation

- ### Run under Ubuntu system

1. Clone the repository to local: `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install the necessary packages (The script has only been tested on x86_64 Ubuntu-20.04/22.04)

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-22.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2204-build-armbian-depends)
```

3. Enter the `~/amlogic-s9xxx-armbian` root directory, and then run `sudo ./recompile -d -k 5.10.125` and other specified parameter commands to compile the kernel. The script will automatically download and install the compilation environment and kernel source code and make all settings. The packaged kernel file is stored in the `compile-kernel/output` directory.

- ### Run under Armbian system

1. Update the local compile environment and config files: `armbian-kernel -u`

2. Compile the kernel: Run `armbian-kernel -d -k 5.10.125` and other specified parameter commands to compile the kernel. The script will automatically download and install the compilation environment and kernel source code and make all settings. The packaged kernel file is stored in the `/opt/kernel/compile-kernel/output` directory.

- ### Description of local compilation parameters

| Parameter | Meaning | Description |
| ---- | ---- | ---- |
| -d | Defaults | Compile all kernels with default configuration. |
| -k | Kernel | Specify kernel name, such as `-k 5.10.125`. Multiple kernels use `_` to connect, such as `- k 5.10.125_5.15.50` |
| -a | AutoKernel | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find whether there is a newer version of the kernel specified in `-k` such as `5.10.125` of the series. If there is the latest version after `5.10.125`, it will be automatically replaced with the latest version . When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| -p | PackageList | Set the package list for compiling the kernel. The default value is `all`, which will compile all files of `Image, modules, dtbs`. When set to `dtbs` only 3 dtbs files are compiled. |
| -n | CustomName | Set the kernel custom signature. The default value is `-ophub` and the generated kernel is `5.10.125-ophub`. Do not include spaces when setting a custom signature. |
| -r | Repository | Specifies the source code repository for the compiled kernel. Defaults to `unifreq` . You can choose the source code of `kernel.org` and the kernel source of `github.com` code repository. For example `-r kernel.org` or `-r unifreq`, etc., When using the kernel source code repository of `github.com`, the parameter format can be set to the three-item combination of `owner/repo@branch` , The owner name `owner` in the parameters is a required parameter, the kernel source code repository name `/repo` and the repository branch name `@branch` are optional parameters. When only the owner name `owner` parameter is specified, it will automatically match kernel source code repositories whose owner's name is in `linux-5.x.y` format and branch is `main`. If the repository name or branch name is different, use a combination, such as `owner@branch` or `owner/repo` or `owner/repo@branch` |

- `sudo ./recompile -d`: Use the default configuration to compiled kernel.
- `sudo ./recompile -d -k 5.10.125`: Use the default configuration, and use the `-k` parameter to specify the kernel version to be compiled, and use `_` to link when multiple versions are compiled at the same time.
- `sudo ./recompile -d -k 5.10.125 -a true`: Use the default configuration, and use the `-a` parameter to set whether to automatically upgrade to the latest kernel of the same series when compiling the kernel.
- `sudo ./recompile -d -k 5.10.125 -n -ophub`: Use the default configuration, and use the `-n` parameter to set the kernel custom signature.
- `sudo ./recompile -d -k 5.10.125 -r kernel.org`: Use the default configuration, and set the kernel source code repository through the `-r` parameter.
- `sudo ./recompile -d -k 5.10.125 -p dtbs`: Use the default configuration, and compile the dtbs file with the `-p` parameter setting.
- `sudo ./recompile -d -k 5.10.125_5.15.50 -a true -n -ophub -r kernel.org`: Use the default configuration, and set through multiple parameters.

💡Tip: It is recommended to use the kernel source code of unifreq's [5.10](https://github.com/unifreq/linux-5.10.y), [5.15](https://github.com/unifreq/linux-5.15.y) and other repositories for compilation. He has added drivers and patches for related TV Boxes. It is recommended to use the templates in [tools/config](tools/config), which have been pre-configured according to the relevant TV Boxes and can be customized on this basis.

## Compile the kernel using GitHub Actions

This way you compile the kernel using your modified repository.

1. Select ***`Compile the kernel`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

2. Using Templates [compile-kernel.yml](../.github/workflows/compile-kernel.yml). The code is as follows:

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 5.10.125_5.15.50
    kernel_auto: true
    kernel_sign: -yourname
```

💡Note: If you `fork` the repository and make changes, When using, you must change the `username` of Actions to your own repository name, and [add TOKEN](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/build-armbian/armbian-docs#2-set-the-privacy-variable-github_token) according to clause 2-3 in the description. Eg:

```yaml
uses: YOUR-REPO/amlogic-s9xxx-armbian@main
```

- ### GitHub Action Input parameter description

The relevant parameters correspond to the `local compilation commands`, please refer to the above description.

| Parameter | Defaults | Description |
| ---- | ---- | ---- |
| build_target   | kernel   | Fixed parameter `kernel`, set the compilation target to the kernel. |
| kernel_version | 5.10.125_5.15.50 | Specify kernel name, such as `5.10.125`. Function reference `-k` |
| kernel_auto | true | Set whether to automatically adopt the latest kernel version of the same series. The default value is `true`. Function reference `-a` |
| kernel_package | all | Set the package list for compiling the kernel. The default is `all`. Function reference `-p` |
| kernel_sign | -ophub | Set the kernel custom signature. The default is `-ophub`. Function reference `-n` |
| kernel_repo    | unifreq  | Specifies the source code repository for the compiled kernel. The default is `unifreq` . Function reference `-r` |
| kernel_config | null | The default uses the configuration templates in the [tools/config](tools/config) directory. You can set the directory where the compiled kernel configuration files are stored in your repository, such as `kernel/config_path` . configuration templates stored in this directory must be named after the major version of the kernel, such as `config-5.10`, `config-5.15`, etc. |

- ### GitHub Action Output variable description

To upload to `Releases`, you need to add `GITHUB_TOKEN` and `GH_TOKEN` to the repository and set `Workflow read and write permissions`, see the [instructions for details](../build-armbian/armbian-docs#2-set-the-privacy-variable-github_token).

| Parameter                         | For example              | Description                         |
|-----------------------------------|--------------------------|-------------------------------------|
| ${{ env.PACKAGED_OUTPUTTAGS }}    | 5.10.125_5.15.50         | The name of the compiled kernel     |
| ${{ env.PACKAGED_OUTPUTPATH }}    | compile-kernel/output    | kernel files storage path           |
| ${{ env.PACKAGED_OUTPUTDATE }}    | 04.13.1058               | compile date(month.day.hour.minute) |
| ${{ env.PACKAGED_STATUS }}        | success                  | Compile status. success / failure   |


## Kernel usage Instructions

This kernel can be used in `Armbian` and `OpenWrt` systems. Take ophub's project as an example.

### Use on Armbian systems

The following describes how to integrate when compiling Armbian firmware and how to install it into an existing system.

- #### Compiling Armbian firmware with the kernel

Compiling Armbian firmware supports localized operations and online compilation using Actions from github.com. For details on how to use localized compilation, see: [Local build instructions](../README.md#local-build-instructions), For details on how to compile online with Actions: [Use GitHub Action to build](../README.md#use-github-actions-to-build)

- #### Install the kernel to an existing Armbian system

The compiled kernel can be installed into an existing Armbian system using the `armbian-update` command, For specific operation methods, please refer to: [Update Armbian Kernel](../README.md#update-armbian-kernel)

### Use in OpenWrt system

The following describes the integration when compiling the OpenWrt firmware and how to install it in an existing system.

- #### Compile OpenWrt firmware with kernel

Compiling OpenWrt firmware supports localized operations, as well as online compilation using Actions from github.com. For details on how to use localized compilation, see: [Local build instructions](https://github.com/ophub/amlogic-s9xxx-openwrt#local-build-instructions), For details on how to compile online with Actions: [Use GitHub Actions to build](https://github.com/ophub/amlogic-s9xxx-openwrt#use-github-actions-to-build)

- #### Install the kernel to an existing OpenWrt system

You can use the [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic/blob/main/README.cn.md) plugin to install the compiled kernel into the existing OpenWrt system, For specific operation methods, please refer to: [Update OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt#update-openwrt)

