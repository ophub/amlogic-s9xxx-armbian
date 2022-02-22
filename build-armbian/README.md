# Description of Amlogic s9xxx series related documents

View Chinese description  |  [查看中文说明](README.cn.md)

Some files needed for compilation related to amlogic-s9xxx kernel are stored in the directory.

## amlogic-armbian

The files stored here are related files that need to be used when packaging Armbian.

## common-files

- files: The files in the `common-files/files` directory are custom files, which must be completely consistent with the structure and file naming and storage under the ***`ROOTFS`*** partiton in Armbian. If there are files in this directory, they will be automatically copied to the Armbian directory during `sudo ./rebuild`. E.g:

```yaml
etc/network/interfaces
usr/sbin
```

- patches: This is the directory where patch files are stored. You can place extension files, patches, etc. in this directory.

