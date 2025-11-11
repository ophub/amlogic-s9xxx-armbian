#!/bin/bash
#=================================================================================
#
# This file is licensed under the terms of the GNU General Public
# License version 2. This program is licensed "as is" without any
# warranty of any kind, whether express or implied.
#
# This file is a part of the Rebuild Armbian
# https://github.com/ophub/amlogic-s9xxx-armbian
#
# Description: Custom startup script for Armbian Docker container.
# Copyright (C) 2021- https://github.com/ophub/amlogic-s9xxx-armbian
#
#=================================================================================

#======================== [Container Initialization Section] ========================
# Function 1: Try to start Nginx service in the background if it exists.
start_nginx_service() {
    echo "[SETUP] Checking for and attempting to start Nginx service..."
    if ! command -v nginx >/dev/null 2>&1; then
        echo "[INFO] Nginx not found, skipping."
        return
    fi

    # Test Nginx configuration before trying to start it.
    if nginx -t; then
        echo "[INFO] Nginx configuration test passed."
        # Start Nginx in the background.
        nginx || echo "[WARNING] Failed to start Nginx daemon, but script will continue."
    else
        # If the configuration is bad, report it clearly and do not start.
        echo "[ERROR] Nginx configuration test failed! Nginx will not be started."
    fi
}

# Function 2: Placeholder for other tasks.
other_initialization() {
    echo "[SETUP] Performing other initialization tasks..."
    # Add any other necessary commands here, for example:
    # cp -rf /path/to/website_code /var/www/html/myblog
}

echo "Container Initialization Started..."
# Step 1: Start background services like Nginx.
start_nginx_service
# Step 2: Perform other setup tasks.
other_initialization

#======================== [Start the Main Foreground Process] ========================
# This command keeps the container alive. Its lifecycle is the container's lifecycle.
# We choose sshd as the primary foreground process.
echo "Initialization Complete. Starting Main Process..."
if command -v sshd >/dev/null 2>&1; then
    echo "[RUN] Starting SSHD as the main process..."
    # Ensure the sshd run directory exists.
    mkdir -p /var/run/sshd
    # Use 'exec' to replace this script with the sshd process.
    exec /usr/sbin/sshd -D
else
    # Fallback if sshd is somehow not installed.
    echo "[RUN] FATAL: sshd command not found."
    echo "[RUN] Starting 'tail -f /dev/null' to keep the container alive for debugging."
    exec tail -f /dev/null
fi
