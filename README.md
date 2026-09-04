> This repo is a fork of https://github.com/ophub/amlogic-s9xxx-armbian to be able to build the older versions of the kernel. Let me know if you need some other version that is not pre-built and i will make a release for it as well.

<details>
<summary> Full walkthrough for flashing an Android Box <i>Amlogic S905X3</i> into a self-hosted Home Assistant + Zigbee hub.</summary>

# Android TV Box → Armbian + Home Assistant + Zigbee2MQTT

A start-to-finish guide for converting a low-cost Amlogic Android TV box into a
self-hosted Home Assistant + Zigbee hub running Armbian Linux.

This guide documents a completed installation on the hardware listed below and records the
problems encountered along the way, so that they can be avoided on repeat. The **⚠️ notes**
mark the steps most likely to cause trouble and are worth reading first.

---

## Scope

1. Obtaining an Armbian image with a *working* kernel
2. Writing it to a USB stick from macOS
3. Booting the box and installing to internal storage (eMMC)
4. Stability notes (this class of hardware is marginal — see Part 5)
5. Installing Home Assistant
6. Installing Mosquitto + Zigbee2MQTT
7. Pairing devices and making a button control a plug

## Hardware used (substitute equivalents as needed)

- **Box:** A95X F3 Air — **Amlogic S905X3**, 4 GB RAM, ~64 GB eMMC, 100 Mbps Ethernet
- **Zigbee coordinator:** SONOFF **ZBDongle-P** (TI CC2652P chip, Z-Stack firmware)
- **Build/flash host:** macOS in this guide (Linux/Windows also work; commands differ)
- **A USB flash drive** (8 GB+). A microSD is possible but proved less reliable — see Part 2.

> If the box is a different Amlogic model, the **board name** (Part 1) and **device tree
> (DTB)** will differ. The remainder of the procedure is unchanged.

---

## ⚠️ Most important point

The newest ophub kernels (**6.18.x** at the time of writing) were found to **kernel-panic**
on this box — frequent crashes reporting
`stack-protector: Kernel stack is corrupted in rcu_sched_clock_irq`.

The **6.6 LTS kernel is stable** where 6.18 is unusable. All steps below assume a 6.6 image.

---

## Part 1 — Obtain the Armbian image

Two options. Option A is fastest; Option B produces a fresh build.

### Option A — use the pre-built image (fastest)

A pre-built 6.6 image is published in this repository's **Releases** section. Download the
`.img.gz` and proceed to Part 2.

> If the release is missing or has aged out, use Option B to rebuild one.

### Option B — build the image (GitHub Actions, free)

The **ophub/amlogic-s9xxx-armbian** project is used. Building runs on GitHub's free Linux
runners and costs nothing on a public fork.

1. Create a **GitHub account** if needed.
2. Fork <https://github.com/ophub/amlogic-s9xxx-armbian> (the **Fork** button, top-right). A
   fork of a public repository is itself public, which is what makes the build free.
3. In the fork → **Actions** tab → click the **"I understand… enable workflows"** banner if
   shown.
4. Open the **"Build Armbian arm64 server image"** workflow → **Run workflow**.
5. Set the inputs:
   - **Select OS Release:** `bookworm`
   - **Select device board:** `s905x3-a95xf3`  *(dedicated A95X F3 target; the correct DTB is
     included automatically)*
   - **Select kernel version:** `6.6.y`  ← **not 6.18**
   - Leave the remainder at defaults (`ophub/kernel`, `stable`, `ext4`, `server`).
6. Run it. Build time is roughly **40–90 minutes** (the rootfs is compiled from source).
7. On completion the image is published to the fork's **Releases** tab. Download the
   `.img.gz`; the filename resembles
   `Armbian_..._amlogic_s905x3-a95xf3_bookworm_6.6.x_server_....img.gz`.

> **Board note:** a `s905x3-a95xf3-gb` option also exists; `-gb` appears to denote a
> gigabit-Ethernet variant. This box is 100 Mbps, so the plain `s905x3-a95xf3` was used. If
> Ethernet is absent after first boot, this is the first parameter to reconsider.

---

## Part 2 — Write the image to a USB stick (macOS)

A **USB flash drive is recommended over a microSD card.** During this installation, two
separate SD cards / readers failed mid-write (`dd: Device not configured`, and balenaEtcher's
writer process terminating) — behaviour consistent with a failing card or a flaky reader. A
USB stick avoids both failure modes.

### Option A — balenaEtcher (simplest)

1. Install from <https://etcher.balena.io> and open it.
2. **Flash from file** → the downloaded `.img.gz` (Etcher decompresses it automatically).
3. **Select target** → the USB stick (verify it is not another disk).
4. **Flash!** → supply the macOS password when prompted. Allow it to flash **and** validate.
5. If macOS reports "disk not readable" at the end, click **Ignore**, then eject.

### Option B — command line (robust; the method used here)

```bash
cd ~/Downloads

# 1. Verify the archive is not corrupt (should print "OK")
gunzip -tv Armbian_*.img.gz

# 2. Decompress to a raw .img (-k keeps the compressed copy)
gunzip -k Armbian_*.img.gz

# 3. Identify the USB stick — look for an "external, physical" disk
diskutil list

# 4. Unmount it (substitute diskN, e.g. disk4)
diskutil unmountDisk /dev/diskN

# 5. Write it — note the 'r' in rdiskN (raw device; substantially faster)
sudo dd if=Armbian_*.img of=/dev/rdiskN bs=4m
#   (no output during the write; press Ctrl+T to print progress)

# 6. Flush and eject
sync && diskutil eject /dev/diskN
```

> **⚠️ A write that dies with `Device not configured`, or that crawls at ~2 MB/s**, indicates
> a failing card/reader rather than a bad image. Switch media. A healthy device writes at
> 20–30+ MB/s.

---

## Part 3 — First boot

Amlogic boxes must be instructed to boot from external media via a hidden reset button.

1. Power off and **unplug** the box.
2. Insert the USB stick.
3. Locate the reset button — on the A95X F3 Air it is reported to be **inside the AV /
   headphone jack** (pressed with a toothpick). *(If unresponsive, a pinhole button may be
   present on the underside.)*
4. **Hold the button**, apply power **while still holding**, continue holding ~8–10 s, then
   release.
5. Armbian should boot from the USB stick.

> If an **Android recovery** menu appears instead, no valid boot media was found (a failed
> write, or the button released too early). Select **Power off** — do not choose any recovery
> action — and revisit Part 2.

