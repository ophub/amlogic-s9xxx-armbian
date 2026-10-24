/*
 *  Copyright (c) 2009-2010 Atheros Communications Inc.
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "hciattach.h"

#define TRUE    1
#define FALSE   0

#define FW_PATH "/lib/firmware/ar3k/"

struct ps_cfg_entry {
	uint32_t id;
	uint32_t len;
	uint8_t *data;
};

struct ps_entry_type {
	unsigned char type;
	unsigned char array;
};

#define MAX_TAGS              50
#define PS_HDR_LEN            4
#define HCI_VENDOR_CMD_OGF    0x3F
#define HCI_PS_CMD_OCF        0x0B

struct ps_cfg_entry ps_list[MAX_TAGS];

static void load_hci_ps_hdr(uint8_t *cmd, uint8_t ps_op, int len, int index)
{
	hci_command_hdr *ch = (void *)cmd;

	ch->opcode = htobs(cmd_opcode_pack(HCI_VENDOR_CMD_OGF,
						HCI_PS_CMD_OCF));
	ch->plen = len + PS_HDR_LEN;
	cmd += HCI_COMMAND_HDR_SIZE;

	cmd[0] = ps_op;
	cmd[1] = index;
	cmd[2] = index >> 8;
	cmd[3] = len;
}

#define PS_EVENT_LEN 100

/*
 * Send HCI command and wait for command complete event.
 * The event buffer has to be freed by the caller.
 */
static int send_hci_cmd_sync(int dev, uint8_t *cmd, int len, uint8_t **event)
{
	int err;
	uint8_t *hci_event;
	uint8_t pkt_type = HCI_COMMAND_PKT;

	if (len == 0)
		return len;

	if (write(dev, &pkt_type, 1) != 1)
		return -EILSEQ;
	if (write(dev, (unsigned char *)cmd, len) != len)
		return -EILSEQ;

	hci_event = (uint8_t *)malloc(PS_EVENT_LEN);
	if (!hci_event)
		return -ENOMEM;

	err = read_hci_event(dev, (unsigned char *)hci_event, PS_EVENT_LEN);
	if (err > 0) {
		*event = hci_event;
	} else {
		free(hci_event);
		return -EILSEQ;
	}

	return len;
}

#define HCI_EV_SUCCESS        0x00

static int read_ps_event(uint8_t *event, uint16_t ocf)
{
	hci_event_hdr *eh;
	uint16_t opcode = htobs(cmd_opcode_pack(HCI_VENDOR_CMD_OGF, ocf));

	event++;

	eh = (void *)event;
	event += HCI_EVENT_HDR_SIZE;

	if (eh->evt == EVT_CMD_COMPLETE) {
		evt_cmd_complete *cc = (void *)event;

		event += EVT_CMD_COMPLETE_SIZE;

		if (cc->opcode == opcode && event[0] == HCI_EV_SUCCESS)
			return 0;
		else
			return -EILSEQ;
	}

	return -EILSEQ;
}

static int write_cmd(int fd, uint8_t *buffer, int len)
{
	uint8_t *event;
	int err;

	err = send_hci_cmd_sync(fd, buffer, len, &event);
	if (err < 0)
		return err;

	err = read_ps_event(event, HCI_PS_CMD_OCF);

	free(event);

	return err;
}

#define PS_WRITE           1
#define PS_RESET           2
#define WRITE_PATCH        8
#define ENABLE_PATCH       11

#define HCI_PS_CMD_HDR_LEN 7

#define PS_RESET_PARAM_LEN 6
#define HCI_MAX_CMD_SIZE   260
#define PS_RESET_CMD_LEN   (HCI_PS_CMD_HDR_LEN + PS_RESET_PARAM_LEN)

#define PS_ID_MASK         0xFF

