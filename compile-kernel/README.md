# Compile the kernel

View Chinese description  |  [查看中文说明](README.cn.md)

Compile a custom kernel as needed. This kernel can be used in [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian) and [OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt) systems. Commonly used in systems made with the [flippy](https://github.com/unifreq/openwrt_packit) standard.

## Compile command description

- `sudo ./recompile -k 5.4.150`: Use the `-k` parameter to specify the kernel version to be compiled, and use `_` to link when multiple versions are compiled at the same time.
- `sudo ./recompile -k 5.4.150 -a true`: Use the `-a` parameter to set whether to automatically upgrade to the latest kernel of the same series when compiling the kernel.
- `sudo ./recompile -k 5.4.150 -n leifeng`: Use the `-n` parameter to set the kernel custom signature.
- `sudo ./recompile -k 5.4.150 -r kernel.org`: Use the `-r` parameter to set the download station of the compiled source code.
- `sudo ./recompile -k 5.10.70_5.4.150 -a true -n leifeng -r kernel.org`: Set through multiple parameters.

| Parameter | Meaning | Description |
| ---- | ---- | ---- |
| -k | Kernel | Specify [kernel](https://cdn.kernel.org/pub/linux/kernel/v5.x/) name, such as `-k 5.4.150`. Multiple kernels use `_` to connect, such as `- k 5.10.70_5.4.150` |
| -a | AutoKernel | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find whether there is a newer version of the kernel specified in `-k` such as `5.4.150` of the `5.4` series. If there is the latest version after `5.4.150`, it will be automatically replaced with the latest version . When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| -n | CustomName | Set the kernel custom signature. The default value is `-meson64-beta` and the generated kernel is `5.4.150-meson64-beta`. Do not include spaces when setting a custom signature. |
| -r | Repo | Specify the download site of the kernel compilation source code. The available options are [kernel.org](https://www.kernel.org/) and [flippy](https://github.com/unifreq), the default is `kernel.org` |

💡Tip: You can use the source code of `flippy` to compile the `latest version` of [5.4](https://github.com/unifreq/linux-5.4.y) / [5.10](https://github.com/unifreq/linux-5.10.y) / [5.12](https://github.com/unifreq/linux-5.12.y) / [5.13](https://github.com/unifreq/linux-5.13.y) / [5.14](https://github.com/unifreq/linux-5.14.y) / [5.15](https://github.com/unifreq/linux-5.15.y). `Other versions or historical versions` can be compiled with [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/).

- ### Compile with GitHub Action

1. The configuration of the Workflows file is in the [.yml](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/.github/workflows) file.

2. Select ***`Compile Armbian For Amlogic`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

- ### Local compilation

1. Please install the Armbian system in your box and install the following dependent environment.

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/armbian-kernel-server)
```

2. Clone the repository to local: `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

3. For example, use the source code of [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/) to compile, Please download the corresponding kernel and unzip it to the directory `compile-kernel/kernle`; If you use the source code of [flippy](https://github.com/unifreq) to compile, Please clone the source code of the specified kernel series such as `git clone --depth 1 https://github.com/unifreq/linux-5.4.y compile-kernel/kernle/linux-5.4.y` to the corresponding directory. After completion, enter the corresponding kernel such as `compile-kernel/kernle/linux-5.4.150` directory, Run the personalized configuration selection command `make menuconfig` to make a selection, save it after completion, A custom kernel `.config` configuration file will be generated in the kernel directory. 

4. Enter the root directory of `~/amlogic-s9xxx-armbian`, and then run the `sudo ./recompile -k 5.4.150` command to compile the kernel. The packaged kernel file is stored in the `compile-kernel/output` directory.

## Other instructions

1. Priority of kernel compilation file inspection: If there is a folder of the specified kernel in the `compile-kernel/kernle` directory, such as `linux-5.4.150`, the local source code will be used for compilation; when there is no specified kernel folder, but there is a compressed file of the specified kernel, such as linux -5.4.150.tar.xz, it will be automatically decompressed and compiled; when no kernel is specified locally, it will be automatically downloaded and compiled from the server.

2. If there is no [.config](tools/config) file in the local kernel directory such as `compile-kernel/kernle/linux-5.4.150`, the file will be automatically copied from template.

3. When cross-compiling the `Armbian` kernel in a system such as `Ubuntu` under the environment of `x86_64`, the [uInitrd](tools/uInitrd) file cannot be generated. When the kernel file is packaged, the file of the same kernel series in the repository will be automatically used instead. At present, cross-compilation in the `x86_64` environment cannot completely replace the kernel compilation in the real `Armbian` environment, and the produced kernel is unstable.

4. After the kernel is compiled, it will be automatically packaged into 5 kernel files according to the organization of the kernel files shared by flippy and stored in the `compile-kernel/output` directory. These kernel files will be automatically cleared from the Armbian system compiled with the current kernel. If you want to install on the current system, you can enter the corresponding kernel directory such as `compile-kernel/output/5.4.150` and execute the specified kernel installation command such as `armbian-update 5.4.150` to install.

5. If a kernel with the same name such as `5.4.150-meson64-beta` is already installed in the current `Armbian` system, the compilation will automatically stop, because the local kernel file with the same name will be deleted during packaging, which will cause the system to crash.

6. Four basic principles: During the kernel test, please test on the `USB/TF` device, and don't write to the `EMMC` partition rashly to avoid bricking; Please do not perform a custom kernel test before you have mastered the method of system recovery proficiently; Please do not test the custom kernel in a formal production environment; Skilled use of [Stable Kernel](https://github.com/ophub/kernel/tree/main/pub/stable) can effectively avoid being fined by the boss and being punished by the wife.

