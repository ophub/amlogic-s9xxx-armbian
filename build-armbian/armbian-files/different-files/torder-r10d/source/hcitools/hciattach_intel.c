/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2012 Intel Corporation. All rights reserved.
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <time.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "hciattach.h"

#ifdef INTEL_DEBUG
#define DBGPRINT(fmt, args...)	printf("DBG: " fmt "\n", ## args)
#define PRINT_PACKET(buf, len, msg)	{	\
	int i;					\
	printf("%s\n", msg);			\
	for (i = 0; i < len; i++)		\
		printf("%02X ", buf[i]);	\
	printf("\n");				\
	}
#else
#define DBGPRINT(fmt, args...)
#define PRINT_PACKET(buf, len, msg)
#endif

#define PATCH_SEQ_EXT           ".bseq"
#define PATCH_FILE_PATH         "/lib/firmware/intel/"
#define PATCH_MAX_LEN           260
#define PATCH_TYPE_CMD          1
#define PATCH_TYPE_EVT          2

#define INTEL_VER_PARAM_LEN     9
#define INTEL_MFG_PARAM_LEN     2

/**
 * A data structure for a patch entry.
 */
struct patch_entry {
	int type;
	int len;
	unsigned char data[PATCH_MAX_LEN];
};

/**
 * A structure for patch context
 */
struct patch_ctx {
	int dev;
	int fd;
	int patch_error;
	int reset_enable_patch;
};

/**
 * Send HCI command to the controller
 */
static int intel_write_cmd(int dev, unsigned char *buf, int len)
{
	int ret;

	PRINT_PACKET(buf, len, "<----- SEND CMD: ");

	ret = write(dev, buf, len);
	if (ret < 0)
		return -errno;

	if (ret != len)
		return -1;

	return ret;
}

/**
 * Read the event from the controller
 */
static int intel_read_evt(int dev, unsigned char *buf, int len)
{
	int ret;

	ret = read_hci_event(dev, buf, len);
	if (ret < 0)
		return -1;

	PRINT_PACKET(buf, ret, "-----> READ EVT: ");

	return ret;
}

/**
 * Validate HCI events
 */
static int validate_events(struct patch_entry *event,
		struct patch_entry *entry)
{
	if (event == NULL || entry == NULL) {
		DBGPRINT("invalid patch entry parameters");
		return -1;
	}

	if (event->len != entry->len) {
		DBGPRINT("lengths are mismatched:[%d|%d]",
				event->len, entry->len);
		return -1;
	}

	if (memcmp(event->data, entry->data, event->len)) {
		DBGPRINT("data is mismatched");
		return -1;
	}

	return 0;
}

/**
 * Read the next patch entry one line at a time
 */
static int get_next_patch_entry(int fd, struct patch_entry *entry)
{
	int size;
	char rb;

	if (read(fd, &rb, 1) <= 0)
		return 0;

	entry->type = rb;

	switch (entry->type) {
	case PATCH_TYPE_CMD:
		entry->data[0] = HCI_COMMAND_PKT;

		if (read(fd, &entry->data[1], 3) < 0)
			return -1;

		size = (int)entry->data[3];

		if (read(fd, &entry->data[4], size) < 0)
			return -1;

		entry->len = HCI_TYPE_LEN + HCI_COMMAND_HDR_SIZE + size;

		break;

	case PATCH_TYPE_EVT:
		entry->data[0] = HCI_EVENT_PKT;

		if (read(fd, &entry->data[1], 2) < 0)
			return -1;

		size = (int)entry->data[2];

		if (read(fd, &entry->data[3], size) < 0)
			return -1;

		entry->len = HCI_TYPE_LEN + HCI_EVENT_HDR_SIZE + size;

		break;

	default:
		fprintf(stderr, "invalid patch entry(%d)\n", entry->type);
		return -1;
	}

	return entry->len;
}

/**
 * Download the patch set to the controller and verify the event
 */
static int intel_download_patch(struct patch_ctx *ctx)
{
	int ret;
	struct patch_entry entry;
	struct patch_entry event;

	DBGPRINT("start patch downloading");

	do {
		ret = get_next_patch_entry(ctx->fd, &entry);
		if (ret <= 0) {
			ctx->patch_error = 1;
			break;
		}

		switch (entry.type) {
		case PATCH_TYPE_CMD:
			ret = intel_write_cmd(ctx->dev,
					entry.data,
					entry.len);
			if (ret <= 0) {
				fprintf(stderr, "failed to send cmd(%d)\n",
						ret);
				return ret;
			}
			break;

		case PATCH_TYPE_EVT:
			ret = intel_read_evt(ctx->dev, event.data,
					sizeof(event.data));
			if (ret <= 0) {
				fprintf(stderr, "failed to read evt(%d)\n",
						ret);
				return ret;
			}
			event.len = ret;

			if (validate_events(&event, &entry) < 0) {
				DBGPRINT("events are mismatched");
				ctx->patch_error = 1;
				return -1;
			}
			break;

		default:
			fprintf(stderr, "unknown patch type(%d)\n",
					entry.type);
			return -1;
		}
	} while (1);

	return ret;
}

