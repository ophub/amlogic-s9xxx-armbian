/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2007-2008  Texas Instruments, Inc.
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
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "hciattach.h"

#ifdef HCIATTACH_DEBUG
#define DPRINTF(x...)	printf(x)
#else
#define DPRINTF(x...)
#endif

#define HCIUARTGETDEVICE	_IOR('U', 202, int)

#define MAKEWORD(a, b)  ((uint16_t)(((uint8_t)(a)) | ((uint16_t)((uint8_t)(b))) << 8))

#define TI_MANUFACTURER_ID	13

#define FIRMWARE_DIRECTORY	"/lib/firmware/ti-connectivity/"

#define ACTION_SEND_COMMAND	1
#define ACTION_WAIT_EVENT	2
#define ACTION_SERIAL		3
#define ACTION_DELAY		4
#define ACTION_RUN_SCRIPT	5
#define ACTION_REMARKS		6

#define BRF_DEEP_SLEEP_OPCODE_BYTE_1	0x0c
#define BRF_DEEP_SLEEP_OPCODE_BYTE_2	0xfd
#define BRF_DEEP_SLEEP_OPCODE		\
	(BRF_DEEP_SLEEP_OPCODE_BYTE_1 | (BRF_DEEP_SLEEP_OPCODE_BYTE_2 << 8))

#define FILE_HEADER_MAGIC	0x42535442

/*
 * BRF Firmware header
 */
struct bts_header {
	uint32_t	magic;
	uint32_t	version;
	uint8_t	future[24];
	uint8_t	actions[0];
}__attribute__ ((packed));

/*
 * BRF Actions structure
 */
struct bts_action {
	uint16_t	type;
	uint16_t	size;
	uint8_t	data[0];
} __attribute__ ((packed));

struct bts_action_send {
	uint8_t data[0];
} __attribute__ ((packed));

struct bts_action_wait {
	uint32_t msec;
	uint32_t size;
	uint8_t data[0];
}__attribute__ ((packed));

struct bts_action_delay {
	uint32_t msec;
}__attribute__ ((packed));

struct bts_action_serial {
	uint32_t baud;
	uint32_t flow_control;
}__attribute__ ((packed));

static FILE *bts_load_script(const char *file_name, uint32_t *version)
{
	struct bts_header header;
	FILE *fp;

	fp = fopen(file_name, "rb");
	if (!fp) {
		perror("can't open firmware file");
		return NULL;
	}

	if (1 != fread(&header, sizeof(struct bts_header), 1, fp)) {
		perror("can't read firmware file");
		goto errclose;
	}

	if (header.magic != FILE_HEADER_MAGIC) {
		fprintf(stderr, "%s not a legal TI firmware file\n", file_name);
		goto errclose;
	}

	if (NULL != version)
		*version = header.version;

	return fp;

errclose:
	fclose(fp);

	return NULL;
}

static unsigned long bts_fetch_action(FILE *fp, unsigned char *action_buf,
				unsigned long buf_size, uint16_t *action_type)
{
	struct bts_action action_hdr;
	unsigned long nread;

	if (!fp)
		return 0;

	if (1 != fread(&action_hdr, sizeof(struct bts_action), 1, fp))
		return 0;

	if (action_hdr.size > buf_size) {
		fprintf(stderr, "bts_next_action: not enough space to read next action\n");
		return 0;
	}

	nread = fread(action_buf, sizeof(uint8_t), action_hdr.size, fp);
	if (nread != (action_hdr.size)) {
		fprintf(stderr, "bts_next_action: fread failed to read next action\n");
		return 0;
	}

	*action_type = action_hdr.type;

	return nread * sizeof(uint8_t);
}

static void bts_unload_script(FILE *fp)
{
	if (fp)
		fclose(fp);
}

static int is_it_texas(const uint8_t *respond)
{
	uint16_t manufacturer_id;

	manufacturer_id = MAKEWORD(respond[11], respond[12]);

	return TI_MANUFACTURER_ID == manufacturer_id ? 1 : 0;
}

static const char *get_firmware_name(const uint8_t *respond)
{
	static char firmware_file_name[PATH_MAX] = {0};
	uint16_t version = 0, chip = 0, min_ver = 0, maj_ver = 0;

	version = MAKEWORD(respond[13], respond[14]);
	chip =  (version & 0x7C00) >> 10;
	min_ver = (version & 0x007F);
	maj_ver = (version & 0x0380) >> 7;

	if (version & 0x8000)
		maj_ver |= 0x0008;

	sprintf(firmware_file_name, FIRMWARE_DIRECTORY "TIInit_%d.%d.%d.bts", chip, maj_ver, min_ver);

	return firmware_file_name;
}

