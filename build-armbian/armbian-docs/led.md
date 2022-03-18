# LED test instructions

- Upgrade the kernel to version 5.4.185, 5.10.106, 5.15.29, 5.16.15 or later via the `armbian-update` command.

- Currently, six devices have been tested, including `x96max.conf`, `x96maxplus.conf`, `h96max-x3.conf`, `hk1-x3.conf`, `hk1box.conf`, and `tx3.conf`. The configuration of other devices can be viewed: [arthur-liberman/vfd-configurations](https://github.com/arthur-liberman/vfd-configurations) and [LibreELEC/linux_openvfd](https://github.com/LibreELEC/linux_openvfd/tree/master/conf) to modify, It is necessary to adjust the corresponding content in the configuration files of these two websites, and use it after subtracting `1` from the value of the second field, such as:

```yaml
vfd_gpio_clk='0,69,0'
vfd_gpio_dat='0,70,0'
```
change into:

```yaml
vfd_gpio_clk='0,68,0'
vfd_gpio_dat='0,69,0'
```

- Name the configuration file `diy.conf` and upload it to the `/usr/share/openvfd/conf` directory, enter the command `armbian-led 99` to test.

- You can disable the LED display and clear system processes with the command `armbian-led 0`, before each test a new configuration, please execute this disable command first, and then execute `armbian-led 99` to make changes After the configuration test.

- You are welcome to share the conf file(xxx.conf) of your own devices after testing, so that more people can benefit.

|     Box    |    Command     |   Function  |
| ---------- | -------------- | ----------- |
| -          | armbian-led 0  | Disable LED |
| x96max     | armbian-led 10 | Enable LED  |
| x96maxplus | armbian-led 11 | Enable LED  |
| h96max-x3  | armbian-led 12 | Enable LED  |
| hk1-x3     | armbian-led 13 | Enable LED  |
| hk1box     | armbian-led 14 | Enable LED  |
| tx3        | armbian-led 15 | Enable LED  |
| diy        | armbian-led 99 | Enable LED  |


# LED 测试说明

- 通过 `armbian-update` 命令将内核升级到 5.4.185、5.10.106、5.15.29、5.16.15 或更高版本。

- 目前有 `x96max.conf`、`x96maxplus.conf`、`h96max-x3.conf`、`hk1-x3.conf`、`hk1box.conf`、`tx3.conf` 共六个设备经过测试，其他设备的配置可以查看：[arthur-liberman/vfd-configurations](https://github.com/arthur-liberman/vfd-configurations) 和 [LibreELEC/linux_openvfd](https://github.com/LibreELEC/linux_openvfd/tree/master/conf) 进行修改，需要把这两个网站中配置文件里对应内容中进行调整，把第二个字段的值减 `1` 后使用，如：

```yaml
vfd_gpio_clk='0,69,0'
vfd_gpio_dat='0,70,0'
```
修改为：

```yaml
vfd_gpio_clk='0,68,0'
vfd_gpio_dat='0,69,0'
```

- 将配置文件命名为 `diy.conf` 并上传至 `/usr/share/openvfd/conf` 目录下，输入命令 `armbian-led 99` 进行测试。

- 通过命令 `armbian-led 0` 可以禁用 LED 显示并清除系统进程，在每次测试新的配置前，请先执行此禁用命令，再执行 `armbian-led 99` 进行更改后的配置测试。

- 欢迎大家测试后分享自己设备的配置文件（ diy.conf ），让更多人受益。

|     盒子    |      命令      |   功能   |
| ---------- | -------------- | ------- |
| -          | armbian-led 0  | 禁用 LED |
| x96max     | armbian-led 10 | 启用 LED |
| x96maxplus | armbian-led 11 | 启用 LED |
| h96max-x3  | armbian-led 12 | 启用 LED |
| hk1-x3     | armbian-led 13 | 启用 LED |
| hk1box     | armbian-led 14 | 启用 LED |
| tx3        | armbian-led 15 | 启用 LED |
| diy        | armbian-led 99 | 启用 LED |
