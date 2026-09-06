# W103D MT7663 firmware lifecycle

Patch `0005-mt7663-w103d-combo-lifecycle.patch` installs these two headers and
the Bluetooth hooks. Patch 0003 contains their isolated MT7663S consumer.
Build and deploy `btmtksdio.ko` and `mt7663s.ko` together, with
`CONFIG_MT7663S_W103D_COMBO=y` (default when the two SDIO drivers are enabled
with compatible built-in/module settings). There is no separate combo module.

On W103D, Bluetooth publishes a reference-counted state object for its SDIO
card. Wi-Fi defers probe until that supplier is bound, then waits at most
20 seconds for HCI initialization to reach `post_init`, without holding the
SDIO host. Firmware `setup` success alone is insufficient: the following HCI
initialization still reads BD_ADDR through the shared eFuse engine and must
finish before Wi-Fi reads its EEPROM. Setup failure wakes waiters immediately;
an HCI initialization failure after setup remains bounded by the timeout.
The timeout is a failure bound, not an initialization delay. A failed setup
does not authorize Wi-Fi initialization. A new Bluetooth bind creates a new
completion; it never inherits a previous driver's success.

A per-card mutex serializes Bluetooth setup/shutdown with Wi-Fi probe, MCU
initialization and removal. Wi-Fi removal waits for its asynchronous MCU work,
unregisters mac80211 with the transport still alive, requests firmware restart,
and only then stops its watchdog/workers, releases IRQ and disables function 1.
The interrupt initialized flag remains set until firmware exit finishes.

The symbol dependency loads Bluetooth before `modprobe mt7663s` and prevents
unloading `btmtksdio` while the Wi-Fi module still references it. A persistent
managed device link orders SDIO unbind and system PM; rebinding Bluetooth also
requests a new Wi-Fi probe. Both features are kernel mechanisms and need no
blacklist, startup service, `dmesg` polling or userspace sleeps. Normal network
association/DHCP and Bluetooth policy remain userspace responsibilities.

Typical reload: `modprobe -r mt7663s` followed by `modprobe mt7663s`.
The module loader handles unused dependencies. `rmmod mt7663s` can reload
only Wi-Fi while retaining Bluetooth. Forced module removal is unsupported.
This does not implement recovery from arbitrary firmware crashes or reset the
entire SDIO card while another function is active.

Run the actual coordinator's native failure/lifetime/serialization regression:

```sh
python3 compile-kernel/tools/mt76/tests/test_combo.py
```

The pthread harness substitutes kernel primitives and device-link APIs. It
does not simulate driver-core binding, SDIO timing or Bluetooth firmware;
real-board load-order, unbind/rebind, traffic and boot tests remain required.
All board modules require explicit `LOCALVERSION=-ophub` and exact equality
between their vermagic release and the target's `uname -r` before deployment.
