# Description of Related Files

View Chinese description | [查看中文说明](README.cn.md)

The files required to build Armbian are stored in the relevant directories.

## armbian-files

The files stored here are related files that are required for building Armbian. The `common-files` directory contains common files, the `platform-files` directory contains platform-specific files, and the `different-files` directory contains differentiated files for different devices. For more information, please refer to Chapter 12.15 in [documents](documents/README.md).

The required firmware will be automatically downloaded from the [ophub/firmware](https://github.com/ophub/firmware) repository to the `common-files/usr/lib/firmware` directory.

## documents

This directory contains Armbian usage documentation.

## kernel

Create a folder corresponding to the version number in the `kernel` directory, such as `stable/5.10.125`. Create directories for multiple kernels and place the corresponding kernel files in them. Kernel files can be downloaded from the [kernel](https://github.com/ophub/kernel) repository or [custom compiled](../compile-kernel). If no manual download of kernel files is found, the script will automatically download them from the kernel repository during compilation.

## u-boot

These are the system's boot files. Depending on the version of the kernel, they will be automatically installed/updated by related scripts when needed.
