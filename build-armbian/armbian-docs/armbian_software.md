# Armbian Software Center

According to the user's demand feedback in the [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), gradually integrate commonly used [software](../common-files/rootfs/usr/share/ophub/armbian-software/software-list.conf) to achieve one-click install/update/uninstall and other shortcut operations. Including `docker images`, `desktop software`, `application services`, etc.

## Software Center Instructions

Login in to armbian → input command:

```yaml
armbian-software
```

A list of currently integrated software quick installation/management will be displayed, such as:

```yaml
root@armbian:~# armbian-software
[ STEPS ] Start selecting software [ Current system: bullseye ]...
-----------------------------------------------------------------------------------
ID    NAME                                          STATE           MANAGE
-----------------------------------------------------------------------------------
101   docker                                        installed       update/remove
102   portainer(for-docker)                         installed       update/remove
103   transmission(for-docker)                      installed       update/remove
104   qbittorrent(for-docker)                       installed       update/remove
201   desktop                                       not-installed   install
202   vlc-media-player(for-desktop)                 not-installed   install
203   firefox(for-desktop)                          not-installed   install
301   frps                                          not-installed   install
302   frpc                                          not-installed   install
303   plex-media-server                             installed       update/remove
-----------------------------------------------------------------------------------
[ OPTIONS ] Please Input Software ID:
```

- `Uninstalled software`: The status is displayed as `not-installed`, enter the `ID` corresponding to the software to `install`.
- `Installed software`: The status is displayed as `installed`, enter the `ID` corresponding to the software, and select `update` or `delete` according to the prompts.

## Software Center Development Instructions

Software Center scripts/commands are centrally stored in the [/usr/share/ophub/armbian-software](../common-files/rootfs/usr/share/ophub/armbian-software) directory, use the `armbian-software -u` command to synchronously download this directory to the local and update the local software center list. The file starting with a number is the `one-click installation script` of the corresponding software, and the file [software-command.sh](../common-files/rootfs/usr/share/ophub/armbian-software/software-command.sh) is the `unified command file` for installing/updating/deleting operations using commands. [software-list.conf](../common-files/rootfs/usr/share/ophub/armbian-software/software-list.conf) is the software list configuration file, described as follows:

```yaml
# software-list.conf description
# 1.ID     2.SoftwareName(less than 40 characters)     3.AuthMethod@Package      4.Command/Shell     5.SupportedRelease
102        :portainer(for-docker)                      :docker@portainer         :command            :jammy@focal@bullseye
203        :firefox(for-desktop)                       :dpkg@firefox             :command            :jammy@focal@bullseye
302        :frpc                                       :which@frpc               :302-frpc.sh        :jammy@focal@bullseye
```

- `ID`: The `unique serial number` of the software.
- `SoftwareName`: `Software name` (Name length requirement is less than 40 characters).
- `AuthMethod@Package`: The `check method` of the software installation status, and the corresponding `package`, separated by the `@` symbol.
  - Use `docker` to check the image installed in the `docker` container. For example, to check whether the `portainer` image is installed, use `docker@portainer` to check;
  - Packages installed by `apt` are checked by `dpkg`. For example, to check whether the `firefox` package is installed, use `dpkg@firefox` to check;
  - Use `wget` to download and install the binary executable file, and use `which` to check. For example, to check whether the `frpc` service is installed, use `which@frpc` to check.
- `Command/Shell`: Set the software to be managed using a `unified command file` or `stand-alone script`.
  - For `reduced` operation commands, they are written in the `software-command.sh` file and named after the software serial number. For example, the serial number of `portainer` is `102`, and its operation is written in `software_203()`;
  - For `complex operations with long instruction contents`, independent script file management is performed. For example, the stand-alone script for installing `frpc` is named `302-frpc.sh` starting with the serial number.
- `SupportedRelease`: Set the supported Armbian `System Release`. Use the `@` symbol to separate.

Welcome to add more software. Support requests are welcome in [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues).



# Armbian 软件中心

