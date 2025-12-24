<div align="center">
    <img alt="Armbian" src="https://github.com/user-attachments/assets/74e55052-031b-48f8-9aca-e5f1dd9e256a" />
</div>

# Armbian / 岸边

查看英文说明 | [View English description](README.md)

Armbian（中文名：岸边）系统是基于 Debian/Ubuntu 而构建的专门用于 ARM 芯片的轻量级 Linux 系统。Armbian 系统精益、干净，并且 100% 兼容并继承了 Debian/Ubuntu 系统的功能和丰富的软件生态，可以安全稳定地运行在 TF/SD/USB 及设备的 eMMC 里。这个项目保留了 Armbian 官方系统的完整性，并进一步拓展了在电视盒子等一些非官方支持设备上的使用，增加了一些便捷操作指令。现在你可以将电视盒子的安卓 TV 系统更换为 Armbian 系统，让他成为一台功能强大的服务器。

本项目依托众多的[贡献者](CONTRIBUTORS.md)，为 `Amlogic`，`Rockchip` 和 `Allwinner` 盒子构建 Armbian 系统，支持写入 eMMC 中使用，支持更新内核等功能，使用方法详见 [Armbian 使用文档](./documents/README.cn.md)。最新的 Armbian 系统可以在 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 中下载。欢迎 `Fork` 并进行个性化定制。如果对你有用，可以点仓库右上角的 `Star` 表示支持。

## Armbian 系统默认信息

