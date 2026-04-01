#!/bin/bash
set -euo pipefail

POLL_INTERVAL=0.2
FLASH_MS=700

declare -A LED_PATH=(
  [sata0_green]="/dev/disk/by-path/platform-fe210000.sata-ata-1"
  [sata1_green]="/dev/disk/by-path/platform-fe230000.sata-ata-1"
)
declare -A LED_STATE LAST_STAT LAST_DEV FLASH_UNTIL

now_ms() {
  date +%s%3N
}

led_ensure_none() {
  local led="$1" base="/sys/class/leds/$1"
  [ -w "$base/trigger" ] || return 0
  local trig
  trig=$(<"$base/trigger")
  [[ "$trig" == *"[none]"* ]] || echo none > "$base/trigger"
}

led_write() {
  local led="$1" val="$2" base="/sys/class/leds/$1"
  [ -w "$base/brightness" ] || return 0
  if [[ "${LED_STATE[$led]:-x}" != "$val" ]]; then
    echo "$val" > "$base/brightness"
    LED_STATE[$led]="$val"
  fi
}

map_dev() {
  local path="$1"
  [ -L "$path" ] || return 0
  basename "$(readlink -f "$path")"
}

read_stat() {
  local dev="$1"
  awk '{print $1 + $5}' "/sys/class/block/$dev/stat" 2>/dev/null || true
}

restore_presence_state() {
  local led dev
  for led in "${!LED_PATH[@]}"; do
    led_ensure_none "$led"
    dev=$(map_dev "${LED_PATH[$led]}")
    if [[ -n "$dev" && -r "/sys/class/block/$dev/stat" ]]; then
      led_write "$led" 1
    else
      led_write "$led" 0
    fi
  done
}

cleanup() {
  restore_presence_state || true
}
trap cleanup EXIT INT TERM

restore_presence_state

while true; do
  now=$(now_ms)
  for led in "${!LED_PATH[@]}"; do
    led_ensure_none "$led"
    dev=$(map_dev "${LED_PATH[$led]}")

    if [[ -z "$dev" || ! -r "/sys/class/block/$dev/stat" ]]; then
      unset LAST_STAT["$led"] LAST_DEV["$led"] FLASH_UNTIL["$led"]
      led_write "$led" 0
      continue
    fi

    stat=$(read_stat "$dev")
    if [[ -z "$stat" ]]; then
      led_write "$led" 0
      continue
    fi

    if [[ "${LAST_DEV[$led]:-}" != "$dev" ]]; then
      LAST_DEV["$led"]="$dev"
      LAST_STAT["$led"]="$stat"
      FLASH_UNTIL["$led"]=0
    elif [[ "${LAST_STAT[$led]:-}" != "$stat" ]]; then
      FLASH_UNTIL["$led"]=$((now + FLASH_MS))
      LAST_STAT["$led"]="$stat"
    fi

    if (( now < ${FLASH_UNTIL[$led]:-0} )); then
      if (( ((now / 200) % 2) == 0 )); then
        led_write "$led" 1
      else
        led_write "$led" 0
      fi
    else
      led_write "$led" 1
    fi
  done
  sleep "$POLL_INTERVAL"
done
