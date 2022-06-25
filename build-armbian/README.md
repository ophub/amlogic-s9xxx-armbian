# Description of related documents

View Chinese description  |  [查看中文说明](README.cn.md)

The files needed to compile Armbian are stored in the relevant directory.

## amlogic-armbian

The files stored here are related files that need to be used when packaging Armbian.

## amlogic-kernel

Create a folder corresponding to the version number in the `amlogic-kernel` directory, such as `5.15.25` , and put the 4 needed kernel files into this directory (Requires header-xxx.tar.gz, boot-xxx.tar.gz, dtb-amlogic-xxx.tar.gz, modules-xxx.tar.gz. Other kernel files are not required, and putting them in does not affect use). Multiple kernels create directories in turn and put corresponding kernel files. Kernel files can be downloaded from the [kernel](https://github.com/ophub/kernel) repository or [custom compilation](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel). If the kernel file is not downloaded and stored manually, the script will also be automatically downloaded from the kernel repository at compile time.

## amlogic-u-boot

When you use Armbian with 5.10 kernel, you need to copy the corresponding u-boot file in the overload directory as `u-boot.ext`, when using in EMMC, you need to copy the u-boot file as `u-boot.emmc ` . Some devices need to write the corresponding bootloader file. These duplications are automated in the repository's packaging and install/upgrade scripts, eliminating the need for manual duplication.

## armbian-docs

Here are the Armbian documentation.

## common-files

- rootfs: The files in the directory are custom files, which must be completely consistent with the structure and file naming and storage under the ***`ROOTFS`*** partiton in Armbian. If there are files in this directory, they will be automatically copied to the Armbian directory during `sudo ./rebuild`. E.g:

```yaml
etc/network/interfaces
usr/sbin
```

- bootfs: This is the personalized configuration file in the `/boot` directory of the Armbian system.

- patches: This is the directory where patch files are stored. You can place extension files, patches, etc. in this directory.