/* Sends PS commands using vendor specficic HCI commands */
static int write_ps_cmd(int fd, uint8_t opcode, uint32_t ps_param)
{
	uint8_t cmd[HCI_MAX_CMD_SIZE];
	uint32_t i;

	switch (opcode) {
	case ENABLE_PATCH:
		load_hci_ps_hdr(cmd, opcode, 0, 0x00);

		if (write_cmd(fd, cmd, HCI_PS_CMD_HDR_LEN) < 0)
			return -EILSEQ;
		break;

	case PS_RESET:
		load_hci_ps_hdr(cmd, opcode, PS_RESET_PARAM_LEN, 0x00);

		cmd[7] = 0x00;
		cmd[PS_RESET_CMD_LEN - 2] = ps_param & PS_ID_MASK;
		cmd[PS_RESET_CMD_LEN - 1] = (ps_param >> 8) & PS_ID_MASK;

		if (write_cmd(fd, cmd, PS_RESET_CMD_LEN) < 0)
			return -EILSEQ;
		break;

	case PS_WRITE:
		for (i = 0; i < ps_param; i++) {
			load_hci_ps_hdr(cmd, opcode, ps_list[i].len,
							ps_list[i].id);

			memcpy(&cmd[HCI_PS_CMD_HDR_LEN], ps_list[i].data,
							ps_list[i].len);

			if (write_cmd(fd, cmd, ps_list[i].len +
						HCI_PS_CMD_HDR_LEN) < 0)
				return -EILSEQ;
		}
		break;
	}

	return 0;
}

#define __is_delim(ch) ((ch) == ':')
#define MAX_PREAMBLE_LEN 4

/* Parse PS entry preamble of format [X:X] for main type and subtype */
static int get_ps_type(char *ptr, int index, char *type, char *sub_type)
{
	int i;
	int delim = FALSE;

	if (index > MAX_PREAMBLE_LEN)
		return -EILSEQ;

	for (i = 1; i < index; i++) {
		if (__is_delim(ptr[i])) {
			delim = TRUE;
			continue;
		}

		if (isalpha(ptr[i])) {
			if (delim == FALSE)
				(*type) = toupper(ptr[i]);
			else
				(*sub_type) = toupper(ptr[i]);
		}
	}

	return 0;
}

#define ARRAY   'A'
#define STRING  'S'
#define DECIMAL 'D'
#define BINARY  'B'

#define PS_HEX           0
#define PS_DEC           1

static int get_input_format(char *buf, struct ps_entry_type *format)
{
	char *ptr = NULL;
	char type = '\0';
	char sub_type = '\0';

	format->type = PS_HEX;
	format->array = TRUE;

	if (strstr(buf, "[") != buf)
		return 0;

	ptr = strstr(buf, "]");
	if (!ptr)
		return -EILSEQ;

	if (get_ps_type(buf, ptr - buf, &type, &sub_type) < 0)
		return -EILSEQ;

	/* Check is data type is of array */
	if (type == ARRAY || sub_type == ARRAY)
		format->array = TRUE;

	if (type == STRING || sub_type == STRING)
		format->array = FALSE;

	if (type == DECIMAL || type == BINARY)
		format->type = PS_DEC;
	else
		format->type = PS_HEX;

	return 0;
}

#define UNDEFINED 0xFFFF

static unsigned int read_data_in_section(char *buf, struct ps_entry_type type)
{
	char *ptr = buf;

	if (!buf)
		return UNDEFINED;

	if (buf == strstr(buf, "[")) {
		ptr = strstr(buf, "]");
		if (!ptr)
			return UNDEFINED;

		ptr++;
	}

	if (type.type == PS_HEX && type.array != TRUE)
		return strtol(ptr, NULL, 16);

	return UNDEFINED;
}

struct tag_info {
	unsigned section;
	unsigned line_count;
	unsigned char_cnt;
	unsigned byte_count;
};

static inline int update_char_count(const char *buf)
{
	char *end_ptr;

	if (strstr(buf, "[") == buf) {
		end_ptr = strstr(buf, "]");
		if (!end_ptr)
			return 0;
		else
			return (end_ptr - buf) + 1;
	}

	return 0;
}

/* Read PS entries as string, convert and add to Hex array */
static void update_tag_data(struct ps_cfg_entry *tag,
				struct tag_info *info, const char *ptr)
{
	char buf[3];

	buf[2] = '\0';

	strncpy(buf, &ptr[info->char_cnt], 2);
	tag->data[info->byte_count] = strtol(buf, NULL, 16);
	info->char_cnt += 3;
	info->byte_count++;

