#include <sys/uio.h>
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
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/param.h>
#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "hciattach.h"

#define FAILIF(x, args...) do {   \
	if (x) {					  \
		fprintf(stderr, ##args);  \
		return -1;				  \
	}							  \
} while(0)

typedef struct {
	uint8_t uart_prefix;
	hci_event_hdr hci_hdr;
	evt_cmd_complete cmd_complete;
	uint8_t status;
	uint8_t data[16];
} __attribute__((packed)) command_complete_t;

static int read_command_complete(int fd, unsigned short opcode, unsigned char len) {
	command_complete_t resp;
	/* Read reply. */
	FAILIF(read_hci_event(fd, (unsigned char *)&resp, sizeof(resp)) < 0,
		   "Failed to read response");

	/* Parse speed-change reply */
	FAILIF(resp.uart_prefix != HCI_EVENT_PKT,
		   "Error in response: not an event packet, but 0x%02x!\n",
		   resp.uart_prefix);

	FAILIF(resp.hci_hdr.evt != EVT_CMD_COMPLETE, /* event must be event-complete */
		   "Error in response: not a cmd-complete event, "
		   "but 0x%02x!\n", resp.hci_hdr.evt);

	FAILIF(resp.hci_hdr.plen < 4, /* plen >= 4 for EVT_CMD_COMPLETE */
		   "Error in response: plen is not >= 4, but 0x%02x!\n",
		   resp.hci_hdr.plen);

	/* cmd-complete event: opcode */
	FAILIF(resp.cmd_complete.opcode != (uint16_t)opcode,
		   "Error in response: opcode is 0x%04x, not 0x%04x!",
		   resp.cmd_complete.opcode, opcode);

	return resp.status == 0 ? 0 : -1;
}

typedef struct {
	uint8_t uart_prefix;
	hci_command_hdr hci_hdr;
	uint32_t speed;
} __attribute__((packed)) texas_speed_change_cmd_t;

static int texas_change_speed(int fd, uint32_t speed)
{
	return 0;
}

static int texas_load_firmware(int fd, const char *firmware) {

	int fw = open(firmware, O_RDONLY);

	fprintf(stdout, "Opening firmware file: %s\n", firmware);

	FAILIF(fw < 0,
		   "Could not open firmware file %s: %s (%d).\n",
		   firmware, strerror(errno), errno);

	fprintf(stdout, "Uploading firmware...\n");
	do {
		/* Read each command and wait for a response. */
		unsigned char data[1024];
		unsigned char cmdp[1 + sizeof(hci_command_hdr)];
		hci_command_hdr *cmd = (hci_command_hdr *)(cmdp + 1);
		int nr;
		nr = read(fw, cmdp, sizeof(cmdp));
		if (!nr)
			break;
		FAILIF(nr != sizeof(cmdp), "Could not read H4 + HCI header!\n");
		FAILIF(*cmdp != HCI_COMMAND_PKT, "Command is not an H4 command packet!\n");

		FAILIF(read(fw, data, cmd->plen) != cmd->plen,
			   "Could not read %d bytes of data for command with opcode %04x!\n",
			   cmd->plen,
			   cmd->opcode);

		{
			int nw;
#if 0
			fprintf(stdout, "\topcode 0x%04x (%d bytes of data).\n",
					cmd->opcode,
					cmd->plen);
#endif
			struct iovec iov_cmd[2];
			iov_cmd[0].iov_base = cmdp;
			iov_cmd[0].iov_len	= sizeof(cmdp);
			iov_cmd[1].iov_base = data;
			iov_cmd[1].iov_len	= cmd->plen;
			nw = writev(fd, iov_cmd, 2);
			FAILIF(nw != (int) sizeof(cmd) +	cmd->plen,
				   "Could not send entire command (sent only %d bytes)!\n",
				   nw);
		}

		/* Wait for response */
		if (read_command_complete(fd,
								  cmd->opcode,
								  cmd->plen) < 0) {
			return -1;
		}

	} while(1);
	fprintf(stdout, "Firmware upload successful.\n");

	close(fw);
	return 0;
}

int texasalt_init(int fd, int speed, struct termios *ti)
{
	struct timespec tm = {0, 50000};
	char cmd[4];
	unsigned char resp[100];		/* Response */
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
	if ((resp[11] & 0xFF) != 0x0d)
		fprintf(stderr,"WARNING : module's manufacturer is not Texas Instrument\n");

	/* Print LMP version */
	fprintf(stderr, "Texas module LMP version : 0x%02x\n", resp[10] & 0xFF);

	/* Print LMP subversion */
	{
		unsigned short lmp_subv = resp[13] | (resp[14] << 8);
		unsigned short brf_chip = (lmp_subv & 0x7c00) >> 10;
		static const char *c_brf_chip[8] = {
			"unknown",
			"unknown",
			"brf6100",
			"brf6150",
			"brf6300",
			"brf6350",
			"unknown",
			"wl1271"
		};
		char fw[100];

		fprintf(stderr, "Texas module LMP sub-version : 0x%04x\n", lmp_subv);

		fprintf(stderr,
				"\tinternal version freeze: %d\n"
				"\tsoftware version: %d\n"
				"\tchip: %s (%d)\n",
				lmp_subv & 0x7f,
				((lmp_subv & 0x8000) >> (15-3)) | ((lmp_subv & 0x380) >> 7),
				((brf_chip > 7) ? "unknown" : c_brf_chip[brf_chip]),
				brf_chip);

		sprintf(fw, "/etc/firmware/%s.bin", c_brf_chip[brf_chip]);
		texas_load_firmware(fd, fw);

		texas_change_speed(fd, speed);
	}
	nanosleep(&tm, NULL);
	return 0;
}
