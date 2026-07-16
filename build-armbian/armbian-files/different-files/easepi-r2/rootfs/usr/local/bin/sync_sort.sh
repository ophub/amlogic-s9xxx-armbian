#!/bin/bash
#
# sync_sort.sh - Inject network interface sort order into frontend JS
#
# Uses bind mount to overlay the injected file onto the original.
# trim_main periodically restores /usr/trim/www/ from encrypted zip,
# but /usr/local/share/ is not monitored, so the injected source survives.
#
# Handles the case where trim_main replaces the file under the mount
# (detected by checking if the target inode has 0 links).
#
# Called by:
#   - sort_sync_access.sh (on-demand when frontend is accessed)
#   - sort-mount.service (at boot time)

LOG=/var/log/sort_sync.log
ASSETS_DIR=/usr/trim/www/assets
INJECTED_SRC=/usr/local/share/injected_Setting.js
INJECTED_VER=/var/cache/sort_inject_version
MAX_RETRY=5
RETRY_DELAY=3

log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S'): $*" >> "$LOG"
}

touch "$LOG"
chown root:root "$LOG"
chmod 600 "$LOG"

# Log rotation
if [ -f "$LOG" ]; then
    log_size=$(stat -c%s "$LOG" 2>/dev/null || echo 0)
    if [ "$log_size" -gt $((512 * 1024)) ] 2>/dev/null; then
        tail -n 100 "$LOG" > "${LOG}.tmp" && mv "${LOG}.tmp" "$LOG"
        chown root:root "$LOG"
        chmod 600 "$LOG"
    fi
fi

SORT_MARKER="eth0\",\"eth1\",\"eth2\",\"eth3\",\"wlan0\",\"4Gnet"

# Check if the bind mount is broken (target has 0 links - file was replaced)
is_bind_mount_broken() {
    local target="$1"
    if ! mount | grep -qF "$target"; then
        return 0
    fi
    local links=$(stat -c "%h" "$target" 2>/dev/null)
    if [ "$links" = "0" ]; then
        return 0
    fi
    return 1
}

for attempt in $(seq 1 $MAX_RETRY); do
    JS_FILE=$(find "$ASSETS_DIR" -maxdepth 1 -name "Setting-*.js" 2>/dev/null | xargs ls -t 2>/dev/null | head -1)

    if [ -z "$JS_FILE" ]; then
        sleep "$RETRY_DELAY"
        continue
    fi

    if [ ! -f "$JS_FILE" ]; then
        sleep "$RETRY_DELAY"
        continue
    fi

    # Fix broken bind mount
    if is_bind_mount_broken "$JS_FILE"; then
        umount "$JS_FILE" 2>/dev/null
        log "detected broken bind mount, unmounted"
    fi

    # Check if the injected source file is already valid AND matches current JS version
	if [ -f "$INJECTED_SRC" ] && grep -qF "$SORT_MARKER" "$INJECTED_SRC" 2>/dev/null; then
	    # Check version: if JS file name changed, regenerate instead of reusing old injection
	    js_basename=$(basename "$JS_FILE")
	    cached_ver=$(cat "$INJECTED_VER" 2>/dev/null)
	    if [ "$cached_ver" != "$js_basename" ]; then
                log "JS version changed ($cached_ver -> $js_basename), will regenerate"
                chattr -i "$INJECTED_SRC" 2>/dev/null
                rm -f "$INJECTED_SRC"
	    else
	        if mount | grep -qF "$JS_FILE"; then
	            if grep -qF "$SORT_MARKER" "$JS_FILE" 2>/dev/null; then
	                log "bind mount active and valid, skipping"
	                exit 0
	            fi
	            umount "$JS_FILE" 2>/dev/null
	            log "bind mount had wrong content, re-mounting"
	        fi
	        mount --bind "$INJECTED_SRC" "$JS_FILE" 2>/dev/null
                if mount | grep -qF "$JS_FILE" && grep -qF "$SORT_MARKER" "$JS_FILE" 2>/dev/null; then
                    chattr +i "$INJECTED_SRC" 2>/dev/null
                    log "bind mount re-established"
	            exit 0
	        fi
	        log "bind mount failed, will regenerate"
	    fi
	fi

    if ! python3 -c "import re; content=open('$JS_FILE').read(); print(1 if re.search(r'\.sort\(\(([^,]+),([^)]+)\)=>\1\.index-\2\.index\)', content) else 0)" | grep -q "1"; then
        if grep -qF "$SORT_MARKER" "$JS_FILE" 2>/dev/null; then
                log "sort already present in source, creating bind mount source"
                chattr -i "$INJECTED_SRC" 2>/dev/null
                cp "$JS_FILE" "$INJECTED_SRC"
                chown root:root "$INJECTED_SRC"
                chmod 644 "$INJECTED_SRC"
                mount --bind "$INJECTED_SRC" "$JS_FILE"
                chattr +i "$INJECTED_SRC" 2>/dev/null
                log "bind mount created from already-injected source"
            exit 0
        fi
        log "target pattern not found in source JS (version changed?): $(basename "$JS_FILE")"
        exit 0
    fi

    js_basename=$(basename "$JS_FILE")
    log "generating injected JS: ${js_basename}"

    if /usr/local/bin/sort_inject.py "$JS_FILE" "$INJECTED_SRC.tmp" 2>/dev/null; then
            chattr -i "$INJECTED_SRC" 2>/dev/null
            mv "$INJECTED_SRC.tmp" "$INJECTED_SRC"
        chown root:root "$INJECTED_SRC"
        chmod 644 "$INJECTED_SRC"

        if grep -qF "$SORT_MARKER" "$INJECTED_SRC" 2>/dev/null; then
            mount --bind "$INJECTED_SRC" "$JS_FILE" 2>/dev/null
            if mount | grep -qF "$JS_FILE" && grep -qF "$SORT_MARKER" "$JS_FILE" 2>/dev/null; then
                    echo "$js_basename" > "$INJECTED_VER"
                    chattr +i "$INJECTED_SRC" 2>/dev/null
                    log "sort injected and bind mounted: ${js_basename}"
                exit 0
            else
                log "bind mount failed after injection: ${js_basename}"
            fi
        else
            log "injection verification failed: ${js_basename}"
        fi
    else
        log "sort_inject.py execution failed"
    fi

    sleep "$RETRY_DELAY"
done

exit 1
