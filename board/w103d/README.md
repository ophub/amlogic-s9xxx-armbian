# ZTE W103D board support

This port targets the ZTE W103D (Amlogic S905L3A/G12A, 2 GiB RAM,
32 GiB eMMC) with Linux 6.12.107. It provides a standalone board DTS,
kernel configuration fragment, MT7663S Wi-Fi/SDIO Bluetooth integration,
and a model-database entry. Boot and wireless validation used removable
USB media and the vendor U-Boot; eMMC installation is not validated.

## Kernel integration

Apply the six patches in `compile-kernel/tools/patch/linux-6.12.y` in
numbered order. They were built against `unifreq/linux-6.12.y` commit
`4c81f10ed9fea62e29081f0d2a129f1b59bf2fe5` (6.12.107):

1. Add the standalone `meson-g12a-w103d.dts` and DTB build target.
2. Handle Meson SRAM transfer alignment and the W103D SDIO setup.
3. Install the isolated MT7663S transport, firmware and aggregation support.
4. Route GPIOX SDIO pins to SD_EMMC_B, which supports descriptor DMA.
5. Coordinate same-card Bluetooth and Wi-Fi initialization and teardown.
6. Complete successful, copy-free W103D SDIO requests in the hard IRQ.

The review copies in `linux/`, `compile-kernel/tools/mt76/mt7663s/` and
`compile-kernel/tools/mt76/combo/` match the patched kernel sources. Update
the corresponding patch whenever changing a review copy.

The DTS includes `meson-g12a.dtsi` directly. SDIO uses a 4-bit SDR104 bus
at 200 MHz, GPIOX_6 power/reset, a 32 kHz clock, and the SD_EMMC_B controller
at `ffe05000`. Function 1 is Wi-Fi (`037a:7603`); function 2 is Bluetooth
(`037a:7663`). The board has no UART Bluetooth child. SD_EMMC_A is disabled;
microSD operation is not validated. eMMC is described as 8-bit HS200.

## Wireless behavior

RX enhance uses the MT7663 16-entry encoding and consumes complete
112-byte interrupt snapshots, including the trailer after received data.
Completion credits are accounted exactly once across IRQ, polling and RX
trailer paths. Malformed lengths, transfer failures and oversized TX frames
are checked before unsafe access or queueing.

Bluetooth publishes per-card readiness only after HCI `post_init`, so its
initial address read finishes before Wi-Fi reads the shared eFuse engine.
Wi-Fi waits at most 20 seconds without holding the SDIO host. Setup errors
do not authorize initialization. A per-card transition mutex and managed
device link coordinate firmware transitions, unbind and system PM ordering.
Module dependencies load Bluetooth for `modprobe mt7663s`; no dedicated
ordering service, blacklist or userspace delay is required. Association,
DHCP and Bluetooth power policy remain normal userspace responsibilities.

The W103D MMC completion path avoids waiting behind the schedutil worker
for successful terminal CMD52/CMD53 requests. Errors, bounce reads and
command chains retain threaded handling. DMA-aligned interrupt buffers
allow the small status reads to use the direct path. This does not require
CPU frequency or IRQ affinity overrides.

See the [combo documentation](../../compile-kernel/tools/mt76/combo/README.md)
for lifecycle details and boundaries.

## Build and ABI checks

Layer `linux/w103d-6.12.config` on ophub/kernel's stable `config-6.12` and
run `olddefconfig` after applying the patches. Verify
`CONFIG_MT7663S_W103D_COMBO=y` with both SDIO drivers enabled.

All builds intended for the validated board must explicitly use:

```text
ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- LOCALVERSION=-ophub
```

For example, with the patched source and configured output tree:

```sh
make -C "$KERNEL_SRC" O="$KERNEL_OUT" \
  ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- LOCALVERSION=-ophub \
  -j"$(nproc)" Image modules amlogic/meson-g12a-w103d.dtb
```