| 系统名称        | 默认账号 | 默认密码  | SSH 端口 | IP 地址 |
| -------------- | ------- | ------- | ------- | ------- |
| 🐧 [Armbian.OS](https://github.com/ophub/amlogic-s9xxx-armbian/releases) | root | 1234 | 22 | 从路由器获取 IP |
| 🐋 [Armbian.Docker](https://hub.docker.com/u/ophub) | root | 1234 | 22 | 静态 MacVLAN IP |

## 支持的设备列表

⬆️ 各平台（晶晨/瑞芯微/全志）型号均按 SoC 性能由高至低排列。

| SoC  | [设备](https://github.com/ophub/amlogic-s9xxx-armbian/releases) | [内核](https://github.com/ophub/kernel) |
| ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/99), [WXY-OES](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2666) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s922x | [Beelink-GT-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/370), [Beelink-GT-King-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/707), [Ugoos-AM6-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/464), [ODROID-N2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/201), [X88-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/988), [Ali-CT2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1150), [WXY-OES-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3029) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905x3 | [X96-Max+](https://github.com/ophub/amlogic-s9xxx-armbian/issues/351), [HK1-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/414), [Vontar-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1006), [H96-Max-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1250), [Ugoos-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/782), [TX3(QZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/644), [TX3(BZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1077), [X96-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/366), [X96-Max+_A100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/779), [A95X-F3-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2282), [Tencent-Aurora-3Pro(s905x3-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/506), [X96-Max+Q1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/788), [X96-Max+100W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/909), [X96-Max+_2101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1086), [Infinity-B32](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1181), [Whale](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1166), [X88-Pro-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [X99-Max-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [Transpeed-X3-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905x2 | [X96Max-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/453), [X96Max-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/95), [MECOOL-KM3-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/79), [Tanix-Tx5-Max](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/351), [A95X-F2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/851), [HG680-FJ](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3089) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l3a | [E900V22C/D](https://github.com/Calmact/e900v22c), [CM311-1a-YST](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/732), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [UNT413A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [ZTE-B863AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/741), [CM311-1a-CH](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1508), [IP112H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1520), [B863AV3.1-M2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2292) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l3b | [CM201-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2209), [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1180), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1268), [E900V21D](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2447), [E900V22D](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1256), [E900V21E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1514), [E900V22E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/939), [M302A/M304A](https://github.com/ophub/amlogic-s9xxx-armbian/pull/615), [Hisense-IP103H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1154), [TY1608](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1332), [TY1608](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1568), [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1613), [B860AV-2.1M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1598), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1712), [RG020ET-CA](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1860), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3272) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l3 | [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1318), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/763), [HG680-LC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/978), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921#issuecomment-1453143251), [UNT400G1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1277), [UNT400G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2625), [UNT402A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1481), [ZXV10-BV310](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1512), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1817), [ZXV10-B860AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2012), [ZXV10-B860AV2.1-U](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2273), [E900V22D-2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2058), [CM201-1-6-YS](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2539), [IP108H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2539), [M301A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3055) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s912 | [Tanix-TX8-Max](https://github.com/ophub/amlogic-s9xxx-armbian/issues/500), [Tanix-TX9-Pro(3G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/315), [Tanix-TX9-Pro(2G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/740), [Tanix-TX92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1012790770), [Tanix-TX9S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3282), [Nexbox-A1](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [Nexbox-A95X-A2](https://www.cafago.com/en/p-v2979eu-2g.html),  [A95X](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [H96-Pro-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1013071513), [VORKE-Z6-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72), [Mecool-M8S-PRO-L](https://github.com/ophub/amlogic-s9xxx-armbian/issues/158), [Vontar-X92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/525), [T95Z-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/668), [Octopus-Planet](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1020), [Phicomm-T1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/522), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1882), [OneCloudPro-V1.1_V1.2](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2241) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905d | [MECOOL-KI-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/59), [Phicomm-N1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/925), [SML-5442TW](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/451) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905x | [HG680P](https://github.com/ophub/amlogic-s9xxx-armbian/issues/262), [B860H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/60), [TBee-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/98), [T95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/285), [TX9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/645), [XiaoMI-3S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1405), [X96](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1480), [Nexbox-a95x](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1714), [BTV9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3256) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905mb | [S65](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1644) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l | [UNT402A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1481), [M201-S](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/444), [MiBox-4](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2101), [MiBox-4C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1826), [MG101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1912), [E900V21C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2341), [IP108H-53u1m](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2357), [Tencent-Aurora-1s](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2465), [B860AV2.1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2491), [B860AV2.1U](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2499), [HM201](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2585) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l2 | [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [MGV2000-K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1839), [MGV3000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921), [Wojia-TV-IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [M301A](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/405), [E900v21E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1278), [e900v21d](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2127), [CM201-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2188), [IP108H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2598), [MGV2000-CW](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2616) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905lb | [Q96-mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/734), [BesTV-R3300L](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993), [SumaVision-Q7](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1190), [MG101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1570), [s65](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2128), [IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993#issuecomment-2276804591) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905w | [X96-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/621), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1062), [W95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/570), [X96W/FunTV](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1044), [MXQ-Pro-4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1140), [MeCool-m8s-pro-W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3245) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905 | [Beelink-Mini-MX-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/127), [Sunvell-T95M](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/337), [MXQ-Pro+4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/715), [SumaVision-Q5](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1175) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3588(s) | [Radxa-Rock5B](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1240), [Radxa-Rock5C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2324), [Orange-Pi-5-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2400), [Beelink-IPC-R](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415), [HLink-H88K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H88K-V3](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [NanoPC-T6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2453), [Smart-Am60](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2817), [DC-A588](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2988), [Orangepi-5B](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3052) | [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) |
| rk3399 | [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132), [ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247), [CRRC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1280), [SMART-AM40](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1317), [SW799](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1326), [ZYSJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380), [DG-3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1492), [DLFR100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522), [Emb3531](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1549), [Leez-p710](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609), [tvi3315a](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1687), [xiaobao](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1698), [Fine3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1790), [Firefly-RK3399](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/491), [LX-R3S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2026), [Hugsun-x99](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2050), [Tb-ls3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2146), [Hisense-hs530r](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/572), [Tpm312](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2403), [ZK-rk39a](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2446), [YSKJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2673), [Fmx1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2691), [Sv-33a6x](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/748), [AIO-3399b](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3185) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3568 | [FastRhino-R66S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1061), [FastRhino-R68S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/774), [Radxa-E25](https://wiki.radxa.com/Rock3/CM/CM3I/E25), [NanoPi-R5S](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217), [NanoPi-R5C](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217), [HLink-H66K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H68K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H69K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [Seewo-sv21](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2017), [Mrkaio-m68s](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2155), [Swan1-w28](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2407), [Ruisen-box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2508), [DG-TN3568](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2661), [Alark35-3500](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2911), [MMBox-Anas3035](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2995), [Wocyber-A3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2978), [Photonicat](https://github.com/ophub/amlogic-s9xxx-openwrt/pull/827), [NSY-G16-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/845), [NSY-G68-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/845), [BDY-G18-Pro](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/847) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3566 | [Panther-X2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1319), [JP-TvBox](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1867), [LCKFB-Taishan-Pi](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2538), [WXY-OEC-turbo-4g](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2736), [Station-M2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/744) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3528 | [HLink-H28K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1726), [Radxa-E20C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2324), [H96-Max-M2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2404) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3328 | [BeikeYun](https://github.com/ophub/amlogic-s9xxx-armbian/issues/852), [Chainedbox-L1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1680), [Station-M1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Bqeel-MVR9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Renegade/Firefly](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1861) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3318 | [RX3318-Box](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2129) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| h6 | [Vplus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1100), [Tanix-TX6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1120), [TQC-A01](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1638) | [h6](https://github.com/ophub/kernel/releases/tag/kernel_h6)<br>[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |

> [!TIP]
> 目前 [s905 的盒子](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1173)只能在 `TF/SD/USB` 中使用，其他型号的盒子支持写入 `EMMC` 中使用。更多信息请查阅 [✅支持的设备列表说明](build-armbian/armbian-files/common-files/etc/model_database.conf)。可以参考说明文档中 12.15 章节的方法[添加新的支持设备](documents/README.cn.md#1215-如何添加新的支持设备)。使用前先阅读 [Armbian 使用文档](./documents/README.cn.md)，常见问题都有解决方法。

## 安装及升级 Armbian 的相关说明

选择和你的盒子型号对应的 Armbian 系统，不同设备的使用方法查看对应的说明。

- ### 安装 Armbian 到 EMMC

1. `Rockchip` 平台的安装方法请查看说明文档中的 [第 8 章节](documents/README.cn.md#8-安装-armbian-到-emmc)。

2. `Amlogic` 和 `Allwinner` 平台，使用 [Rufus](https://rufus.ie/) 或者 [balenaEtcher](https://www.balena.io/etcher/) 等工具将系统写入 USB 里，然后把写好系统的 USB 插入盒子。登录 Armbian 系统 (默认用户: root, 默认密码: 1234) → 输入命令：

```shell
armbian-install
```

| 可选参数  | 默认值   | 选项     | 说明                |
| -------  | ------- | ------  | -----------------   |
| -m       | no      | yes/no  | 使用主线 u-boot |
| -a       | yes     | yes/no  | 使用 [ampart](https://github.com/7Ji/ampart) 分区表调整工具 |
| -l       | no      | yes/no  | 显示全部设备列表 |

举例: `armbian-install -m yes -a no`

- ### 更新 Armbian 内核

登录 Armbian 系统 → 输入命令：

```shell
# 使用 root 用户运行 (sudo -i)
# 如果不指定参数，将更新为最新版本。
armbian-update
```

| 可选参数  | 默认值        | 选项           | 说明                              |
| -------- | ------------ | ------------- | -------------------------------- |
| -r       | ophub/kernel | `<owner>/<repo>` | 设置从 github.com 下载内核的仓库  |
| -u       | 自动化        | stable/flippy/beta/rk3588/rk35xx/h6 | 设置使用的内核的 [tags 后缀](https://github.com/ophub/kernel/releases) |
| -k       | 最新版        | 内核版本       | 设置[内核版本](https://github.com/ophub/kernel/releases/tag/kernel_stable)  |
| -b       | yes          | yes/no        | 更新内核时自动备份当前系统使用的内核    |
| -m       | no           | yes/no        | 使用主线 u-boot                    |
| -s       | 无           | 无/磁盘名称     | [SOS] 恢复 eMMC/NVMe/sdX 等磁盘中的系统内核 |
| -h       | 无           | 无             | 查看使用帮助                       |

举例: `armbian-update -k 5.15.50 -u stable`

通过 `-k` 参数指定内核版本号时，可以准确指定具体版本号，例如：`armbian-update -k 5.15.50`，也可以模糊指定到内核系列，例如：`armbian-update -k 5.15`，当模糊指定时将自动使用指定系列的最新版本。

更新内核时会自动备份当前系统使用的内核，存储路径在 `/ddbr/backup` 目录里，保留最近使用过的 3 个版本的内核，如果新安装的内核不稳定，可以随时恢复回备份的内核。如果更新内核失败导致系统无法启动，可以通过 `armbian-update -s` 来恢复系统内核。更多说明详见 [帮助文档](documents/README.cn.md#10-更新-armbian-内核)

- ### 更换 Armbian 源

登录 Armbian 系统 → 输入命令：

```shell
armbian-apt
```

根据你所在的国家或地区选择合适的软件源，可以提高软件的下载速度。更多说明详见 [帮助文档](documents/README.cn.md#11-安装常用软件)

- ### 安装常用软件

登录 Armbian 系统 → 输入命令：

```shell
armbian-software
```

使用 `armbian-software -u` 命令可以更新本地的软件中心列表。根据用户在 [Issue](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中的需求反馈，逐步整合常用[软件](build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf)，实现一键安装/更新/卸载等快捷操作。包括 `docker 镜像`、`桌面软件`、`应用服务` 等。详见更多[说明](documents/armbian_software.md)。

- ### 修改 Armbian 配置

登录 Armbian 系统 → 输入命令：

```shell
armbian-config
```

- ### 为 Armbian 创建 swap

如果你在使用 `docker` 等内存占用较大的应用时，觉得当前盒子的内存不够使用，可以创建 `swap` 虚拟内存分区，将磁盘空间的一定容量虚拟成内存来使用。下面命令输入参数的单位是 `GB`，默认为 `1`。

登录 Armbian 系统 → 输入命令：

```shell
armbian-swap 1
```

- ### 控制 LED 显示

登录 Armbian 系统 → 输入命令：

```shell
armbian-openvfd
```

根据 [LED 屏显示控制说明](documents/led_screen_display_control.md) 进行调试。

- ### 备份/还原 EMMC 原系统

支持在 `TF/SD/USB` 中对盒子的 `EMMC` 分区进行备份/恢复。建议您在全新的盒子里安装 Armbian 系统前，先对当前盒子自带的安卓 TV 系统进行备份，以便日后在恢复电视系统等情况下使用。

请从 `TF/SD/USB` 启动 Armbian 系统 → 输入命令：

```shell
armbian-ddbr
```

根据提示输入 `b` 进行系统备份，输入 `r` 进行系统恢复。

> [!IMPORTANT]
> 除此之外也可以通过线刷的方法，将安卓系统刷入 eMMC 中，安卓系统的下载镜像可在 [Tools](https://github.com/ophub/kernel/releases/tag/tools) 中查找。

- ### 在 Armbian 中编译内核

在 Armbian 中编译内核的用法详见 [编译内核](compile-kernel/README.cn.md) 说明文档。登录 Armbian 系统 → 输入命令：

```shell
armbian-kernel -u
armbian-kernel -k 6.6.12
```

- ### 更多使用说明

将本地系统中的全部服务脚本更新到最新版本，可以登录 Armbian 系统 → 输入命令：

```shell
armbian-sync
```

在 Armbian 的使用中，一些可能遇到的常见问题详见 [documents](documents/README.cn.md)

## 本地化打包

1. 克隆仓库到本地 `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. 安装必要的软件包（如 Ubuntu 24.04）

```shell
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-24.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2404-build-armbian-depends)
```

3. 进入 `~/amlogic-s9xxx-armbian` 根目录，在根目录下创建文件夹 `build/output/images` ，并上传 Armbian 镜像文件 ( 如：`Armbian_21.11.0-trunk_Odroidn2_current_5.15.50.img` ) 到 `~/amlogic-s9xxx-armbian/build/output/images` 目录里。原版 Armbian 镜像文件名称中的发行版本号（如：`21.11.0`）和内核版本号（如：`5.15.50`）请保留，它将在重构后用作 Armbian 系统的名称。

4. 进入 `~/amlogic-s9xxx-armbian` 根目录，然后运行 `sudo ./rebuild -b s905x3 -k 6.6.12` 命令即可生成指定 board 的 Armbian 镜像文件。生成的文件保存在 `build/output/images` 目录里。

- ### 本地化打包参数说明

| 参数  | 含义       | 说明        |
| ---- | ---------- | ---------- |
| -b   | Board      | 指定需要编译的设备代号。例如，`-b s905x3` 表示编译代号为 s905x3 的设备，多个设备可用下划线连接，如 `-b s905x3_s905d`。特殊值：`all` 表示编译全部设备，`top55` 表示编译设备库中的前 55 个，`after55` 表示从第 56 个开始至最后一个。设备代号列表详见 [model_database.conf](build-armbian/armbian-files/common-files/etc/model_database.conf) 中的 `BOARD` 配置项。默认值为 `all` |
| -r   | KernelRepo | 指定 github.com 内核仓库的 `<owner>/<repo>`。默认值：`ophub/kernel` |
| -u   | kernelUsage | 设置使用的内核的 `tags 后缀`，如 [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable), [flippy](https://github.com/ophub/kernel/releases/tag/kernel_flippy), [beta](https://github.com/ophub/kernel/releases/tag/kernel_beta)。默认值：`stable` |
| -k   | Kernel     | 指定 [kernel](https://github.com/ophub/kernel/releases/tag/kernel_stable) 名称，如 `-k 6.6.12` 。多个内核使用 `_` 进行连接，如 `-k 6.6.12_5.15.50` 。通过 `-k` 参数自由指定的内核版本只对使用 `stable/flippy/beta` 的内核有效。其他内核系列例如 [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) / [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) / [h6](https://github.com/ophub/kernel/releases/tag/kernel_h6) 等只能使用特定内核。  |
| -a   | AutoKernel | 设置是否自动采用同系列最新版本内核。当为 `true` 时，将自动在内核库中查找在 `-k` 中指定的内核如 6.6.12 的同系列是否有更新的版本，如有 6.6.12 之后的最新版本时，将自动更换为最新版。设置为 `false` 时将编译指定版本内核。默认值：`true` |
| -t   | RootfsType | 对系统的 ROOTFS 分区的文件系统类型进行设置，可选项为 `ext4` 或 `btrfs` 类型。例如： `-t btrfs`。默认值：`ext4` |
| -s   | Size       | 对系统的镜像分区大小进行设置，只设置 ROOTFS 分区大小时可以只指定一个数值，例如： `-s 2560`。需要同时设置 BOOTFS 和 ROOTFS 分区大小时，使用 / 对两个数值进行连接，例如： `-s 512/2560`。默认值：`512/2560` |
| -n   | BuilderName | 设置 Armbian 系统构建者签名。设置签名时请勿包含空格。默认值：`无` |

- `sudo ./rebuild` : 使用默认配置，对全部型号的电视盒子进行打包。
- `sudo ./rebuild -b s905x3 -k 6.6.12` : 推荐使用. 使用默认配置进行相关内核打包。
- `sudo ./rebuild -b s905x3 -k 6.1.y` : 使用默认配置进行相关内核打包，内核使用 6.1.y 系列的最新版。
- `sudo ./rebuild -b s905x3_s905d -k 6.6.12_5.15.50` : 使用默认配置，进行多个内核同时打包。使用 `_` 进行多内核参数连接。
- `sudo ./rebuild -b s905x3 -k 6.6.12 -s 2560` : 使用默认配置，指定一个内核，一个型号进行打包，系统大小设定为2560MiB。
- `sudo ./rebuild -b s905x3_s905d`  使用默认配置，对多个型号的电视盒子进行全部内核打包, 使用 `_` 进行多型号连接。
- `sudo ./rebuild -k 6.6.12_5.15.50` : 使用默认配置，指定多个内核，进行全部型号电视盒子进行打包, 内核包使用 `_` 进行连接。
- `sudo ./rebuild -k 6.6.12_5.15.50 -a true` : 使用默认配置，指定多个内核，进行全部型号电视盒子进行打包, 内核包使用 `_` 进行连接。自动升级到同系列最新内核。
- `sudo ./rebuild -t btrfs -s 2560 -k 6.6.12` : 使用默认配置，设置文件系统为 btrfs 格式，分区大小为 2560MiB, 并指定内核为 6.6.12 ，对全部型号电视盒子进行打包。

## 使用 GitHub Actions 进行编译

1. 关于 Workflows 文件的配置在 [.github/workflows/](.github/workflows/) 文件里。

2. 全新编译：在 [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) 页面里选择 ***`Build Armbian server image`*** 即可使用 [build-armbian-arm64-server-image.yml](.github/workflows/build-armbian-arm64-server-image.yml) 进行编译。可以选择 Ubuntu 系列：`jammy`，或者 Debian 系列：`bullseye` 等。点击 ***`Run workflow`*** 按钮即可编译。

3. 再次编译：如果 [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) 中有已经编译好的 `Armbian_.*-trunk_.*.img.gz` 文件，你只是想再次制作其他不同 board 的盒子，可以跳过 Armbian 源文件的编译，直接使用 [build-armbian-using-releases-files.yml](.github/workflows/build-armbian-using-releases-files.yml) 进行二次制作。

4. 使用其他 Armbian 系统，如 Armbian 官方系统下载网站 [armbian.tnahosting.net](https://armbian.tnahosting.net/dl/) 提供的 [odroidn2](https://armbian.tnahosting.net/dl/odroidn2/archive/) 系统，仅在流程控制文件 [build-armbian-using-official-image.yml](.github/workflows/build-armbian-using-official-image.yml) 中引入本仓库的脚本进行 Armbian 重构，即可适配其他盒子的使用。代码如下:

```yaml
- name: Build Armbian
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img
    armbian_board: s905d_s905x3_s922x_s905x
    armbian_kernel: 6.1.y_5.15.y
```

- ### GitHub Actions 输入参数说明

相关参数与`本地打包命令`相对应，请参考上面的说明。

| 参数              | 默认值         | 说明                                             |
|------------------|---------------|--------------------------------------------------|
| armbian_path     | 无            | 设置原版 Armbian 文件的路径，支持使用当前工作流中的文件路径如 `build/output/images/*.img` ，也支持使用网络下载地址如： `https://dl.armbian.com/*/Armbian_*.img.xz` |
| armbian_board    | all           | 设置打包盒子的 `board` ，功能参考 `-b`                 |
| kernel_repo      | ophub/kernel  | 指定 github.com 内核仓库的 `<owner>/<repo>`，功能参考 `-r` |
| kernel_usage     | stable        | 设置使用的内核的 `tags 后缀`。功能参考 `-u` |
| armbian_kernel   | 6.1.y_5.15.y  | 设置内核 [版本](https://github.com/ophub/kernel/releases/tag/kernel_stable)，功能参考 `-k` |
| auto_kernel      | true          | 设置是否自动采用同系列最新版本内核，功能参考 `-a`       |
| armbian_fstype   | ext4          | 设置系统 ROOTFS 分区的文件系统类型，功能参考 `-t`     |
| armbian_size     | 512/2560      | 设置系统 BOOTFS 和 ROOTFS 分区的大小，功能参考 `-s`  |
| armbian_files    | false         | 添加自定义 Armbian 文件。设置后，该目录下的所有文件将被复制到 [common-files](build-armbian/armbian-files/common-files) 中。目录结构必须与 Armbian 根目录保持一致，以确保文件被正确覆盖到固件中（例如：默认配置文件应存放于 `etc/default/` 子目录下）。 |
| builder_name     | 无             | 设置 Armbian 系统构建者签名，功能参考 `-n`           |

- ### GitHub Actions 输出变量说明

上传到 `Releases` 需要给仓库设置 `Workflow 读写权限`，详见[使用说明](documents/README.cn.md#2-设置隐私变量-github_token)。

| 参数                                 | 默认值         | 说明                       |
|-------------------------------------|---------------|----------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}      | out           | Armbian 系统文件输出路径      |
| ${{ env.PACKAGED_OUTPUTDATE }}      | 04.13.1058    | 打包日期（月.日.时分）         |
| ${{ env.PACKAGED_STATUS }}          | success       | 打包状态：success / failure  |

## 制作 Armbian Docker 镜像

Armbian 系统 [Docker](https://hub.docker.com/u/ophub) 镜像的制作方法可以参考 [armbian_docker](./compile-kernel/tools/script/docker) 制作脚本。

## 使用 GitHub Actions 编译内核

内核的编译方法详见 [compile-kernel](compile-kernel/README.cn.md)

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 6.1.y_5.15.y
    kernel_auto: true
    kernel_sign: -yourname
```

## Armbian 贡献者

首先感谢 [150balbes](https://github.com/150balbes) 为在 Amlogic 电视盒子中使用 Armbian 所做出的杰出贡献和奠定的良好基础。这里编译的 [Armbian](https://github.com/armbian/build) 系统直接使用了官方当前的最新源码进行实时编译。程序的开发思路来自 [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box) 等作者的教程。感谢各位的奉献和分享，让我们可以在更多盒子里使用 Armbian 系统。

本系统所使用的 [u-boot](https://github.com/ophub/u-boot), [kernel](https://github.com/ophub/kernel) 等资源主要从 [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) 的项目中复制而来，部分文件由用户在 [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) / [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian) / [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic) / [kernel](https://github.com/ophub/kernel) / [u-boot](https://github.com/ophub/u-boot) 等项目的 [Pull](https://github.com/ophub/amlogic-s9xxx-armbian/pulls) 和 [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 中提供分享。为感谢这些开拓者和分享者，从现在开始（本源代码库创建于`2021-09-19`），我统一在 [CONTRIBUTORS.md](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/CONTRIBUTORS.md) 中进行了记录。再次感谢大家为盒子赋予了新的生命和意义。

## 其他发行版

- [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) 项目提供了在盒子中使用的 `OpenWrt` 系统，在支持 Armbian 的相关设备中同样适用。
- [unifreq](https://github.com/unifreq/openwrt_packit) 为晶晨、瑞芯微和全志等更多盒子制作了 `OpenWrt` 系统，属于盒子圈的标杆，推荐使用。
- [Scirese](https://github.com/Scirese/alarm) 在安卓电视盒子里测试了 `Arch Linux ARM` / `Manjaro` 系统的制作、安装和使用，具体详见他仓库中的相关说明。
- [7Ji](https://7ji.github.io/) 在他的博客中发表了一些在 Amlogic 平台上的逆向工程和开发的文章，比如以 ArchLinux 的方式安装 ArchLinux ARM 系统，对 Amlogic 平台的启动机制的介绍等。在他的 [ampart](https://github.com/7Ji/ampart) 项目中，提供了一款分区工具，能够读取并编辑 Amlogic 的 eMMC 分区表和 DTB 内分区，可以 100% 利用 eMMC 空间。在 [amlogic-s9xxx-archlinuxarm](https://github.com/7Ji/amlogic-s9xxx-archlinuxarm) 项目中提供了 `Arch Linux ARM` 系统的制作和使用方法。在 [YAopenvfD](https://github.com/7Ji/YAopenvfD) 项目中提供了另一个 openvfd 守护进程。
- [13584452567](https://github.com/13584452567) 是本仓库 `Rockchip` 系列设备的开拓者，因为他的分享而拓展了对 [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132), [ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247), [tvi3315a](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1687), [xiaobao](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1698) 等众多 `Rockchip` 设备的支持，他也是 [TQC-A01](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1638) 等 `Allwinner` 设备 [专用内核](https://github.com/13584452567/linux-6.6.y) 的维护者。他在 [论坛](https://github.com/ophub/amlogic-s9xxx-armbian/discussions/1634) 和 [问答区](https://github.com/ophub/amlogic-s9xxx-armbian/issues) 里提供了大量的技术支持和解决方案，为盒子圈的发展做出了巨大的贡献。
- [cooip-jm](https://github.com/cooip-jm) 在他的 [wiki](https://github.com/cooip-jm/About-openwrt/wiki) 里分享了很多 Armbian，lxc，docker，AdGuard 等应用的使用方法，推荐学习。


## 链接

- [armbian](https://github.com/armbian/build)
- [unifreq](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## License

The amlogic-s9xxx-armbian © OPHUB is licensed under [GPL-2.0](LICENSE)

