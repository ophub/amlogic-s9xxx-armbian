#!/bin/bash

rename_iface() {
    ip link set $1 down && ip link set $1 name $2
}

device_to_iface() {
    local device=$1
    if [[ $device = *-*:* ]]; then
        #usb
        ls /sys/bus/usb/devices/$device/net/ | head -1
    elif [[ $device = *:*:* ]]; then
        #pci
        ls /sys/bus/pci/devices/$device/net/ | head -1
    else
        #platform
        ls /sys/devices/platform/$device/net/ | head -1
    fi
}

reorder_eth() {
    local index=0
    local iface toiface
    while [[ -n "$1" ]]; do
        toiface="eth$index"
        iface=$(device_to_iface $1)
        if [[ -n "$iface" && "$iface" != "$toiface" ]]; then
            rename_iface $toiface rename_tmp 2>/dev/null
            rename_iface $iface $toiface
            rename_iface rename_tmp $iface 2>/dev/null
        fi
        index=$(( $index + 1 ))
        shift
    done
}

if [[ -s /proc/device-tree/eth_order || -s /etc/eth_order ]]; then
    if [[ -s /proc/device-tree/eth_order ]]; then
        reorder_eth $(cat /proc/device-tree/eth_order | tr ',' ' ')
    else
        reorder_eth $(cat /etc/eth_order | tr ',' ' ')
    fi
fi
