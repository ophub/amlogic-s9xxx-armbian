#!/bin/bash
#
# sort_sync_access.sh - On-demand sort cache trigger via nginx access log
#
# Monitors nginx access log for requests to Setting-*.js files.
# When a frontend user opens the network settings page (which loads
# the Setting JS file), this script:
#   1. Ensures the nginx conf.d config file exists (trim_main may have deleted it)
#   2. Reloads nginx if the config was restored
#   3. Runs sync_sort.sh to validate/regenerate the sort cache
#
# Unlike the previous timer-based approach, this is purely on-demand:
#   - No cache generation at boot (only nginx conf.d setup)
#   - Cache is checked/generated only when frontend is actually opened
#   - If cache is valid (hash matches), sync_sort.sh exits immediately
#
# A 60-second cooldown prevents excessive runs when multiple users
# access the frontend simultaneously.

ACCESS_LOG=/usr/trim/nginx/logs/access.log
COOLDOWN_FILE=/var/cache/sort_sync_access_cooldown
COOLDOWN_SECS=60
INIT_DELAY=10
NGINX_CONFD_FILE=/usr/trim/nginx/conf/conf.d/00_trim_sort_cache.conf
NGINX_CONFD_DIR=/usr/trim/nginx/conf/conf.d
NGINX_CONFD_CONTENT='location ^~ /assets/Setting- {
    root /var/cache/trim_www;
    try_files $uri @original_assets;
    add_header cache-control "max-age=1209600, immutable";
}

location @original_assets {
    root /usr/trim/www;
    try_files $uri =404;
    add_header cache-control "max-age=1209600, immutable";
}'

# Ensure the nginx conf.d file exists (trim_main periodically deletes it).
# Also handles the case where the conf.d directory itself was deleted.
# Returns 0 if nginx was reloaded, 1 otherwise.
ensure_nginx_confd() {
    # If conf.d directory doesn't exist, nginx is mid-restore; wait and retry
    if [ ! -d "$NGINX_CONFD_DIR" ]; then
        return 1
    fi

    if [ -f "$NGINX_CONFD_FILE" ]; then
        return 1
    fi

    echo "$NGINX_CONFD_CONTENT" > "$NGINX_CONFD_FILE"

    # Test config and reload
    if /usr/trim/nginx/sbin/nginx -t 2>/dev/null; then
        # Reload via HUP signal (PID file may be unreliable)
        local master_pid
        master_pid=$(ps aux | grep "nginx: master" | grep -v grep | awk '{print $2}')
        if [ -n "$master_pid" ]; then
            kill -HUP "$master_pid" 2>/dev/null
        fi
        echo "$(date '+%Y-%m-%d %H:%M:%S'): nginx conf.d restored and reloaded" >> /var/log/sort_sync.log
    else
        rm -f "$NGINX_CONFD_FILE"
        echo "$(date '+%Y-%m-%d %H:%M:%S'): nginx config test failed, removed conf.d file" >> /var/log/sort_sync.log
        return 1
    fi
    return 0
}

# Wait for access log to exist (nginx may not have started yet)
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

# Startup: only set up nginx conf.d, do NOT generate cache yet.
# Cache generation is deferred until the first frontend access.
sleep "$INIT_DELAY"
ensure_nginx_confd

# Monitor access log for Setting-*.js requests
tail -F "$ACCESS_LOG" 2>/dev/null | while read -r line; do
    # Only trigger on Setting-*.js requests
    if ! echo "$line" | grep -q 'Setting-.*\.js'; then
        continue
    fi

    # Cooldown check: avoid running too frequently
    if [ -f "$COOLDOWN_FILE" ]; then
        last_run=$(cat "$COOLDOWN_FILE" 2>/dev/null)
        now=$(date +%s)
        if [ -n "$last_run" ] && [ $((now - last_run)) -lt $COOLDOWN_SECS ] 2>/dev/null; then
            continue
        fi
    fi

    date +%s > "$COOLDOWN_FILE"

    # Ensure nginx config is in place (trim_main may have deleted it)
    # before running the sort cache check
    ensure_nginx_confd
    /usr/local/bin/sync_sort.sh
done