根据用户在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中的需求反馈，逐步整合常用[软件](../common-files/rootfs/usr/share/ophub/armbian-software/software-list.conf)，实现一键安装/更新/卸载等快捷操作。包括 `docker 镜像`、`桌面软件`、`应用服务` 等。

## 软件中心使用说明

登录 Armbian 系统 → 输入命令：

```yaml
armbian-software
```

将出现当前已经集成的软件快捷安装/管理列表，如：

```yaml
root@armbian:~# armbian-software
[ STEPS ] Start selecting software [ Current system: bullseye ]...
-----------------------------------------------------------------------------------
ID    NAME                                          STATE           MANAGE
-----------------------------------------------------------------------------------
101   docker                                        installed       update/remove
102   portainer(for-docker)                         installed       update/remove
103   transmission(for-docker)                      installed       update/remove
104   qbittorrent(for-docker)                       installed       update/remove
201   desktop                                       not-installed   install
202   vlc-media-player(for-desktop)                 not-installed   install
203   firefox(for-desktop)                          not-installed   install
301   frps                                          not-installed   install
302   frpc                                          not-installed   install
303   plex-media-server                             installed       update/remove
-----------------------------------------------------------------------------------
[ OPTIONS ] Please Input Software ID:
```

- `未安装的软件`：状态显示为 `not-installed`，输入软件对应的 `ID` 即可 `安装`。
- `已安装的软件`：状态显示为 `installed`，输入软件对应的 `ID` ，根据提示选择 `更新` 或 `删除`。

## 软件中心开发说明

软件中心的脚本/命令集中存放在 [/usr/share/ophub/armbian-software](../common-files/rootfs/usr/share/ophub/armbian-software) 目录下，使用 `armbian-software -u` 命令可以同步下载此目录至本地，更新本地的软件中心列表。其中以数字开头的文件是对应软件的 `一键安装脚本` 文件。[software-command.sh](../common-files/rootfs/usr/share/ophub/armbian-software/software-command.sh) 是使用命令安装/更新/删除操作的`统一指令文件`。[software-list.conf](../common-files/rootfs/usr/share/ophub/armbian-software/software-list.conf) 是软件列表配置文件，说明如下：

```yaml
# software-list.conf description
# 1.ID     2.SoftwareName(less than 40 characters)     3.AuthMethod@Package      4.Command/Shell     5.SupportedRelease
102        :portainer(for-docker)                      :docker@portainer         :command            :jammy@focal@bullseye
203        :firefox(for-desktop)                       :dpkg@firefox             :command            :jammy@focal@bullseye
302        :frpc                                       :which@frpc               :302-frpc.sh        :jammy@focal@bullseye
```

- `ID`: 软件的 `唯一序号` 。
- `SoftwareName`：`软件名称`（名称长度要求小于 40 个字符）。
- `AuthMethod@Package`：软件安装状态的`检查方法`，与对应的`软件包`，使用 `@` 符号分割。
  - 使用 `docker` 容器安装的镜像，采用 `docker` 方式检查，如检查是否安装了 `portainer` 镜像，使用 `docker@portainer` 进行检查；
  - 使用 `apt` 方式安装的软件包采用 `dpkg` 方式检查，如检查是否安装了 `firefox` 软件包，使用 `dpkg@firefox` 进行检查；
  - 使用 `wget` 等方式下载安装的二进制执行文件，采用 `which` 方式检查，如检查是否安装了 `frpc` 服务，使用 `which@frpc` 进行检查。
- `Command/Shell`：设置软件使用 `统一指令文件` 或 `独立脚本` 进行管理。
  - 对于 `比较精简` 的操作命令，集中写在 `software-command.sh` 文件中，以软件序号进行命名。如 `portainer` 的序号是 `102`，他的操作写在 `software_203()` 中；
  - 对于 `比较复杂、指令内容较长` 的操作，进行独立脚本文件管理。如安装 `frpc` 的独立脚本，以序号开头，命名为 `302-frpc.sh` 。
- `SupportedRelease`：设置支持的 Armbian `系统版本`。使用 `@` 符号分割。

欢迎大家补充更多软件。欢迎在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中提交支持需求。

