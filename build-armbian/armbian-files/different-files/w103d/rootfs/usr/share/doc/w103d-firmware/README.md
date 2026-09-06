# W103D tested firmware pair

The overlay ships the exact N9 and ROM-patch bytes used by the Linux
6.12.107 and 6.18.49 board tests. `manifest.json` pins both hashes. The N9
was extracted from the W103D factory Android image at
`vendor/firmware/WIFI_RAM_CODE_MT7663.bin` and renamed for the mt76 loader;
its contents were not edited. It reports `7663mp1827 / 20210308205639`.
No board EEPROM, OTP, MAC override or network credentials are shipped.

The previous N9 (`d8b8488f...`, build timestamp `20200604161656`) came from
a recent linux-firmware package, but the binary itself dates from 2020.
It failed authentication in the recorded W103D comparison; replacing only
N9 with the pinned 2021 binary restored association. A package release date
does not identify the age or compatibility of the firmware inside it.

## Provenance and licensing

These are separate MediaTek device binaries, not GPL-licensed kernel code.
This project does not relicense them under its top-level GPL license.

The ROM patch is the linux-firmware file identified by MediaTek's
[MT7663 update](https://kernel.googlesource.com/pub/scm/linux/kernel/git/firmware/linux-firmware/+/f39b68729aab3787b89c35bbbd76538e3edb13c4).
Its redistribution notice is retained in `LICENCE.mediatek`.

The factory 2021 N9 is user-supplied vendor firmware. No separate license
notice for these exact bytes accompanied the extracted file. The
linux-firmware notice for a different N9 version is **not** asserted to
grant rights over this one. This exact-version licensing question remains
for upstream review; neither successful hardware tests nor a matching
filename resolves it. Inclusion for review is not a new license grant.

`verify_image.py` rejects missing or changed firmware and, when given a
boot tree, rejects an unpatched or mismatched W103D kernel/module set.
The rebuild workflow runs that check before declaring an image successful.
