# Set the box to boot from USB/TF/SD

- Write the firmware to USB/TF/SD, insert it into the box after writing.
- Open the developer mode: Settings → About this machine → Version number (for example: X96max plus...), click on the version number for 5 times in quick succession, See the prompt of `Enable Developer Mode` displayed by the system.
- Turn on USB debugging: System → Advanced options → Developer options again (after entering, confirm that the status is on, and the `USB debugging` status in the list is also on). Enable `ADB` debugging.
- Install ADB tools: Download [adb](https://github.com/ophub/script/releases/download/dev/adb.tar.gz) and unzip it, copy the three files `adb.exe`, `AdbWinApi.dll`, and `AdbWinUsbApi.dll` to the two files `system32` and `syswow64` under the directory of `c://windows/` Folder, then open the `cmd` command panel, use `adb --version` command, if it is displayed, it is ready to use.
- Enter the `cmd` command mode. Enter the `adb connect 192.168.1.137` command (the ip is modified according to your box, and you can check it in the router device connected to the box), If the link is successful, it will display `connected to 192.168.1.137:5555`
- Enter the `adb shell reboot update` command, the box will restart and boot from the USB/TF/SD you inserted, access the firmware IP address from a browser, or SSH to enter the firmware.


# 设置盒子从 USB/TF/SD 中启动

- 把刷好固件的 USB/TF/SD 插入盒子。
- 开启开发者模式: 设置 → 关于本机 → 版本号 (如: X96max plus...), 在版本号上快速连击 5 次鼠标左键, 看到系统显示 `开启开发者模式` 的提示。
- 开启 USB 调试模式: 系统 → 高级选选 → 开发者选项 (设置 `开启USB调试` 为启用)。启用 `ADB` 调试。
- 安装 ADB 工具：下载 [adb](https://github.com/ophub/script/releases/download/dev/adb.tar.gz) 并解压，将 `adb.exe`，`AdbWinApi.dll`，`AdbWinUsbApi.dll` 三个文件拷⻉到 `c://windows/` 目录下的 `system32` 和 `syswow64` 两个文件夹内，然后打开 `cmd` 命令面板，使用 `adb --version` 命令，如果有显示就表示可以使用了。
- 进入 `cmd` 命令模式。输入 `adb connect 192.168.1.137` 命令（其中的 ip 根据你的盒子修改，可以到盒子所接入的路由器设备里查看），如果链接成功会显示 `connected to 192.168.1.137:5555`
- 输入 `adb shell reboot update` 命令，盒子将重启并从你插入的 USB/TF/SD 启动，从浏览器访问固件的 IP 地址，或者 SSH 访问即可进入固件。