	strncpy(buf, &ptr[info->char_cnt], 2);
	tag->data[info->byte_count] = strtol(buf, NULL, 16);
	info->char_cnt += 3;
	info->byte_count++;
}

#define PS_UNDEF   0
#define PS_ID      1
#define PS_LEN     2
#define PS_DATA    3

#define PS_MAX_LEN         500
#define LINE_SIZE_MAX      (PS_MAX_LEN * 2)
#define ENTRY_PER_LINE     16

#define __check_comment(buf) (((buf)[0] == '/') && ((buf)[1] == '/'))
#define __skip_space(str)      while (*(str) == ' ') ((str)++)

static int ath_parse_ps(FILE *stream)
{
	char buf[LINE_SIZE_MAX + 1];
	char *ptr;
	uint8_t tag_cnt = 0;
	int16_t byte_count = 0;
	struct ps_entry_type format;
	struct tag_info status = { 0, 0, 0, 0 };

	do {
		int read_count;
		struct ps_cfg_entry *tag;

		ptr = fgets(buf, LINE_SIZE_MAX, stream);
		if (!ptr)
			break;

		__skip_space(ptr);
		if (__check_comment(ptr))
			continue;

		/* Lines with a '#' will be followed by new PS entry */
		if (ptr == strstr(ptr, "#")) {
			if (status.section != PS_UNDEF) {
				return -EILSEQ;
			} else {
				status.section = PS_ID;
				continue;
			}
		}

		tag = &ps_list[tag_cnt];

		switch (status.section) {
		case PS_ID:
			if (get_input_format(ptr, &format) < 0)
				return -EILSEQ;

			tag->id = read_data_in_section(ptr, format);
			status.section = PS_LEN;
			break;

		case PS_LEN:
			if (get_input_format(ptr, &format) < 0)
				return -EILSEQ;

			byte_count = read_data_in_section(ptr, format);
			if (byte_count > PS_MAX_LEN)
				return -EILSEQ;

			tag->len = byte_count;
			tag->data = (uint8_t *)malloc(byte_count);

			status.section = PS_DATA;
			status.line_count = 0;
			break;

		case PS_DATA:
			if (status.line_count == 0)
				if (get_input_format(ptr, &format) < 0)
					return -EILSEQ;

			__skip_space(ptr);

			status.char_cnt = update_char_count(ptr);

			read_count = (byte_count > ENTRY_PER_LINE) ?
					ENTRY_PER_LINE : byte_count;

			if (format.type == PS_HEX && format.array == TRUE) {
				while (read_count > 0) {
					update_tag_data(tag, &status, ptr);
					read_count -= 2;
				}

				if (byte_count > ENTRY_PER_LINE)
					byte_count -= ENTRY_PER_LINE;
				else
					byte_count = 0;
			}

			status.line_count++;

			if (byte_count == 0)
				memset(&status, 0x00, sizeof(struct tag_info));

			if (status.section == PS_UNDEF)
				tag_cnt++;

			if (tag_cnt == MAX_TAGS)
				return -EILSEQ;
			break;
		}
	} while (ptr);

	return tag_cnt;
}

#define MAX_PATCH_CMD 244
struct patch_entry {
	int16_t len;
	uint8_t data[MAX_PATCH_CMD];
};

#define SET_PATCH_RAM_ID	0x0D
#define SET_PATCH_RAM_CMD_SIZE	11
#define ADDRESS_LEN		4
static int set_patch_ram(int dev, char *patch_loc, int len)
{
	int err;
	uint8_t cmd[20];
	int i, j;
	char loc_byte[3];
	uint8_t *event;
	uint8_t *loc_ptr = &cmd[7];

	if (!patch_loc)
		return -1;

	loc_byte[2] = '\0';

	load_hci_ps_hdr(cmd, SET_PATCH_RAM_ID, ADDRESS_LEN, 0);

	for (i = 0, j = 3; i < 4; i++, j--) {
		loc_byte[0] = patch_loc[0];
		loc_byte[1] = patch_loc[1];
		loc_ptr[j] = strtol(loc_byte, NULL, 16);
		patch_loc += 2;
	}

	err = send_hci_cmd_sync(dev, cmd, SET_PATCH_RAM_CMD_SIZE, &event);
	if (err < 0)
		return err;

	err = read_ps_event(event, HCI_PS_CMD_OCF);

	free(event);

	return err;
}