static int open_patch_file(struct patch_ctx *ctx, char *fw_ver)
{
	char patch_file[PATH_MAX];

	snprintf(patch_file, PATH_MAX, "%s%s%s", PATCH_FILE_PATH,
			fw_ver, PATCH_SEQ_EXT);
	DBGPRINT("PATCH_FILE: %s", patch_file);

	ctx->fd = open(patch_file, O_RDONLY);
	if (ctx->fd < 0) {
		DBGPRINT("cannot open patch file. go to post patch");
		return -1;
	}

	return 0;
}

/**
 * Prepare the controller for patching.
 */
static int pre_patch(struct patch_ctx *ctx)
{
	int ret, i;
	struct patch_entry entry;
	char fw_ver[INTEL_VER_PARAM_LEN * 2];

	DBGPRINT("start pre_patch");

	entry.data[0] = HCI_COMMAND_PKT;
	entry.data[1] = 0x11;
	entry.data[2] = 0xFC;
	entry.data[3] = 0x02;
	entry.data[4] = 0x01;
	entry.data[5] = 0x00;
	entry.len = HCI_TYPE_LEN + HCI_COMMAND_HDR_SIZE + INTEL_MFG_PARAM_LEN;

	ret = intel_write_cmd(ctx->dev, entry.data, entry.len);
	if (ret < 0) {
		fprintf(stderr, "failed to send cmd(%d)\n", ret);
		return ret;
	}

	ret = intel_read_evt(ctx->dev, entry.data, sizeof(entry.data));
	if (ret < 0) {
		fprintf(stderr, "failed to read evt(%d)\n", ret);
		return ret;
	}
	entry.len = ret;

	if (entry.data[6] != 0x00) {
		DBGPRINT("command failed. status=%02x", entry.data[6]);
		ctx->patch_error = 1;
		return -1;
	}

	entry.data[0] = HCI_COMMAND_PKT;
	entry.data[1] = 0x05;
	entry.data[2] = 0xFC;
	entry.data[3] = 0x00;
	entry.len = HCI_TYPE_LEN + HCI_COMMAND_HDR_SIZE;

	ret = intel_write_cmd(ctx->dev, entry.data, entry.len);
	if (ret < 0) {
		fprintf(stderr, "failed to send cmd(%d)\n", ret);
		return ret;
	}

	ret = intel_read_evt(ctx->dev, entry.data, sizeof(entry.data));
	if (ret < 0) {
		fprintf(stderr, "failed to read evt(%d)\n", ret);
		return ret;
	}
	entry.len = ret;

	if (entry.data[6] != 0x00) {
		DBGPRINT("command failed. status=%02x", entry.data[6]);
		ctx->patch_error = 1;
		return -1;
	}

	for (i = 0; i < INTEL_VER_PARAM_LEN; i++)
		sprintf(&fw_ver[i*2], "%02x", entry.data[7+i]);

	if (open_patch_file(ctx, fw_ver) < 0) {
		ctx->patch_error = 1;
		return -1;
	}

	return ret;
}

/*
 * check the event is startup event
 */
static int is_startup_evt(unsigned char *buf)
{
	if (buf[1] == 0xFF && buf[2] == 0x01 && buf[3] == 0x00)
		return 1;

	return 0;
}

/**
 * Finalize the patch process and reset the controller
 */
static int post_patch(struct patch_ctx *ctx)
{
	int ret;
	struct patch_entry entry;

	DBGPRINT("start post_patch");

	entry.data[0] = HCI_COMMAND_PKT;
	entry.data[1] = 0x11;
	entry.data[2] = 0xFC;
	entry.data[3] = 0x02;
	entry.data[4] = 0x00;
	if (ctx->reset_enable_patch)
		entry.data[5] = 0x02;
	else
		entry.data[5] = 0x01;

	entry.len = HCI_TYPE_LEN + HCI_COMMAND_HDR_SIZE + INTEL_MFG_PARAM_LEN;

	ret = intel_write_cmd(ctx->dev, entry.data, entry.len);
	if (ret < 0) {
		fprintf(stderr, "failed to send cmd(%d)\n", ret);
		return ret;
	}

	ret = intel_read_evt(ctx->dev, entry.data, sizeof(entry.data));
	if (ret < 0) {
		fprintf(stderr, "failed to read evt(%d)\n", ret);
		return ret;
	}
	entry.len = ret;

	if (entry.data[6] != 0x00) {
		fprintf(stderr, "cmd failed. st=%02x\n", entry.data[6]);
		return -1;
	}

	do {
		ret = intel_read_evt(ctx->dev, entry.data,
					sizeof(entry.data));
		if (ret < 0) {
			fprintf(stderr, "failed to read cmd(%d)\n", ret);
			return ret;
		}
		entry.len = ret;
	} while (!is_startup_evt(entry.data));

	return ret;
}

