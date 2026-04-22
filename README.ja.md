<div align="center">
    <img alt="Armbian" src="https://github.com/user-attachments/assets/74e55052-031b-48f8-9aca-e5f1dd9e256a" />
</div>

# Armbian

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

Armbianは、Debian/Ubuntuをベースに、ARMチップ向けに特別に構築された軽量Linuxディストリビューションです。Armbianシステムはスリムでクリーンであり、Debian/Ubuntuと100%互換性があり、その機能と豊富なソフトウェアエコシステムを継承しています。TF/SD/USBストレージおよびデバイスのeMMCで安全かつ安定して動作します。本プロジェクトは、公式Armbianシステムの完全性を維持しつつ、TVボックスなどの公式にサポートされていないデバイスへのサポートを拡張し、便利な管理コマンドのセットを追加しています。TVボックスのAndroid TVシステムをArmbianに置き換え、強力なサーバーに変身させることができます。

本プロジェクトは多くの[コントリビューター](CONTRIBUTORS.md)に支えられ、`Amlogic`、`Rockchip`、`Allwinner`デバイス向けのArmbianシステムを構築しています。eMMCへの書き込み、カーネル更新などの機能をサポートしています。詳しい使い方は[Armbianユーザードキュメント](./documents/README.ja.md)をご覧ください。最新のArmbianシステムは[Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases)からダウンロードできます。`Fork`してカスタマイズすることを歓迎します。本プロジェクトがお役に立ちましたら、右上の`Star`ボタンをクリックしてサポートをお願いします。

## Armbianシステムのデフォルト情報

