/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2005-2010  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <sys/param.h>

#include <bluetooth/bluetooth.h>

#include "hciattach.h"

static int debug = 0;

static int do_command(int fd, uint8_t ogf, uint16_t ocf,
			uint8_t *cparam, int clen, uint8_t *rparam, int rlen)
{
	//uint16_t opcode = (uint16_t) ((ocf & 0x03ff) | (ogf << 10));
	unsigned char cp[260], rp[260];
	int len, size, offset = 3;

	cp[0] = 0x01;
	cp[1] = ocf & 0xff;
	cp[2] = ogf << 2 | ocf >> 8;
	cp[3] = clen;

	if (clen > 0)
		memcpy(cp + 4, cparam, clen);

	if (debug) {
		int i;
		printf("[<");
		for (i = 0; i < clen + 4; i++)
			printf(" %02x", cp[i]);
		printf("]\n");
	}

	if (write(fd, cp, clen + 4) < 0)
		return -1;

	do {
		if (read(fd, rp, 1) < 1)
			return -1;
	} while (rp[0] != 0x04);

	if (read(fd, rp + 1, 2) < 2)
		return -1;

	do {
		len = read(fd, rp + offset, sizeof(rp) - offset);
		offset += len;
	} while (offset < rp[2] + 3);

	if (debug) {
		int i;
		printf("[>");
		for (i = 0; i < offset; i++)
			printf(" %02x", rp[i]);
		printf("]\n");
	}

	if (rp[0] != 0x04) {
		errno = EIO;
		return -1;
	}

	switch (rp[1]) {
	case 0x0e:	/* command complete */
		if (rp[6] != 0x00)
			return -ENXIO;
		offset = 3 + 4;
		size = rp[2] - 4;
		break;
	case 0x0f:	/* command status */
		/* fall through */
	default:
		offset = 3;
		size = rp[2];
		break;
	}

	if (!rparam || rlen < size)
		return -ENXIO;

	memcpy(rparam, rp + offset, size);

	return size;
}

static int load_file(int dd, uint16_t version, const char *suffix)
{
	DIR *dir;
	struct dirent *d;
	char pathname[PATH_MAX], filename[NAME_MAX], prefix[20];
	unsigned char cmd[256];
	unsigned char buf[256];
	uint8_t seqnum = 0;
	int fd, size, len, found_fw_file;

	memset(filename, 0, sizeof(filename));

	snprintf(prefix, sizeof(prefix), "STLC2500_R%d_%02d_",
						version >> 8, version & 0xff);

	strcpy(pathname, "/lib/firmware");
	dir = opendir(pathname);
	if (!dir) {
		strcpy(pathname, ".");
		dir = opendir(pathname);
		if (!dir)
			return -errno;
	}

	found_fw_file = 0;
	while (1) {
		d = readdir(dir);
		if (!d)
			break;

		if (strncmp(d->d_name + strlen(d->d_name) - strlen(suffix),
						suffix, strlen(suffix)))
			continue;

		if (strncmp(d->d_name, prefix, strlen(prefix)))
			continue;

		snprintf(filename, sizeof(filename), "%s/%s",
							pathname, d->d_name);
		found_fw_file = 1;
	}

	closedir(dir);

	if (!found_fw_file)
		return -ENOENT;

	printf("Loading file %s\n", filename);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("Can't open firmware file");
		return -errno;
	}

	while (1) {
		size = read(fd, cmd + 1, 254);
		if (size <= 0)
			break;

		cmd[0] = seqnum;

		len = do_command(dd, 0xff, 0x002e, cmd, size + 1, buf, sizeof(buf));
		if (len < 1)
			break;

		if (buf[0] != seqnum) {
			fprintf(stderr, "Sequence number mismatch\n");
			break;
		}

		seqnum++;
	}

	close(fd);

	return 0;
}

int stlc2500_init(int dd, bdaddr_t *bdaddr)
{
	unsigned char cmd[16];
	unsigned char buf[254];
	uint16_t version;
	int len;
	int err;

	/* Hci_Cmd_Ericsson_Read_Revision_Information */
	len = do_command(dd, 0xff, 0x000f, NULL, 0, buf, sizeof(buf));
	if (len < 0)
		return -1;

	printf("%s\n", buf);

	/* HCI_Read_Local_Version_Information */
	len = do_command(dd, 0x04, 0x0001, NULL, 0, buf, sizeof(buf));
	if (len < 0)
		return -1;

	version = buf[2] << 8 | buf[1];

	err = load_file(dd, version, ".ptc");
	if (err < 0) {
		if (err == -ENOENT)
			fprintf(stderr, "No ROM patch file loaded.\n");
		else
			return -1;
	}

	err = load_file(dd, buf[2] << 8 | buf[1], ".ssf");
	if (err < 0) {
		if (err == -ENOENT)
			fprintf(stderr, "No static settings file loaded.\n");
		else
			return -1;
	}

	cmd[0] = 0xfe;
	cmd[1] = 0x06;
	bacpy((bdaddr_t *) (cmd + 2), bdaddr);

	/* Hci_Cmd_ST_Store_In_NVDS */
	len = do_command(dd, 0xff, 0x0022, cmd, 8, buf, sizeof(buf));
	if (len < 0)
		return -1;

	/* HCI_Reset : applies parameters*/
	len = do_command(dd, 0x03, 0x0003, NULL, 0, buf, sizeof(buf));
	if (len < 0)
		return -1;

	return 0;
}

int bgb2xx_init(int dd, bdaddr_t *bdaddr)
{
	unsigned char cmd[16];
	unsigned char buf[254];
	int len;

	len = do_command(dd, 0xff, 0x000f, NULL, 0, buf, sizeof(buf));
	if (len < 0)
		return -1;

	printf("%s\n", buf);

	cmd[0] = 0xfe;
	cmd[1] = 0x06;
	bacpy((bdaddr_t *) (cmd + 2), bdaddr);

	len = do_command(dd, 0xff, 0x0022, cmd, 8, buf, sizeof(buf));
	if (len < 0)
		return -1;

	len = do_command(dd, 0x03, 0x0003, NULL, 0, buf, sizeof(buf));
	if (len < 0)
		return -1;

	return 0;
}
