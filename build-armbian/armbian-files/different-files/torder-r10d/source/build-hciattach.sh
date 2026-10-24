#!/bin/bash
# Corresponding source and NOTICE are shipped alongside this script.
# Source: https://github.com/orangepi-xunlong/orangepi-build
# Revision: a95129bf30ea11886cc35f08ebb3e69695a26656
# Directory: external/cache/sources/hcitools
set -euo pipefail
src=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)/hcitools
out=$(realpath -m -- "${1:?Usage: build-hciattach.sh OUTPUT_DIRECTORY}")
[[ "$out" != "$src" && "$out" != "$src/"* ]] || { echo "Use a separate build directory" >&2; exit 1; }
mkdir -p "$out"
cp -a "$src/." "$out/"
make -C "$out" -j"${JOBS:-4}" CC="${CC:-aarch64-linux-gnu-gcc}" hciattach_opi
binary="$out/output/hciattach_opi"
# The helper is shared by Debian Bookworm and Ubuntu Noble images.
# Reject accidental dependencies on a newer build-host glibc.
python3 - "$binary" "${READELF:-aarch64-linux-gnu-readelf}" <<'PY'
import re, subprocess, sys
versions = re.findall(r'GLIBC_(\d+)\.(\d+)', subprocess.check_output([sys.argv[2], '--version-info', sys.argv[1]], text=True))
latest = max((tuple(map(int, v)) for v in versions), default=(0, 0))
if latest > (2, 35):
    raise SystemExit('Use an Ubuntu 20.04/22.04-compatible arm64 cross sysroot (glibc <= 2.35); built helper requires ' + str(latest))
print('ELF ABI check passed; highest required glibc:', latest)
PY
sha256sum "$binary"
