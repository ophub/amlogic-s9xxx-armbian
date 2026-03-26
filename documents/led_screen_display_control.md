# LED Screen Display Control Instructions

[English Instructions](#led-screen-display-control-instructions) | [中文说明](#led-屏显示控制说明) | [日本語説明](#led-スクリーン表示制御の説明)

- The configuration files are located in the [/usr/share/openvfd](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd) directory of the `Armbian/OpenWrt` system. The `Armbian` command file is at [/usr/sbin/armbian-openvfd](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/sbin/armbian-openvfd), and the `OpenWrt` command file is at [/usr/sbin/openwrt-openvfd](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/make-openwrt/openwrt-files/common-files/usr/sbin/openwrt-openvfd). If these files are not present in your system, upload them manually and assign execution permissions: `chmod +x /usr/share/openvfd/vfdservice /usr/sbin/*-openvfd`.

- Update the system kernel to the latest version. For the `Armbian` system, use the `armbian-sync` command. For the `OpenWrt` system, navigate to `System Menu` → `Amlogic Service` → `Online Download Update`.

- The following devices have been tested: `x96max.conf`, `x96maxplus.conf`, `h96max-x3.conf`, `hk1-x3.conf`, `hk1box.conf`, `tx3.conf`, `x96air.conf`, and `x88pro-x3.conf`. Configurations for other devices can be adapted from [arthur-liberman/vfd-configurations](https://github.com/arthur-liberman/vfd-configurations) and [LibreELEC/linux_openvfd](https://github.com/LibreELEC/linux_openvfd/tree/master/conf). Note: the second field value in configuration entries from these sources must be decremented by `1`. For example:

```yaml
vfd_gpio_clk='0,69,0'
vfd_gpio_dat='0,70,0'
```
Modify to:

```yaml
vfd_gpio_clk='0,68,0'
vfd_gpio_dat='0,69,0'
```

- Taking the configuration of [x96maxplus](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd/conf/x96maxplus.conf) as an example: If the order of the displayed time text is incorrect, you can adjust the order of numbers in `vfd_chars='4,0,1,2,3'` to `vfd_chars='1,2,3,4,0'` for testing. If the time is displayed upside down, you can adjust the `first value 0x02` in `vfd_display_type='0x02,0x00,0x01,0x00'` to `0x01` for testing. The content displayed can be adjusted according to the specific situation of your device in `functions='usb apps setup sd hdmi cvbs'`.

- Rename the configuration file to `diy.conf` and upload it to the `/usr/share/openvfd/conf` directory, then enter the command `armbian-openvfd 99` for testing.

- The command `armbian-openvfd 0` disables the LED display and clears associated system processes. Before testing each new configuration, always run this disable command first, then execute `armbian-openvfd 99` to test the modified configuration.

- Some devices may display a boot message before Linux starts (e.g., showing `BOOT`). To clear this message, first run `armbian-openvfd 0` to stop existing services, then run `armbian-openvfd <boxid>` to take control of the LED display. To disable the display entirely, run `armbian-openvfd 0` again.

- Once the display is working correctly, add it to the boot startup tasks. Replace `15` in the following commands with your device's `BoxID`:

```yaml
# Execute the following command in the terminal to enable the openvfd service
sed -i 's|^#*openvfd_enable=.*|openvfd_enable="yes"|g' /etc/custom_service/start_service.sh
sed -i 's|^#*openvfd_boxid=.*|openvfd_boxid="15"|g' /etc/custom_service/start_service.sh
# Some devices require restarting the OpenVFD service to clear 'BOOT' and related messages
sed -i 's|^#*openvfd_restart=.*|openvfd_restart="yes"|g' /etc/custom_service/start_service.sh
```

- Everyone is welcome to test and share their device configuration files (diy.conf) to benefit the community.

|  BoxName   | `BoxID` |  Armbian Command      |   OpenWrt Command       |  Function   |
| ---------- | ------- | --------------------- | ----------------------- | ----------- |
| x96max     |  11     |  armbian-openvfd 11   |   openwrt-openvfd 11    | Enable LED  |
| x96maxplus |  12     |  armbian-openvfd 12   |   openwrt-openvfd 12    | Enable LED  |
| x96air     |  13     |  armbian-openvfd 13   |   openwrt-openvfd 13    | Enable LED  |
| h96max-x3  |  14     |  armbian-openvfd 14   |   openwrt-openvfd 14    | Enable LED  |
| hk1-x3     |  15     |  armbian-openvfd 15   |   openwrt-openvfd 15    | Enable LED  |
| hk1box     |  16     |  armbian-openvfd 16   |   openwrt-openvfd 16    | Enable LED  |
| tx3        |  17     |  armbian-openvfd 17   |   openwrt-openvfd 17    | Enable LED  |
| tx3-mini   |  18     |  armbian-openvfd 18   |   openwrt-openvfd 18    | Enable LED  |
| t95        |  19     |  armbian-openvfd 19   |   openwrt-openvfd 19    | Enable LED  |
| t95z-plus  |  20     |  armbian-openvfd 20   |   openwrt-openvfd 20    | Enable LED  |
| tx9-pro    |  21     |  armbian-openvfd 21   |   openwrt-openvfd 21    | Enable LED  |
| x92        |  22     |  armbian-openvfd 22   |   openwrt-openvfd 22    | Enable LED  |
| whale      |  23     |  armbian-openvfd 23   |   openwrt-openvfd 23    | Enable LED  |
| x88pro-x3  |  24     |  armbian-openvfd 24   |   openwrt-openvfd 24    | Enable LED  |
| diy        |  99     |  armbian-openvfd 99   |   openwrt-openvfd 99    | Enable LED  |
| -          |  0      |  armbian-openvfd 0    |   openwrt-openvfd 0     | Disable LED |
| -          |  -u     |  armbian-openvfd -u   |   openwrt-openvfd -u    | Update Conf |

# LED 屏显示控制说明

- 配置文件位于 `Armbian/OpenWrt` 系统的 [/usr/share/openvfd](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd) 目录下。`Armbian` 系统的命令文件位于 [/usr/sbin/armbian-openvfd](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/sbin/armbian-openvfd)，`OpenWrt` 系统的命令文件位于 [/usr/sbin/openwrt-openvfd](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/make-openwrt/openwrt-files/common-files/usr/sbin/openwrt-openvfd)。若当前系统中缺少这些文件，可手动上传并赋予执行权限：`chmod +x /usr/share/openvfd/vfdservice /usr/sbin/*-openvfd`。

- 将系统内核升级至最新版本。`Armbian` 系统使用 `armbian-sync` 命令升级；`OpenWrt` 系统通过 `系统菜单` → `晶晨宝盒` → `在线下载更新` 进行升级。

- 目前已测试的设备配置包括 `x96max.conf`、`x96maxplus.conf`、`h96max-x3.conf`、`hk1-x3.conf`、`hk1box.conf`、`tx3.conf`、`x96air.conf` 和 `x88pro-x3.conf` 等。其他设备的配置可参考 [arthur-liberman/vfd-configurations](https://github.com/arthur-liberman/vfd-configurations) 和 [LibreELEC/linux_openvfd](https://github.com/LibreELEC/linux_openvfd/tree/master/conf) 进行修改。注意：需将这两个网站配置文件中对应字段的第二个值减 `1` 后使用，例如：

```yaml
vfd_gpio_clk='0,69,0'
vfd_gpio_dat='0,70,0'
```
修改为：

```yaml
vfd_gpio_clk='0,68,0'
vfd_gpio_dat='0,69,0'
```

- 以 [x96maxplus](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd/conf/x96maxplus.conf) 的配置为例：如果显示的时间文字顺序不正确，可以调整 `vfd_chars='4,0,1,2,3'` 的数字顺序为 `vfd_chars='1,2,3,4,0'` 等进行测试。如果时间是翻转显示，可以调整 `vfd_display_type='0x02,0x00,0x01,0x00'` 中的 `第一个值 0x02` 为 `0x01` 等进行测试。显示的内容可根据自己的设备支持的具体情况调整 `functions='usb apps setup sd hdmi cvbs'` 中的值。

- 将配置文件命名为 `diy.conf` 并上传至 `/usr/share/openvfd/conf` 目录下，输入命令 `armbian-openvfd 99` 进行测试。

- 通过命令 `armbian-openvfd 0` 可禁用 LED 显示并清除相关系统进程。每次测试新配置前，请先执行此禁用命令，再执行 `armbian-openvfd 99` 进行配置测试。

- 部分设备可能在 Linux 启动前显示开机信息（例如显示 `BOOT`）。要清除此信息，请先执行 `armbian-openvfd 0` 停止现有服务，然后执行 `armbian-openvfd <boxid>` 接管 LED 显示。若要禁用显示屏，再次执行 `armbian-openvfd 0` 即可。

- 屏幕显示正常后，可将其添加到开机自启动任务。将以下命令中的 `15` 替换为你的设备编号：

```yaml
# 在终端执行以下命令启用 openvfd 服务
sed -i 's|^#*openvfd_enable=.*|openvfd_enable="yes"|g' /etc/custom_service/start_service.sh
sed -i 's|^#*openvfd_boxid=.*|openvfd_boxid="15"|g' /etc/custom_service/start_service.sh
# 有些设备需要重启 OpenVFD 服务以清除 'BOOT' 等相关信息
sed -i 's|^#*openvfd_restart=.*|openvfd_restart="yes"|g' /etc/custom_service/start_service.sh
```

- 欢迎大家测试后分享自己设备的配置文件（diy.conf），让更多人受益。

|  盒子名称   | `盒子编号` |  Armbian 使用命令      |   OpenWrt 使用命令       |   功能   |
| ---------- | -------- | --------------------- | ----------------------- | ------- |
| x96max     |  11      |  armbian-openvfd 11   |   openwrt-openvfd 11    | 启用 LED |
| x96maxplus |  12      |  armbian-openvfd 12   |   openwrt-openvfd 12    | 启用 LED |
| x96air     |  13      |  armbian-openvfd 13   |   openwrt-openvfd 13    | 启用 LED |
| h96max-x3  |  14      |  armbian-openvfd 14   |   openwrt-openvfd 14    | 启用 LED |
| hk1-x3     |  15      |  armbian-openvfd 15   |   openwrt-openvfd 15    | 启用 LED |
| hk1box     |  16      |  armbian-openvfd 16   |   openwrt-openvfd 16    | 启用 LED |
| tx3        |  17      |  armbian-openvfd 17   |   openwrt-openvfd 17    | 启用 LED |
| tx3-mini   |  18      |  armbian-openvfd 18   |   openwrt-openvfd 18    | 启用 LED |
| t95        |  19      |  armbian-openvfd 19   |   openwrt-openvfd 19    | 启用 LED |
| t95z-plus  |  20      |  armbian-openvfd 20   |   openwrt-openvfd 20    | 启用 LED |
| tx9-pro    |  21      |  armbian-openvfd 21   |   openwrt-openvfd 21    | 启用 LED |
| x92        |  22      |  armbian-openvfd 22   |   openwrt-openvfd 22    | 启用 LED |
| whale      |  23      |  armbian-openvfd 23   |   openwrt-openvfd 23    | 启用 LED |
| x88pro-x3  |  24      |  armbian-openvfd 24   |   openwrt-openvfd 24    | 启用 LED |
| diy        |  99      |  armbian-openvfd 99   |   openwrt-openvfd 99    | 启用 LED |
| -          |  0       |  armbian-openvfd 0    |   openwrt-openvfd 0     | 禁用 LED |
| -          |  -u      |  armbian-openvfd -u   |   openwrt-openvfd -u    | 更新配置  |



# LED スクリーン表示制御の説明

- 設定ファイルは `Armbian/OpenWrt` システムの [/usr/share/openvfd](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd) ディレクトリにあります。`Armbian` システムのコマンドファイルは [/usr/sbin/armbian-openvfd](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/sbin/armbian-openvfd) に、`OpenWrt` システムのコマンドファイルは [/usr/sbin/openwrt-openvfd](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/make-openwrt/openwrt-files/common-files/usr/sbin/openwrt-openvfd) にあります。現在のシステムにこれらのファイルがない場合は、手動でアップロードして実行権限を付与してください：`chmod +x /usr/share/openvfd/vfdservice /usr/sbin/*-openvfd`。

- システムカーネルを最新バージョンにアップグレードしてください。`Armbian` システムは `armbian-sync` コマンドでアップグレードします。`OpenWrt` システムは `システムメニュー` → `晶晨宝盒` → `オンラインダウンロード更新` からアップグレードします。

- 現在テスト済みのデバイス設定には `x96max.conf`、`x96maxplus.conf`、`h96max-x3.conf`、`hk1-x3.conf`、`hk1box.conf`、`tx3.conf`、`x96air.conf`、`x88pro-x3.conf` などがあります。その他のデバイスの設定は [arthur-liberman/vfd-configurations](https://github.com/arthur-liberman/vfd-configurations) と [LibreELEC/linux_openvfd](https://github.com/LibreELEC/linux_openvfd/tree/master/conf) を参考に修正できます。注意：これら2つのサイトの設定ファイルの対応フィールドの2番目の値を `1` 減らしてから使用する必要があります。例：

```yaml
vfd_gpio_clk='0,69,0'
vfd_gpio_dat='0,70,0'
```
以下に修正：

```yaml
vfd_gpio_clk='0,68,0'
vfd_gpio_dat='0,69,0'
```

- [x96maxplus](../build-armbian/armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd/conf/x96maxplus.conf) の設定を例に説明します：表示される時刻テキストの順序が正しくない場合、`vfd_chars='4,0,1,2,3'` の数字の順序を `vfd_chars='1,2,3,4,0'` などに調整してテストできます。時刻が反転表示される場合、`vfd_display_type='0x02,0x00,0x01,0x00'` の `最初の値 0x02` を `0x01` などに調整してテストできます。表示内容はデバイスでサポートされている具体的な状況に応じて `functions='usb apps setup sd hdmi cvbs'` の値を調整できます。

- 設定ファイルを `diy.conf` にリネームして `/usr/share/openvfd/conf` ディレクトリにアップロードし、コマンド `armbian-openvfd 99` を入力してテストします。

- コマンド `armbian-openvfd 0` で LED 表示を無効にし、関連するシステムプロセスをクリアできます。新しい設定をテストする前に、まずこの無効化コマンドを実行し、その後 `armbian-openvfd 99` を実行して設定をテストしてください。

- 一部のデバイスは Linux 起動前に起動メッセージを表示する場合があります（例：`BOOT` と表示）。このメッセージをクリアするには、まず `armbian-openvfd 0` を実行して既存のサービスを停止し、次に `armbian-openvfd <boxid>` を実行して LED 表示を制御します。表示を完全に無効にするには、再度 `armbian-openvfd 0` を実行してください。

- スクリーン表示が正常に動作した後、起動時の自動開始タスクに追加できます。以下のコマンドの `15` をお使いのデバイスの番号に置き換えてください：

```yaml
# ターミナルで以下のコマンドを実行して openvfd サービスを有効にする
sed -i 's|^#*openvfd_enable=.*|openvfd_enable="yes"|g' /etc/custom_service/start_service.sh
sed -i 's|^#*openvfd_boxid=.*|openvfd_boxid="15"|g' /etc/custom_service/start_service.sh
# 一部のデバイスでは 'BOOT' などの関連メッセージをクリアするために OpenVFD サービスの再起動が必要です
sed -i 's|^#*openvfd_restart=.*|openvfd_restart="yes"|g' /etc/custom_service/start_service.sh
```

- テスト後にご自身のデバイスの設定ファイル（diy.conf）を共有していただくことを歓迎します。より多くの方の参考になります。

|  デバイス名  | `デバイス番号` |  Armbian コマンド       |   OpenWrt コマンド        |   機能    |
| ---------- | ----------- | --------------------- | ----------------------- | -------- |
| x96max     |  11         |  armbian-openvfd 11   |   openwrt-openvfd 11    | LED 有効  |
| x96maxplus |  12         |  armbian-openvfd 12   |   openwrt-openvfd 12    | LED 有効  |
| x96air     |  13         |  armbian-openvfd 13   |   openwrt-openvfd 13    | LED 有効  |
| h96max-x3  |  14         |  armbian-openvfd 14   |   openwrt-openvfd 14    | LED 有効  |
| hk1-x3     |  15         |  armbian-openvfd 15   |   openwrt-openvfd 15    | LED 有効  |
| hk1box     |  16         |  armbian-openvfd 16   |   openwrt-openvfd 16    | LED 有効  |
| tx3        |  17         |  armbian-openvfd 17   |   openwrt-openvfd 17    | LED 有効  |
| tx3-mini   |  18         |  armbian-openvfd 18   |   openwrt-openvfd 18    | LED 有効  |
| t95        |  19         |  armbian-openvfd 19   |   openwrt-openvfd 19    | LED 有効  |
| t95z-plus  |  20         |  armbian-openvfd 20   |   openwrt-openvfd 20    | LED 有効  |
| tx9-pro    |  21         |  armbian-openvfd 21   |   openwrt-openvfd 21    | LED 有効  |
| x92        |  22         |  armbian-openvfd 22   |   openwrt-openvfd 22    | LED 有効  |
| whale      |  23         |  armbian-openvfd 23   |   openwrt-openvfd 23    | LED 有効  |
| x88pro-x3  |  24         |  armbian-openvfd 24   |   openwrt-openvfd 24    | LED 有効  |
| diy        |  99         |  armbian-openvfd 99   |   openwrt-openvfd 99    | LED 有効  |
| -          |  0          |  armbian-openvfd 0    |   openwrt-openvfd 0     | LED 無効  |
| -          |  -u         |  armbian-openvfd -u   |   openwrt-openvfd -u    | 設定更新  |
