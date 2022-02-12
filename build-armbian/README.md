# amlogic-s9xxx kernel related files

View Chinese description  |  [查看中文说明](README.cn.md)

Some files needed for compilation related to amlogic-s9xxx kernel are stored in the directory.

## amlogic-armbian

The files stored here are related files that need to be used when packaging Armbian.

## amlogic-dtb

For more Armbian firmware .dtb files are in the amlogic-dtb directory.  When writing into EMMC through `armbian-install`, select `0`: Enter the dtb file name of your box.

## amlogic-kernel

The kernel files needed to compile Armbian are stored in the `amlogic-kernel` directory. For usage, see [amlogic-kernel/README.md](amlogic-kernel/README.md)

## amlogic-u-boot

When using the 5.10 kernel version, you need to copy the corresponding [u-boot-*.bin](amlogic-u-boot) file to `u-boot.ext` (TF/SD card boot file) and `u-boot.emmc` (EMMC boot file).

## common-files

- files: The files in the `common-files/files` directory are custom files, which must be completely consistent with the structure and file naming and storage under the ***`ROOTFS`*** partiton in Armbian. If there are files in this directory, they will be automatically copied to the Armbian directory during `sudo ./rebuild`. E.g:

```yaml
etc/config/network
lib/u-boot
```

- patches: This is the directory where patch files are stored. You can place extension files, patches, etc. in this directory.

