# LED screen display control instructions

- The configuration file is placed in the [/usr/share/openvfd](../armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd) directory of the `Armbian/OpenWrt` system, and the command file for `Armbian` systems is located at [/usr/sbin/armbian-openvfd](../armbian-files/platform-files/amlogic/rootfs/usr/sbin/armbian-openvfd), and the command file for `OpenWrt` systems is located at [/usr/sbin/openwrt-openvfd](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/make-openwrt/openwrt-files/common-files/usr/sbin/openwrt-openvfd). If it is not in the current firmware, it can be uploaded manually, And give the file execute permission: `chmod +x /usr/share/openvfd/vfdservice /usr/sbin/*-openvfd`

- Upgrade your system's kernel to the latest version. `Armbian` systems are updated using the `armbian-update` command. For `OpenWrt` system, use `System menu` → `Amlogic Service` → `Online Download Update` to upgrade the function.

- At present, there are several boxes such as `x96max.conf`, `x96maxplus.conf`, `h96max-x3.conf`, `hk1-x3.conf`, `hk1box.conf`, `tx3.conf`, `x96air.conf` etc. that have passed the test. The configuration of other devices can be viewed: [arthur-liberman/vfd-configurations](https://github.com/arthur-liberman/vfd-configurations) and [LibreELEC/linux_openvfd](https://github.com/LibreELEC/linux_openvfd/tree/master/conf) to modify, It is necessary to adjust the corresponding content in the configuration files of these two websites, and use it after subtracting `1` from the value of the second field, such as:

```yaml
vfd_gpio_clk='0,69,0'
vfd_gpio_dat='0,70,0'
```
change into:

```yaml
vfd_gpio_clk='0,68,0'
vfd_gpio_dat='0,69,0'
```

- Take the configuration of [x96maxplus](../armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd/conf/x96maxplus.conf) as an example: if the displayed time and text order is not correct, you can adjust the numerical order of `vfd_chars='4,0,1,2,3'` to `vfd_chars='1,2,3,4,0'`, etc. for testing. If the time is displayed in reverse, you can adjust the `first value 0x02` in `vfd_display_type='0x02,0x00,0x01,0x00'` to `0x01`, etc. for testing. The displayed content can adjust the value in `functions='usb apps setup sd hdmi cvbs'` according to the specific situation supported by your own device

- Name the configuration file `diy.conf` and upload it to the `/usr/share/openvfd/conf` directory, enter the command `armbian-openvfd 99` to test.

- You can disable the LED display and clear system processes with the command `armbian-openvfd 0`, before each test a new configuration, please execute this disable command first, and then execute `armbian-openvfd 99` to make changes After the configuration test.

- After the screen is displayed normally, you can add it to the self-starting task at boot, Please modify the `15` in the following command according to the serial number corresponding to the box in the `armbian-openvfd` option:

```yaml
# Execute the following command in the terminal to add the Armbian system
sed -i '/armbian-openvfd/d' /etc/rc.local
sed -i '/^exit 0/i\[[ -f "/usr/sbin/armbian-openvfd" ]] && armbian-openvfd 15' /etc/rc.local

# Execute the following command in the terminal to add the OpenWrt system
sed -i '/openwrt-openvfd/d' /etc/rc.local
sed -i '/^exit 0/i\[[ -f "/usr/sbin/openwrt-openvfd" ]] && openwrt-openvfd 15' /etc/rc.local
```

- You are welcome to share the conf file(xxx.conf) of your own devices after testing, so that more people can benefit.

|     Box    |   ID   |  Armbian Command      |   OpenWrt Command       |  Function   |
| ---------- |  ----- | --------------------- | ----------------------- | ----------- |
| x96max     |  11    |  armbian-openvfd 11   |   openwrt-openvfd 11    | Enable LED  |
| x96maxplus |  12    |  armbian-openvfd 12   |   openwrt-openvfd 12    | Enable LED  |
| x96air     |  13    |  armbian-openvfd 13   |   openwrt-openvfd 13    | Enable LED  |
| h96max-x3  |  14    |  armbian-openvfd 14   |   openwrt-openvfd 14    | Enable LED  |
| hk1-x3     |  15    |  armbian-openvfd 15   |   openwrt-openvfd 15    | Enable LED  |
| hk1box     |  16    |  armbian-openvfd 16   |   openwrt-openvfd 16    | Enable LED  |
| tx3        |  17    |  armbian-openvfd 17   |   openwrt-openvfd 17    | Enable LED  |
| tx3-mini   |  18    |  armbian-openvfd 18   |   openwrt-openvfd 18    | Enable LED  |
| t95        |  19    |  armbian-openvfd 19   |   openwrt-openvfd 19    | Enable LED  |
| t95z-plus  |  20    |  armbian-openvfd 20   |   openwrt-openvfd 20    | Enable LED  |
| tx9-pro    |  21    |  armbian-openvfd 21   |   openwrt-openvfd 21    | Enable LED  |
| x92        |  22    |  armbian-openvfd 22   |   openwrt-openvfd 22    | Enable LED  |
| diy        |  99    |  armbian-openvfd 99   |   openwrt-openvfd 99    | Enable LED  |
| -          |  0     |  armbian-openvfd 0    |   openwrt-openvfd 0     | Disable LED |
| -          |  -u    |  armbian-openvfd -u   |   openwrt-openvfd -u    | Update Conf |

# LED 屏显示控制说明

- 配置文件放在 `Armbian/OpenWrt` 系统的 [/usr/share/openvfd](../armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd) 目录下，`Armbian` 系统的命令文件位于 [/usr/sbin/armbian-openvfd](../armbian-files/platform-files/amlogic/rootfs/usr/sbin/armbian-openvfd)，`OpenWrt` 系统的命令文件位于 [/usr/sbin/openwrt-openvfd](https://github.com/ophub/amlogic-s9xxx-openwrt/blob/main/make-openwrt/openwrt-files/common-files/usr/sbin/openwrt-openvfd)。如果当前固件中没有的可以手动上传，并赋予文件执行权限：`chmod +x /usr/share/openvfd/vfdservice /usr/sbin/*-openvfd`

- 将系统的内核升级到最新版本。`Armbian` 系统使用 `armbian-update` 命令升级。`OpenWrt` 系统使用 `系统菜单` → `晶晨宝盒` → `在线下载更新` 功能升级。

- 目前有 `x96max.conf`、`x96maxplus.conf`、`h96max-x3.conf`、`hk1-x3.conf`、`hk1box.conf`、`tx3.conf`、`x96air.conf` 等设备经过测试，其他设备的配置可以查看：[arthur-liberman/vfd-configurations](https://github.com/arthur-liberman/vfd-configurations) 和 [LibreELEC/linux_openvfd](https://github.com/LibreELEC/linux_openvfd/tree/master/conf) 进行修改，需要把这两个网站中配置文件里对应内容中进行调整，把第二个字段的值减 `1` 后使用，如：

```yaml
vfd_gpio_clk='0,69,0'
vfd_gpio_dat='0,70,0'
```
修改为：

```yaml
vfd_gpio_clk='0,68,0'
vfd_gpio_dat='0,69,0'
```

- 以 [x96maxplus](../armbian-files/platform-files/amlogic/rootfs/usr/share/openvfd/conf/x96maxplus.conf) 的配置为例：如果显示的时间文字顺序不正确，可以调整 `vfd_chars='4,0,1,2,3'` 的数字顺序为 `vfd_chars='1,2,3,4,0'` 等进行测试。如果时间是翻转显示，可以调整 `vfd_display_type='0x02,0x00,0x01,0x00'` 中的 `第一个值 0x02` 为 `0x01` 等进行测试。显示的内容可根据自己的设备支持的具体情况调整 `functions='usb apps setup sd hdmi cvbs'` 中的值。

- 将配置文件命名为 `diy.conf` 并上传至 `/usr/share/openvfd/conf` 目录下，输入命令 `armbian-openvfd 99` 进行测试。

- 通过命令 `armbian-openvfd 0` 可以禁用 LED 显示并清除系统进程，在每次测试新的配置前，请先执行此禁用命令，再执行 `armbian-openvfd 99` 进行更改后的配置测试。

- 屏幕显示正常后，可以添加至开机自启动任务，下面命令中的 `15` 请根据 `armbian-openvfd` 选项中盒子对应的序号进行修改：

```yaml
# Armbian 系统在终端执行以下命令添加
sed -i '/armbian-openvfd/d' /etc/rc.local
sed -i '/^exit 0/i\[[ -f "/usr/sbin/armbian-openvfd" ]] && armbian-openvfd 15' /etc/rc.local

# OpenWrt 系统在终端执行以下命令添加
sed -i '/openwrt-openvfd/d' /etc/rc.local
sed -i '/^exit 0/i\[[ -f "/usr/sbin/openwrt-openvfd" ]] && openwrt-openvfd 15' /etc/rc.local
```

- 欢迎大家测试后分享自己设备的配置文件（ diy.conf ），让更多人受益。

|     盒子    |  编号  |  Armbian 使用命令       |   OpenWrt 使用命令       |   功能   |
| ---------- |  ----- | --------------------- | ----------------------- | ------- |
| x96max     |  11    |  armbian-openvfd 11   |   openwrt-openvfd 11    | 启用 LED |
| x96maxplus |  12    |  armbian-openvfd 12   |   openwrt-openvfd 12    | 启用 LED |
| x96air     |  13    |  armbian-openvfd 13   |   openwrt-openvfd 13    | 启用 LED |
| h96max-x3  |  14    |  armbian-openvfd 14   |   openwrt-openvfd 14    | 启用 LED |
| hk1-x3     |  15    |  armbian-openvfd 15   |   openwrt-openvfd 15    | 启用 LED |
| hk1box     |  16    |  armbian-openvfd 16   |   openwrt-openvfd 16    | 启用 LED |
| tx3        |  17    |  armbian-openvfd 17   |   openwrt-openvfd 17    | 启用 LED |
| tx3-mini   |  18    |  armbian-openvfd 18   |   openwrt-openvfd 18    | 启用 LED |
| t95        |  19    |  armbian-openvfd 19   |   openwrt-openvfd 19    | 启用 LED |
| t95z-plus  |  20    |  armbian-openvfd 20   |   openwrt-openvfd 20    | 启用 LED |
| tx9-pro    |  21    |  armbian-openvfd 21   |   openwrt-openvfd 21    | 启用 LED |
| x92        |  22    |  armbian-openvfd 22   |   openwrt-openvfd 22    | 启用 LED |
| diy        |  99    |  armbian-openvfd 99   |   openwrt-openvfd 99    | 启用 LED |
| -          |  0     |  armbian-openvfd 0    |   openwrt-openvfd 0     | 禁用 LED |
| -          |  -u    |  armbian-openvfd -u   |   openwrt-openvfd -u    | 更新配置  |

