#!/bin/bash

WLAN_IF="wlan0"
RFKILL_WLAN=""

for f in /var/lib/systemd/rfkill/*:wlan; do
    if [ -f "$f" ]; then
        RFKILL_WLAN="$f"
        break
    fi
done

if [ -n "$RFKILL_WLAN" ]; then
    echo 0 > "$RFKILL_WLAN" 2>/dev/null
fi

if [ -x /usr/bin/update-alternatives ]; then
    UPSTREAM_DB="/lib/firmware/regulatory.db-upstream"
    UPSTREAM_P7S="/lib/firmware/regulatory.db.p7s-upstream"
    if [ -f "$UPSTREAM_DB" ] && [ -f "$UPSTREAM_P7S" ]; then
        update-alternatives --set regulatory.db "$UPSTREAM_DB" >/dev/null 2>&1
        update-alternatives --set regulatory.db.p7s "$UPSTREAM_P7S" >/dev/null 2>&1
    fi
fi

if [ -d /sys/class/net/$WLAN_IF ]; then
    nmcli radio wifi on 2>/dev/null
fi
