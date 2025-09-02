FROM scratch
ADD armbian-rootfs.tar.gz /

RUN apt-get update && apt-get install -y tzdata ca-certificates gnupg2 && \
    rm -rf /var/lib/apt/lists/*

ENV TZ=Etc/UTC
RUN ln -snf /usr/share/zoneinfo/${TZ} /etc/localtime && \
    echo "${TZ}" > /etc/timezone

EXPOSE 22
CMD ["/sbin/init"]