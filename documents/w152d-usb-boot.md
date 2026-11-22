# ZTE W152D USB boot

Use the W152D Armbian image with the vendor U-Boot 2019.01 on the GXLX4 BT203 board. Flash the image to a USB drive and connect a 921600 baud, 8N1 serial console. Run `printenv bootcmd recovery_from_udisk common_dtb_load storeboot` at the `gxlx4_bt203#` prompt. Save the original `bootcmd` so it can be restored.

Set this boot command for the current session first:

```text
setenv bootcmd 'gpio set GPIOAO_11; gpio set GPIOC_2; gpio set GPIOC_3; gpio set GPIOC_4; gpio clear GPIOH_6; setenv w152d_bootargs "${bootargs}"; run recovery_from_udisk; setenv bootargs "${w152d_bootargs}"; setenv w152d_bootargs; run common_dtb_load; run storeboot'
run bootcmd
```

`GPIOAO_11` powers USB3. `GPIOC_2`, `GPIOC_3`, and `GPIOC_4` power the USB2 ports, and clearing `GPIOH_6` releases the hub. These steps run before the vendor USB scan. The boot command tries the Armbian script on the USB drive and falls back to the vendor Android boot path when the USB attempt returns.

After testing both Armbian and the no-drive Android fallback, set the same `bootcmd` again and verify it with `printenv bootcmd`. Run `saveenv` to make it persistent. Keep the power connected until U-Boot reports success. For USB boot, connect the Armbian drive before power-on and use one USB storage device. To restore the previous boot order, set the saved original `bootcmd` and run `saveenv` again.

The Armbian W152D image is for USB boot. The device keeps its vendor bootloader and Android installation.

## SC2355 Wi-Fi

The W152D root filesystem overlay installs `wcnmodem.bin` and `wifi_56630001_3ant.ini` under `/usr/lib/firmware/`. These are the files used on the tested W152D. They come from the stock Android vendor paths `/vendor/firmware/wcnmodem.bin` and `/vendor/etc/wifi_board_config.ini`, respectively.

The kernel build provides `sprdwl_ng.ko`, which loads automatically after the USB root filesystem is mounted. After boot, select a Wi-Fi network and enter its credentials using the system's network settings.
