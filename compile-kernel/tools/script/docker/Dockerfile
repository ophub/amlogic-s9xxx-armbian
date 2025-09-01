FROM scratch
ADD armbian-rootfs.tar.gz /

ENV TZ=Etc/UTC
RUN ln -snf /usr/share/zoneinfo/${TZ} /etc/localtime && echo "${TZ}" > /etc/timezone

EXPOSE 22
CMD ["/sbin/init"]