#define PATCH_LOC_KEY    "DA:"
#define PATCH_LOC_STRING_LEN    8
static int ps_patch_download(int fd, FILE *stream)
{
	char byte[3];
	char ptr[MAX_PATCH_CMD + 1];
	int byte_cnt;
	int patch_count = 0;
	char patch_loc[PATCH_LOC_STRING_LEN + 1];

	byte[2] = '\0';

	while (fgets(ptr, MAX_PATCH_CMD, stream)) {
		if (strlen(ptr) <= 1)
			continue;
		else if (strstr(ptr, PATCH_LOC_KEY) == ptr) {
			strncpy(patch_loc, &ptr[sizeof(PATCH_LOC_KEY) - 1],
							PATCH_LOC_STRING_LEN);
			if (set_patch_ram(fd, patch_loc, sizeof(patch_loc)) < 0)
				return -1;
		} else if (isxdigit(ptr[0]))
			break;
		else
			return -1;
	}

	byte_cnt = strtol(ptr, NULL, 16);

	while (byte_cnt > 0) {
		int i;
		uint8_t cmd[HCI_MAX_CMD_SIZE];
		struct patch_entry patch;

		if (byte_cnt > MAX_PATCH_CMD)
			patch.len = MAX_PATCH_CMD;
		else
			patch.len = byte_cnt;

		for (i = 0; i < patch.len; i++) {
			if (!fgets(byte, 3, stream))
				return -1;

			patch.data[i] = strtoul(byte, NULL, 16);
		}

		load_hci_ps_hdr(cmd, WRITE_PATCH, patch.len, patch_count);
		memcpy(&cmd[HCI_PS_CMD_HDR_LEN], patch.data, patch.len);

		if (write_cmd(fd, cmd, patch.len + HCI_PS_CMD_HDR_LEN) < 0)
			return -1;

		patch_count++;
		byte_cnt = byte_cnt - MAX_PATCH_CMD;
	}

	if (write_ps_cmd(fd, ENABLE_PATCH, 0) < 0)
		return -1;

	return patch_count;
}

#define PS_RAM_SIZE 2048

static int ps_config_download(int fd, int tag_count)
{
	if (write_ps_cmd(fd, PS_RESET, PS_RAM_SIZE) < 0)
		return -1;

	if (tag_count > 0)
		if (write_ps_cmd(fd, PS_WRITE, tag_count) < 0)
			return -1;
	return 0;
}

#define PS_ASIC_FILE			"PS_ASIC.pst"
#define PS_FPGA_FILE			"PS_FPGA.pst"

static void get_ps_file_name(uint32_t devtype, uint32_t rom_version,
							char *path)
{
	char *filename;

	if (devtype == 0xdeadc0de)
		filename = PS_ASIC_FILE;
	else
		filename = PS_FPGA_FILE;

	snprintf(path, MAXPATHLEN, "%s%x/%s", FW_PATH, rom_version, filename);
}

#define PATCH_FILE        "RamPatch.txt"
#define FPGA_ROM_VERSION  0x99999999
#define ROM_DEV_TYPE      0xdeadc0de

static void get_patch_file_name(uint32_t dev_type, uint32_t rom_version,
				uint32_t build_version, char *path)
{
	if (rom_version == FPGA_ROM_VERSION && dev_type != ROM_DEV_TYPE &&
					dev_type != 0 && build_version == 1)
		path[0] = '\0';
	else
		snprintf(path, MAXPATHLEN, "%s%x/%s",
				FW_PATH, rom_version, PATCH_FILE);
}

#define VERIFY_CRC   9
#define PS_REGION    1
#define PATCH_REGION 2