static void brf_delay(struct bts_action_delay *delay)
{
	usleep(1000 * delay->msec);
}

static int brf_set_serial_params(struct bts_action_serial *serial_action,
						int fd, int *speed, struct termios *ti)
{
	fprintf(stderr, "texas: changing baud rate to %u, flow control to %u\n",
				serial_action->baud, serial_action->flow_control );
	tcflush(fd, TCIOFLUSH);

	if (serial_action->flow_control)
		ti->c_cflag |= CRTSCTS;
	else
		ti->c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, ti) < 0) {
		perror("Can't set port settings");
		return -1;
	}

	tcflush(fd, TCIOFLUSH);

	if (set_speed(fd, ti, serial_action->baud) < 0) {
		perror("Can't set baud rate");
		return -1;
	}

	if (speed)
		*speed = serial_action->baud;

	return 0;
}

static int brf_send_command_socket(int fd, struct bts_action_send *send_action)
{
	char response[1024] = {0};
	hci_command_hdr *cmd = (hci_command_hdr *) send_action->data;
	uint16_t opcode = cmd->opcode;

	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf    = cmd_opcode_ogf(opcode);
	rq.ocf    = cmd_opcode_ocf(opcode);
	rq.event  = EVT_CMD_COMPLETE;
	rq.cparam = &send_action->data[3];
	rq.clen   = send_action->data[2];
	rq.rparam = response;
	rq.rlen   = sizeof(response);

	if (hci_send_req(fd, &rq, 15) < 0) {
		perror("Cannot send hci command to socket");
		return -1;
	}

	/* verify success */
	if (response[0]) {
		errno = EIO;
		return -1;
	}

	return 0;
}

static int brf_send_command_file(int fd, struct bts_action_send *send_action,
								long size)
{
	unsigned char response[1024] = {0};
	long ret = 0;

	/* send command */
	if (size != write(fd, send_action, size)) {
		perror("Texas: Failed to write action command");
		return -1;
	}

	/* read response */
	ret = read_hci_event(fd, response, sizeof(response));
	if (ret < 0) {
		perror("texas: failed to read command response");
		return -1;
	}

	/* verify success */
	if (ret < 7 || 0 != response[6]) {
		fprintf( stderr, "TI init command failed.\n" );
		errno = EIO;
		return -1;
	}

	return 0;
}


static int brf_send_command(int fd, struct bts_action_send *send_action,
						long size, int hcill_installed)
{
	int ret = 0;
	char *fixed_action;

	/* remove packet type when giving to socket API */
	if (hcill_installed) {
		fixed_action = ((char *) send_action) + 1;
		ret = brf_send_command_socket(fd, (struct bts_action_send *) fixed_action);
	} else {
		ret = brf_send_command_file(fd, send_action, size);
	}

	return ret;
}

static int brf_do_action(uint16_t brf_type, uint8_t *brf_action, long brf_size,
				int fd, int *speed, struct termios *ti, int hcill_installed)
{
	int ret = 0;

	switch (brf_type) {
	case ACTION_SEND_COMMAND:
		DPRINTF("W");
		ret = brf_send_command(fd,
					(struct bts_action_send *) brf_action,
					brf_size, hcill_installed);
		break;
	case ACTION_WAIT_EVENT:
		DPRINTF("R");
		break;
	case ACTION_SERIAL:
		DPRINTF("S");
		ret = brf_set_serial_params((struct bts_action_serial *) brf_action, fd, speed, ti);
		break;
	case ACTION_DELAY:
		DPRINTF("D");
		brf_delay((struct bts_action_delay *) brf_action);
		break;
	case ACTION_REMARKS:
		DPRINTF("C");
		break;
	default:
		fprintf(stderr, "brf_init: unknown firmware action type (%d)\n", brf_type);
		break;
	}

	return ret;
}

/*
 * tests whether a given brf action is a HCI_VS_Sleep_Mode_Configurations cmd
 */
static int brf_action_is_deep_sleep(uint8_t *brf_action, long brf_size,
							uint16_t brf_type)
{
	uint16_t opcode;

	if (brf_type != ACTION_SEND_COMMAND)
		return 0;

	if (brf_size < 3)
		return 0;

	if (brf_action[0] != HCI_COMMAND_PKT)
		return 0;

	/* HCI data is little endian */
	opcode = brf_action[1] | (brf_action[2] << 8);

	if (opcode != BRF_DEEP_SLEEP_OPCODE)
		return 0;

	/* action is deep sleep configuration command ! */
	return 1;
}