**Login:** the default is commonly `root` / `1234`, with a forced password change on first
login. *(This detail is unverified from documentation; the on-screen first-boot prompt is
authoritative.)*

**Confirm the boot source is the USB stick, not the internal eMMC:**

```bash
lsblk
```

- The **eMMC** appears as `mmcblk2`, identifiable by its `mmcblk2boot0` / `mmcblk2boot1`
  siblings (capacity varies by model — commonly 32 GB or 64 GB).
- The **USB stick** appears as `sda`. If root (`/`) is mounted from `sda`, the stick was
  booted.

---

## Part 4 — Install to internal storage (eMMC)

Running from USB works, but installing to eMMC makes the system permanent and allows booting
without the stick.

```bash
sudo armbian-install
```

- At the device list, select the **A95X F3** entry (DTB `meson-sm1-a95xf3-air.dtb`).
- For the filesystem type, select **ext4**.
- Allow it to complete (a few minutes): eMMC is formatted, the system copied, and u-boot
  written.

On completion: **power off, remove the USB stick, power on normally** (the reset-button trick
is only for external boot). The system boots from eMMC.

> **⚠️ `armbian-update` must not be run.** It fetches the newest kernel (6.18.x) and
> reintroduces the crashes. If a kernel update is ever required, it must be pinned:
> `armbian-update -k 6.6.y`. Ordinary `apt upgrade` is safe — it does not touch the ophub
> kernel.

---

## Part 5 — Stability notes (⚠️ marginal hardware)

The dominant cause of instability on this box is the kernel version. Running the **6.6 LTS
kernel** (Part 1) eliminated the frequent panics observed under 6.18; this is the single most
important measure.

A secondary, load-dependent class of crash was observed on 6.6 after Home Assistant and the
Zigbee dongle were added. Two contributing factors are plausible: aggressive CPU frequency
scaling toward an unstable top operating point, and marginal power delivery from the stock
supply once a USB dongle draws additional current.

Possible mitigations, in order of expected effectiveness:

- **A higher-quality power supply** rated **5 V / 3 A or more** for the box. ⚠️ It must match
  the required **voltage, polarity, and barrel size** printed on the unit — an incorrect
  supply can destroy the box.
- **A powered USB hub with a short extension cable** for the Zigbee dongle. This offloads the
  dongle's current draw from the box and, as a secondary benefit, moves the 2.4 GHz radio
  away from the HDMI/USB3 ports, whose emissions degrade Zigbee range.
- **Capping the maximum CPU frequency** via `/etc/default/cpufrequtils` (the `MAX_SPEED`
  value; `1800000` was chosen, a value below the top of the `scaling_available_frequencies`
  list). **Note:** in this installation, editing that file produced no observable effect, and
  the matter was not pursued further because the system remained stable in practice. It is
  recorded here as a possible avenue rather than a confirmed fix.

In this installation, no powered hub was obtained; the dongle was connected directly to the
box, and the system nonetheless operated stably during subsequent use. The stability of this
hardware class should still be regarded as marginal, and a solid power supply should be
prioritised for any continuous (24/7) deployment.

---

## Part 6 — Install Home Assistant

**Home Assistant Supervised** is used (full add-on/app store; feature-equivalent to HAOS).

```bash
armbian-config --cmd HAS001
```

Then:

1. Wait for the containers to be pulled (first boot: **10–20 minutes**). Progress can be
   observed with `docker ps` until a `homeassistant` container is running and remains up.
2. Determine the box IP: `ip -4 addr show eth0 | grep inet`
3. Open **`http://<box-ip>:8123`** and complete onboarding (the administrator account is
   created at this step).

> **Expected warnings:** the system will be flagged **"unsupported"**, and possibly
> **"unhealthy"**, under Settings → System. This is normal — the Supervised method is not
> officially supported by Home Assistant, and TV-box hardware is likewise unofficial.
> Functionality is unaffected. (Backups are stored under `/armbian/haos`.)

> **⚠️ Interface naming:** recent Home Assistant releases renamed the **"Add-ons"** section to
> **"Apps"**, and moved **Developer Tools** to **Settings → Tools**. Where older material
> says "Add-ons," read "Apps."

---

## Part 7 — Mosquitto broker + Zigbee2MQTT

### 7a. Install Mosquitto

Settings → **Apps** → **App Store** → install **Mosquitto broker** → **Start** it. Home
Assistant normally auto-detects it and offers to configure the **MQTT integration** — accept.

### 7b. Create a dedicated MQTT user

The broker requires authentication, and **every client must use valid credentials** or it
silently fails to connect. (This was a significant source of difficulty during this
installation: button presses reached the broker but not Home Assistant, because the
credentials did not match.)

Settings → **People** → **Users** tab → **Add user**:

- Name: `mqtt`
- Password: a recorded value
- Type: **Local access only** (a service account requires no administrator rights and still
  authenticates for LAN MQTT).

### 7c. Add the Zigbee2MQTT repository and install it

Zigbee2MQTT is **not** in the default store; its repository must be added first:

1. App Store → **⋮ (top-right)** → **Repositories**
2. Add: **`https://github.com/zigbee2mqtt/hassio-zigbee2mqtt`**
3. Install **Zigbee2MQTT** (the stable variant, not "Edge").

### 7d. Configure Zigbee2MQTT

On first start, Zigbee2MQTT presents an **onboarding wizard** (reached via the app's **Open
Web UI**).

