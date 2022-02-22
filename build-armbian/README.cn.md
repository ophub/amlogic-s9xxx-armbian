# Amlogic s9xxx 系列相关文件说明

查看英文说明 | [View English description](README.md)

在相关目录中存储了一些与 Amlogic s9xxx 内核相关的编译 Armbian 所需的文件。

## amlogic-armbian

这里存放的文件是打包 Armbian 时需要使用的相关文件。

## common-files

- files: 这里存放的是 Armbian 固件的个性化配置文件，将在打包脚本 `sudo ./rebuild` 执行时自动将相关文件集成到你的固件里。相关目录及文件命名均须与 Armbian 中 ROOTFS 分区 ( 即在 TTYD 终端里输入： `cd / && ls .` 你所看到的目录及各目录里面的文件名称 ) 保持完全一致。

```yaml
etc/network/interfaces
usr/sbin
```

- patches: 这是补丁文件存放目录，你可以将扩展文件，补丁等放置在该目录。
