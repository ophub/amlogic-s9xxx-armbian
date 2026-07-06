#!/bin/bash
#
# sort_sync_access.sh - On-demand sort injection trigger via nginx access log
#
# Monitors nginx access log for requests to Setting-*.js files.
# When a frontend user opens the network settings page, this script
# runs sync_sort.sh to ensure the sort injection bind mount is active.
#
# A 60-second cooldown prevents excessive runs.

ACCESS_LOG=/usr/trim/nginx/logs/access.log
COOLDOWN_FILE=/var/cache/sort_sync_access_cooldown
COOLDOWN_SECS=60

# Wait for access log to exist
for i in $(seq 1 30); do
    if [ -f "$ACCESS_LOG" ]; then
        break
    fi
    sleep 2
done

if [ ! -f "$ACCESS_LOG" ]; then
    echo "$(date '+%Y-%m-%d %H:%M:%S'): access log not found after 60s, exiting" >> /var/log/sort_sync.log
    exit 1
fi

# NOTE: Boot-time sync_sort.sh is handled by sort-mount.service (oneshot).
# This service only monitors access log for on-demand re-injection.

# Monitor access log for Setting-*.js requests
tail -F "$ACCESS_LOG" 2>/dev/null | while read -r line; do
    if ! echo "$line" | grep -q 'Setting-.*\.js'; then
        continue
    fi

    # Cooldown check
    if [ -f "$COOLDOWN_FILE" ]; then
        last_run=$(cat "$COOLDOWN_FILE" 2>/dev/null)
        now=$(date +%s)
        if [ -n "$last_run" ] && [ $((now - last_run)) -lt $COOLDOWN_SECS ] 2>/dev/null; then
            continue
        fi
    fi

    date +%s > "$COOLDOWN_FILE"
    /usr/local/bin/sync_sort.sh
done
