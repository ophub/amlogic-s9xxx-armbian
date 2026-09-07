# W103D RX enhance regression

Run on Linux with Python 3 and GCC:

```sh
python3 compile-kernel/tools/mt76/tests/test_rx_enhance.py
python3 compile-kernel/tools/mt76/tests/test_combo.py
python3 compile-kernel/tools/mt76/tests/test_scan_rlm.py
python3 compile-kernel/tools/mt76/tests/test_connection_rate.py
python3 compile-kernel/tools/mt76/tests/test_tx_rate_reporting.py /path/to/patched/linux
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
release exactly matches the target board's `uname -r` (currently
`6.18.49-ophub`; `6.12.107-ophub` is the retained fallback).

The combo test compiles the actual per-card coordinator with pthread-backed
mutexes/completions under ASan/UBSan. It checks missing and unbound suppliers,
setup failure/timeout, device-link and allocation-action failure, card isolation,
transition exclusion, removal with outstanding references, and fresh-bind state.
Readiness is published by the actual HCI post-init callback; a late callback
after detach must retain the removal failure and cannot revive the supplier.
Device-link driver-core behavior is validated separately on the board.

The scan test compiles the driver's actual 22-byte gen4m BSS RLM encoder
and submission helper. It checks wire offsets, all primary-channel positions
in two VHT80 blocks, HT40 above/below and HT20 on 2.4 GHz, rate-set mapping,
NSS, zeroed reserved fields, disconnected state and MCU submission failures.
It does not emulate firmware scanning: full-channel discovery, continuous
association and traffic recovery must also be checked on the board.

The connection-rate test compiles the real 5 GHz TXWI encoder and the frame
classification expressions. It checks that Null/QoS Null and minimum-rate
requests use basic OFDM with retries and no BA/STBC/LDPC, while payload traffic
and minstrel sampling retain VHT bandwidth, NSS, coding and selected rates.
It does not simulate RF acknowledgements; repeated scans and connection
probe status must still be traced on the board.

The TX-rate reporting test executes the patched mac80211 reporting block.
It checks that Null and management frames preserve the last payload rate,
while genuine low-rate payloads, explicit rate-control reports and hardware
encapsulation still update it. This changes telemetry, not the transmitted
rate or the connection-monitor ACK decision.

The MMC test extracts the actual Meson hard-IRQ handler from the supplied
patched kernel tree. It checks 3552 combinations of SDIO/non-SDIO commands,
copy requirements, command chains, CRC/timeout/data errors and combined SDIO
interrupts, plus no-interrupt and SDIO-only cases under ASan/UBSan. Real-board
traces and same-core schedutil/IRQ latency tests are still required: these
stubs cannot validate hardware timing or the kernel scheduler.
