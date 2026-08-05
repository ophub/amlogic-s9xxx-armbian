# Mainline 6.18.y device tree for Houzzkit F1 (RK3568-JL-RM01)

Hand-written port of the vendor 6.1 BSP board file to the mainline 6.18.y
kernel used by ophub Armbian rebuilds (`unifreq/linux-6.18.y`).

## Files

- `rk3568-jl-rm01.dts` - board device tree source
- `rk3568-jl-rm01.dtb` - compiled device tree blob

## How it was verified

Compiled against `unifreq/linux-6.18.y` (`main`) with the standard kernel
preprocessor and `dtc`:

```sh
cpp -nostdinc -I arch/arm64/boot/dts -I arch/arm64/boot/dts/include \
    -I arch/arm64/boot/dts/rockchip -I scripts/dtc/include-prefixes \
    -I include -undef -D__DTS__ -x assembler-with-cpp \
    rk3568-jl-rm01.dts -o /tmp/rm01.dts

dtc -I dts -O dtb -b 0 -@ -o rk3568-jl-rm01.dtb /tmp/rm01.dts
```

The resulting dtb contains the expected nodes: UART0/2/3/4/7/9, dual RGMII
(gmac0/gmac1), eMMC + SD, USB2/USB3, PCIe2x1/3x2, HDMI, RK809 PMIC,
RTC (hym8563), GPU and NPU (`rknpu` with `interrupt-names = "npu_irq"`).

## Mainline kernel support

The Houzzkit F1 board is supported on the ophub mainline 6.18.y kernel
(`armbian-update -u stable -k 6.18`), verified on device:

| Function | Status |
| --- | --- |
| Kernel | 6.18.42-ophub |
| Ethernet (dual RGMII, YT8531) | OK - 1Gbps full duplex, both PHYs detected |
| eMMC / SD | OK |
| USB2 / USB3 | OK |
| PCIe2x1 (M.2 WiFi) | OK - RTL8822CE link up |
| PCIe3x2 (M.2 SSD) | OK - host bridge up (no SSD fitted during test) |
| WiFi / Bluetooth | OK - RTL8822CE scan OK, hci0 present |
| HDMI | OK - DRM node present (no display connected during test) |
| RK809 PMIC / audio (HDMI + analog) | OK |
| RTC (hym8563) | OK |
| GPU (panfrost) | OK - mali-g52 initialized |
| System LED (GPIO0_C2) | OK - heartbeat |
| NPU | Node present and enabled, but the unifreq/ophub 6.18 kernel has no rknpu driver (not even in the source tree), so no `/dev/dri/renderD129` - use the 6.1 BSP kernel for NPU |

Known kernel-level limitation: UART7/UART9 cannot register because the ophub
6.18 kernel's 8250 port count is limited (UART0-4 work, including UART3 for
the Zigbee gateway).

## Notes

- The source depends on `rk3568-ip.dtsi` (and the vpu/gpu/npu/crypto dtsi
  files) from `unifreq/linux-6.18.y`.  These are Rockchip BSP additions kept
  in the unifreq tree; they are not part of upstream mainline.
- BSP-only bindings that do not exist upstream are omitted:
  `rockchip_headset`, `wlan-platdata`, HDMI `rockchip,phy-table`,
  `mpp_srv` userspace service, PMIC sleep pin muxing.
- To build via `make dtbs`, place the dts in
  `arch/arm64/boot/dts/rockchip/` and add
  `dtb-$(CONFIG_ARCH_ROCKCHIP) += rk3568-jl-rm01.dtb` to the `Makefile`.

## Installing on the device (kernel 6.18)

```sh
# after: armbian-update -u stable -k 6.18
cp rk3568-jl-rm01.dtb /boot/dtb/rockchip/rk3568-jl-rm01.dtb
reboot
```

`/boot/armbianEnv.txt` must contain `fdtfile=rk3568-jl-rm01.dtb`.
