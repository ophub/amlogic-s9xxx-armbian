# Armbian Software Center

According to the user's demand feedback in the [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues), gradually integrate commonly used [software](../common-files/rootfs/usr/share/ophub/armbian-software/software-list.conf) to achieve one-click install/update/uninstall and other shortcut operations. Including `docker images`, `desktop software`, `application services`, etc.

## Software introduction

| ID | SoftwareName           | Website/Docker        | Port | Software introduction                             |
| --- | --------------------- | --------------------- | ---- | ------------------------------------------------- |
| 101 | Docker                | [Web](https://www.docker.com/) / [Docker](https://docs.docker.com/engine/install/) | -    | Docker is an open platform for developing, shipping, and running applications. |
| 102 | Portainer             | [Web](https://www.portainer.io/) / [Docker](https://hub.docker.com/r/portainer/portainer-ce) | 9443 | The most popular container management platform in the world. |
| 103 | Yacht                 | [Web](https://yacht.sh/) / [Docker](https://hub.docker.com/r/selfhostedpro/yacht) | 8001 | A container management UI with a focus on templates and 1-click deployments. |
| 104 | Transmission          | [Web](https://transmissionbt.com/) / [Docker](https://github.com/linuxserver/docker-transmission) | 9091 | Transmission is a cross-platform BitTorrent client. |
| 105 | qBittorrent           | [Web](https://www.qbittorrent.org/) / [Docker](https://hub.docker.com/r/linuxserver/qbittorrent) | 8080 | qBittorrent is a BitTorrent client. |
| 106 | NextCloud             | [Web](https://nextcloud.com/) / [Docker](https://hub.docker.com/r/arm64v8/nextcloud) | 8088 | Nextcloud offers an on-premise Universal File Access and sync platform with powerful collaboration capabilities and desktop, mobile and web interfaces. |
| 107 | Jellyfin              | [Web](https://jellyfin.org/) / [Docker](https://hub.docker.com/r/linuxserver/jellyfin) | 8096 | Jellyfin is a Free Software Media System that puts you in control of managing and streaming your media. |
| 108 | HomeAssistant         | [Web](https://www.home-assistant.io/) / [Docker](https://hub.docker.com/r/linuxserver/homeassistant) | 8123 | Home Assistant integrates with over a thousand different devices and services, Has powerful automation features. |
| 109 | Kodbox                | [Web](https://kodcloud.com/) / [Docker](https://hub.docker.com/r/kodcloud/kodbox) | 8081 | Private cloud online document management solution. |
| 110 | CouchPotato           | [Web](https://couchpota.to/) / [Docker](https://hub.docker.com/r/linuxserver/couchpotato) | 5050 | Couchpotato is an automatic NZB and torrent downloader, Automatically find movies you want to watch. |
| 111 | Sonarr                | [Web](https://sonarr.tv/) / [Docker](https://hub.docker.com/r/linuxserver/sonarr) | 8989 | Sonarr is a PVR for Usenet and BitTorrent users. |
| 112 | Radarr                | [Web](https://radarr.video/) / [Docker](https://hub.docker.com/r/linuxserver/radarr) | 7878 | Radarr is a movie collection manager for Usenet and BitTorrent users. |
| 113 | Syncthing             | [Web](https://syncthing.net/) / [Docker](https://hub.docker.com/r/linuxserver/syncthing) | 8384 | Syncthing is a continuous file synchronization program. |
| 114 | FileBrowser           | [Web](https://filebrowser.org/) / [Docker](https://hub.docker.com/r/filebrowser/filebrowser) | 8002 | File Browser provides a file managing interface within a specified directory and it can be used to upload, delete, preview, rename and edit your files. |
| 115 | Heimdall              | [Web](https://heimdall.site/) / [Docker](https://hub.docker.com/r/linuxserver/heimdall) | 8003 | Heimdall is a way to organise all those links to your most used web sites and web applications in a simple way. |
| 201 | Desktop               | [Web](https://www.armbian.com/) | -    | Install the desktop visual operation interface. |
| 202 | Firefox               | [Web](https://www.mozilla.org/) | -    | An excellent web browser. |
| 203 | VLC                   | [Web](https://www.videolan.org/)            | -    | VLC is a free, open source, cross-platform multimedia player and framework that can play most multimedia files. |
| 204 | MPV                   | [Web](https://mpv.io/) | -    | A free, open source, and cross-platform media player. |
| 205 | GIMP                  | [Web](https://www.gimp.org/) | -    | GIMP is a cross-platform image editor. |
| 301 | Frps                  | [Web](https://gofrp.org/) | -    | A fast reverse proxy to help you expose a local server behind a NAT or firewall to the internet. |
| 302 | Frpc                  | [Web](https://gofrp.org/) | -    | Frp is a high-performance reverse proxy application focusing on intranet penetration, supporting TCP, UDP, HTTP, HTTPS and other protocols. |
| 303 | Plex                  | [Web](https://www.plex.tv/) | -    | From personal media on your own server, to free and on-demand Movies & Shows, live TV, podcasts, and web shows, to streaming music, you can enjoy it all in one app, on any device. |
| 304 | Emby-Server           | [Web](https://emby.media/) | -    | Sync your personal media to the cloud for easy backup, archiving, and converting. Store your content in multiple resolutions to enable direct streaming from any device. |
| 305 | OpenMediaVault | [Web](https://www.openmediavault.org/) | -    | openmediavault is the next generation network attached storage (NAS) solution based on Debian. |

## Software Center Instructions

Login in to armbian → input command:

```yaml
armbian-software
```

A list of currently integrated software quick installation/management will be displayed, such as:

```yaml
root@armbian:~# armbian-software
[ STEPS ] Start selecting software [ Current system: debian:bullseye ]...
----------------------------------------------------------
ID    NAME                STATE           MANAGE
----------------------------------------------------------
102   Portainer           installed       update/remove
202   Firefox             installed       update/remove
302   Frpc                not-installed   install
...
----------------------------------------------------------
[ OPTIONS ] Please Input Software ID:
```

- `Uninstalled software`: The status is displayed as `not-installed`, enter the `ID` corresponding to the software to `install`.
- `Installed software`: The status is displayed as `installed`, enter the `ID` corresponding to the software, and select `update` or `delete` according to the prompts.

## Software Center Development Instructions

Software Center scripts/commands are centrally stored in the [/usr/share/ophub/armbian-software](../common-files/rootfs/usr/share/ophub/armbian-software) directory, use the `armbian-software -u` command to synchronously download this directory to the local and update the local software center list. The file starting with a number is the `one-click installation script` of the corresponding software, and the file [software-command.sh](../common-files/rootfs/usr/share/ophub/armbian-software/software-command.sh) is the `unified command file` for installing/updating/deleting operations using commands. [software-list.conf](../common-files/rootfs/usr/share/ophub/armbian-software/software-list.conf) is the software list configuration file, described as follows:

```yaml
# 1.ID     2.SoftwareName     3.AuthMethod@Package      4.Command/Shell     5.SupportedRelease
102        :portainer         :docker@portainer         :command            :jammy@focal@bullseye
203        :firefox           :dpkg@firefox             :command            :jammy@focal@bullseye
302        :frpc              :which@frpc               :302-frpc.sh        :jammy@focal@bullseye
...
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

## 软件介绍

| ID | 软件名称               | Website/Docker        | 端口 | 软件介绍                                            |
| --- | --------------------- | --------------------- | ---- | ------------------------------------------------- |
| 101 | Docker                | [Web](https://www.docker.com/) / [Docker](https://docs.docker.com/engine/install/) | -    | Docker 是一个用于开发、发布和运行应用程序的开放平台。 |
| 102 | Portainer             | [Web](https://www.portainer.io/) / [Docker](https://hub.docker.com/r/portainer/portainer-ce) | 9443 | 全球最受欢迎的容器管理平台。 |
| 103 | Yacht                 | [Web](https://yacht.sh/) / [Docker](https://hub.docker.com/r/selfhostedpro/yacht) | 8001 | 容器管理 UI，侧重于模板和一键式部署。 |
| 104 | Transmission          | [Web](https://transmissionbt.com/) / [Docker](https://github.com/linuxserver/docker-transmission) | 9091 | Transmission 是一个跨平台的 BitTorrent 客户端。 |
| 105 | qBittorrent           | [Web](https://www.qbittorrent.org/) / [Docker](https://hub.docker.com/r/linuxserver/qbittorrent) | 8080 | qBittorrent 是一个 BitTorrent 客户端。  |
| 106 | NextCloud             | [Web](https://nextcloud.com/) / [Docker](https://hub.docker.com/r/arm64v8/nextcloud) | 8088 | Nextcloud 提供了一个本地通用文件访问和同步平台，具有强大的协作功能以及桌面，移动和 Web 界面。 |
| 107 | Jellyfin              | [Web](https://jellyfin.org/) / [Docker](https://hub.docker.com/r/linuxserver/jellyfin) | 8096 | Jellyfin 是一个自由软件媒体系统，让你控制管理和流媒体你的媒体。 |
| 108 | HomeAssistant         | [Web](https://www.home-assistant.io/) / [Docker](https://hub.docker.com/r/linuxserver/homeassistant) | 8123 | Home Assistant 集成了一千多种不同的设备和服务，具有强大的自动化功能。 |
| 109 | Kodbox                | [Web](https://kodcloud.com/) / [Docker](https://hub.docker.com/r/kodcloud/kodbox) | 8081 | 私有云在线文档管理解决方案。 |
| 110 | CouchPotato           | [Web](https://couchpota.to/) / [Docker](https://hub.docker.com/r/linuxserver/couchpotato) | 5050 | Couchpotato 是一个自动 NZB 和 torrent 下载器，自动查找要观看的电影。 |
| 111 | Sonarr                | [Web](https://sonarr.tv/) / [Docker](https://hub.docker.com/r/linuxserver/sonarr) | 8989 | Sonarr 是 Usenet 和 BitTorrent 用户的个人视频录像机。 |
| 112 | Radarr                | [Web](https://radarr.video/) / [Docker](https://hub.docker.com/r/linuxserver/radarr) | 7878 | Radarr 是 Usenet 和 BitTorrent 用户的电影收藏管理器。 |
| 113 | Syncthing             | [Web](https://syncthing.net/) / [Docker](https://hub.docker.com/r/linuxserver/syncthing) | 8384 | Syncthing 是一个连续的文件同步程序。 |
| 114 | FileBrowser           | [Web](https://filebrowser.org/) / [Docker](https://hub.docker.com/r/filebrowser/filebrowser) | 8002 | File Browser 在指定目录中提供文件管理界面，可用于上传，删除，预览，重命名和编辑文件。 |
| 115 | Heimdall              | [Web](https://heimdall.site/) / [Docker](https://hub.docker.com/r/linuxserver/heimdall) | 8003 | Heimdall 是一种以简单的方式组织所有这些链接到您最常用的网站和Web应用程序的方法。 |
| 201 | Desktop               | [Web](https://www.armbian.com/) | -    | 安装桌面可视化操作界面。 |
| 202 | Firefox               | [Web](https://www.mozilla.org/) | -    | 一款优秀的网页浏览器。 |
| 203 | VLC                   | [Web](https://www.videolan.org/)            | -    | VLC 是一款自由、开源的跨平台多媒体播放器及框架，可播放大多数多媒体文件。 |
| 204 | MPV                   | [Web](https://mpv.io/) | -    | 一个免费的、开源的、跨平台的媒体播放器。 |
| 205 | GIMP                  | [Web](https://www.gimp.org/) | -    | GIMP 是一个跨平台的图像编辑器。 |
| 301 | Frps                  | [Web](https://gofrp.org/) | -    | Frp 是一种快速反向代理，可帮助您将 NAT 或防火墙后面的本地服务器暴露给互联网。 |
| 302 | Frpc                  | [Web](https://gofrp.org/) | -    | Frp 是一个专注于内网穿透的高性能的反向代理应用，支持 TCP、UDP、HTTP、HTTPS 等多种协议。 |
| 303 | Plex                  | [Web](https://www.plex.tv/) | -    | 从您自己服务器上的个人媒体，到免费和点播的电影和节目，直播电视，播客和网络节目，再到流媒体音乐，您可以在任何设备上的一个应用程序中享受所有这些内容。 |
| 304 | Emby-Server           | [Web](https://emby.media/) | -    | 将您的个人媒体同步到云，以便轻松备份、存档和转换。以多种分辨率存储您的内容，以便从任何设备直接流式传输。 |
| 305 | OpenMediaVault | [Web](https://www.openmediavault.org/) | -    | openmediavault 是基于 Debian 的下一代网络附加存储（NAS）解决方案。 |

## 软件中心使用说明

登录 Armbian 系统 → 输入命令：

```yaml
armbian-software
```

将出现当前已经集成的软件快捷安装/管理列表，如：

```yaml
root@armbian:~# armbian-software
[ STEPS ] Start selecting software [ Current system: debian:bullseye ]...
----------------------------------------------------------
ID    NAME                STATE           MANAGE
----------------------------------------------------------
102   Portainer           installed       update/remove
202   Firefox             installed       update/remove
302   Frpc                not-installed   install
...
----------------------------------------------------------
[ OPTIONS ] Please Input Software ID:
```

- `未安装的软件`：状态显示为 `not-installed`，输入软件对应的 `ID` 即可 `安装`。
- `已安装的软件`：状态显示为 `installed`，输入软件对应的 `ID` ，根据提示选择 `更新` 或 `删除`。

## 软件中心开发说明

软件中心的脚本/命令集中存放在 [/usr/share/ophub/armbian-software](../common-files/rootfs/usr/share/ophub/armbian-software) 目录下，使用 `armbian-software -u` 命令可以同步下载此目录至本地，更新本地的软件中心列表。其中以数字开头的文件是对应软件的 `一键安装脚本` 文件。[software-command.sh](../common-files/rootfs/usr/share/ophub/armbian-software/software-command.sh) 是使用命令安装/更新/删除操作的`统一指令文件`。[software-list.conf](../common-files/rootfs/usr/share/ophub/armbian-software/software-list.conf) 是软件列表配置文件，说明如下：

```yaml
# 1.ID     2.SoftwareName     3.AuthMethod@Package      4.Command/Shell     5.SupportedRelease
102        :portainer         :docker@portainer         :command            :jammy@focal@bullseye
203        :firefox           :dpkg@firefox             :command            :jammy@focal@bullseye
302        :frpc              :which@frpc               :302-frpc.sh        :jammy@focal@bullseye
...
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

