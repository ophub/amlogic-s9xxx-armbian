FROM scratch
ADD armbian-rootfs.tar.gz /

# Install necessary packages
RUN apt-get update && apt-get install -y \
    tzdata \
    ca-certificates \
    gnupg2 \
    openssh-server \
    nginx \
    && rm -rf /var/lib/apt/lists/*

# Set timezone
ENV TZ=Etc/UTC
RUN ln -snf /usr/share/zoneinfo/${TZ} /etc/localtime && \
    echo "${TZ}" > /etc/timezone

# Copy custom startup script
COPY docker_startup.sh /usr/local/bin/docker_startup.sh
# Make the script executable
RUN chmod +x /usr/local/bin/docker_startup.sh

# Expose necessary ports (SSH and HTTP/HTTPS)
EXPOSE 22 80 443

# Set the default command to run our script
CMD ["/usr/local/bin/docker_startup.sh"]
