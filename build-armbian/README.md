# Build Files Reference

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

The files required for compiling the Armbian system are organized in their respective directories.

## armbian-files

This directory contains all the files used during the Armbian build process. The `common-files` subdirectory holds platform-independent shared files, the `platform-files` subdirectory holds platform-specific files, and the `different-files` subdirectory holds device-specific configuration files. For further details, refer to Section `12.15` of the [documentation](../documents/).

Required firmware files are automatically downloaded from the [ophub/firmware](https://github.com/ophub/firmware) repository into the `common-files/usr/lib/firmware` directory.

## kernel

Create a version-specific folder under the `kernel` directory, e.g., `stable/5.10.125`. For multiple kernels, create separate directories sequentially and place the corresponding kernel files in each. Kernel files can be downloaded from the [kernel](https://github.com/ophub/kernel) repository, or you can [compile them yourself](../compile-kernel). If no kernel files are present locally, the build script will automatically download them from the kernel repository.

## u-boot

System bootloader files. Depending on the kernel version, the appropriate u-boot files are automatically managed by the installation and update scripts as needed. Required u-boot files are automatically downloaded from the [ophub/u-boot](https://github.com/ophub/u-boot) repository into the `u-boot` directory.