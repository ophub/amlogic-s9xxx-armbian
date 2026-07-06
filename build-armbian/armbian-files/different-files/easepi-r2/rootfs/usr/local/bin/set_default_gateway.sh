#!/bin/bash

LOG=/var/log/default_gateway.log

PRIORITY_MAP=(
    "eth0:50"
    "eth1:60"
    "eth2:70"
    "eth3:80"
)

log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S'): $*" >> "$LOG"
}

touch "$LOG"
chown root:root "$LOG"
chmod 600 "$LOG"

if [ -f "$LOG" ]; then
    log_size=$(stat -c%s "$LOG" 2>/dev/null || echo 0)
    if [ "$log_size" -gt $((512 * 1024)) ] 2>/dev/null; then
        tail -n 100 "$LOG" > "${LOG}.tmp" && mv "${LOG}.tmp" "$LOG"
        chown root:root "$LOG"
        chmod 600 "$LOG"
    fi
fi

set_interface_metric() {
    local iface="$1"
    local target_metric="$2"
    
    local connection_name=$(nmcli -t -f NAME,DEVICE connection show 2>/dev/null | grep ":${iface}$" | cut -d: -f1)
    
    if [ -z "$connection_name" ]; then
        log "No connection found for $iface"
        return 1
    fi
    
    local current_metric=$(nmcli -t -f ipv4.route-metric connection show "$connection_name" 2>/dev/null | cut -d: -f2)
    
    if [ "$current_metric" -eq "$target_metric" 2>/dev/null ]; then
        log "$iface metric already $target_metric, no change needed"
        return 0
    fi
    
    log "Setting $iface metric to $target_metric"
    nmcli connection modify "$connection_name" ipv4.route-metric "$target_metric" 2>/dev/null
    
    sleep 1
    
    # NOTE: nmcli connection up is required for the route-metric change to take
    # effect. If the connection is already active, this may cause a brief
    # reconnection (< 1s). The early return above (metric already correct)
    # prevents unnecessary calls.
    nmcli connection up "$connection_name" 2>/dev/null || true
    
    sleep 2
    
    local new_metric=$(nmcli -t -f ipv4.route-metric connection show "$connection_name" 2>/dev/null | cut -d: -f2)
    log "$iface metric after setting: $new_metric"
    
    return 0
}

process_all_interfaces() {
    log "Processing all Ethernet interfaces..."
    
    for mapping in "${PRIORITY_MAP[@]}"; do
        local iface=$(echo "$mapping" | cut -d: -f1)
        local target_metric=$(echo "$mapping" | cut -d: -f2)
        
        if ip link show "$iface" 2>/dev/null | grep -q "UP"; then
            local carrier=$(cat "/sys/class/net/${iface}/carrier" 2>/dev/null)
            if [ "$carrier" == "1" ]; then
                log "$iface is up with carrier, setting priority"
                set_interface_metric "$iface" "$target_metric"
            else
                log "$iface is up but no carrier (no cable)"
            fi
        else
            log "$iface is not up"
        fi
    done
    
    log "Current default routes:"
    ip route show default 2>/dev/null | while read route; do
        log "  $route"
    done
}

process_single_interface() {
    local iface="$1"
    local action="$2"
    
    log "Processing $iface $action"
    
    for mapping in "${PRIORITY_MAP[@]}"; do
        local mapped_iface=$(echo "$mapping" | cut -d: -f1)
        local target_metric=$(echo "$mapping" | cut -d: -f2)
        
        if [ "$mapped_iface" == "$iface" ]; then
            if [ "$action" == "up" ]; then
                local carrier=$(cat "/sys/class/net/${iface}/carrier" 2>/dev/null)
                if [ "$carrier" == "1" ]; then
                    log "$iface connected, setting metric to $target_metric"
                    set_interface_metric "$iface" "$target_metric"
                else
                    log "$iface up but no carrier"
                fi
            elif [ "$action" == "down" ]; then
                log "$iface disconnected"
            fi
            break
        fi
    done
}

if [ $# -eq 2 ]; then
    process_single_interface "$1" "$2"
else
    process_all_interfaces
fi

exit 0