# Description of Relevant Files

View Chinese description | [查看中文说明](README.cn.md)

The files required to compile Armbian are stored in the corresponding directories.

## armbian-files

The files stored here are related files that need to be used when building Armbian. Among them, the files under the `common-files` directory are general files, the files under the `platform-files` directory are files for each platform, and the files under the `different-files` directory are differentiated files for different devices. More explanations can be found in the `12.15` section of [documents](../documents/).

The required firmware will be automatically downloaded from the [ophub/firmware](https://github.com/ophub/firmware) repository to the `common-files/usr/lib/firmware` directory.

## kernel

Create a folder corresponding to the version number under the `kernel` directory, such as `stable/5.10.125`. Multiple kernels create directories in sequence and put the corresponding kernel files into them. Kernel files can be downloaded from the [kernel](https://github.com/ophub/kernel) repository, or you can [customize the compilation](../compile-kernel). If the kernel files are not manually downloaded and stored, the script will automatically download from the kernel repository during compilation.

## u-boot

System boot files, according to different versions of the kernel, will be automated by installation/update and other related scripts when needed. The necessary u-boot will be automatically downloaded from the [ophub/u-boot](https://github.com/ophub/u-boot) repository to the `u-boot` directory.