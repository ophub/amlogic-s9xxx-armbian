#!/bin/bash
#
# sync_sort.sh - Generate network interface sort cache for fnnas frontend
#
# This script generates a cached copy of the frontend JS file with network
# interface sort order injected, served via nginx try_files from cache dir.
# The original JS file is NEVER modified, avoiding frontend disconnects and
# race conditions with trim_main's file restoration.
#
# Triggered by: sort_sync_access.sh (on-demand, when frontend is opened)
# Nginx conf.d management is handled by sort_sync_access.sh.

LOG=/var/log/sort_sync.log
CACHE_DIR=/var/cache/trim_www/assets
HASH_FILE=/var/cache/sort_sync_hash
JS_PATH_FILE=/var/cache/sort_sync_js_path
ASSETS_DIR=/usr/trim/www/assets
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
        log "log rotated (was ${log_size} bytes)"
    fi
fi

OLD='children:p==null?void 0:p.map('
NEW='children:p==null?void 0:p.sort((m,n)=>{const o=["eth0","eth1","eth2","eth3","wlan0","4Gnet"];return o.indexOf(m.name)-o.indexOf(n.name)}).map('

for attempt in $(seq 1 $MAX_RETRY); do
    JS_FILE=""

    # Resolve JS file path (use cached path if still valid)
    if [ -f "$JS_PATH_FILE" ] && [ -f "$(cat "$JS_PATH_FILE")" ]; then
        JS_FILE=$(cat "$JS_PATH_FILE")
    else
        JS_FILE=$(find "$ASSETS_DIR" -maxdepth 1 -name "Setting-*.js" 2>/dev/null | head -1)
        if [ -n "$JS_FILE" ]; then
            echo "$JS_FILE" > "$JS_PATH_FILE"
        fi
    fi

    if [ -z "$JS_FILE" ]; then
        sleep "$RETRY_DELAY"
        continue
    fi

    if [ ! -f "$JS_FILE" ]; then
        rm -f "$JS_PATH_FILE"
        sleep "$RETRY_DELAY"
        continue
    fi

    # Compute hash of the ORIGINAL source file
    src_hash=$(md5sum "$JS_FILE" 2>/dev/null | awk '{print $1}')
    if [ -z "$src_hash" ]; then
        sleep "$RETRY_DELAY"
        continue
    fi

    js_basename=$(basename "$JS_FILE")
    CACHE_FILE="${CACHE_DIR}/${js_basename}"

    # Check if cache is still valid:
    #   1. Cached hash matches source hash (source file unchanged)
    #   2. Cache file exists and contains sort injection
    if [ -f "$HASH_FILE" ]; then
        cached_hash=$(cat "$HASH_FILE" 2>/dev/null)
        if [ "$src_hash" = "$cached_hash" ] && [ -f "$CACHE_FILE" ]; then
            if grep -qF 'p.sort((m,n)' "$CACHE_FILE" 2>/dev/null; then
                log "cache valid (src hash: ${src_hash:0:8}...), skipping"
                exit 0
            else
                log "cache file exists but missing sort, regenerating"
            fi
        fi
    fi

    # Check if source already has sort (e.g. from previous direct injection)
    if grep -qF 'p.sort((m,n)' "$JS_FILE" 2>/dev/null; then
        log "sort already present in source, generating cache from it"
        mkdir -p "$CACHE_DIR"
        cp "$JS_FILE" "$CACHE_FILE"
        chown root:www-data "$CACHE_FILE"
        chmod 640 "$CACHE_FILE"
        echo "$src_hash" > "$HASH_FILE"
        chown root:root "$HASH_FILE"
        chmod 600 "$HASH_FILE"
        exit 0
    fi

    # Check if target pattern exists in source
    if ! grep -qF "$OLD" "$JS_FILE" 2>/dev/null; then
        log "target pattern not found in source JS (version changed?), clearing cache"
        rm -f "$CACHE_FILE" "$HASH_FILE"
        exit 0
    fi

    log "regenerating sort cache (src hash: ${src_hash:0:8}...)"

    # Clean up stale cache files from previous JS versions
    if [ -f "$JS_PATH_FILE" ]; then
        old_js=$(cat "$JS_PATH_FILE" 2>/dev/null)
        if [ -n "$old_js" ] && [ "$old_js" != "$JS_FILE" ]; then
            old_basename=$(basename "$old_js")
            old_cache="${CACHE_DIR}/${old_basename}"
            if [ -f "$old_cache" ]; then
                rm -f "$old_cache"
                log "cleaned up stale cache: ${old_basename}"
            fi
        fi
    fi

    # Generate cached copy with sort injection
    mkdir -p "$CACHE_DIR"
    cp "$JS_FILE" "$CACHE_FILE"
    sed -i "s|$OLD|$NEW|g" "$CACHE_FILE"

    # Verify injection succeeded
    if ! grep -qF 'p.sort((m,n)' "$CACHE_FILE" 2>/dev/null; then
        log "sed injection failed, removing cache"
        rm -f "$CACHE_FILE"
        exit 1
    fi

    # Set permissions: readable by nginx (www-data), writable by root
    chown root:www-data "$CACHE_FILE"
    chmod 640 "$CACHE_FILE"

    # Store hash of the SOURCE file (not the cache) for version tracking
    echo "$src_hash" > "$HASH_FILE"
    chown root:root "$HASH_FILE"
    chmod 600 "$HASH_FILE"

    log "sort cache generated: ${js_basename} (src hash: ${src_hash:0:8}...)"
    exit 0
done

exit 1