| システム名    | デフォルトユーザー名 | デフォルトパスワード  | SSHポート  | IPアドレス  |
| -------------- | ---------------- | ----------------- | --------- | ----------- |
| 🐧 [Armbian.OS](https://github.com/ophub/amlogic-s9xxx-armbian/releases) | root | 1234 | 22 | ルーターから取得 |
| 🐋 [Armbian.Docker](https://hub.docker.com/u/ophub) | root | 1234 | 22 | 静的MacVLAN IP |

## 対応デバイス一覧

⬆️ 各プラットフォーム（Amlogic/Rockchip/Allwinner）のモデルは、SoC性能の高い順に並べられています。

| SoC | [デバイス](https://github.com/ophub/amlogic-s9xxx-armbian/releases) | [カーネル](https://github.com/ophub/kernel) |
| ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/99), [WXY-OES](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2666) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s922x | [Beelink-GT-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/370), [Beelink-GT-King-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/707), [Ugoos-AM6-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/464), [ODROID-N2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/201), [X88-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/988), [Ali-CT2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1150), [WXY-OES-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3029) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905x3 | [X96-Max+](https://github.com/ophub/amlogic-s9xxx-armbian/issues/351), [HK1-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/414), [Vontar-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1006), [H96-Max-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1250), [Ugoos-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/782), [TX3(QZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/644), [TX3(BZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1077), [X96-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/366), [X96-Max+_A100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/779), [A95X-F3-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2282), [Tencent-Aurora-3Pro(s905x3-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/506), [X96-Max+Q1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/788), [X96-Max+100W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/909), [X96-Max+_2101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1086), [Infinity-B32](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1181), [Whale](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1166), [X88-Pro-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [X99-Max-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [Transpeed-X3-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [TOX1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3441), [Khadas-VIM3L](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3482) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
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
| rk3588(s) | [Radxa-Rock5B](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1240), [Radxa-Rock5C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2324), [Orange-Pi-5-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2400), [Beelink-IPC-R](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415), [HLink-H88K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H88K-V3](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [NanoPC-T6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2453), [Smart-Am60](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2817), [DC-A588](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2988), [Orangepi-5B](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3052), [CM3588-NAS](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3306), [Rock-5-ITX](https://github.com/ophub/fnnas/issues/355), [LZ-D3588](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3328), [Boca-tcn100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3437), [Boca-tcn200](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3461), [Firefly-ITX-3588J](https://github.com/ophub/fnnas/issues/194) | [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) |
| rk3576 | [NanoPi-m5](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3207), [LCKFB-Taishan-Pi-3M](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3470) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3399 | [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132), [ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247), [CRRC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1280), [SMART-AM40](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1317), [SW799](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1326), [ZYSJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380), [DG-3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1492), [DLFR100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522), [Emb3531](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1549), [Leez-p710](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609), [tvi3315a](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1687), [xiaobao](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1698), [Fine3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1790), [Firefly-RK3399](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/491), [LX-R3S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2026), [Hugsun-x99](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2050), [Tb-ls3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2146), [Hisense-hs530r](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/572), [Tpm312](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2403), [ZK-rk39a](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2446), [YSKJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2673), [Fmx1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2691), [Fmx1-Pro-B](https://github.com/ophub/fnnas/issues/250), [Sv-33a6x](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/748), [Sv-33a6x(VPU)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3372), [AIO-3399B](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3185), [AIO-3399C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3339), [AIO-3399C(AI)](https://github.com/ophub/fnnas/issues/108), [TaraM](https://github.com/ophub/u-boot/pull/28), [NanoPC-T4](https://github.com/ophub/u-boot/pull/30), [Firefly-Core-3399-JD4](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3354), [GEA-6319](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3383) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3568 | [FastRhino-R66S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1061), [FastRhino-R68S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/774), [Radxa-E25](https://wiki.radxa.com/Rock3/CM/CM3I/E25), [NanoPi-R5S](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217), [NanoPi-R5C](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217), [HLink-H66K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H68K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H69K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [Seewo-sv21](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2017), [Mrkaio-m68s](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2155), [Swan1-w28](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2407), [Ruisen-box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2508), [DG-TN3568](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2661), [Alark35-3500](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2911), [MMBox-Anas3035](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2995), [Wocyber-A3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2978), [Photonicat](https://github.com/ophub/amlogic-s9xxx-openwrt/pull/827), [NSY-G16-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/845), [NSY-G68-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/845), [BDY-G18-Pro](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/847), [Gzpeite-P01](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3221), [LZ-D3568](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3387), [LZ-K3568](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3304), [BDKJ-One](https://github.com/ophub/u-boot/pull/29), [Station-P2](https://github.com/ophub/fnnas/pull/350), [Lyt-t68m](https://github.com/ophub/fnnas/issues/435) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3566 | [Panther-X2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1319), [JP-TvBox](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1867), [LCKFB-Taishan-Pi](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2538), [WXY-OEC-turbo-4g](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2736), [Station-M2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/744), [Orange-Pi-3B](https://github.com/ophub/fnnas/issues/261), [X88Pro20](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3443) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3528 | [HLink-H28K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1726), [Radxa-E20C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2324), [H96-Max-M2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2404), [HK1-Rbox-K8S](https://github.com/ophub/fnnas/issues/464), [HT2](https://github.com/ophub/fnnas/issues/464), [CD1000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3302) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3328 | [BeikeYun](https://github.com/ophub/amlogic-s9xxx-armbian/issues/852), [Chainedbox-L1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1680), [Station-M1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Bqeel-MVR9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Renegade/Firefly](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1861) | [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx)<br />[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3318 | [RX3318-Box](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2129) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| h6 | [Vplus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1100), [Tanix-TX6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1120), [TQC-A01](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1638) | [h6](https://github.com/ophub/kernel/releases/tag/kernel_h6)<br>[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| h618 | [OrangePi-Zero3](https://github.com/ophub/fnnas/issues/158), [H618-DevBoard(PCDN)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3479), [vontar-h618](https://github.com/ophub/fnnas/issues/525), [X98H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3434) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |

> [!TIP]
> 現在、[s905ボックス](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1173)は`TF/SD/USB`でのみ使用できます。その他のボックスモデルは`EMMC`への書き込みに対応しています。詳細は[✅ 対応デバイス一覧](build-armbian/armbian-files/common-files/etc/model_database.conf)をご参照ください。新しいデバイスサポートの追加については、ドキュメントのセクション12.15：[新しい対応デバイスの追加方法](documents/README.ja.md#1215-新しいサポートデバイスの追加方法)をご覧ください。ご使用前に[Armbianユーザードキュメント](./documents/README.ja.md)をお読みください。よくある問題の解決方法が記載されています。

## Armbianのインストールとアップグレード手順

お使いのデバイスモデルに合ったArmbianシステムを選択してください。デバイス固有の使用方法については、対応するドキュメントをご参照ください。

- ### ArmbianをEMMCにインストール

1. `Rockchip`プラットフォームのデバイスについては、ドキュメントの[第8章](documents/README.ja.md#8-armbian-を-emmc-にインストールする)をご参照ください。

2. `Amlogic`および`Allwinner`プラットフォームのデバイスについては、[Rufus](https://rufus.ie/)や[balenaEtcher](https://www.balena.io/etcher/)などのツールを使用してシステムをUSBメモリに書き込み、デバイスに挿入します。Armbianシステムにログイン（デフォルトユーザー：root、デフォルトパスワード：1234）し、以下のコマンドを入力します：

```shell
armbian-install
```

| オプション | デフォルト | 選択肢 | 説明       |
| -------- | ------- | ------- | ----------------- |
| -m       | no      | yes/no  | メインラインu-bootを使用 |
| -a       | yes     | yes/no  | [ampart](https://github.com/7Ji/ampart)パーティション調整ツールを使用 |
| -l       | no      | yes/no  | 完全なデバイスリストを表示 |

例：`armbian-install -m yes -a no`

- ### Armbianカーネルの更新

Armbianシステムにログインし、以下のコマンドを入力します：

```shell
# rootユーザーとして実行（sudo -i）
# パラメータを指定しない場合、最新バージョンに更新されます。
armbian-update
```

| オプション | デフォルト      | 選択肢       | 説明                      |
| -------- | ------------ | ------------- | -------------------------------- |
| -r       | ophub/kernel | `<owner>/<repo>` | github.comからカーネルをダウンロードするリポジトリを設定 |
| -u       | 自動判定   | stable/flippy/beta/rk3588/rk35xx/h6 | カーネルの[タグサフィックス](https://github.com/ophub/kernel/releases)を設定 |
| -k       | 最新バージョン | カーネルバージョン | [カーネルバージョン](https://github.com/ophub/kernel/releases/tag/kernel_stable)を設定 |
| -b       | yes          | yes/no        | 更新時に現在使用中のカーネルを自動バックアップ |
| -d       | deb          | tar/deb       | 優先するカーネルパッケージ形式を設定。利用できない場合、スクリプトが自動的に代替形式を試みます。カスタムドライバーのコンパイルには`deb`形式を推奨。 |
| -m       | no           | yes/no        | メインラインu-bootを使用 |
| -s       | なし         | なし/ディスク名 | [SOS] eMMC/NVMe/sdXまたは他のディスク上のシステムカーネルを復元 |
| -h       | なし         | なし          | ヘルプ情報を表示 |

例：`armbian-update -k 5.15 -u stable -d deb`

`-k`パラメータでカーネルバージョンを指定する際、正確なバージョン番号（例：`armbian-update -k 5.15.50`）またはカーネルシリーズのみ（例：`armbian-update -k 5.15`）を指定できます。シリーズを指定した場合、そのシリーズ内の最新バージョンが自動的に使用されます。

カーネル更新時、現在実行中のカーネルは自動的に`/ddbr/backup`ディレクトリにバックアップされ、最新の3つのバージョンが保持されます。新しくインストールしたカーネルが不安定な場合、いつでもバックアップカーネルを復元できます。カーネル更新によりシステムが起動できなくなった場合は、`armbian-update -s`でシステムカーネルを復元できます。詳細は[ヘルプドキュメント](documents/README.ja.md#10-armbian-カーネルの更新)をご覧ください。

- ### Armbianソースの変更

Armbianシステムにログインし、以下のコマンドを入力します：

```shell
armbian-apt
```

お住まいの国や地域に適したソフトウェアソースを選択することで、ダウンロード速度を大幅に向上させることができます。詳細は[ヘルプドキュメント](documents/README.ja.md#11-よく使うソフトウェアのインストール)をご覧ください。

- ### よく使うソフトウェアのインストール

Armbianシステムにログインし、以下のコマンドを入力します：

```shell
armbian-software
```

`armbian-software -u`コマンドでローカルのソフトウェアセンターリストを更新できます。[Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues)でのユーザーフィードバックに基づき、よく使われる[ソフトウェア](build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf)がワンクリックでのインストール/更新/アンインストールに対応するよう段階的に統合されています。`Dockerイメージ`、`デスクトップソフトウェア`、`アプリケーションサービス`などが含まれます。[詳細な説明](documents/armbian_software.md)をご覧ください。

- ### Armbian設定の変更

Armbianシステムにログインし、以下のコマンドを入力します：

```shell
armbian-config
```

- ### ArmbianのSwap作成

`Docker`などのメモリ集約型アプリケーションの実行時にデバイスのメモリが不足する場合、`swap`仮想メモリパーティションを作成して、ディスク容量の一部を追加メモリとして使用できます。パラメータの単位は`GB`で、デフォルト値は`1`です。

Armbianシステムにログインし、以下のコマンドを入力します：

```shell
armbian-swap 1
```

- ### LEDディスプレイの制御

Armbianシステムにログインし、以下のコマンドを入力します：

```shell
armbian-openvfd
```

[LEDスクリーンディスプレイ制御手順](documents/led_screen_display_control.md)に従ってデバッグしてください。

- ### EMMC元システムのバックアップ/復元

`TF/SD/USB`経由でデバイスの`EMMC`パーティションのバックアップと復元をサポートします。新しいデバイスにArmbianをインストールする前に、必要に応じて将来復元できるよう、元のAndroid TVシステムをバックアップすることをお勧めします。

`TF/SD/USB`からArmbianシステムを起動し、以下のコマンドを入力します：

```shell
armbian-ddbr
```

プロンプトで`b`を入力するとシステムをバックアップ、`r`を入力すると復元します。

> [!IMPORTANT]
> または、USBケーブル経由でAndroidシステムをeMMCに直接書き込むこともできます。Androidシステムイメージは[Tools](https://github.com/ophub/kernel/releases/tag/tools)から入手できます。

- ### Armbian内でのカーネルコンパイル

カーネルコンパイルの手順については、[カーネルコンパイル](compile-kernel/README.ja.md)ドキュメントをご覧ください。Armbianシステムにログインし、以下のコマンドを入力します：

```shell
armbian-kernel -u
armbian-kernel -k 6.6.12
```

- ### その他の使用方法

システム内のすべてのサービススクリプトを最新バージョンに更新するには、Armbianシステムにログインし、以下のコマンドを入力します：

```shell
armbian-sync
```

Armbian使用時のよくある問題とその解決方法については、[documents](documents/README.ja.md)をご覧ください。

## ローカルパッケージング

1. リポジトリをローカルにクローン `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. 必要なソフトウェアパッケージをインストール（Ubuntu 24.04を例として）

`~/amlogic-s9xxx-armbian`ルートディレクトリに移動し、インストールコマンドを実行します：

```shell
sudo apt-get update -y
sudo apt-get full-upgrade -y
# Ubuntu-24.04の場合
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2404-build-armbian-depends)
```

3. `~/amlogic-s9xxx-armbian`ルートディレクトリに移動し、`build/output/images`フォルダを作成して、Armbianイメージファイル（例：`Armbian_21.11.0-trunk_Odroidn2_current_5.15.50.img`）を`~/amlogic-s9xxx-armbian/build/output/images`ディレクトリにアップロードします。元のファイル名のリリースバージョン番号（例：`21.11.0`）とカーネルバージョン番号（例：`5.15.50`）を保持してください。再構築されたArmbianシステムの命名に使用されます。

4. `~/amlogic-s9xxx-armbian`ルートディレクトリに移動し、`sudo ./rebuild -b s905x3 -k 6.6.12`を実行して、指定したボード用のArmbianイメージを生成します。出力ファイルは`build/output/images`ディレクトリに保存されます。

- ### ローカルパッケージングパラメータの説明

| パラメータ | 意味     | 説明 |
| ----      | ----------  | ----------  |
| -b        | ボード      | ターゲットデバイスのコードネームを指定します。特定のデバイスを指定してコンパイルする（例：`-b s905x3`）か、アンダースコアで複数のデバイスコードネームを繋いで一括コンパイルする（例：`-b s905x3_s905d`）ことができます。このパラメータは、特殊なキーワードによる一括コンパイルもサポートしています。`all` はすべてのデバイスをコンパイルし、`first50` はデバイスライブラリの最初の50個をコンパイルします。`range50_100` は51番目から100番目のデバイスをコンパイルし（`range100_150` も同様）、`last20` は最後の20個をコンパイルすることを意味します。さらに、ハードウェアプラットフォーム（`amlogic`、`rockchip`、`allwinner`）別のコンパイルにも対応しており、プラットフォーム名を直接入力するだけで対応するすべてのイメージをコンパイルできます（例：`-b amlogic`）。プラットフォーム名の後に数値を追加すると、そのプラットフォームのリスト内の特定の範囲を指定してコンパイルできます。たとえば、`-b amlogic50` はAmlogicプラットフォームのサポートリストの最初の50個のデバイスをコンパイルし、`-b amlogic50_100` は51番目から100番目のデバイスをコンパイルすることを意味します。サポートされているデバイスコードネームの具体的なリストについては、[model_database.conf](build-armbian/armbian-files/common-files/etc/model_database.conf) の `BOARD` 設定項目を参照してください。デフォルト値：`all` |
| -r        | カーネルリポジトリ | github.comのカーネルリポジトリの`<owner>/<repo>`を指定。デフォルト値：`ophub/kernel` |
| -u        | カーネル用途 | 使用するカーネルの`タグサフィックス`を設定（[stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)、[flippy](https://github.com/ophub/kernel/releases/tag/kernel_flippy)、[beta](https://github.com/ophub/kernel/releases/tag/kernel_beta)など）。デフォルト値：`stable` |
| -k        | カーネル     | [カーネル](https://github.com/ophub/kernel/releases/tag/kernel_stable)名を指定（例：`-k 6.6.12`）。複数のカーネルは`_`で結合（例：`-k 6.6.12_5.15.50`）。`-k`パラメータで自由に指定できるカーネルバージョンは、`stable/flippy/beta`を使用するカーネルにのみ有効です。[rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) / [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) / [h6](https://github.com/ophub/kernel/releases/tag/kernel_h6)などの他のカーネルシリーズは、専用のカーネルのみ使用できます。 |
| -a        | 自動カーネル | 同じシリーズの最新カーネルを自動的に使用するかどうかを設定。`true`の場合、`-k`で指定されたシリーズ（例：6.6.12）内の新しいバージョンがカーネルリポジトリで確認され、見つかった場合は最新バージョンが自動的に使用されます。`false`の場合、指定された正確なバージョンが使用されます。デフォルト：`true` |
| -t        | ルートFSタイプ | ROOTFSパーティションのファイルシステムタイプを設定。選択肢：`ext4`または`btrfs`。例：`-t btrfs`。デフォルト：`ext4` |
| -s        | サイズ       | イメージパーティションサイズを設定。ROOTFSパーティションのみ設定する場合は単一の値を指定（例：`-s 2560`）。BOOTFSとROOTFSの両方を設定する場合は`/`で結合（例：`-s 512/2560`）。デフォルト：`512/2560` |
| -n        | ビルダー名 | Armbianシステムのビルダー署名を設定。スペースを含めないでください。デフォルト：なし |

- `sudo ./rebuild`：デフォルト設定ですべてのデバイスモデルをパッケージング。
- `sudo ./rebuild -b s905x3 -k 6.6.12`：推奨。指定したカーネルでデフォルト設定のビルド。
- `sudo ./rebuild -b s905x3 -k 6.1.y`：デフォルト設定で6.1.yシリーズの最新カーネルを使用してビルド。
- `sudo ./rebuild -b s905x3_s905d -k 6.6.12_5.15.50`：デフォルト設定で複数のカーネルを同時にビルド。複数のカーネルは`_`で結合。
- `sudo ./rebuild -b s905x3 -k 6.6.12 -s 2560`：単一のカーネルとモデルでデフォルト設定のビルド。ROOTFSパーティションサイズを2560 MiBに設定。
- `sudo ./rebuild -b s905x3_s905d`：複数のモデルですべてのカーネルをデフォルト設定でビルド。複数のモデルは`_`で結合。
- `sudo ./rebuild -k 6.6.12_5.15.50`：すべてのモデルで複数の指定カーネルをデフォルト設定でビルド。カーネルは`_`で結合。
- `sudo ./rebuild -k 6.6.12_5.15.50 -a true`：上記と同じ。各シリーズ内の最新カーネルに自動アップグレード。
- `sudo ./rebuild -t btrfs -s 2560 -k 6.6.12`：btrfsファイルシステム、2560 MiBのROOTFS、カーネル6.6.12ですべてのモデルをビルド。

## GitHub Actionsを使用したコンパイル

1. ワークフロー設定ファイルは[.github/workflows/](.github/workflows/)ディレクトリにあります。

2. 新規ビルド：[Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions)ページで***`Build Armbian server image`***を選択し、[build-armbian-arm64-server-image.yml](.github/workflows/build-armbian-arm64-server-image.yml)ワークフローを使用します。Ubuntuシリーズ（例：`resolute`）またはDebianシリーズ（例：`trixie`）から選択できます。***`Run workflow`***をクリックしてビルドを開始します。

3. 再ビルド：[Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases)にコンパイル済みの`Armbian_.*-trunk_.*.img.gz`ファイルがすでにあり、他のボード用に再パッケージするだけの場合は、ソースコンパイル手順をスキップし、[build-armbian-using-releases-files.yml](.github/workflows/build-armbian-using-releases-files.yml)を使用して二次ビルドを行います。

4. 他のArmbianシステム（例：公式[armbian.tnahosting.net](https://armbian.tnahosting.net/dl/)ダウンロードサイトの[odroidn2](https://armbian.tnahosting.net/dl/odroidn2/archive/)イメージ）を使用する場合は、ワークフローファイル[build-armbian-using-official-image.yml](.github/workflows/build-armbian-using-official-image.yml)で本リポジトリのスクリプトを参照するだけで、他のデバイスをサポートするためのArmbian再構築が可能です。例：

```yaml
- name: Build Armbian
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img
    armbian_board: s905d_s905x3_s922x_s905x
    armbian_kernel: 6.12.y_6.18.y
```

- ### GitHub Actions入力パラメータの説明

これらのパラメータは、上記のローカルパッケージングコマンドオプションに対応しています。

| パラメータ       | デフォルト       | 説明                                             |
|-----------------|---------------|---------------------------------------------------------|
| armbian_path    | なし          | 元のArmbianファイルのパスを設定。ワークフローファイルパス（例：`build/output/images/*.img`）およびネットワークダウンロードURL（例：`https://dl.armbian.com/*/Armbian_*.img.xz`）をサポート。 |
| armbian_board   | all           | パッケージボックスの`board`を設定。`-b`を参照       |
| kernel_repo     | ophub/kernel  | github.comのカーネルリポジトリの`<owner>/<repo>`を指定。`-r`を参照 |
| kernel_usage    | stable        | 使用するカーネルの`タグサフィックス`を設定。`-u`を参照 |
| armbian_kernel  | 6.12.y_6.18.y | カーネルの[バージョン](https://github.com/ophub/kernel/releases/tag/kernel_stable)を設定。`-k`を参照 |
| auto_kernel     | true          | 同じシリーズの最新バージョンのカーネルを自動的に採用するかどうかを設定。`-a`を参照       |
| armbian_fstype  | ext4          | システムのROOTFSパーティションのファイルシステムタイプを設定。`-t`を参照  |
| armbian_size    | 512/2560      | システムのBOOTFSおよびROOTFSパーティションのサイズを設定。`-s`を参照  |
| armbian_files   | false         | カスタムArmbianファイルを追加。設定した場合、このディレクトリ内のすべてのファイルが[common-files](build-armbian/armbian-files/common-files)にコピーされます。ファイルが正しくオーバーレイされるよう、ディレクトリ構造はArmbianのルートディレクトリを反映する必要があります（例：デフォルト設定ファイルは`etc/default/`配下に配置）。 |
| builder_name    | なし          | Armbianシステムのビルダー署名を設定。`-n`を参照 |

- ### GitHub Actions出力変数の説明

`Releases`へのアップロードには、リポジトリの`ワークフロー読み取りおよび書き込み権限`が必要です。詳細は[使用方法の説明](documents/README.ja.md#2-プライバシー変数-github_token-等の設定)をご覧ください。

| パラメータ                        | デフォルト       | 説明                           |
|----------------------------------|---------------|---------------------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}   | out           | Armbianシステムファイルの出力パス      |
| ${{ env.PACKAGED_OUTPUTDATE }}   | 04.13.1058    | パッケージング日時（月.日.時分） |
| ${{ env.PACKAGED_STATUS }}       | success       | パッケージングステータス：success / failure   |

## Armbian Dockerイメージのビルド

Armbianシステムの[Docker](https://hub.docker.com/u/ophub)イメージの作成については、[armbian_docker](./compile-kernel/tools/script/docker)ビルドスクリプトをご参照ください。

## GitHub Actionsを使用したカーネルコンパイル

カーネルコンパイルの手順については、[compile-kernel](compile-kernel/README.ja.md)をご覧ください。

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 6.12.y_6.18.y
    kernel_auto: true
    kernel_sign: -yourname
```

## Armbianコントリビューター

まず、[150balbes](https://github.com/150balbes)氏の素晴らしい貢献と、Amlogic TVボックスでArmbianを実行するために築かれた確かな基盤に感謝します。ここでコンパイルされる[Armbian](https://github.com/armbian/build)システムは、最新の公式ソースコードを使用してリアルタイムでビルドされています。開発アプローチは、[ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box)などの著者のチュートリアルからインスピレーションを受けています。皆様の献身と共有に感謝し、Armbianがますます多くのデバイスで動作できるようになっています。

このシステムで使用される[u-boot](https://github.com/ophub/u-boot)、[kernel](https://github.com/ophub/kernel)、その他のリソースは、主に[unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit)プロジェクトから提供されています。一部のファイルは、[amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt)、[amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian)、[luci-app-amlogic](https://github.com/ophub/luci-app-amlogic)、[u-boot](https://github.com/ophub/u-boot)、[kernel](https://github.com/ophub/kernel)などのプロジェクトで、ユーザーが[Pull Requests](https://github.com/ophub/amlogic-s9xxx-armbian/pulls)や[Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues)を通じて貢献したものです。これらの先駆者やコントリビューターを称えるため、リポジトリ作成以降（`2021-09-19`）のすべての貢献が[CONTRIBUTORS.md](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/CONTRIBUTORS.md)に記録されています。皆様のおかげで、これらのデバイスに新しい命と目的が与えられたことに改めて感謝いたします。

## その他のディストリビューション

- [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt)プロジェクトは、TVボックス向けの`OpenWrt`システムを提供しており、Armbianがサポートするデバイスにも適用可能です。
- [fnnas](https://github.com/ophub/fnnas)プロジェクトは、TVボックス向けの`FnNAS`システムを提供しており、Armbianがサポートするデバイスにも適用可能です。
- [unifreq](https://github.com/unifreq/openwrt_packit)は、幅広いAmlogic、Rockchip、Allwinnerデバイス向けの`OpenWrt`システムを作成しており、コミュニティのベンチマークプロジェクトです。強くお勧めします。
- [Scirese](https://github.com/Scirese/alarm)は、Android TVボックスでの`Arch Linux ARM` / `Manjaro`システムのビルド、インストール、使用をテストしています。詳細はリポジトリをご覧ください。
- [7Ji](https://7ji.github.io/)は、Amlogicプラットフォームのリバースエンジニアリングと開発に関する記事を公開しており、ArchLinux ARMのインストールやAmlogicブートメカニズムの分析などのトピックを扱っています。[ampart](https://github.com/7Ji/ampart)プロジェクトは、Amlogic eMMCパーティションテーブルとDTBパーティションの読み取り・編集用パーティションツールを提供し、eMMCスペースの100%活用を実現しています。[amlogic-s9xxx-archlinuxarm](https://github.com/7Ji/amlogic-s9xxx-archlinuxarm)プロジェクトは、`Arch Linux ARM`のビルドおよび使用手順を提供しています。[YAopenvfD](https://github.com/7Ji/YAopenvfD)プロジェクトは、代替のopenvfdデーモン実装を提供しています。
- [13584452567](https://github.com/13584452567)は、本リポジトリにおける`Rockchip`デバイスサポートの先駆者です。彼の貢献により、[EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991)、[King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080)、[TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094)、[Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132)、[ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247)、[tvi3315a](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1687)、[xiaobao](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1698)など、多数の`Rockchip`デバイスのサポートが拡大されました。また、[TQC-A01](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1638)などの`Allwinner`デバイス用の専用[カーネル](https://github.com/13584452567/linux-6.6.y)をメンテナンスし、[Discussions](https://github.com/ophub/amlogic-s9xxx-armbian/discussions/1634)や[Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues)で広範な技術サポートとソリューションを提供し、コミュニティに多大な貢献をしています。
- [cooip-jm](https://github.com/cooip-jm)は、Armbian、LXC、Docker、AdGuard、その他のアプリケーションに関する多くのガイドを[wiki](https://github.com/cooip-jm/About-openwrt/wiki)で共有しています。ぜひご覧ください。


## リンク

- [armbian](https://github.com/armbian/build)
- [unifreq](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## ライセンス

amlogic-s9xxx-armbian © OPHUBは[GPL-2.0](LICENSE)の下でライセンスされています。
