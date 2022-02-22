# Description of Amlogic s9xxx series related documents

View Chinese description  |  [查看中文说明](README.cn.md)

Some files needed for compilation related to amlogic-s9xxx kernel are stored in the directory.

## amlogic-armbian

The files stored here are related files that need to be used when packaging Armbian.

## amlogic-dtb

For more Armbian firmware .dtb files are in the amlogic-dtb directory.  When writing into EMMC through `armbian-install`, select `0`: Enter the dtb file name of your box.

## amlogic-kernel

The kernel files needed to compile Armbian are stored in the `amlogic-kernel` directory. For usage, see [amlogic-kernel/README.md](amlogic-kernel/README.md)

## amlogic-u-boot

When you use Armbian with 5.10 kernel, you need to copy the corresponding u-boot file in the overload directory as `u-boot.ext`, when using in EMMC, you need to copy the u-boot file as `u-boot.emmc ` . Some devices need to write the corresponding bootloader file.

These duplications are automated in the repository's packaging and install/upgrade scripts, eliminating the need for manual duplication. For the specific files corresponding to each model, see [amlogic-u-boot](amlogic-u-boot/README.md)

## common-files

- files: The files in the `common-files/files` directory are custom files, which must be completely consistent with the structure and file naming and storage under the ***`ROOTFS`*** partiton in Armbian. If there are files in this directory, they will be automatically copied to the Armbian directory during `sudo ./rebuild`. E.g:

```yaml
etc/network/interfaces
usr/sbin
```

- patches: This is the directory where patch files are stored. You can place extension files, patches, etc. in this directory.

