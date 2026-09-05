# W103D RX enhance regression

Run on Linux with Python 3 and GCC:

```sh
python3 compile-kernel/tools/mt76/tests/test_rx_enhance.py
python3 compile-kernel/tools/mt76/tests/test_combo.py
python3 compile-kernel/tools/mt76/tests/test_mmc_irq.py /path/to/patched/linux
```

The test extracts the actual driver C functions and executes them with a fake
SDIO FIFO under AddressSanitizer and UndefinedBehaviorSanitizer. It covers byte
and block transfer boundaries, 16 packets including TXS, invalid counts,
allocation/read failures, padding validation, and an E0 → RX0 tail E1 → RX1
tail E2 sequence with exactly-once completion accounting and pending RX work.

SDIO, allocation and kernel scheduling are stubbed. This is not a hardware
concurrency, firmware recovery or throughput test. The normal kernel build and
real-board tests remain necessary. Do not deploy a module unless its vermagic
release exactly matches the target kernel (`6.12.107-ophub` for W103D).

The combo test compiles the actual per-card coordinator with pthread-backed
mutexes/completions under ASan/UBSan. It checks missing and unbound suppliers,
setup failure/timeout, device-link and allocation-action failure, card isolation,
transition exclusion, removal with outstanding references, and fresh-bind state.
Readiness is published by the actual HCI post-init callback; a late callback
after detach must retain the removal failure and cannot revive the supplier.
Device-link driver-core behavior is validated separately on the board.

The MMC test extracts the actual Meson hard-IRQ handler from the supplied
patched kernel tree. It checks 3552 combinations of SDIO/non-SDIO commands,
copy requirements, command chains, CRC/timeout/data errors and combined SDIO
interrupts, plus no-interrupt and SDIO-only cases under ASan/UBSan. Real-board
traces and same-core schedutil/IRQ latency tests are still required: these
stubs cannot validate hardware timing or the kernel scheduler.
