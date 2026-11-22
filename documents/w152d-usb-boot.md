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

The standard rebuild takes `wcnmodem.bin` and `wifi_56630001_3ant.ini` from [ophub/firmware](https://github.com/ophub/firmware/pull/9). They are stored under `/usr/lib/firmware/w152d/`. The board overlay adds links to the names requested by the driver. These are the files used on the tested W152D. They come from the stock Android vendor paths `/vendor/firmware/wcnmodem.bin` and `/vendor/etc/wifi_board_config.ini`, respectively.

The kernel build provides `sprdwl_ng.ko`, which loads automatically after the USB root filesystem is mounted. After boot, select a Wi-Fi network and enter its credentials using the system's network settings.

## Hardware and support status

| Function | Status on the tested GXLX4 BT203 W152D |
| --- | --- |
| USB boot and mass storage | Three USB2 ports and one USB3 port tested with the vendor U-Boot USB-first configuration. The SanDisk drive enumerated at 5 Gbps on USB3. |
| Wired Ethernet | 100 Mbps full duplex, DHCP and LAN traffic verified. |
| SC2355 Wi-Fi | SDIO driver, firmware and board configuration included. Association, DHCP and SSH verified. Users supply their own network credentials. |
| Serial and front panel | UART console at 921600 baud, 8N1. Power and Ethernet link LEDs verified. |
| Internal flash | USB-only Armbian. With the drive removed, the tested box returns to its existing Android 11 installation. |
| Unsupported in this port | HDMI output, Bluetooth, hardware video decoding, temperature readout, CPU frequency scaling and the hardware watchdog. |

The [RC3 image](https://github.com/Zlion-Y/amlogic-s9xxx-armbian/releases/tag/W152D-Bookworm-6.12.118-rc3) passed clean USB3 first boot, ext4 expansion,
wired and wireless checks, and a same-version kernel package reinstall followed
by a reboot. The three USB2 ports and no-drive Android fallback were checked
earlier on the same box with the same boot configuration.

Cross-version kernel upgrades and physical regression tests on other boards
using the shared kernel are pending. The box's direct connection to GitHub
stalled during the update check, so the verified packages were transferred over
the LAN for the reinstall test.

## Image build and updates

The vendor U-Boot reads the prebuilt FAT `aml_autoscript`. Regenerate it
after changing the script source:

~~~sh
SOURCE_DATE_EPOCH=0 mkimage -A arm -O linux -T script -C none \
    -n 'Armbian USB boot' -d aml_autoscript.cmd aml_autoscript
~~~

Use the standard `rebuild` and the stable 6.12 kernel package containing
`meson-gxlx4-w152d.dtb`. Select `w152d` explicitly. The stock Bookworm image
runs `armbian-tf` on first boot. The image supports USB boot only.

The board overlay was rebuilt offline using the [ophub 6.12.118 package](https://github.com/ophub/kernel/actions/runs/45017563483).
Its kernel and initramfs match the published package byte for byte.
The RC3 results above describe the prior hardware run. Fresh hardware
boot and updates using the narrowed image are pending.