static int get_ath3k_crc(int dev)
{
	uint8_t cmd[7];
	uint8_t *event;
	int err;

	load_hci_ps_hdr(cmd, VERIFY_CRC, 0, PS_REGION | PATCH_REGION);

	err = send_hci_cmd_sync(dev, cmd, sizeof(cmd), &event);
	if (err < 0)
		return err;
	/* Send error code if CRC check patched */
	if (read_ps_event(event, HCI_PS_CMD_OCF) >= 0)
		err = -EILSEQ;

	free(event);

	return err;
}

#define DEV_REGISTER      0x4FFC
#define GET_DEV_TYPE_OCF  0x05

static int get_device_type(int dev, uint32_t *code)
{
	uint8_t cmd[8];
	uint8_t *event;
	uint32_t reg;
	int err;
	uint8_t *ptr = cmd;
	hci_command_hdr *ch = (void *)cmd;

	ch->opcode = htobs(cmd_opcode_pack(HCI_VENDOR_CMD_OGF,
						GET_DEV_TYPE_OCF));
	ch->plen = 5;
	ptr += HCI_COMMAND_HDR_SIZE;

	ptr[0] = (uint8_t)DEV_REGISTER;
	ptr[1] = (uint8_t)DEV_REGISTER >> 8;
	ptr[2] = (uint8_t)DEV_REGISTER >> 16;
	ptr[3] = (uint8_t)DEV_REGISTER >> 24;
	ptr[4] = 0x04;

	err = send_hci_cmd_sync(dev, cmd, sizeof(cmd), &event);
	if (err < 0)
		return err;

	err = read_ps_event(event, GET_DEV_TYPE_OCF);
	if (err < 0)
		goto cleanup;

	reg = event[10];
	reg = (reg << 8) | event[9];
	reg = (reg << 8) | event[8];
	reg = (reg << 8) | event[7];
	*code = reg;

cleanup:
	free(event);

	return err;
}

#define GET_VERSION_OCF 0x1E

static int read_ath3k_version(int pConfig, uint32_t *rom_version,
					uint32_t *build_version)
{
	uint8_t cmd[3];
	uint8_t *event;
	int err;
	int status;
	hci_command_hdr *ch = (void *)cmd;

	ch->opcode = htobs(cmd_opcode_pack(HCI_VENDOR_CMD_OGF,
						GET_VERSION_OCF));
	ch->plen = 0;

	err = send_hci_cmd_sync(pConfig, cmd, sizeof(cmd), &event);
	if (err < 0)
		return err;

	err = read_ps_event(event, GET_VERSION_OCF);
	if (err < 0)
		goto cleanup;

	status = event[10];
	status = (status << 8) | event[9];
	status = (status << 8) | event[8];
	status = (status << 8) | event[7];
	*rom_version = status;

	status = event[14];
	status = (status << 8) | event[13];
	status = (status << 8) | event[12];
	status = (status << 8) | event[11];
	*build_version = status;

cleanup:
	free(event);

	return err;
}

static void convert_bdaddr(char *str_bdaddr, char *bdaddr)
{
	char bdbyte[3];
	char *str_byte = str_bdaddr;
	int i, j;
	int colon_present = 0;

	if (strstr(str_bdaddr, ":"))
		colon_present = 1;

	bdbyte[2] = '\0';

	/* Reverse the BDADDR to LSB first */
	for (i = 0, j = 5; i < 6; i++, j--) {
		bdbyte[0] = str_byte[0];
		bdbyte[1] = str_byte[1];
		bdaddr[j] = strtol(bdbyte, NULL, 16);

		if (colon_present == 1)
			str_byte += 3;
		else
			str_byte += 2;
	}
}

static int write_bdaddr(int pConfig, char *bdaddr)
{
	uint8_t *event;
	int err;
	uint8_t cmd[13];
	uint8_t *ptr = cmd;
	hci_command_hdr *ch = (void *)cmd;

	memset(cmd, 0, sizeof(cmd));

	ch->opcode = htobs(cmd_opcode_pack(HCI_VENDOR_CMD_OGF,
						HCI_PS_CMD_OCF));
	ch->plen = 10;
	ptr += HCI_COMMAND_HDR_SIZE;

	ptr[0] = 0x01;
	ptr[1] = 0x01;
	ptr[2] = 0x00;
	ptr[3] = 0x06;

	convert_bdaddr(bdaddr, (char *)&ptr[4]);

	err = send_hci_cmd_sync(pConfig, cmd, sizeof(cmd), &event);
	if (err < 0)
		return err;

	err = read_ps_event(event, HCI_PS_CMD_OCF);

	free(event);

	return err;
}