- **Devices found:** select the entry resembling
  `usb-1a86_USB_Serial-if00-port0 (1a86), /dev/ttyUSB0` — the ZBDongle-P. *(The `/dev/ttySx`
  entries are the box's internal serial ports and should be ignored.)*
- **Serial → port:** `/dev/ttyUSB0`
- **Serial → adapter:** **`zstack`** ← **critical.** Auto-detection **fails** on a generic
  serial chip; the type must be specified. The ZBDongle-P (CC2652P) runs Z-Stack, hence
  `zstack`.
- **MQTT:** leave blank — with the Mosquitto add-on in use, Zigbee2MQTT connects
  automatically. If explicit values are requested, use broker `core-mosquitto`, port `1883`,
  and the `mqtt` credentials.
- **Network** (PAN ID / Extended PAN ID / Network key): for a **new** network, retain the
  random defaults. *(These must instead match the old values only when migrating an existing
  Zigbee network, in which case "Restore mode" is also used.)*
- Optionally set the Zigbee **channel** to avoid Wi-Fi interference (Advanced tab; 15/20/25
  are clean choices). Changing it later requires re-pairing all devices, so it should be
  decided now.

Save and start. A healthy **Log** tab shows `zigbee-herdsman started`, coordinator/firmware
details, and `Connected to MQTT`.

> **⚠️ "No valid USB adapter found"** in the log indicates a missing/incorrect `adapter`
> type. Set `adapter: zstack` explicitly.

> **Recommendation — use a stable port path.** Once operational, replace `/dev/ttyUSB0` with
> the `by-id` path so that re-enumeration on reboot cannot rename it:
> ```bash
> ls -l /dev/serial/by-id/
> ```
> Use the resulting `/dev/serial/by-id/usb-1a86_...` string as the port.

### 7e. Point the MQTT integration at the `mqtt` user

Settings → Devices & Services → **MQTT** → Configure (or remove and re-add): broker
`core-mosquitto`, port `1883`, username `mqtt`, the recorded password. The discovery prefix
remains `homeassistant` (default).

---

## Part 8 — Pair devices and automate

### Pairing

1. In the Zigbee2MQTT web UI, click **Permit join (All)**.
2. Place a device in pairing mode (device-specific — often a ~5 s button hold; a new bulb may
   require 3–5 power cycles).
3. The device appears in Zigbee2MQTT, is interviewed, and receives controls. **Rename it
   immediately** — the name propagates to Home Assistant, and later renaming is awkward.
4. Turn **Permit join off** when finished.

> **Plugs appear in Home Assistant automatically; buttons frequently do not** — a button has
> no persistent state, so no dashboard card is auto-created. It remains connected and usable
> in automations. This is expected behaviour.

### Making a button toggle a plug

Buttons emit momentary MQTT **actions** (`single`, `double`, `hold`), which are captured with
an **MQTT trigger** automation (a "device" trigger does not reliably catch momentary
actions).

First, confirm the emitted value: in the Zigbee2MQTT dashboard, press the button and observe
the `action` value (e.g. `single`).

Then Settings → **Automations** → Create → **⋮ → Edit in YAML**. The following configuration
was found to work:

```yaml
alias: Click toggles plug
description: ''
triggers:
  - trigger: mqtt
    topic: zigbee2mqtt/button-room-plug
    value_template: "{{ value_json.action }}"
    payload: single
    alias: When button is single clicked
conditions: []
actions:
  - action: switch.toggle
    target:
      entity_id: switch.plug_room_light
mode: single
```

Common errors that prevent this from firing:

- **`topic` and `payload` must sit directly on the trigger** — they must not be nested under
  an `options:` key (the YAML editor sometimes inserts one; remove it).
- **`value_template: "{{ value_json.action }}"` is required** — the message is JSON
  (`{"action":"single",...}`), so without the template a bare `payload: single` never
  matches.
- **The plug's real entity ID must be confirmed:** Settings → Devices → *the plug* → its
  switch entity → copy the exact ID. It may not be `switch.plug_room_light`.

Save and press the button. For diagnosis: **⋮ → Traces** on the automation (a trace indicates
the trigger fired; no trace indicates a non-matching trigger), and **Settings → Tools → MQTT
→ Listen** on `zigbee2mqtt/<button>` confirms whether Home Assistant receives the action.

---

## Troubleshooting summary (issues encountered during this installation)

| Symptom | Cause | Resolution |
|---|---|---|
| Constant kernel panics (`rcu_sched_clock_irq`) | Kernel 6.18 on this box | Use **6.6 LTS** (Part 1). Never run `armbian-update`. |
| `dd`: `Device not configured`, ~2 MB/s writes | Failing SD card / reader | Use a **USB stick** |
| Boots into Android recovery | No valid boot media found | Re-flash; hold the reset button longer |
| Crashes under load | Marginal power / CPU frequency | Good 5 V/3 A PSU + powered hub (frequency capping was attempted but produced no observable effect here) |
| Zigbee2MQTT: "No valid USB adapter found" | adapter type not set | Set **`adapter: zstack`** |
| MQTT: "Connection Refused: not authorised" | Broker requires credentials | Create an **`mqtt` user** and use it for every client |
| Button works in Zigbee2MQTT but not in Home Assistant | Malformed MQTT trigger / wrong entity ID | Use the YAML above; verify the entity ID |
| "Add-ons" / "Developer Tools" not found | Renamed in recent Home Assistant | Look under **"Apps"** and **Settings → Tools** |

---

## Condensed procedure

1. Obtain a 6.6 image — either from this repository's **Releases**, or by forking
   ophub/amlogic-s9xxx-armbian and running **Build Armbian arm64 server image** with
   `bookworm` / `s905x3-a95xf3` / **`6.6.y`**.
2. Write the `.img` to a **USB stick** (not SD) with `dd`.
3. Boot via the reset-button trick → `armbian-install` to eMMC → **never `armbian-update`**.
4. For stability, prioritise a good power supply (and, if needed, a powered hub).
5. `armbian-config --cmd HAS001` → onboard at `:8123`.
6. Install **Mosquitto**, create an **`mqtt` user** (Local access only), add the
   **zigbee2mqtt/hassio-zigbee2mqtt** repository, install **Zigbee2MQTT**.
7. Zigbee2MQTT: port `/dev/ttyUSB0`, adapter **`zstack`**, MQTT blank (auto). MQTT integration
   → `core-mosquitto:1883` with the `mqtt` user.
8. Pair devices; automate buttons with an **MQTT trigger** using `value_json.action`.
</details>

<details>
<summary>The readme from the original repo</summary>

<div align="center">
    <img alt="Armbian" src="https://github.com/user-attachments/assets/74e55052-031b-48f8-9aca-e5f1dd9e256a" />
</div>

# Armbian

[English Instructions](README.md) | [中文说明](README.cn.md) | [日本語説明](README.ja.md)

Armbian is a lightweight Linux distribution built specifically for ARM chips, based on Debian/Ubuntu. The Armbian system is lean, clean, and 100% compatible with Debian/Ubuntu, inheriting its functionality and rich software ecosystem. It runs securely and stably on TF/SD/USB storage and the device's eMMC. This project preserves the integrity of the official Armbian system while extending support for unofficially supported devices such as TV boxes, and adds a set of convenient management commands. You can now replace the Android TV system on your TV box with Armbian, transforming it into a powerful server.

This project relies on many [contributors](CONTRIBUTORS.md) to build the Armbian system for `Amlogic`, `Rockchip`, and `Allwinner` devices. It supports writing to eMMC, kernel updates, and other features. For detailed usage, see the [📚Armbian User Documentation](./documents). The latest Armbian system can be downloaded from [⬇️Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases). Welcome to `Fork` and customize. If this project is helpful, please click the `⭐Star` button in the upper right corner to show your support.

## Default Information for Armbian System

| System Name    | Default Username | Default Password  | SSH Port  | IP Address  |
| -------------- | ---------------- | ----------------- | --------- | ----------- |
| 🐧 [Armbian.OS](https://github.com/ophub/amlogic-s9xxx-armbian/releases) | root | 1234 | 22 | Obtain from router |
| 🐋 [Armbian.Docker](https://hub.docker.com/u/ophub) | root | 1234 | 22 | Static MacVLAN IP |

## Supported Devices

⬆️ Models from each platform (Amlogic/Rockchip/Allwinner) are ranked by SoC performance from high to low.

| SoC | [Device](https://github.com/ophub/amlogic-s9xxx-armbian/releases) | [Kernel](https://github.com/ophub/kernel) |
| ---- | ---- | ---- |
| a311d | [Khadas-VIM3](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/99), [WXY-OES](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2666) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s922x | [Beelink-GT-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/370), [Beelink-GT-King-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/707), [Ugoos-AM6-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/464), [ODROID-N2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/201), [X88-King](https://github.com/ophub/amlogic-s9xxx-armbian/issues/988), [Ali-CT2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1150), [WXY-OES-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3029) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905x3 | [X96-Max+](https://github.com/ophub/amlogic-s9xxx-armbian/issues/351), [HK1-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/414), [Vontar-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1006), [H96-Max-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1250), [Ugoos-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/782), [TX3(QZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/644), [TX3(BZ)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1077), [X96-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/366), [X96-Max+_A100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/779), [A95X-F3-Air](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2282), [Tencent-Aurora-3Pro(s905x3-b)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/506), [X96-Max+Q1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/788), [X96-Max+100W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/909), [X96-Max+_2101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1086), [Infinity-B32](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1181), [Whale](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1166), [X88-Pro-X3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [X99-Max-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [Transpeed-X3-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1621), [TOX1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3441), [Khadas-VIM3L](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3482) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905x2 | [X96Max-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/453), [X96Max-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/95), [MECOOL-KM3-4G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/79), [Tanix-Tx5-Max](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/351), [A95X-F2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/851), [HG680-FJ](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3089) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l3a | [E900V22C/D](https://github.com/Calmact/e900v22c), [CM311-1a-YST](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/732), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/517), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [UNT413A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/970), [ZTE-B863AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/741), [CM311-1a-CH](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1508), [IP112H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1520), [B863AV3.1-M2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2292) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l3b | [CM201-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2209), [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1180), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1268), [E900V21D](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2447), [E900V22D](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1256), [E900V21E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1514), [E900V22E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/939), [M302A/M304A](https://github.com/ophub/amlogic-s9xxx-armbian/pull/615), [Hisense-IP103H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1154), [TY1608](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1332), [TY1608](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1568), [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1613), [B860AV-2.1M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1598), [UNT403A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1712), [RG020ET-CA](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1860), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3272) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l3 | [CM211-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1318), [CM311-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/763), [HG680-LC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/978), [M401A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921#issuecomment-1453143251), [UNT400G1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1277), [UNT400G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2625), [UNT402A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1481), [ZXV10-BV310](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1512), [M411A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1817), [ZXV10-B860AV3.2-M](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2012), [ZXV10-B860AV2.1-U](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2273), [E900V22D-2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2058), [CM201-1-6-YS](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2539), [IP108H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2539), [M301A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3055), [B860AV2.1-A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3484) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s912 | [Tanix-TX8-Max](https://github.com/ophub/amlogic-s9xxx-armbian/issues/500), [Tanix-TX9-Pro(3G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/315), [Tanix-TX9-Pro(2G)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/740), [Tanix-TX92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1012790770), [Tanix-TX9S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3282), [Nexbox-A1](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [Nexbox-A95X-A2](https://www.cafago.com/en/p-v2979eu-2g.html),  [A95X](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/260), [H96-Pro-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72#issuecomment-1013071513), [VORKE-Z6-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/72), [Mecool-M8S-PRO-L](https://github.com/ophub/amlogic-s9xxx-armbian/issues/158), [Vontar-X92](https://github.com/ophub/amlogic-s9xxx-armbian/issues/525), [T95Z-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/668), [Octopus-Planet](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1020), [Phicomm-T1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/522), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1882), [OneCloudPro-V1.1_V1.2](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2241) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905d | [MECOOL-KI-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/59), [Phicomm-N1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/925), [SML-5442TW](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/451) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905x | [HG680P](https://github.com/ophub/amlogic-s9xxx-armbian/issues/262), [B860H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/60), [TBee-Box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/98), [T95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/285), [TX9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/645), [XiaoMI-3S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1405), [X96](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1480), [Nexbox-a95x](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1714), [BTV9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3256) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905mb | [S65](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1644) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l | [UNT402A](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1481), [M201-S](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/444), [MiBox-4](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2101), [MiBox-4C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1826), [MG101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1912), [E900V21C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2341), [IP108H-53u1m](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2357), [Tencent-Aurora-1s](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2465), [B860AV2.1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2491), [B860AV2.1U](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2499), [HM201](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2585), [XiaoMI-4C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3520) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905l2 | [MGV2000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [MGV2000-K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1839), [MGV3000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/921), [Wojia-TV-IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/issues/648), [M301A](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/405), [E900v21E](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1278), [e900v21d](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2127), [CM201-1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2188), [IP108H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2598), [MGV2000-CW](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2616) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905lb | [Q96-mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/734), [BesTV-R3300L](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993), [SumaVision-Q7](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1190), [MG101](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1570), [s65](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2128), [IPBS9505](https://github.com/ophub/amlogic-s9xxx-armbian/pull/993#issuecomment-2276804591) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905w | [X96-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/621), [TX3-Mini](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1062), [W95](https://github.com/ophub/amlogic-s9xxx-armbian/issues/570), [X96W/FunTV](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1044), [MXQ-Pro-4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1140), [MeCool-m8s-pro-W](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3245) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| s905 | [Beelink-Mini-MX-2G](https://github.com/ophub/amlogic-s9xxx-armbian/issues/127), [Sunvell-T95M](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/337), [MXQ-Pro+4K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/715), [SumaVision-Q5](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1175) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| rk3588(s) | [Radxa-Rock5B](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1240), [Radxa-Rock5C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2324), [Orange-Pi-5-Plus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2400), [Orange-Pi-5-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3518), [Orange-Pi-5-Ultra](https://github.com/ophub/fnnas/pull/589), [Beelink-IPC-R](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/415), [HLink-H88K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H88K-V3](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [NanoPC-T6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2453), [Smart-Am60](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2817), [DC-A588](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2988), [Orangepi-5B](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3052), [CM3588-NAS](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3306), [Rock-5-ITX](https://github.com/ophub/fnnas/issues/355), [LZ-D3588](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3328), [Boca-tcn100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3437), [Boca-tcn200](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3461), [Firefly-ITX-3588J](https://github.com/ophub/fnnas/issues/194), [Indiedroid-Nova](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3499), [LubanCat-4](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3507), [Seewo-SV50](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3517), [EasePi-R2](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3525), [BDY-G98](https://github.com/ophub/fnnas/issues/606), [Youyeetoo-YY3588](https://github.com/ophub/fnnas/issues/633), [Youyeetoo-R1-V3](https://github.com/ophub/fnnas/issues/633), [ELFboard-elf2](https://github.com/ophub/fnnas/issues/634) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)<br />[rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) |
| rk3576 | [NanoPi-m5](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3207), [LCKFB-Taishan-Pi-3M](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3470), [reComputer-rk3576-Devkit](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3588), [LubanCat-3](https://github.com/ophub/fnnas/issues/573) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)<br />[rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3399 | [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132), [ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247), [CRRC](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1280), [SMART-AM40](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1317), [SW799](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1326), [ZYSJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1380), [DG-3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1492), [DLFR100](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1522), [Emb3531](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1549), [Leez-p710](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1609), [tvi3315a](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1687), [xiaobao](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1698), [Fine3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1790), [Firefly-RK3399](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/491), [LX-R3S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2026), [Hugsun-x99](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2050), [Tb-ls3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2146), [Hisense-hs530r](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/572), [Tpm312](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2403), [ZK-rk39a](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2446), [YSKJ](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2673), [Fmx1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2691), [Fmx1-Pro-B](https://github.com/ophub/fnnas/issues/250), [Sv-33a6x](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/748), [Sv-33a6x(VPU)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3372), [AIO-3399B](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3185), [AIO-3399C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3339), [AIO-3399C(AI)](https://github.com/ophub/fnnas/issues/108), [TaraM](https://github.com/ophub/u-boot/pull/28), [NanoPC-T4](https://github.com/ophub/u-boot/pull/30), [Firefly-Core-3399-JD4](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3354), [GEA-6319](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3383), [NanoPi-SOM-RK3399](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3573), [SZ3390](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3644), [H339PC](https://github.com/ophub/fnnas/issues/647) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)<br />[rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3568 | [FastRhino-R66S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1061), [FastRhino-R68S](https://github.com/ophub/amlogic-s9xxx-armbian/issues/774), [Radxa-E25](https://wiki.radxa.com/Rock3/CM/CM3I/E25), [NanoPi-R5S](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217), [NanoPi-R5C](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1217), [HLink-H66K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H68K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [HLink-H69K](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1726), [Seewo-sv21](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2017), [Mrkaio-m68s](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2155), [Swan1-w28](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2407), [Ruisen-box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2508), [DG-TN3568](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2661), [Alark35-3500](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2911), [MMBox-Anas3035](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2995), [Wocyber-A3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2978), [Photonicat](https://github.com/ophub/amlogic-s9xxx-openwrt/pull/827), [NSY-G16-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/845), [NSY-G68-Plus](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/845), [BDY-G18-Pro](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/847), [Gzpeite-P01](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3221), [LZ-D3568](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3387), [LZ-K3568](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3304), [BDKJ-One](https://github.com/ophub/u-boot/pull/29), [Station-P2](https://github.com/ophub/fnnas/pull/350), [Lyt-t68m](https://github.com/ophub/fnnas/issues/435), [LubanCat-2](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3488), [Rock-3B](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3518), [EasePi-A2](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3525), [Roceos-K40Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3530), [DG-Nas-Lite](https://github.com/ophub/fnnas/issues/8), [Houzzkit-F1](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3625), [Youyeetoo-YY3568](https://github.com/ophub/fnnas/issues/633) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)<br />[rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3566 | [Panther-X2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1319), [JP-TvBox](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1867), [LCKFB-Taishan-Pi](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2538), [WXY-OEC-turbo-4g](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2736), [Station-M2](https://github.com/ophub/amlogic-s9xxx-openwrt/issues/744), [Orange-Pi-3B](https://github.com/ophub/fnnas/issues/261), [X88Pro20](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3443), [LubanCat-1](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3488), [Rock-3C](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3521), [Inspur-MD1000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3503) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)<br />[rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3528 | [HLink-H28K](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1726), [Radxa-E20C](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2324), [H96-Max-M2](https://github.com/ophub/amlogic-s9xxx-armbian/issues/2404), [HK1-Rbox-K8S](https://github.com/ophub/fnnas/issues/464), [HT2](https://github.com/ophub/fnnas/issues/464), [CD1000](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3302), [P51(X88-Pro-13)](https://github.com/ophub/amlogic-s9xxx-armbian/pull/3597) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)<br />[rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3328 | [BeikeYun](https://github.com/ophub/amlogic-s9xxx-armbian/issues/852), [Chainedbox-L1-Pro](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1680), [Station-M1](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Bqeel-MVR9](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1313), [Renegade/Firefly](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1861) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable)<br />[rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) |
| rk3318 | [RX3318-Box](https://github.com/ophub/amlogic-s9xxx-armbian/pull/2129) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| h618 | [OrangePi-Zero3](https://github.com/ophub/fnnas/issues/158), [H618-DevBoard(PCDN)](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3479), [Vontar-h618](https://github.com/ophub/fnnas/issues/525), [BT-100M](https://github.com/ophub/fnnas/issues/525), [BT-1000M](https://github.com/ophub/fnnas/issues/525), [PaiNet-P3](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3485), [PaiNet-P5](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3577), [X98H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3434), [AI-Douhe](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3556), [CJDS](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3652) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| h6 | [Vplus](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1100), [Tanix-TX6](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3534), [TX6-H](https://github.com/ophub/amlogic-s9xxx-armbian/issues/3528), [T95-max](https://github.com/ophub/fnnas/issues/546), [TQC-A01](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1638) | [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable) |

> [!TIP]
> At present, the [s905 box](https://github.com/ophub/amlogic-s9xxx-armbian/issues/1173) can only be used with `TF/SD/USB`; other box models support writing to `EMMC`. For more information, refer to the [✅ Supported Device List](build-armbian/armbian-files/common-files/etc/model_database.conf). To add new device support, see Section 12.15 of the documentation: [✳️Adding New Supported Devices](documents/README.md#1215-how-to-add-new-supported-devices). Please read the [📚Armbian User Documentation](./documents) before use, as it provides solutions to common issues.

## Installation and Upgrade Instructions for Armbian

Choose the Armbian system that matches your device model. Refer to the corresponding documentation for device-specific usage instructions.

- ### Install Armbian to EMMC

1. For `Rockchip` platform devices, refer to [Chapter 8](documents#8-installing-armbian-to-emmc) of the documentation.

2. For `Amlogic` and `Allwinner` platform devices, use tools such as [Rufus](https://rufus.ie/) or [balenaEtcher](https://www.balena.io/etcher/) to write the system to a USB stick, then insert the USB stick into the device. Log in to the Armbian system (default user: root, default password: 1234) and enter the command:

```shell
armbian-install
```

| Optional | Default | Options | Description       |
| -------- | ------- | ------- | ----------------- |
| -m       | no      | yes/no  | Use mainline u-boot |
| -a       | yes     | yes/no  | Use [ampart](https://github.com/7Ji/ampart) partition adjustment tool |
| -l       | no      | yes/no  | Show full device list |

Example: `armbian-install -m yes -a no`

- ### Update Armbian Kernel

Log in to the Armbian system and enter the command:

```shell
# Run as root user (sudo -i)
# If no parameter is specified, it will be updated to the latest version.
armbian-update
```

| Optional | Default      | Options       | Description                      |
| -------- | ------------ | ------------- | -------------------------------- |
| -r       | ophub/kernel | `<owner>/<repo>` | Set the repository for downloading kernels from github.com |
| -u       | Automation   | stable/flippy/beta/rk3588/rk35xx | Set the kernel [tags suffix](https://github.com/ophub/kernel/releases) |
| -k       | Latest version | Kernel version | Set the [kernel version](https://github.com/ophub/kernel/releases/tag/kernel_stable) |
| -b       | yes          | yes/no        | Automatically back up the currently used kernel when updating |
| -d       | deb          | tar/deb       | Set the preferred kernel package format. If unavailable, the script will automatically try the alternative format. The `deb` format is recommended for compiling custom drivers. |
| -m       | no           | yes/no        | Use mainline u-boot |
| -s       | None         | None/DiskName | [SOS] Restore the system kernel on eMMC/NVMe/sdX or other disks |
| -h       | None         | None          | View help information |

Example: `armbian-update -k 5.15 -u stable -d deb`

When specifying the kernel version via the `-k` parameter, you can provide an exact version number (e.g., `armbian-update -k 5.15.50`) or specify only the kernel series (e.g., `armbian-update -k 5.15`). When a series is specified, the latest version within that series will be used automatically.

During kernel updates, the currently running kernel is automatically backed up to the `/ddbr/backup` directory, retaining the 3 most recent versions. If the newly installed kernel proves unstable, you can restore a backup kernel at any time. If a kernel update renders the system unbootable, use `armbian-update -s` to restore the system kernel. For more details, see the [Help Document](documents#10-updating-armbian-kernel).

- ### Replace Armbian Sources

Log in to the Armbian system and enter the command:

```shell
armbian-apt
```

Choosing the appropriate software source for your country or region can significantly improve download speeds. For more details, see the [Help Document](documents#11-installing-common-software).

- ### Install Common Software

Log in to the Armbian system and enter the command:

```shell
armbian-software
```

The command `armbian-software -u` updates the local software center list. Based on user feedback in [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues), commonly used [software](build-armbian/armbian-files/common-files/usr/share/ophub/armbian-software/software-list.conf) has been gradually integrated with one-click install/update/uninstall support. This includes `Docker images`, `desktop software`, `application services`, and more. See the [detailed instructions](documents/armbian_software.md).

- ### Modify Armbian Configuration

Log in to the Armbian system and enter the command:

```shell
armbian-config
```

- ### Create Swap for Armbian

If you find the device's memory insufficient when running memory-intensive applications such as `Docker`, you can create a `swap` virtual memory partition to use a portion of disk space as additional memory. The parameter unit is `GB`, with a default value of `1`.

Log in to the Armbian system and enter the command:

```shell
armbian-swap 1
```

- ### Control LED Display

Log in to the Armbian system and enter the command:

```shell
armbian-openvfd
```

Debug according to the [LED Screen Display Control Instructions](documents/led_screen_display_control.md).

- ### Backup/Restore EMMC Original System

Supports backing up and restoring the device's `EMMC` partition via `TF/SD/USB`. Before installing Armbian on a new device, it is recommended to back up the original Android TV system for future restoration if needed.

Boot the Armbian system from `TF/SD/USB` and enter the command:

```shell
armbian-ddbr
```

Enter `b` at the prompt to back up the system, or `r` to restore.

> [!IMPORTANT]
> Alternatively, the Android system can be flashed directly into eMMC via USB cable. Android system images are available in [Tools](https://github.com/ophub/kernel/releases/tag/tools).

- ### Compile the Kernel in Armbian

For kernel compilation instructions, see the [Compile Kernel](compile-kernel) documentation. Log in to the Armbian system and enter the command:

```shell
armbian-kernel -u
armbian-kernel -k 6.6.12
```

- ### More Usage Instructions

To update all service scripts in the system to the latest version, log in to the Armbian system and enter the command:

```shell
armbian-sync
```

For common issues and their solutions when using Armbian, see [documents](documents).

## Local Packaging

1. Clone the repository to local `git clone --depth 1 https://github.com/ophub/amlogic-s9xxx-armbian.git`

2. Install the necessary software packages (using Ubuntu 24.04 as an example)

Enter the `~/amlogic-s9xxx-armbian` root directory, then run the installation command:

```shell
sudo apt-get update -y
sudo apt-get full-upgrade -y
# For Ubuntu-24.04
sudo apt-get install -y $(cat compile-kernel/tools/script/ubuntu2404-build-armbian-depends)
```

3. Enter the `~/amlogic-s9xxx-armbian` root directory, create the `build/output/images` folder, and upload the Armbian image file (e.g., `Armbian_21.11.0-trunk_Odroidn2_current_5.15.50.img`) to the `~/amlogic-s9xxx-armbian/build/output/images` directory. Retain the release version number (e.g., `21.11.0`) and kernel version number (e.g., `5.15.50`) in the original filename, as they will be used for naming the rebuilt Armbian system.

4. Enter the `~/amlogic-s9xxx-armbian` root directory and run `sudo ./rebuild -b s905x3 -k 6.6.12` to generate the Armbian image for the specified board. Output files are saved in the `build/output/images` directory.

- ### Local Packaging Parameter Description

| Parameter | Meaning     | Description |
| ----      | ----------  | ----------  |
| -b        | Board      | Specifies the target device codename (default is `all`). You can specify a single device (e.g., `-b s905x3`) or connect multiple codenames with underscores to compile them together (e.g., `-b s905x3_s905d`). The parameter also supports special keywords for batch compilation: `all` compiles every device in the database, `first50` compiles the first 50 devices, `range50_100` compiles devices from the 51st to the 100th (similarly for `range100_150`), and `last20` compiles the last 20 devices. Additionally, you can compile by hardware platform (`amlogic`, `rockchip`, `allwinner`) to build all images for that specific platform, for example, `-b amlogic`. Appending numeric values to the platform name allows you to compile a specific range within that platform's support list; for example, `-b amlogic50` builds the first 50 devices under the Amlogic platform, and `-b amlogic50_100` builds the 51st to the 100th devices. For a complete list of supported device codenames, please refer to the `BOARD` configuration items in [model_database.conf](build-armbian/armbian-files/common-files/etc/model_database.conf). Default: `all` |
| -r        | KernelRepo | Specify the `<owner>/<repo>` of the github.com kernel repository. Default value: `ophub/kernel` |
| -u        | kernelUsage | Set the `tags suffix` of the kernel used, such as [stable](https://github.com/ophub/kernel/releases/tag/kernel_stable), [flippy](https://github.com/ophub/kernel/releases/tag/kernel_flippy), [beta](https://github.com/ophub/kernel/releases/tag/kernel_beta). Default value: `stable` |
| -k        | Kernel     | Specify [kernel](https://github.com/ophub/kernel/releases/tag/kernel_stable) name, such as `-k 6.6.12`. Connect multiple kernels with `_`, such as `-k 6.6.12_5.15.50`. The kernel version freely specified by the `-k` parameter is only valid for kernels using `stable/flippy/beta`. Other kernel series such as [rk3588](https://github.com/ophub/kernel/releases/tag/kernel_rk3588) / [rk35xx](https://github.com/ophub/kernel/releases/tag/kernel_rk35xx) can only use specific kernels. |
| -a        | AutoKernel | Set whether to automatically use the latest kernel within the same series. When `true`, the kernel repository is checked for newer versions within the series specified by `-k` (e.g., 6.6.12), and if found, the latest version is used automatically. When `false`, the exact specified version is used. Default: `true` |
| -t        | RootfsType | Set the file system type of the ROOTFS partition. Options: `ext4` or `btrfs`. Example: `-t btrfs`. Default: `ext4` |
| -s        | Size       | Set the image partition sizes. To set only the ROOTFS partition, specify a single value (e.g., `-s 2560`). To set both BOOTFS and ROOTFS, join them with `/` (e.g., `-s 512/2560`). Default: `512/2560` |
| -n        | BuilderName | Set the Armbian system builder signature. Do not include spaces. Default: None |

- `sudo ./rebuild`: Use default configuration to package all device models.
- `sudo ./rebuild -b s905x3 -k 6.6.12`: Recommended. Build with default configuration for the specified kernel.
- `sudo ./rebuild -b s905x3 -k 6.1.y`: Build with default configuration, using the latest kernel in the 6.1.y series.
- `sudo ./rebuild -b s905x3_s905d -k 6.6.12_5.15.50`: Build with default configuration for multiple kernels simultaneously. Use `_` to join multiple kernels.
- `sudo ./rebuild -b s905x3 -k 6.6.12 -s 2560`: Build a single kernel and model with default configuration; ROOTFS partition size set to 2560 MiB.
- `sudo ./rebuild -b s905x3_s905d`: Build all kernels for multiple models with default configuration. Use `_` to join multiple models.
- `sudo ./rebuild -k 6.6.12_5.15.50`: Build all models with default configuration and multiple specified kernels. Use `_` to join kernels.
- `sudo ./rebuild -k 6.6.12_5.15.50 -a true`: Same as above, with automatic upgrade to the latest kernel within each series.
- `sudo ./rebuild -t btrfs -s 2560 -k 6.6.12`: Build all models with btrfs filesystem, 2560 MiB ROOTFS, and kernel 6.6.12.

## Use GitHub Actions for Compilation

1. Workflow configuration files are located in the [.github/workflows/](.github/workflows/) directory.

2. Fresh Build: On the [Actions](https://github.com/ophub/amlogic-s9xxx-armbian/actions) page, select ***`Build Armbian server image`*** to use the [build-armbian-arm64-server-image.yml](.github/workflows/build-armbian-arm64-server-image.yml) workflow. You can choose from Ubuntu series (e.g., `resolute`) or Debian series (e.g., `trixie`). Click ***`Run workflow`*** to start the build.

3. Rebuild: If [Releases](https://github.com/ophub/amlogic-s9xxx-armbian/releases) already contains compiled `Armbian_.*-trunk_.*.img.gz` files and you only need to repackage for other boards, skip the source compilation step and use [build-armbian-using-releases-files.yml](.github/workflows/build-armbian-using-releases-files.yml) for secondary builds.

4. To use other Armbian systems (e.g., the [odroidn2](https://armbian.tnahosting.net/dl/odroidn2/archive/) image from the official [armbian.tnahosting.net](https://armbian.tnahosting.net/dl/) download site), simply reference this repository's script in the workflow file [build-armbian-using-official-image.yml](.github/workflows/build-armbian-using-official-image.yml) for Armbian restructuring to support other devices. Example:

```yaml
- name: Build Armbian
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: armbian
    armbian_path: build/output/images/*.img
    armbian_board: s905d_s905x3_s922x_s905x
    armbian_kernel: 6.12.y_6.18.y
```

- ### GitHub Actions Input Parameter Description

These parameters correspond to the local packaging command options described above.

| Parameter       | Default       | Description                                             |
|-----------------|---------------|---------------------------------------------------------|
| armbian_path    | None          | Set the path of the original Armbian file. Supports workflow file paths (e.g., `build/output/images/*.img`) and network download URLs (e.g., `https://dl.armbian.com/*/Armbian_*.img.xz`). |
| armbian_board   | all           | Set the `board` of the package box, refer to `-b`       |
| kernel_repo     | ophub/kernel  | Specify `<owner>/<repo>` of the github.com kernel repository, refer to `-r` |
| kernel_usage    | stable        | Set the `tags suffix` of the used kernel. Refer to `-u` |
| armbian_kernel  | 6.12.y_6.18.y | Set the [version](https://github.com/ophub/kernel/releases/tag/kernel_stable) of the kernel, refer to `-k` |
| auto_kernel     | true          | Set whether to automatically adopt the latest version of the same series kernel, refer to `-a`       |
| armbian_fstype  | ext4          | Set the file system type of the system's ROOTFS partition, refer to `-t`  |
| armbian_size    | 512/2560      | Set the size of the system BOOTFS and ROOTFS partitions, function reference `-s`  |
| armbian_files   | false         | Add custom Armbian files. When set, all files in this directory will be copied to [common-files](build-armbian/armbian-files/common-files). The directory structure must mirror the Armbian root directory to ensure files are correctly overlaid (e.g., default configuration files should be placed under `etc/default/`). |
| builder_name    | None          | Set the Armbian system builder signature, refer to `-n` |

- ### GitHub Actions Output Variable Description

Uploading to `Releases` requires `Workflow read and write permissions` for the repository. See the [usage instructions](documents/README.md#2-set-up-private-variable-github_token) for details.

| Parameter                        | Default       | Description                           |
|----------------------------------|---------------|---------------------------------------|
| ${{ env.PACKAGED_OUTPUTPATH }}   | out           | Armbian system files output path      |
| ${{ env.PACKAGED_OUTPUTDATE }}   | 04.13.1058    | Packaging date (month.day.hourminute) |
| ${{ env.PACKAGED_STATUS }}       | success       | Packaging status: success / failure   |

## Build Armbian Docker Image

For creating [Docker](https://hub.docker.com/u/ophub) images of the Armbian system, refer to the [armbian_docker](./compile-kernel/tools/script/docker) build script.

## Compiling Kernel Using GitHub Actions

For kernel compilation instructions, see [compile-kernel](compile-kernel).

```yaml
- name: Compile the kernel
  uses: ophub/amlogic-s9xxx-armbian@main
  with:
    build_target: kernel
    kernel_version: 6.12.y_6.18.y
    kernel_auto: true
    kernel_sign: -yourname
```

## Armbian Contributors

First and foremost, thanks to [150balbes](https://github.com/150balbes) for the outstanding contributions and the solid foundation laid for running Armbian on Amlogic TV boxes. The [Armbian](https://github.com/armbian/build) system compiled here uses the latest official source code for real-time builds. The development approach is inspired by tutorials from authors such as [ebkso](https://www.kflyo.com/howto-compile-armbian-for-n1-box). Thanks to everyone's dedication and sharing, enabling Armbian to run on an ever-growing range of devices.

The [u-boot](https://github.com/ophub/u-boot), [kernel](https://github.com/ophub/kernel), and other resources used in this system are primarily sourced from the [unifreq/openwrt_packit](https://github.com/unifreq/openwrt_packit) project. Some files have been contributed by users through [Pull Requests](https://github.com/ophub/amlogic-s9xxx-armbian/pulls) and [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues) in projects such as [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt), [amlogic-s9xxx-armbian](https://github.com/ophub/amlogic-s9xxx-armbian), [luci-app-amlogic](https://github.com/ophub/luci-app-amlogic), [u-boot](https://github.com/ophub/u-boot), and [kernel](https://github.com/ophub/kernel). To acknowledge these pioneers and contributors, all contributions since the repository's creation (`2021-09-19`) are recorded in [CONTRIBUTORS.md](https://github.com/ophub/amlogic-s9xxx-armbian/blob/main/CONTRIBUTORS.md). Thanks again to everyone for breathing new life and purpose into these devices.

## Other Distributions

- The [amlogic-s9xxx-openwrt](https://github.com/ophub/amlogic-s9xxx-openwrt) project provides the `OpenWrt` system for TV boxes, also applicable to Armbian-supported devices.
- The [fnnas](https://github.com/ophub/fnnas) project provides the `FnNAS` system for TV boxes, also applicable to Armbian-supported devices.
- [unifreq](https://github.com/unifreq/openwrt_packit) has created `OpenWrt` systems for a wide range of Amlogic, Rockchip, and Allwinner devices, serving as a benchmark project in the community. Highly recommended.
- [Scirese](https://github.com/Scirese/alarm) has tested the build, installation, and usage of `Arch Linux ARM` / `Manjaro` systems on Android TV boxes. See his repository for details.
- [7Ji](https://7ji.github.io/) has published articles on reverse engineering and development for the Amlogic platform, covering topics such as installing ArchLinux ARM and analyzing the Amlogic boot mechanism. His [ampart](https://github.com/7Ji/ampart) project provides a partition tool for reading and editing Amlogic eMMC partition tables and DTB partitions, enabling 100% utilization of eMMC space. The [amlogic-s9xxx-archlinuxarm](https://github.com/7Ji/amlogic-s9xxx-archlinuxarm) project provides build and usage instructions for `Arch Linux ARM`. The [YAopenvfD](https://github.com/7Ji/YAopenvfD) project offers an alternative openvfd daemon implementation.
- [13584452567](https://github.com/13584452567) is the pioneer for `Rockchip` device support in this repository. Through his contributions, support was expanded for numerous `Rockchip` devices including [EAIDK-610](https://github.com/ophub/amlogic-s9xxx-armbian/pull/991), [King3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1080), [TN3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1094), [Kylin3399](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1132), [ZCube1-Max](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1247), [tvi3315a](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1687), [xiaobao](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1698), and more. He also maintains specialized [kernels](https://github.com/13584452567/linux-6.6.y) for `Allwinner` devices such as [TQC-A01](https://github.com/ophub/amlogic-s9xxx-armbian/pull/1638), and has provided extensive technical support and solutions in [Discussions](https://github.com/ophub/amlogic-s9xxx-armbian/discussions/1634) and [Issues](https://github.com/ophub/amlogic-s9xxx-armbian/issues), making significant contributions to the community.
- [cooip-jm](https://github.com/cooip-jm) shares many guides on Armbian, LXC, Docker, AdGuard, and other applications in his [wiki](https://github.com/cooip-jm/About-openwrt/wiki). Recommended reading.


## Links

- [armbian](https://github.com/armbian/build)
- [unifreq](https://github.com/unifreq)
- [kernel.org](https://kernel.org)

## License

The amlogic-s9xxx-armbian © OPHUB is licensed under [GPL-2.0](LICENSE)

</details>