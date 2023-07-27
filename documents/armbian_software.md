# Armbian Software Center

According to user feedback and demands in the [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) section, common [software](../armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) is gradually integrated to achieve one-click installation/update/uninstallation and other convenient operations. This includes `docker images`, `desktop software`, `application services`, etc.

## Software introduction

| ID | SoftwareName           | Home | DockerHub:Port | Software introduction                             |
| --- | --------------------- | --------------------- | ---- | ------------------------------------------------- |
| 101 | Docker                | [Home](https://www.docker.com/) | [Docker](https://docs.docker.com/engine/install/) | Docker is an open platform for developing, shipping, and running applications. |
| 102 | Portainer             | [Home](https://www.portainer.io/) | [Docker](https://hub.docker.com/r/portainer/portainer-ce) :9443 | The most popular container management platform in the world. |
| 103 | Yacht                 | [Home](https://yacht.sh/) | [Docker](https://hub.docker.com/r/selfhostedpro/yacht) :8001 | A container management UI with a focus on templates and 1-click deployments. |
| 104 | Transmission          | [Home](https://transmissionbt.com/) | [Docker](https://github.com/linuxserver/docker-transmission) :9091 | Transmission is a cross-platform BitTorrent client. |
| 105 | qBittorrent           | [Home](https://www.qbittorrent.org/) | [Docker](https://hub.docker.com/r/linuxserver/qbittorrent) :8080 | qBittorrent is a BitTorrent client. |
| 106 | NextCloud             | [Home](https://nextcloud.com/) | [Docker](https://hub.docker.com/r/arm64v8/nextcloud) :8088 | Nextcloud offers an on-premise Universal File Access and sync platform with powerful collaboration capabilities and desktop, mobile and web interfaces. |
| 107 | Jellyfin              | [Home](https://jellyfin.org/) | [Docker](https://hub.docker.com/r/linuxserver/jellyfin) :8096 | Jellyfin is a Free Software Media System that puts you in control of managing and streaming your media. |
| 108 | HomeAssistant         | [Home](https://www.home-assistant.io/) | [Docker](https://hub.docker.com/r/linuxserver/homeassistant) :8123 | Home Assistant integrates with over a thousand different devices and services, Has powerful automation features. |
| 109 | Kodbox                | [Home](https://kodcloud.com/) | [Docker](https://hub.docker.com/r/kodcloud/kodbox) :8081 | Private cloud online document management solution. |
| 110 | CouchPotato           | [Home](https://couchpota.to/) | [Docker](https://hub.docker.com/r/linuxserver/couchpotato) :5050 | Couchpotato is an automatic NZB and torrent downloader, Automatically find movies you want to watch. |
| 111 | Sonarr                | [Home](https://sonarr.tv/) | [Docker](https://hub.docker.com/r/linuxserver/sonarr) :8989 | Sonarr is a PVR for Usenet and BitTorrent users. |
| 112 | Radarr                | [Home](https://radarr.video/) | [Docker](https://hub.docker.com/r/linuxserver/radarr) :7878 | Radarr is a movie collection manager for Usenet and BitTorrent users. |
| 113 | Syncthing             | [Home](https://syncthing.net/) | [Docker](https://hub.docker.com/r/linuxserver/syncthing) :8384 | Syncthing is a continuous file synchronization program. |
| 114 | FileBrowser           | [Home](https://filebrowser.org/) | [Docker](https://hub.docker.com/r/filebrowser/filebrowser) :8002 | File Browser provides a file managing interface within a specified directory and it can be used to upload, delete, preview, rename and edit your files. |
| 115 | Heimdall              | [Home](https://heimdall.site/) | [Docker](https://hub.docker.com/r/linuxserver/heimdall) :8003 | Heimdall is a way to organise all those links to your most used web sites and web applications in a simple way. |
| 116 | Node-RED              | [Home](https://nodered.org/) | [Docker](https://nodered.org/docs/getting-started/docker) :1880 | Node-RED is a programming tool. It provides a browser-based editor that makes it easy to wire together flows using the wide range of nodes in the palette that can be deployed to its runtime in a single-click. |
| 117 | Mosquitto             | [Home](https://www.mosquitto.org/) | [Docker](https://hub.docker.com/r/arm64v8/eclipse-mosquitto) :1883 | The MQTT protocol provides a lightweight method of carrying out messaging using a publish/subscribe model. Eclipse Mosquitto is an open source implementation of a server for versions 5, 3.1.1, and 3.1 of the MQTT protocol. |
| 118 | OpenWrt               | [Home](https://www.openwrt.org/) | [Docker](https://hub.docker.com/r/ophub/openwrt-aarch64) :80 | The OpenWrt Project is a Linux operating system targeting embedded devices, it has more than 3000+ standardized application packages and a very rich third-party plug-in support. |
| 119 | Netdata               | [Home](https://learn.netdata.cloud/) | [Docker](https://hub.docker.com/r/netdata/netdata) :19999 | Netdata is distributed, real-time, performance and health monitoring for systems and applications. |
| 120 | XunLei                | [Home](https://github.com/cnk3x/xunlei) | [Docker](https://hub.docker.com/r/cnk3x/xunlei) :2345 | The Thunder remote download service program extracted from the Thunder Synology suite. |
| 121 | Docker-Headless       | [Home](https://github.com/infrastlabs/docker-headless) | [Docker](https://hub.docker.com/r/infrastlabs/docker-headless) :10081 | Remote Desktop with Audio/Locale/IBus Support. Multi Desktop (Gnome, Plasma, Mate, Xfce, Cinnamon) |
| 122 | Navidrome             | [Home](https://www.navidrome.org/) | [Docker](https://hub.docker.com/r/deluan/navidrome) :4533  | Navidrome is a self-hosted, open source music server and streamer. It gives you freedom to listen to your music collection from any browser or mobile device. |
| 123 | Alist                 | [Home](https://alist.nn.ci/) | [Docker](https://hub.docker.com/r/xhofe/alist) :5244  | A file list program that supports multiple storage, powered by Gin and Solidjs. |
| 124 | QingLong              | [Home](https://github.com/whyour/qinglong) | [Docker](https://hub.docker.com/r/whyour/qinglong) :5700  | A timed task management panel that supports typescript, javaScript, python3, and shell. |
| 125 | Chatgpt-Web           | [Home](https://github.com/Chanzhaoyu/chatgpt-web) | [Docker](https://hub.docker.com/r/chenzhaoyu94/chatgpt-web) :3002  | ChatGPT demo webpage built with Express and Vue3. |
| 126 | Pandora(Chatgpt)      | [Home](https://github.com/pengzhile/pandora) | [Docker](https://hub.docker.com/r/pengzhile/pandora) :3003  | Pandora has achieved a local offline deployment of the web version of ChatGPT, with the same functionality and impressive speed. |
| 201 | Desktop:GNOME         | [Home](https://www.gnome.org/) | -    | GNOME provides a focused desktop working environment that helps you get things done. |
| 202 | Firefox               | [Home](https://www.mozilla.org/) | -    | An excellent web browser. |
| 203 | VLC                   | [Home](https://www.videolan.org/) | -    | VLC is a free, open source, cross-platform multimedia player and framework that can play most multimedia files. |
| 204 | MPV                   | [Home](https://mpv.io/) | -    | A free, open source, and cross-platform media player. |
| 205 | GIMP                  | [Home](https://www.gimp.org/) | -    | GIMP is a cross-platform image editor. |
| 206 | Krita                 | [Home](https://krita.org/) | -    | Krita is a professional FREE and open source painting program. |
| 207 | LibreOffice           | [Home](https://www.libreoffice.org/) | -    | LibreOffice is a free and powerful office suite, Its clean interface and feature-rich tools help you unleash your creativity and enhance your productivity. |
| 208 | Shotcut               | [Home](https://shotcut.org/) | -    | Shotcut is a free, open source, cross-platform video editor. |
| 209 | Kdenlive              | [Home](https://kdenlive.org/) | -    | Kdenlive is Free and Open Source Video Editor. |
| 210 | Thunderbird           | [Home](https://www.thunderbird.net/) | -    | Thunderbird makes email better for you, bringing together speed, privacy and the latest technologies. |
| 211 | Evolution             | [Home](https://github.com/GNOME/evolution) | -    | Evolution is a personal information management application that provides integrated mail, calendaring and address book functionality. |
| 212 | Gwenview              | [Home](https://apps.kde.org/gwenview/) | -    | Gwenview is a fast and easy to use image viewer by KDE, ideal for browsing and displaying a collection of images. |
| 213 | Eog                   | [Home](https://gitlab.gnome.org/GNOME/eog) | -    | Eye of GNOME(eog) is a image viewer program.  It is meant to be a fast and functional image viewer. |
| 214 | VisualStudioCode      | [Home](https://code.visualstudio.com/) | -    | Visual Studio Code is a lightweight but powerful source code editor. |
| 215 | Gedit                 | [Home](https://github.com/GNOME/gedit) | -    | Gedit is a full-featured text editor for the GNOME desktop environment. You can use it to prepare simple notes and documents, or you can use some of its advanced features, making it your own software development environment. |
| 216 | Flameshot             | [Home](https://flameshot.org/) | -    | Powerful, yet simple to use open-source screenshot software. |
| 301 | Frps                  | [Home](https://gofrp.org/) | -    | A fast reverse proxy to help you expose a local server behind a NAT or firewall to the internet. |
| 302 | Frpc                  | [Home](https://gofrp.org/) | -    | Frp is a high-performance reverse proxy application focusing on intranet penetration, supporting TCP, UDP, HTTP, HTTPS and other protocols. |
| 303 | NPS                   | [Home](https://ehang-io.github.io/nps) | -    | NPS is a lightweight, high-performance, powerful intranet penetration proxy server, with a powerful web management terminal. |
| 304 | NPC                   | [Home](https://ehang-io.github.io/nps) | -    | NPCs are clients of NPS. |
| 305 | Plex                  | [Home](https://www.plex.tv/) | -    | From personal media on your own server, to free and on-demand Movies & Shows, live TV, podcasts, and web shows, to streaming music, you can enjoy it all in one app, on any device. |
| 306 | Emby-Server           | [Home](https://emby.media/) | -    | Sync your personal media to the cloud for easy backup, archiving, and converting. Store your content in multiple resolutions to enable direct streaming from any device. |
| 307 | KVM                   | [Home](https://virt-manager.org/) | -    | KVM (for Kernel-Based Virtual Machines) is a complete virtualization solution for Linux with virtualization extensions. The virt-manager application is a desktop user interface for managing virtual machines through libvirt. It primarily targets KVM VMs, but also manages Xen and LXC (linux containers). KVM virtual machine can install [OpenWrt](https://github.com/unifreq/openwrt_packit), Debian, Ubuntu, OpenSUSE, ArchLinux, Centos, Gentoo, KyLin, UOS, etc. |
| 308 | PVE                   | [Home](https://github.com/pimox/pimox7) | https://IP:8006 | Proxmox Virtual Environment is an open source server virtualization management solution based on QEMU/KVM and LXC. You can manage virtual machines, containers, highly available clusters, storage and networks with an integrated, easy-to-use web interface or via CLI. |
| 309 | CasaOS                | [Home](https://github.com/IceWhaleTech/CasaOS) | https://IP:81 | CasaOS is a simple, easy-to-use, elegant open-source Personal Cloud system. |

## Software Center Usage Guide

Log into the Armbian system → Enter the command:

```shell
armbian-software
```

A list of integrated software shortcuts for installation/management will appear, such as:

```shell
root@armbian:~# armbian-software
[ STEPS ] Start selecting software [ Current system: debian/bullseye ]...
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

- `Uninstalled software`: The status shows `not-installed`, input the `ID` of the software to `install`.
- `Installed software`: The status shows `installed`, input the `ID` of the software and choose to `update` or `remove` as prompted.

## Software Center Development Guide

The scripts/commands of the software center are stored in the [/usr/share/ophub/armbian-software](../armbian-files/common-files/usr/share/ophub/armbian-software) directory. You can use the `armbian-software -u` command to sync and download this directory to the local system, updating the local software center list. The files starting with a number are `one-click installation script` files for the corresponding software. [software-command.sh](../armbian-files/common-files/usr/share/ophub/armbian-software/software-command.sh) is a `unified instruction file` for installing/updating/removing software by commands. [software-list.conf](../armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) is the software list configuration file, explained as follows:

```yaml
# 1.ID     2.Software Name     3.AuthMethod@Package      4.Execute Selection     5.Supported Release
102        :Portainer          :docker@portainer         :command-docker.sh      :jammy@focal@bullseye
202        :Firefox            :dpkg@firefox             :command-desktop.sh     :jammy@focal@bullseye
302        :Frpc               :which@frpc               :302-frpc.sh            :jammy@focal@bullseye
303        :NPS                :which@nps                :command-service.sh     :jammy@focal@bullseye
...
```

- `ID`: The `unique serial number` of the software.
- `Software Name`: The `software name` (the name length is required to be less than 40 characters).
- `AuthMethod@Package`: The `check method` for the installation status of the software and the corresponding `software package`, separated by the `@` symbol.
  - For software installed with the `docker` container, check with the `docker` method, such as checking whether the `portainer` image is installed, check with `docker@portainer`.
  - For software installed with the `apt` method, check with the `dpkg` method, such as checking whether the `firefox` software package is installed, check with `dpkg@firefox`.
  - For binary executable files installed by downloading with methods such as `wget`, check with the `which` method, such as checking whether the `frpc` service is installed, check with `which@frpc`.
- `Execute Selection`: Set the software to use a `unified instruction file` or an `independent script` for management.
  - For `more streamlined` operation commands, they are collectively written in `command-docker.sh / command-desktop.sh / command-service.sh` files according to software classification, and named by software number. For example, the serial number of `portainer` is `102`, and its operation is written in `software_203()`.
  - For operations that are `more complex and long in instructions`, they are managed by independent script files. For example, the independent script for installing `frpc` is named `302-frpc.sh` with the number as the prefix.
- `Supported Release`: Set the supported Armbian `system version`. Use the `@` symbol for separation.

We welcome everyone to contribute more software. Feel free to submit support requests in the [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues).



# Armbian 软件中心

根据用户在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中的需求反馈，逐步整合常用[软件](../armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf)，实现一键安装/更新/卸载等快捷操作。包括 `docker 镜像`、`桌面软件`、`应用服务` 等。

## 软件介绍

| ID | 软件名称               | Home | DockerHub:Port | 软件介绍                                            |
| --- | --------------------- | --------------------- | ---- | ------------------------------------------------- |
| 101 | Docker                | [Home](https://www.docker.com/) | [Docker](https://docs.docker.com/engine/install/) | Docker 是一个用于开发、发布和运行应用程序的开放平台。 |
| 102 | Portainer             | [Home](https://www.portainer.io/) | [Docker](https://hub.docker.com/r/portainer/portainer-ce) :9443 | 全球最受欢迎的容器管理平台。 |
| 103 | Yacht                 | [Home](https://yacht.sh/) | [Docker](https://hub.docker.com/r/selfhostedpro/yacht) :8001 | 容器管理 UI，侧重于模板和一键式部署。 |
| 104 | Transmission          | [Home](https://transmissionbt.com/) | [Docker](https://github.com/linuxserver/docker-transmission) :9091 | Transmission 是一个跨平台的 BitTorrent 客户端。 |
| 105 | qBittorrent           | [Home](https://www.qbittorrent.org/) | [Docker](https://hub.docker.com/r/linuxserver/qbittorrent) :8080 | qBittorrent 是一个 BitTorrent 客户端。  |
| 106 | NextCloud             | [Home](https://nextcloud.com/) | [Docker](https://hub.docker.com/r/arm64v8/nextcloud) :8088 | Nextcloud 提供了一个本地通用文件访问和同步平台，具有强大的协作功能以及桌面，移动和 Web 界面。 |
| 107 | Jellyfin              | [Home](https://jellyfin.org/) | [Docker](https://hub.docker.com/r/linuxserver/jellyfin) :8096 | Jellyfin 是一个自由软件媒体系统，让你控制管理和流媒体你的媒体。 |
| 108 | HomeAssistant         | [Home](https://www.home-assistant.io/) | [Docker](https://hub.docker.com/r/linuxserver/homeassistant) :8123 | Home Assistant 集成了一千多种不同的设备和服务，具有强大的自动化功能。 |
| 109 | Kodbox                | [Home](https://kodcloud.com/) | [Docker](https://hub.docker.com/r/kodcloud/kodbox) :8081 | 私有云在线文档管理解决方案。 |
| 110 | CouchPotato           | [Home](https://couchpota.to/) | [Docker](https://hub.docker.com/r/linuxserver/couchpotato) :5050 | Couchpotato 是一个自动 NZB 和 torrent 下载器，自动查找要观看的电影。 |
| 111 | Sonarr                | [Home](https://sonarr.tv/) | [Docker](https://hub.docker.com/r/linuxserver/sonarr) :8989 | Sonarr 是 Usenet 和 BitTorrent 用户的个人视频录像机。 |
| 112 | Radarr                | [Home](https://radarr.video/) | [Docker](https://hub.docker.com/r/linuxserver/radarr) :7878 | Radarr 是 Usenet 和 BitTorrent 用户的电影收藏管理器。 |
| 113 | Syncthing             | [Home](https://syncthing.net/) | [Docker](https://hub.docker.com/r/linuxserver/syncthing) :8384 | Syncthing 是一个连续的文件同步程序。 |
| 114 | FileBrowser           | [Home](https://filebrowser.org/) | [Docker](https://hub.docker.com/r/filebrowser/filebrowser) :8002 | File Browser 在指定目录中提供文件管理界面，可用于上传，删除，预览，重命名和编辑文件。 |
| 115 | Heimdall              | [Home](https://heimdall.site/) | [Docker](https://hub.docker.com/r/linuxserver/heimdall) :8003 | Heimdall 是一种以简单的方式组织所有这些链接到您最常用的网站和 Web 应用程序的方法。 |
| 116 | Node-RED              | [Home](https://nodered.org/) | [Docker](https://nodered.org/docs/getting-started/docker) :1880 | Node-RED 是一种编程工具。它提供了一个基于浏览器的编辑器，使得使用调色板中的各种节点轻松地将流连接在一起，只需单击一下即可部署运行。 |
| 117 | Mosquitto             | [Home](https://www.mosquitto.org/) | [Docker](https://hub.docker.com/r/arm64v8/eclipse-mosquitto) :1883 | MQTT 协议提供了一种使用发布/订阅模型执行消息传递的轻量级方法。Eclipse Mosquitto 是 MQTT 协议版本 5、3.1.1 和 3.1 的服务器的开源实现。 |
| 118 | OpenWrt               | [Home](https://www.openwrt.org/) | [Docker](https://hub.docker.com/r/ophub/openwrt-aarch64) :80 | OpenWrt 项目是一个针对嵌入式设备的 Linux 操作系统，它拥有超过 3000+ 个标准化应用软件包和非常丰富的第三方插件支持。 |
| 119 | Netdata               | [Home](https://learn.netdata.cloud/) | [Docker](https://hub.docker.com/r/netdata/netdata) :19999 | Netdata 是针对系统和应用程序的分布式实时性能和运行状况监控。 |
| 120 | XunLei                | [Home](https://github.com/cnk3x/xunlei) | [Docker](https://hub.docker.com/r/cnk3x/xunlei) :2345 | 从迅雷群晖套件中提取出来的迅雷远程下载服务程序。 |
| 121 | Docker-Headless       | [Home](https://github.com/infrastlabs/docker-headless) | [Docker](https://hub.docker.com/r/infrastlabs/docker-headless) :10081 | 具有音频/区域设置/IBus 支持的远程桌面。 支持多桌面（Gnome、Plasma、Mate、Xfce、Cinnamon） |
| 122 | Navidrome             | [Home](https://www.navidrome.org/) | [Docker](https://hub.docker.com/r/deluan/navidrome) :4533  | Navidrome 是一个自托管的开源音乐服务器和流媒体。 它使您可以自由地从任何浏览器或移动设备收听音乐收藏。 |
| 123 | Alist                 | [Home](https://alist.nn.ci/zh/) | [Docker](https://hub.docker.com/r/xhofe/alist) :5244  | 一个支持多种存储的文件列表程序，使用 Gin 和 Solidjs。 |
| 124 | QingLong              | [Home](https://github.com/whyour/qinglong) | [Docker](https://hub.docker.com/r/whyour/qinglong) :5700  | 支持 python3、javaScript、shell、typescript 的定时任务管理面板。 |
| 125 | Chatgpt-Web           | [Home](https://github.com/Chanzhaoyu/chatgpt-web) | [Docker](https://hub.docker.com/r/chenzhaoyu94/chatgpt-web) :3002  | 用 Express 和 Vue3 搭建的 ChatGPT 演示网页。 |
| 126 | Pandora(Chatgpt)      | [Home](https://github.com/pengzhile/pandora) | [Docker](https://hub.docker.com/r/pengzhile/pandora) :3003  | 潘多拉实现了网页版 ChatGPT 本地离线部署，功能相同,速度喜人。 |
| 201 | Desktop:GNOME         | [Home](https://www.gnome.org/) | -    | GNOME 提供了一个专注的桌面工作环境，可帮助您完成工作。 |
| 202 | Firefox               | [Home](https://www.mozilla.org/) | -    | 一款优秀的网页浏览器。 |
| 203 | VLC                   | [Home](https://www.videolan.org/) | -    | VLC 是一款自由、开源的跨平台多媒体播放器及框架，可播放大多数多媒体文件。 |
| 204 | MPV                   | [Home](https://mpv.io/) | -    | 一个免费的、开源的、跨平台的媒体播放器。 |
| 205 | GIMP                  | [Home](https://www.gimp.org/) | -    | GIMP 是一个跨平台的图像编辑器。 |
| 206 | Krita                 | [Home](https://krita.org/) | -    | Krita 是一个专业的免费和开源绘画程序。 |
| 207 | LibreOffice           | [Home](https://www.libreoffice.org/) | -    | LibreOffice 是一个免费且功能强大的办公套件，其简洁的界面和功能丰富的工具可帮助您释放创造力并提高生产力。 |
| 208 | Shotcut               | [Home](https://shotcut.org/) | -    | Shotcut 是一个免费的，开源的，跨平台的视频编辑器。 |
| 209 | Kdenlive              | [Home](https://kdenlive.org/) | -    | Kdenlive 是一个免费和开源视频编辑器。 |
| 210 | Thunderbird           | [Home](https://www.thunderbird.net/) | -    | Thunderbird 将速度、隐私和最新技术结合在一起，让您更好地利用电子邮件。 |
| 211 | Evolution             | [Home](https://github.com/GNOME/evolution) | -    | Evolution 是一个个人信息管理应用程序，提供集成的邮件、日历和地址簿功能。 |
| 212 | Gwenview              | [Home](https://apps.kde.org/gwenview/) | -    | Gwenview 是 KDE 出品的一款轻便易用的图像查看器，是浏览、显示多张图像时的理想工具。 |
| 213 | Eog                   | [Home](https://gitlab.gnome.org/GNOME/eog) | -    | Eye of GNOME（eog）一个图像查看器程序。它旨在成为一个快速且功能强大的图像查看器。 |
| 214 | VisualStudioCode      | [Home](https://code.visualstudio.com/) | -    | Visual Studio Code 是一个轻量级但功能强大的源代码编辑器。 |
| 215 | Gedit                 | [Home](https://github.com/GNOME/gedit) | -    | Gedit 是一个用于 GNOME 桌面环境的全功能文本编辑器。您可以使用它来准备简单的笔记和文档，也可以使用它的一些高级功能，使其成为您自己的软件开发环境。 |
| 216 | Flameshot             | [Home](https://flameshot.org/) | -    | 功能强大，但易于使用的开源屏幕截图软件。 |
| 301 | Frps                  | [Home](https://gofrp.org/) | -    | Frp 是一种快速反向代理，可帮助您将 NAT 或防火墙后面的本地服务器暴露给互联网。 |
| 302 | Frpc                  | [Home](https://gofrp.org/) | -    | Frp 是一个专注于内网穿透的高性能的反向代理应用，支持 TCP、UDP、HTTP、HTTPS 等多种协议。 |
| 303 | NPS                   | [Home](https://ehang-io.github.io/nps) | -    | NPS 服务端。一款轻量级、高性能、功能强大的内网渗透代理服务器，具有强大的 Web 管理终端。 |
| 304 | NPC                   | [Home](https://ehang-io.github.io/nps) | -    | NPC 是 NPS 的客户端。 |
| 305 | Plex                  | [Home](https://www.plex.tv/) | -    | 从您自己服务器上的个人媒体，到免费和点播的电影和节目，直播电视，播客和网络节目，再到流媒体音乐，您可以在任何设备上的一个应用程序中享受所有这些内容。 |
| 306 | Emby-Server           | [Home](https://emby.media/) | -    | 将您的个人媒体同步到云，以便轻松备份、存档和转换。以多种分辨率存储您的内容，以便从任何设备直接流式传输。 |
| 307 | KVM                   | [Home](https://virt-manager.org/) | -    | KVM（用于基于内核的虚拟机）是包含虚拟化扩展适用于 Linux 的完整虚拟化解决方案。virt-manager 应用程序是一个桌面用户界面，用于通过 libvirt 管理虚拟机。它主要针对 KVM VM，但也管理 Xen 和 LXC（Linux 容器）。KVM 虚拟机可以安装 [OpenWrt](https://github.com/unifreq/openwrt_packit)、Debian、Ubuntu、OpenSUSE、ArchLinux、Centos、Gentoo、KyLin、UOS 等等。 |
| 308 | PVE                   | [Home](https://github.com/pimox/pimox7) | https://IP:8006 | Proxmox 虚拟环境是一个基于 QEMU/KVM 和 LXC 的开源服务器虚拟化管理解决方案。您可以使用集成的、易于使用的 web 界面或通过 CLI 管理虚拟机、容器、高可用集群、存储和网络。 |
| 309 | CasaOS                | [Home](https://github.com/IceWhaleTech/CasaOS) | https://IP:81 | CasaOS 是一个简单、易于使用、优雅的开源个人云系统。 |

## 软件中心使用说明

登录 Armbian 系统 → 输入命令：

```shell
armbian-software
```

将出现当前已经集成的软件快捷安装/管理列表，如：

```shell
root@armbian:~# armbian-software
[ STEPS ] Start selecting software [ Current system: debian/bullseye ]...
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

软件中心的脚本/命令集中存放在 [/usr/share/ophub/armbian-software](../armbian-files/common-files/usr/share/ophub/armbian-software) 目录下，使用 `armbian-software -u` 命令可以同步下载此目录至本地，更新本地的软件中心列表。其中以数字开头的文件是对应软件的 `一键安装脚本` 文件。[software-command.sh](../armbian-files/common-files/usr/share/ophub/armbian-software/software-command.sh) 是使用命令安装/更新/删除操作的`统一指令文件`。[software-list.conf](../armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) 是软件列表配置文件，说明如下：

```yaml
# 1.ID     2.Software Name     3.AuthMethod@Package      4.Execute Selection     5.Supported Release
102        :Portainer          :docker@portainer         :command-docker.sh      :jammy@focal@bullseye
202        :Firefox            :dpkg@firefox             :command-desktop.sh     :jammy@focal@bullseye
302        :Frpc               :which@frpc               :302-frpc.sh            :jammy@focal@bullseye
303        :NPS                :which@nps                :command-service.sh     :jammy@focal@bullseye
...
```

- `ID`: 软件的 `唯一序号` 。
- `Software Name`：`软件名称`（名称长度要求小于 40 个字符）。
- `AuthMethod@Package`：软件安装状态的`检查方法`，与对应的`软件包`，使用 `@` 符号分割。
  - 使用 `docker` 容器安装的镜像，采用 `docker` 方式检查，如检查是否安装了 `portainer` 镜像，使用 `docker@portainer` 进行检查；
  - 使用 `apt` 方式安装的软件包采用 `dpkg` 方式检查，如检查是否安装了 `firefox` 软件包，使用 `dpkg@firefox` 进行检查；
  - 使用 `wget` 等方式下载安装的二进制执行文件，采用 `which` 方式检查，如检查是否安装了 `frpc` 服务，使用 `which@frpc` 进行检查。
- `Execute Selection`：设置软件使用 `统一指令文件` 或 `独立脚本` 进行管理。
  - 对于 `比较精简` 的操作命令，按照软件分类，集中写在 `command-docker.sh / command-desktop.sh / command-service.sh` 文件中，以软件序号进行命名。如 `portainer` 的序号是 `102`，他的操作写在 `software_203()` 中；
  - 对于 `比较复杂、指令内容较长` 的操作，进行独立脚本文件管理。如安装 `frpc` 的独立脚本，以序号开头，命名为 `302-frpc.sh` 。
- `Supported Release`：设置支持的 Armbian `系统版本`。使用 `@` 符号分割。

欢迎大家补充更多软件。欢迎在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中提交支持需求。

