#!/bin/sh
set -eu

usage() {
    printf 'Usage: sudo %s /path/to/base-armbian.img\n' "$0" >&2
    exit 2
}

[ "$#" -eq 1 ] || usage
base_image="$(readlink -f "$1")"
[ -f "$base_image" ] || { printf 'Error: base image not found.\n' >&2; exit 1; }

repo_dir="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
expected_upstream="26ce84cdd716b307517e794880666ebb3ca19944"
expected_image="97776096fa40f0f38d64e5ef11fddd0f1738d3b20753a6dd6d1524f5da60c4be"

git -c safe.directory="$repo_dir" -C "$repo_dir" merge-base --is-ancestor "$expected_upstream" HEAD || {
    printf 'Error: source tree is not based on the pinned ophub revision.\n' >&2
    exit 1
}
printf '%s  %s\n' "$expected_image" "$base_image" | sha256sum --status -c - || {
    printf 'Error: base Armbian image checksum failed.\n' >&2
    exit 1
}

"$repo_dir/scripts/prepare-goldfinger-build.sh"
mkdir -p "$repo_dir/build/output/images"
staged="$repo_dir/build/output/images/Armbian_26.08.0-trunk_goldfinger-base_6.12.103.img"
cp --reflink=auto "$base_image" "$staged"

# ophub emits only a compressed final image. Remove a same-day uncompressed
# artifact left by an earlier run so it cannot be mistaken for the new build.
output_date="$(date +%Y.%m.%d)"
output_dir="$repo_dir/build/output/images"
generated_base="Armbian_26.08.0_amlogic_goldfinger-v14_noble_6.12.103_server_${output_date}"
release_base="Armbian_26.08.0_amlogic_goldfinger-v14-2021-12-07_noble_6.12.103_server_${output_date}"
stale_uncompressed="$output_dir/${generated_base}.img"
rm -f "$stale_uncompressed"
rm -f "$output_dir/${generated_base}.img.gz.sha256"

printf 'Building the board-specific Goldfinger V14 image...\n'
cd "$repo_dir"
./rebuild -b goldfinger-v14 -k 6.12.103 -a false -t ext4 -s 512/3000 -n community

generated_image="$output_dir/${generated_base}.img.gz"
final_image="$output_dir/${release_base}.img.gz"
[ -f "$generated_image" ] || {
    printf 'Error: expected final image was not produced.\n' >&2
    exit 1
}
mv -f "$generated_image" "$final_image"
checksum_tmp="${final_image}.sha256.tmp"
(cd "$(dirname "$final_image")" && sha256sum "$(basename "$final_image")") >"$checksum_tmp"
mv -f "$checksum_tmp" "${final_image}.sha256"
printf 'Final image checksum refreshed: %s\n' "${final_image}.sha256"