#define BDADDR_FILE "ar3kbdaddr.pst"

static void write_bdaddr_from_file(int rom_version, int fd)
{
	FILE *stream;
	char bdaddr[PATH_MAX];
	char bdaddr_file[PATH_MAX];

	snprintf(bdaddr_file, MAXPATHLEN, "%s%x/%s",
			FW_PATH, rom_version, BDADDR_FILE);

	stream = fopen(bdaddr_file, "r");
	if (!stream)
		return;

	if (fgets(bdaddr, PATH_MAX - 1, stream))
		write_bdaddr(fd, bdaddr);

	fclose(stream);
}

static int ath_ps_download(int fd)
{
	int err = 0;
	int tag_count;
	int patch_count = 0;
	uint32_t rom_version = 0;
	uint32_t build_version = 0;
	uint32_t dev_type = 0;
	char patch_file[PATH_MAX];
	char ps_file[PATH_MAX];
	FILE *stream;

	/*
	 * Verfiy firmware version. depending on it select the PS
	 * config file to download.
	 */
	if (get_device_type(fd, &dev_type) < 0) {
		err = -EILSEQ;
		goto download_cmplete;
	}

	if (read_ath3k_version(fd, &rom_version, &build_version) < 0) {
		err = -EILSEQ;
		goto download_cmplete;
	}

	/* Do not download configuration if CRC passes */
	if (get_ath3k_crc(fd) < 0) {
		err = 0;
		goto download_cmplete;
	}

	get_ps_file_name(dev_type, rom_version, ps_file);
	get_patch_file_name(dev_type, rom_version, build_version, patch_file);

	stream = fopen(ps_file, "r");
	if (!stream) {
		perror("firmware file open error\n");
		err = -EILSEQ;
		goto download_cmplete;
	}
	tag_count = ath_parse_ps(stream);

	fclose(stream);

	if (tag_count < 0) {
		err = -EILSEQ;
		goto download_cmplete;
	}

	/*
	 * It is not necessary that Patch file be available,
	 * continue with PS Operations if patch file is not available.
	 */
	if (patch_file[0] == '\0')
		err = 0;

	stream = fopen(patch_file, "r");
	if (!stream)
		err = 0;
	else {
		patch_count = ps_patch_download(fd, stream);
		fclose(stream);

		if (patch_count < 0) {
			err = -EILSEQ;
			goto download_cmplete;
		}
	}

	err = ps_config_download(fd, tag_count);

download_cmplete:
	if (!err)
		write_bdaddr_from_file(rom_version, fd);

	return err;
}

#define HCI_SLEEP_CMD_OCF     0x04

/*
 * Atheros AR300x specific initialization post callback
 */
int ath3k_post(int fd, int pm)
{
	int dev_id, dd;
	struct timespec tm = { 0, 50000 };

	sleep(1);

	dev_id = ioctl(fd, HCIUARTGETDEVICE, 0);
	if (dev_id < 0) {
		perror("cannot get device id");
		return dev_id;
	}

	dd = hci_open_dev(dev_id);
	if (dd < 0) {
		perror("HCI device open failed");
		return dd;
	}

	if (ioctl(dd, HCIDEVUP, dev_id) < 0 && errno != EALREADY) {
		perror("hci down:Power management Disabled");
		hci_close_dev(dd);
		return -1;
	}

	/* send vendor specific command with Sleep feature Enabled */
	if (hci_send_cmd(dd, OGF_VENDOR_CMD, HCI_SLEEP_CMD_OCF, 1, &pm) < 0)
		perror("PM command failed, power management Disabled");

	nanosleep(&tm, NULL);
	hci_close_dev(dd);

	return 0;
}

#define HCI_VENDOR_CMD_OGF    0x3F
#define HCI_PS_CMD_OCF        0x0B
#define HCI_CHG_BAUD_CMD_OCF  0x0C

