# Compile the kernel

View Chinese description  |  [查看中文说明](README.cn.md)

Compile a custom kernel as needed. This kernel can be used in [Armbian](https://github.com/ophub/amlogic-s9xxx-armbian) and [OpenWrt](https://github.com/ophub/amlogic-s9xxx-openwrt) systems. Commonly used in systems made with the [flippy](https://github.com/unifreq/openwrt_packit) standard.

## Compile command description

| Parameter | Meaning | Description |
| ---- | ---- | ---- |
| -d | Defaults | Compile all kernels with default configuration. |
| -k | Kernel | Specify [kernel](https://cdn.kernel.org/pub/linux/kernel/v5.x/) name, such as `-k 5.4.160`. Multiple kernels use `_` to connect, such as `- k 5.10.80_5.4.160` |
| -a | AutoKernel | Set whether to automatically adopt the latest version of the kernel of the same series. When it is `true`, it will automatically find whether there is a newer version of the kernel specified in `-k` such as `5.4.160` of the `5.4` series. If there is the latest version after `5.4.160`, it will be automatically replaced with the latest version . When set to `false`, the specified version of the kernel will be compiled. Default value: `true` |
| -n | CustomName | Set the kernel custom signature. The default value is `-meson64-beta` and the generated kernel is `5.4.160-meson64-beta`. Do not include spaces when setting a custom signature. |
| -r | Repo | Specify the download site of the kernel compilation source code. The available options are [kernel.org](https://www.kernel.org/) and [flippy](https://github.com/unifreq), the default is `flippy` |

- `sudo ./recompile -d -k 5.4.160`: Use the default configuration, and use the `-k` parameter to specify the kernel version to be compiled, and use `_` to link when multiple versions are compiled at the same time.
- `sudo ./recompile -d -k 5.4.160 -a true`: Use the default configuration, and use the `-a` parameter to set whether to automatically upgrade to the latest kernel of the same series when compiling the kernel.
- `sudo ./recompile -d -k 5.4.160 -n leifeng`: Use the default configuration, and use the `-n` parameter to set the kernel custom signature.
- `sudo ./recompile -d -k 5.4.160 -r kernel.org`: Use the default configuration, and use the `-r` parameter to set the download station of the compiled source code.
- `sudo ./recompile -d -k 5.10.80_5.4.160 -a true -n leifeng -r kernel.org`: Use the default configuration, and set through multiple parameters.

💡Tip: You can use the [.config](https://github.com/unifreq/arm64-kernel-configs) template and source code of `flippy` to compile the `latest version` of [5.4](https://github.com/unifreq/linux-5.4.y) / [5.10](https://github.com/unifreq/linux-5.10.y) / [5.12](https://github.com/unifreq/linux-5.12.y) / [5.13](https://github.com/unifreq/linux-5.13.y) / [5.14](https://github.com/unifreq/linux-5.14.y) / [5.15](https://github.com/unifreq/linux-5.15.y). `Other series or historical versions` can be compiled with [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/).

- ### Local compilation

1. Please install the Armbian system in your box and install the following dependent environment.

```yaml
sudo apt-get update -y
sudo apt-get full-upgrade -y
sudo apt-get install -y $(curl -fsSL git.io/armbian-kernel-server)
```

2. Clone the repository to local: `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

3. First create a `kernel` directory under the `~/amlogic-s9xxx-armbian/compile-kernel` directory to store the compiled kernel source code. For example, use the source code of [kernel.org](https://cdn.kernel.org/pub/linux/kernel/v5.x/) to compile, Please download the corresponding kernel such as `linux-5.4.160.tar.xz` and unzip it to the `compile-kernel/kernel/linux-5.4.160` corresponding directory; If you use the source code of [flippy](https://github.com/unifreq) to compile, Please clone the source code of the specified kernel series such as `git clone --depth 1 https://github.com/unifreq/linux-5.4.y compile-kernel/kernel/linux-5.4.y` to the corresponding directory. After completion, enter the corresponding kernel such as `compile-kernel/kernel/linux-5.4.160` directory, Copy the [.config](tools/config) template of the corresponding kernel series to the current kernel directory (For example, copy the config-5.4.160 file and rename it to `.config`), Then run the personalized configuration selection command `make menuconfig` to Make a custom selection, save it after completion, A custom kernel `.config` configuration file will be generated in the kernel directory.

4. Enter the root directory of `~/amlogic-s9xxx-armbian`, and then run `sudo ./recompile -d -k 5.4.160 -r flippy -a false` and other specified parameter commands to compile the kernel. The packaged kernel file is stored in the `compile-kernel/output` directory.

- ### Compile with GitHub Action

1. The configuration of the Workflows file is in the [.yml](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/.github/workflows) file.

2. Select ***`Compile Armbian For Amlogic`*** on the [Action](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page. Click the ***`Run workflow`*** button.

## Other instructions

1. Priority of kernel compilation file inspection: If there is a folder of the specified kernel in the `compile-kernel/kernel` directory, such as `linux-5.4.160`, the local source code will be used for compilation; when there is no specified kernel folder, but there is a compressed file of the specified kernel, such as linux -5.4.160.tar.xz, it will be automatically decompressed and compiled; when no kernel is specified locally, it will be automatically downloaded and compiled from the server.

2. If there is no [.config](tools/config) file in the local kernel directory such as `compile-kernel/kernel/linux-5.4.160`, the file will be automatically copied from template.

3. Currently, compiling the kernel under the `Armbian` system is the best choice, and it is highly recommended. When compiling the kernel under the `x86_64` environment, the Armbian system will be automatically downloaded, and the `uInitrd` file will be generated through chroot. After the kernel is compiled, it will be automatically packaged into 6 kernel files according to the organization of the kernel files shared by flippy and stored in the `compile-kernel/output` directory. These kernel files will be automatically cleared from the system compiled with the current kernel. If you want to install on the current Armbian system, you can enter the corresponding kernel directory such as `compile-kernel/output/5.4.160` and execute the `armbian-update` command to install the kernel. The `headers` files in the kernel is installed in the `/use/local/include` directory.

4. If a kernel with the same name such as `5.4.160-meson64-beta` is already installed in the current `Armbian` system, the compilation will automatically stop, because the local kernel file with the same name will be deleted during packaging, which will cause the system to crash.

5. During the kernel test, please test on the `USB/TF` device, and don't write to the `EMMC` partition rashly to avoid bricking; Please do not perform a custom kernel test before you have mastered the method of system recovery proficiently; Please do not test the custom kernel in a formal production environment.

