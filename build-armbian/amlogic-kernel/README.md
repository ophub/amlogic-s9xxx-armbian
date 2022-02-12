# Kernel Instructions / 内核使用说明

- Create a folder corresponding to the version number in the `amlogic-kernel` directory, such as `5.4.170` , and put the 4 needed kernel files into this directory (Other kernel files are not required, and putting them in does not affect use). Multiple kernels create directories in turn and put corresponding kernel files. Kernel files can be downloaded from the [kernel](https://github.com/ophub/kernel) repository or [custom compilation](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel). If the kernel file is not downloaded and stored manually, the script will also be automatically downloaded from the kernel repository at compile time.

- 在 `amlogic-kernel` 目录下创建版本号对应的文件夹，如 `5.4.170` ，并将 4 个需要的内核文件放入此目录 (其他内核文件不需要，放入也不影响使用)。多个内核依次创建目录并放入对应的内核文件。内核文件可以从 [kernel](https://github.com/ophub/kernel) 仓库下载，也可以[自定义编译](https://github.com/ophub/amlogic-s9xxx-armbian/tree/main/compile-kernel)。如果没有手动下载存放内核文件，在编译时脚本也会自动从 kernel 仓库下载。

```yaml
~/amlogic-s9xxx-openwrt
    └── amlogic-s9xxx
        └── amlogic-kernel
            ├── 5.4.170
            │   ├── header-5.4.170-xxx.tar.gz
            │   ├── boot-5.4.170-xxx.tar.gz
            │   ├── dtb-amlogic-5.4.170-xxx.tar.gz
            │   └── modules-5.4.170-xxx.tar.gz
            │
            ├── 5.10.90
            │   ├── header-5.10.90-xxx.tar.gz
            │   ├── boot-5.10.90-xxx.tar.gz
            │   ├── dtb-amlogic-5.10.90-xxx.tar.gz
            │   └── modules-5.10.90-xxx.tar.gz
            │
            ├── more kernel...
```
