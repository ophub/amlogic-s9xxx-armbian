# W103D scan connection probes and TX bitrate reporting

Validated on 2026-09-07 with the factory 2021 N9 firmware and
`6.18.49-ophub`. This follows the earlier BSS RLM scan repair. Two separate
problems remained: a real disconnect after scanning, and minimum-rate
connection probes appearing as the connection's payload TX bitrate in KDE.

## Disconnect trigger and driver correction

An unmodified-driver trace captured a scan starting at boottime 403.044338
and completing at 406.924615. `ieee80211_mgd_probe_ap(beacon=0)` ran at
406.934671. Two QoS Null frames (`fc=0x01c8`) reported ACK=false at
406.936038 and 406.938328, followed by
`ieee80211_sta_connection_lost(reason=4, tx=0)` at 406.938357. The station
dropped from state 4 to 0 and associated with the same SSID's 2.4 GHz BSS
at 407.493. The previous 16 scan probes in that trace had ACK=true.
This establishes the disconnect trigger; ACK=false alone does not distinguish
an over-the-air failure from incorrect firmware TX status.

The W103D explicit 5 GHz TXWI workaround ignored
`IEEE80211_TX_CTL_USE_MINRATE` and classified Null/QoS Null as ordinary data.
Connection probes could inherit Minstrel payload rates/retries, aggregation
and HT/VHT coding. The correction:

- Uses `ieee80211_is_data_present()` for payload and BA eligibility.
- Gives management/control/Null and minimum-rate requests mandatory 5 GHz
  6 Mbps OFDM with eight retries and no BA.
- Enables STBC/LDPC only when the selected rate is HT/VHT.
- Preserves ordinary payload rate adaptation, Minstrel sampling, bandwidth
  and spatial streams.

With the candidate enabled, over 60 scans produced no QoS Null ACK failures
or disconnects and retained the same 5 GHz association. The connection's
band was unrestricted during this test. The final driver removes the
diagnostic switch. Four subsequent explicit scans completed with 21, 23,
26 and 26 BSS results; one additional EBUSY request is not counted as a scan.

## Independent mac80211 reporting correction

After the driver correction, a transient 6 Mbps display still occurred
without a disconnect. In `net/mac80211/tx.c`, the software rate-control path
updated `sta->deflink.tx_stats.last_rate` for `ieee80211_is_tx_data()`;
that helper includes Null/QoS Null. Minimum-rate connection probes could
therefore replace the reported payload rate. NetworkManager reads
`NL80211_STA_INFO_TX_BITRATE`, and KDE displays NetworkManager's Bitrate.

Patch 0007 updates this statistic only for payload-bearing frames or hardware
encapsulation, including the explicit reported-rate path. Actual low-rate
payload still updates the statistic. The patch does not fix the transmit
rate to a constant, alter ACK decisions or modify KDE. A user also reported
1 Mbps, but that value was not captured synchronously; this test does not
establish 1 Mbps CCK transmission on 5 GHz.

## Final verification and limits

Both final modules were built with
`ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- LOCALVERSION=-ophub`.
Before installation, each vermagic's first field strictly matched the live
`uname -r`, `6.18.49-ophub`; MT7663S dependencies and SDIO alias were checked.
Normal reboot loaded the modules, and live build IDs matched the artifacts:

| Module | GNU build ID |
| --- | --- |
| mt7663s.ko | `503c2a3ea8f976c5f7ad200eb9faa883b66fd4df` |
| mac80211.ko | `baf295c457cd001d9f716255db9f0c3df91fad8f` |

The final 125.498-second window collected 240 samples: all `iw` TX rates
were 866.7 Mbps, all NetworkManager rates were 866700 Kb/s, and frequency
remained 5220 MHz with the same BSSID. All 24 association-time snapshots
matched. Trace recorded 16 scan starts/completions and zero station state
changes; 20 scan requests are not claimed as 20 completed scans. Bluetooth
mouse connectivity remained intact, and the user reported no further
low-rate transient in that observation window.

The local saved connection was subsequently restricted to 5 GHz, without
locking a BSSID. This is a site preference, not a shipped SSID configuration;
the earlier candidate stability result was obtained without that restriction.

Actual scanning packet loss remains unresolved. The candidate's 12-minute
gateway ping received 2818 of 3600 packets (21.7222% loss). Later windows
with the KDE panel closed still contained scans; none establishes a fully
trace-confirmed scan-free baseline. The reporting patch does not repair
this loss, and these finite results are not a long-duration guarantee.

The real TXWI encoder/classifier and mac80211 reporting block passed native
ASan/UBSan regressions, including real low-rate payload reporting. Both complete
seven-patch series passed clean-worktree application, review-copy consistency,
whitespace, MMC IRQ and TX reporting checks against the pinned 6.12.107 and
6.18.49 sources. This follow-up built and deployed only 6.18 modules; it does
not claim a new 6.12 module build or hardware test, or a rebuilt burning image.

An earlier live module reload timed out firmware exit (`0xef`, `-110`), then
failed Bluetooth ownership and Wi-Fi probe with `Bluetooth setup is not ready`.
Restoring the original module on disk did not restore the interface until
reboot. Subsequent deployments used normal reboot and retained original
modules for rollback. Test guards, timers and trace instances were removed
after verification. Earlier successful reload tests do not establish recovery
from this shared firmware failure.
