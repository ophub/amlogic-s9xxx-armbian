# How to restore the original Android TV system

- Under normal circumstances, re-insert the USB hard disk and install it again.

- If you cannot start the Armbian system from the USB hard disk again, connect the Amlogic s9xxx tv box to the computer monitor. If the screen is completely black and there is nothing, you need to restore the Amlogic s9xxx tv box to factory settings first, and then reinstall it. First download the [amlogic_usb_burning_tool](https://github.com/ophub/script/releases/download/dev/amlogic_usb_burning_tool_v3.2.0_and_driver.tar.gz) system recovery tool and install it.

```
Take x96max+ as an example.

Prepare materials:

1. [ A USB male-to-male data cable ]: https://www.ebay.com/itm/152516378334
2. [ A paper clip ]: https://www.ebay.com/itm/133577738858
3. [ Download the Android TV firmware ]: https://xdafirmware.com/x96-max-plus-2
4. [ Find the two short-circuit points on the motherboard ]:
   https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg

Operation method:

1. Open the USB Burning Tool:
   [ File → Import image ]: X96Max_Plus2_20191213-1457_ATV9_davietPDA_v1.5.img
   [ Check ]：Erase flash
   [ Check ]：Erase bootloader
   Click the [ Start ] button
2. Use a [ paper clip ] to connect the [ two shorting points ] on the main board of the box,
   and use a [ USB dual male data cable ] to connect the [ box ] to the [ computer ] at the same time.
3. Loosen the short contact after seeing the [ progress bar moving ].
4. After the [ progress bar is 100% ], the restoration of the original Android TV system is completed.
   Click [ stop ], unplug the [ USB male-to-male data cable ] and [ power ].
5. If the progress bar is interrupted, repeat the above steps until it succeeds.
   If the progress bar does not respond after the short-circuit, plug in the [ power ] supply after the short-circuit.
   Generally, there is no need to plug in the power supply.
```

When the factory reset is completed, the box has been restored to the Android TV system, and other operations to install the Armbian system are the same as the requirements when you installed the system for the first time before, just do it again.


# 如何恢复原安卓 TV 系统

- 一般情况下，重新插入电源，如果可以从 USB 中启动，只要重新安装即可，多试几次。

- 如果接入显示器后，屏幕是黑屏状态，无法从 USB 启动，就需要进行盒子的短接初始化了。先将盒子恢复到原来的安卓系统，再重新刷入 Armbian 系统。首先下载 [amlogic_usb_burning_tool](https://github.com/ophub/script/releases/download/dev/amlogic_usb_burning_tool_v3.2.0_and_driver.tar.gz) 系统恢复工具并安装好。

```
以 x96max+ 为例

刷机准备：

1. [ 准备一条 USB 双公头数据线 ]: https://www.ebay.com/itm/152516378334
2. [ 准备一个曲别针 ]: https://www.ebay.com/itm/133577738858
3. [ 下载盒子的 Android TV 固件包 ]: https://xdafirmware.com/x96-max-plus-2
4. [ 在盒子的主板上确认短接点的位置 ]:
   https://user-images.githubusercontent.com/68696949/110590933-67785300-81b3-11eb-9860-986ef35dca7d.jpg

操作方法：

1. 打开刷机软件 USB Burning Tool:
   [ 文件 → 导入固件包 ]: X96Max_Plus2_20191213-1457_ATV9_davietPDA_v1.5.img
   [ 选择 ]：擦除 flash
   [ 选择 ]：擦除 bootloader
   点击 [ 开始 ] 按钮
2. 使用 [ 曲别针 ] 将盒子主板上的 [ 两个短接点进行短接连接 ]，
   并同时使用 [ USB 双公头数据线 ] 将 [ 盒子 ] 与 [ 电脑 ] 进行连接。
3. 当看到 [ 进度条开始走动 ] 后，拿走曲别针，不再短接。
4. 当看到 [ 进度条 100% ], 则刷机完成，盒子已经恢复成 Android TV 系统。
   点击 [ 停止 ] 按钮, 拔掉 [ 盒子 ] 和 [ 电脑 ] 之间的 [ USB 双公头数据线] 。
5. 如果以上某个步骤失败，就再来一次，直至成功。
   如果进度条没有走动，可以尝试插入电源。通长情况下不用电源支持供电，只 USB 双公头的供电即可满足刷机要求。
```

当完成恢复出厂设置，盒子已经恢复成 Android TV 系统，其他安装 Armbian 系统的操作，就和你之前第一次安装系统时的要求一样了，再来一遍即可。
