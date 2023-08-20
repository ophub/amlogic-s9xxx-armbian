# 相关文件说明

查看英文说明 | [View English description](README.md)

在相关目录中存储了编译 Armbian 所需的文件。

## armbian-files

这里存放的文件是构建 Armbian 时需要使用的相关文件。其中 `common-files` 目录下的是通用文件，`platform-files` 目录下是各平台的文件，`different-files` 目录下是针对不同设备的差异化文件。更多说明详见 [documents](../documents/README.cn.md) 中的 `12.15` 章节。

需要的固件将从 [ophub/firmware](https://github.com/ophub/firmware) 仓库自动下载至 `common-files/usr/lib/firmware` 目录。

## kernel

在 `kernel` 目录下创建版本号对应的文件夹，如 `stable/5.10.125` 。多个内核依次创建目录并放入对应的内核文件。内核文件可以从 [kernel](https://github.com/ophub/kernel) 仓库下载，也可以[自定义编译](../compile-kernel)。如果没有手动下载存放内核文件，在编译时脚本也会自动从 kernel 仓库下载。

## u-boot

系统启动引导文件，根据不同版本的内核，在需要使用时将由安装/更新等相关脚本自动化完成。需要的 u-boot 将从 [ophub/u-boot](https://github.com/ophub/u-boot) 仓库自动下载至 `u-boot` 目录。
