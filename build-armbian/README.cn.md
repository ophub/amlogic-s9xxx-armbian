# 构建文件说明

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

编译 Armbian 系统所需的文件按功能分类存放在各个子目录中。

## armbian-files

此目录存放 Armbian 构建过程中所需的全部文件。其中，`common-files` 目录存放与平台无关的通用文件，`platform-files` 目录存放各平台专用文件，`different-files` 目录存放针对不同设备的差异化配置文件。更多说明详见 [documents](../documents/README.cn.md) 中的 `12.15` 章节。

所需固件将从 [ophub/firmware](https://github.com/ophub/firmware) 仓库自动下载至 `common-files/usr/lib/firmware` 目录。

## kernel

在 `kernel` 目录下创建与版本号对应的文件夹，如 `stable/5.10.125`。若需编译多个内核，依次创建各版本目录并放入对应的内核文件即可。内核文件可从 [kernel](https://github.com/ophub/kernel) 仓库下载，也可[自行编译](../compile-kernel)。若本地未预先存放内核文件，构建脚本将自动从 kernel 仓库下载。

## u-boot

系统引导文件。根据所用内核版本的不同，相应的 u-boot 文件将由安装或更新脚本自动获取和配置。所需的 u-boot 将从 [ophub/u-boot](https://github.com/ophub/u-boot) 仓库自动下载至 `u-boot` 目录。