Before copying any module, the first field from `modinfo -F vermagic`
must exactly equal the target board's `uname -r`: `6.12.107-ophub`.
`6.12.107`, `6.12.107+` and prefix-only matches are not valid.
MT7663S must expose `sdio:c*v037Ad7603*` and depend on
`mt76-connac-lib`, `mt7663-usb-sdio-common`, `mt7615-common`, `mt76-sdio`,
`mt76`, `mac80211`, `cfg80211`, and the combo provider `btmtksdio`.

## Firmware and reproducibility boundary

The W103D rootfs overlay contains these linux-firmware `20260810` blobs:

| File under `mediatek/` | SHA-256 |
| --- | --- |
| `mt7663pr2h.bin` | `534f2152f9b0f48dfcec9c1727b0bab9a3727a655d76d5ec818cc39a55602336` |
| `mt7663_n9_v3.bin` | `d8b8488f37f65e41ded9a911950afb000d236b36e5aa94ad9650ad0c3a86fd45` |

**The final wireless tests used a different N9 firmware:** the factory
2021 build `7663mp1827`, timestamp `20210308205639`, SHA-256
`223f73f17f0f986dc4e7167daa6eef14ffb41c713f22d70f9645eb049bdec80a`.
The ROM patch matched the overlay. The tested factory N9 is not included
in this repository. Consequently, the results below do not certify images
assembled with the overlay's newer N9. Firmware selection and validation
remain an integration requirement before claiming a reproducible release.

## Validation on 2026-09-05

The final kernel and modules were tested on the physical board running
`6.12.107-ophub`, with USB Ethernet and USB Wi-Fi disconnected. Data and TCP
ACKs used onboard Wi-Fi, associated at 5 GHz/VHT80/NSS2; the built-in Ethernet
had no carrier.

- Patch series applies cleanly; review copies match the patched sources.
- Native tests execute the actual RX enhance, combo and MMC IRQ C code
  under ASan/UBSan. The MMC harness covers 3552 command/error/copy/chain
  combinations plus SDIO-only and no-interrupt cases.
- With MMC IRQ and schedutil on the same CPU, ping changed from about
  12 ms to a 2 ms median. A hardware trace measured an 8 microsecond median
  MMC request completion time, down from 984 microseconds.
- Ten complete Wi-Fi/Bluetooth unload/reload cycles recovered networking;
  interface MAC and full EEPROM/OTP checksums remained identical. Before
  deferring readiness to HCI post-init, three of ten cycles had EEPROM
  differences, including two changed interface addresses.
- Three Bluetooth power on/off cycles and a normal reboot passed.
- After reboot, 20 pings all replied in 1–3 ms. Loaded ELF build IDs and
  module disk hashes matched the validated artifacts.
- Post-reboot TCP transfers completed: 100 MiB RX at 415.438 Mbps, 100 MiB
  TX at 77.291 Mbps, and concurrently started 512 MiB RX / 256 MiB TX at
  249.438 / 59.769 Mbps. Each concurrent average covers its own transfer
  duration, not a shared steady-state interval.
- MMC error counters remained zero after traffic; no kernel WARNING,
  BUG, Oops or atomic-sleep report was found. Default schedutil,
  power-save-on and IRQ allowed CPUs `0-3` were retained.

Run the native regressions on Linux with Python 3 and GCC:

```sh
python3 compile-kernel/tools/mt76/tests/test_rx_enhance.py
python3 compile-kernel/tools/mt76/tests/test_combo.py
python3 compile-kernel/tools/mt76/tests/test_mmc_irq.py "$KERNEL_SRC"
```

These finite tests do not establish long-term throughput guarantees,
arbitrary firmware-crash recovery, suspend/resume reliability or complete
peripheral support. Wired Ethernet, IR, analog audio and eMMC installation
are outside this wireless validation. The model database keeps automatic
image building disabled (`BUILD=no`) and specifies no replacement U-Boot.