/**
 * Main routine that handles the device patching process.
 */
static int intel_patch_device(struct patch_ctx *ctx)
{
	int ret;

	ret = pre_patch(ctx);
	if (ret < 0) {
		if (!ctx->patch_error) {
			fprintf(stderr, "I/O error: pre_patch failed\n");
			return ret;
		}

		DBGPRINT("patch failed. proceed to post patch");
		goto post_patch;
	}

	ret = intel_download_patch(ctx);
	if (ret < 0) {
		if (!ctx->patch_error) {
			fprintf(stderr, "I/O error: download_patch failed\n");
			close(ctx->fd);
			return ret;
		}
	} else {
		DBGPRINT("patch done");
		ctx->reset_enable_patch = 1;
	}

	close(ctx->fd);

post_patch:
	ret = post_patch(ctx);
	if (ret < 0) {
		fprintf(stderr, "post_patch failed(%d)\n", ret);
		return ret;
	}

	return 0;
}

static int set_rts(int dev, int rtsval)
{
	int arg;

	if (ioctl(dev, TIOCMGET, &arg) < 0) {
		perror("cannot get TIOCMGET");
		return -errno;
	}
	if (rtsval)
		arg |= TIOCM_RTS;
	else
		arg &= ~TIOCM_RTS;

	if (ioctl(dev, TIOCMSET, &arg) == -1) {
		perror("cannot set TIOCMGET");
		return -errno;
	}

	return 0;
}

static unsigned char get_intel_speed(int speed)
{
	switch (speed) {
	case 9600:
		return 0x00;
	case 19200:
		return 0x01;
	case 38400:
		return 0x02;
	case 57600:
		return 0x03;
	case 115200:
		return 0x04;
	case 230400:
		return 0x05;
	case 460800:
		return 0x06;
	case 921600:
		return 0x07;
	case 1843200:
		return 0x08;
	case 3250000:
		return 0x09;
	case 2000000:
		return 0x0A;
	case 3000000:
		return 0x0B;
	default:
		return 0xFF;
	}
}

/**
 * if it failed to change to new baudrate, it will rollback
 * to initial baudrate
 */
static int change_baudrate(int dev, int init_speed, int *speed,
				struct termios *ti)
{
	int ret;
	unsigned char br;
	unsigned char cmd[5];
	unsigned char evt[7];

	DBGPRINT("start baudrate change");

	ret = set_rts(dev, 0);
	if (ret < 0) {
		fprintf(stderr, "failed to clear RTS\n");
		return ret;
	}

	cmd[0] = HCI_COMMAND_PKT;
	cmd[1] = 0x06;
	cmd[2] = 0xFC;
	cmd[3] = 0x01;

	br = get_intel_speed(*speed);
	if (br == 0xFF) {
		fprintf(stderr, "speed %d is not supported\n", *speed);
		return -1;
	}
	cmd[4] = br;

	ret = intel_write_cmd(dev, cmd, sizeof(cmd));
	if (ret < 0) {
		fprintf(stderr, "failed to send cmd(%d)\n", ret);
		return ret;
	}

	/*
	 *  wait for buffer to be consumed by the controller
	 */
	usleep(300000);

	if (set_speed(dev, ti, *speed) < 0) {
		fprintf(stderr, "can't set to new baud rate\n");
		return -1;
	}

	ret = set_rts(dev, 1);
	if (ret < 0) {
		fprintf(stderr, "failed to set RTS\n");
		return ret;
	}

	ret = intel_read_evt(dev, evt, sizeof(evt));
	if (ret < 0) {
		fprintf(stderr, "failed to read evt(%d)\n", ret);
		return ret;
	}

	if (evt[4] != 0x00) {
		fprintf(stderr,
			"failed to change speed. use default speed %d\n",
			init_speed);
		*speed = init_speed;
	}

	return 0;
}

/**
 * An entry point for Intel specific initialization
 */
int intel_init(int dev, int init_speed, int *speed, struct termios *ti)
{
	int ret = 0;
	struct patch_ctx ctx;

	if (change_baudrate(dev, init_speed, speed, ti) < 0)
		return -1;

	ctx.dev = dev;
	ctx.patch_error = 0;
	ctx.reset_enable_patch = 0;

	ret = intel_patch_device(&ctx);
	if (ret < 0)
		fprintf(stderr, "failed to initialize the device");

	return ret;
}