/*
 * This function is called twice.
 * The first time it is called, it loads the brf script, and executes its
 * commands until it reaches a deep sleep command (or its end).
 * The second time it is called, it assumes HCILL protocol is set up,
 * and sends rest of brf script via the supplied socket.
 */
static int brf_do_script(int fd, int *speed, struct termios *ti, const char *bts_file)
{
	int ret = 0,  hcill_installed = bts_file ? 0 : 1;
	uint32_t vers;
	static FILE *brf_script_file = NULL;
	static uint8_t brf_action[512];
	static long brf_size;
	static uint16_t brf_type;

	/* is it the first time we are called ? */
	if (0 == hcill_installed) {
		DPRINTF("Sending script to serial device\n");
		brf_script_file = bts_load_script(bts_file, &vers );
		if (!brf_script_file) {
			fprintf(stderr, "Warning: cannot find BTS file: %s\n",
					bts_file);
			return 0;
		}

		fprintf( stderr, "Loaded BTS script version %u\n", vers );

		brf_size = bts_fetch_action(brf_script_file, brf_action,
						sizeof(brf_action), &brf_type);
		if (brf_size == 0) {
			fprintf(stderr, "Warning: BTS file is empty !");
			return 0;
		}
	}
	else {
		DPRINTF("Sending script to bluetooth socket\n");
	}

	/* execute current action and continue to parse brf script file */
	while (brf_size != 0) {
		ret = brf_do_action(brf_type, brf_action, brf_size,
						fd, speed, ti, hcill_installed);
		if (ret == -1)
			break;

		brf_size = bts_fetch_action(brf_script_file, brf_action,
						sizeof(brf_action), &brf_type);

		/* if this is the first time we run (no HCILL yet) */
		/* and a deep sleep command is encountered */
		/* we exit */
		if (!hcill_installed &&
				brf_action_is_deep_sleep(brf_action,
							brf_size, brf_type))
			return 0;
	}

	bts_unload_script(brf_script_file);
	brf_script_file = NULL;
	DPRINTF("\n");

	return ret;
}

int texas_init(int fd, int *speed, struct termios *ti)
{
	struct timespec tm = {0, 50000};
	char cmd[4];
	unsigned char resp[100];		/* Response */
	const char *bts_file;
	int n;

	memset(resp,'\0', 100);

	/* It is possible to get software version with manufacturer specific
	   HCI command HCI_VS_TI_Version_Number. But the only thing you get more
	   is if this is point-to-point or point-to-multipoint module */

	/* Get Manufacturer and LMP version */
	cmd[0] = HCI_COMMAND_PKT;
	cmd[1] = 0x01;
	cmd[2] = 0x10;
	cmd[3] = 0x00;

	do {
		n = write(fd, cmd, 4);
		if (n < 0) {
			perror("Failed to write init command (READ_LOCAL_VERSION_INFORMATION)");
			return -1;
		}
		if (n < 4) {
			fprintf(stderr, "Wanted to write 4 bytes, could only write %d. Stop\n", n);
			return -1;
		}

		/* Read reply. */
		if (read_hci_event(fd, resp, 100) < 0) {
			perror("Failed to read init response (READ_LOCAL_VERSION_INFORMATION)");
			return -1;
		}

		/* Wait for command complete event for our Opcode */
	} while (resp[4] != cmd[1] && resp[5] != cmd[2]);

	/* Verify manufacturer */
	if (! is_it_texas(resp)) {
		fprintf(stderr,"ERROR: module's manufacturer is not Texas Instruments\n");
		return -1;
	}

	fprintf(stderr, "Found a Texas Instruments' chip!\n");

	bts_file = get_firmware_name(resp);
	fprintf(stderr, "Firmware file : %s\n", bts_file);

	n = brf_do_script(fd, speed, ti, bts_file);

	nanosleep(&tm, NULL);

	return n;
}

int texas_post(int fd, struct termios *ti)
{
	int dev_id, dd, ret = 0;

	sleep(1);

	dev_id = ioctl(fd, HCIUARTGETDEVICE, 0);
	if (dev_id < 0) {
		perror("cannot get device id");
		return -1;
	}

	DPRINTF("\nAdded device hci%d\n", dev_id);

	dd = hci_open_dev(dev_id);
	if (dd < 0) {
		perror("HCI device open failed");
		return -1;
	}

	if (ioctl(dd, HCIDEVUP, dev_id) < 0 && errno != EALREADY) {
		fprintf(stderr, "Can't init device hci%d: %s (%d)", dev_id,
							strerror(errno), errno);
		hci_close_dev(dd);
		return -1;
	}

	ret = brf_do_script(dd, NULL, ti, NULL);

	hci_close_dev(dd);

	return ret;
}
