# W103D Linux 6.18 patch series

Target: `unifreq/linux-6.18.y` commit
`6943daf37ba41998e600f2249a9b84030290b37c` (Linux 6.18.49).

Apply `0001` through `0006` in numeric order. The board registration and
Bluetooth hook context are rebased onto this tree. In particular, retain
6.18's `btmtksdio_close_hw()` and its workqueue/SDIO-host deadlock fix,
MT7668-specific behavior, HCI quirk helpers and runtime-PM changes.
The same-card W103D readiness callback remains after HCI initialization.

The isolated MT7663S and combo review copies are shared with the 6.12 series
in `compile-kernel/tools/mt76/`. The existing 6.12 source and patches are
unchanged. The separate kernel configuration is
`board/w103d/linux/w103d-6.18.config`.

Use `board/w103d/validation/verify_patch_series.py KERNEL --series 6.18`
to check clean application, source-copy equivalence and the actual MMC IRQ
regression. `board/w103d/validation/build-kernel.sh` builds an already patched
kernel without installing or packaging any module.

The pinned revision passed a complete ARM64 Image, module and DTB build,
module metadata checks, native RX enhance/combo tests and the MMC IRQ
regression on 2026-09-05. See `board/w103d/linux/6.18-validation.md` for the
configuration, exact build target and validation limits.

The matched 6.18.49-ophub kernel/module/boot set was subsequently tested on
W103D on 2026-09-06; see the validation record for measured behavior and
test limits. A running 6.12 kernel must not load these modules or have their
vermagic disguised as 6.12. The rootfs overlay now pins the tested factory
N9/ROM-patch pair; `board/w103d/firmware/README.md` records their provenance
and licensing scope. Image assembly validates this pair and the kernel set.
