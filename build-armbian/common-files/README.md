# Common file description

- files: The files in the `files` directory are custom files, which must be completely consistent with the structure and file naming and storage under the ***`ROOTFS`*** directory in armbian. If there are files in this directory, they will be automatically copied to the armbian directory during `sudo ./rebuild`. E.g:
```yaml
etc/network/interfaces
usr/sbin/
```

- release: It has the same function as `files`. According to the difference of `release` (E.g. focal, bullseye), it is classified and merged in `sudo ./rebuild`.

- patches: The files in the `patches` directory are patch files, which are integrated when rebuild armbian.

