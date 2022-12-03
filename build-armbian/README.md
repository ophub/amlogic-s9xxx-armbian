# Description of related documents

View Chinese description  |  [查看中文说明](README.cn.md)

The files needed to compile Armbian are stored in the relevant directory.

## armbian-files

The files stored here are related files that need to be used when packaging Armbian. The `common-files` directory is a general file, and the `platform-files` directory is a differentiated file for each platform.

## documents

Here are the Armbian documentation.

## firmware

This is the Armbian general firmware.

## kernel

Create a folder corresponding to the version number in the `kernel` directory, such as `stable/5.10.125` . Multiple kernels create directories in turn and put corresponding kernel files. Kernel files can be downloaded from the [kernel](https://github.com/ophub/kernel) repository or [custom compilation](../compile-kernel). If the kernel file is not downloaded and stored manually, the script will also be automatically downloaded from the kernel repository at compile time.

## u-boot

The system starts the bootstrap file. According to different versions of the kernel, it will be automatically completed by related scripts such as install/update when needed.