#define WRITE_BDADDR_CMD_LEN 14
#define WRITE_BAUD_CMD_LEN   6
#define MAX_CMD_LEN          WRITE_BDADDR_CMD_LEN

static int set_cntrlr_baud(int fd, int speed)
{
	int baud;
	struct timespec tm = { 0, 500000 };
	unsigned char cmd[MAX_CMD_LEN], rsp[HCI_MAX_EVENT_SIZE];
	unsigned char *ptr = cmd + 1;
	hci_command_hdr *ch = (void *)ptr;

	cmd[0] = HCI_COMMAND_PKT;

	/* set controller baud rate to user specified value */
	ptr = cmd + 1;
	ch->opcode = htobs(cmd_opcode_pack(HCI_VENDOR_CMD_OGF,
						HCI_CHG_BAUD_CMD_OCF));
	ch->plen = 2;
	ptr += HCI_COMMAND_HDR_SIZE;

	baud = speed/100;
	ptr[0] = (char)baud;
	ptr[1] = (char)(baud >> 8);

	if (write(fd, cmd, WRITE_BAUD_CMD_LEN) != WRITE_BAUD_CMD_LEN) {
		perror("Failed to write change baud rate command");
		return -ETIMEDOUT;
	}

	nanosleep(&tm, NULL);

	if (read_hci_event(fd, rsp, sizeof(rsp)) < 0)
		return -ETIMEDOUT;

	return 0;
}

/*
 * Atheros AR300x specific initialization and configuration file
 * download
 */
int ath3k_init(int fd, int speed, int init_speed, char *bdaddr,
						struct termios *ti)
{
	int r;
	int err = 0;
	struct timespec tm = { 0, 500000 };
	unsigned char cmd[MAX_CMD_LEN], rsp[HCI_MAX_EVENT_SIZE];
	unsigned char *ptr = cmd + 1;
	hci_command_hdr *ch = (void *)ptr;

	cmd[0] = HCI_COMMAND_PKT;

	/* set both controller and host baud rate to maximum possible value */
	err = set_cntrlr_baud(fd, speed);
	if (err < 0)
		return err;

	err = set_speed(fd, ti, speed);
	if (err < 0) {
		perror("Can't set required baud rate");
		return err;
	}

	/* Download PS and patch */
	r = ath_ps_download(fd);
	if (r < 0) {
		perror("Failed to Download configuration");
		err = -ETIMEDOUT;
		goto failed;
	}

	/* Write BDADDR */
	if (bdaddr) {
		ch->opcode = htobs(cmd_opcode_pack(HCI_VENDOR_CMD_OGF,
							HCI_PS_CMD_OCF));
		ch->plen = 10;
		ptr += HCI_COMMAND_HDR_SIZE;

		ptr[0] = 0x01;
		ptr[1] = 0x01;
		ptr[2] = 0x00;
		ptr[3] = 0x06;
		str2ba(bdaddr, (bdaddr_t *)(ptr + 4));

		if (write(fd, cmd, WRITE_BDADDR_CMD_LEN) !=
					WRITE_BDADDR_CMD_LEN) {
			perror("Failed to write BD_ADDR command\n");
			err = -ETIMEDOUT;
			goto failed;
		}

		if (read_hci_event(fd, rsp, sizeof(rsp)) < 0) {
			perror("Failed to set BD_ADDR\n");
			err = -ETIMEDOUT;
			goto failed;
		}
	}

	/* Send HCI Reset */
	cmd[1] = 0x03;
	cmd[2] = 0x0C;
	cmd[3] = 0x00;

	r = write(fd, cmd, 4);
	if (r != 4) {
		err = -ETIMEDOUT;
		goto failed;
	}

	nanosleep(&tm, NULL);
	if (read_hci_event(fd, rsp, sizeof(rsp)) < 0) {
		err = -ETIMEDOUT;
		goto failed;
	}

	err = set_cntrlr_baud(fd, speed);
	if (err < 0)
		return err;

failed:
	if (err < 0) {
		set_cntrlr_baud(fd, init_speed);
		set_speed(fd, ti, init_speed);
	}

	return err;
}
