#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <limits.h>
#include "hciattach.h"

/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define LOG_STR "XRADIO Bluetooth"
#define DBG_ON   1

#define XR_DBG(fmt, arg...)                                \
  do {                                                     \
    if (DBG_ON)                                            \
      fprintf(stderr, "%s: " fmt "\n" , LOG_STR, ##arg);   \
  } while(0)

#define XR_ERR(fmt, arg...)                                \
  do {                                                     \
    fprintf(stderr, "%s ERROR: " fmt "\n", LOG_STR, ##arg);\
    perror(LOG_STR" ERROR reason");                        \
  } while(0)

#define XR_DUMP(buffer, len)                               \
  fprintf(stderr, "%s: ", LOG_STR);                        \
  do {                                                     \
    for (int i = 0; i < len; i++) {                        \
      if (i && !(i % 16)) {                                \
        fprintf(stderr, "\n");                             \
        fprintf(stderr, "%s: ", LOG_STR);                  \
      }                                                    \
      fprintf(stderr, "%02x ", buffer[i]);                 \
    }                                                      \
    fprintf(stderr, "\n");                                 \
  } while (0)

#define BT_FW_PATH_NAME                     "/system/vendor/etc/firmware/fw_xr829_bt.bin"
#define BT_FW_LOAD_ADDR                     0x0000
#define BT_FW_JUMP_ADDR                     0x0000
#define AW1722                              1
#define AW1732                              2
#define CHIP_NAME                           AW1722

#define SZ_1K                               (0x00000400U    )
#define SZ_16K                              (0x00004000U    )

#define SWAP16(d) (((d & 0xff)     <<  8) | ((d & 0xff00)     >>  8))
#define SWAP32(d) (((d & 0xff)     << 24) | ((d & 0xff00)     <<  8)  \
                 | ((d & 0xff0000) >>  8) | ((d & 0xff000000) >> 24))

#define CMD_ID(group, key)   (((group) << 3) | (key))

/*----------------------------*/
/*   COMMANDS FORM PC TO MCU  */
/*----------------------------*/
#define CMD_ID_MEMRW     0x00
#define CMD_ID_SEQRQ     0x01
#define CMD_ID_SYSCTL    0x02
#define CMD_ID_FLASH     0x03

#define CMD_ID_SEQRD     CMD_ID(CMD_ID_SEQRQ, 0)
#define CMD_ID_SEQWR     CMD_ID(CMD_ID_SEQRQ, 1)
/* uart commands */
#define CMD_ID_SETUART   CMD_ID(CMD_ID_SYSCTL, 0)
#define CMD_ID_SETPC     CMD_ID(CMD_ID_SYSCTL, 3)

#define CMD_WRITEN                0
#define CMD_WRITESEQ              1
#define CMD_SETBAUD               2
#define CMD_SETPC                 3
#define DATA_RAW                  4

/******************************************************************************
**  Type definitions
******************************************************************************/

/* vendor serial control block */
typedef struct
{
	int  fd;                     /* fd to Bluetooth device */
	struct termios *ti;          /* serial terminal of BT port */
} vnd_userial_cb_t;

#pragma pack(1)
/* command header
 *
 *    byte 0    byte 1    byte 2   byte 3     byte 4    byte 5    byte 6 -7          byte 8-11
 *  ___________________________________________________________________________________________________
 * |         |         |         |         |         |         |                   |                   |
 * |   'B'   |   'R'   |   'O'   |   'M'   |  Flags  |Reserved | Checksum          | Playload Length   |
 * |_________|_________|_________|_________|_________|_________|__________ ________|___________________|
 */
typedef struct {
	uint8_t  magic[4]; // magic "BROM"
		#define CMD_BROM_MAGIC "BROM"
	uint8_t  flags;
		#define CMD_HFLAG_ERROR   (0x1U << 0)
		#define CMD_HFLAG_ACK     (0x1U << 1)
		#define CMD_HFLAG_CHECK   (0x1U << 2)
		#define CMD_HFLAG_RETRY   (0x1U << 3)
		#define CMD_HFLAG_EXE     (0x1U << 4)
	uint8_t  version:4;
	uint8_t  reserved:4;
	uint16_t checksum;
	uint32_t payload_len;
} __attribute__((packed)) cmd_header_t;
#define MB_CMD_HEADER_SIZE (sizeof(cmd_header_t))

/* acknownledge structure */
typedef struct {
	cmd_header_t h;
	uint8_t      err;
} __attribute__((packed)) cmd_ack_t;

/* sequence read/write command structure */
typedef struct {
	cmd_header_t h;
	uint8_t      cmdid;
	uint32_t     addr;
	uint32_t     dlen;
	uint16_t     dcs;
} __attribute__((packed)) cmd_seq_wr_t;

/* io change command structure */
typedef struct {
	cmd_header_t h;
	uint8_t      cmdid;
	uint32_t     val;
} __attribute__((packed)) cmd_sys_t;

typedef struct {
	cmd_header_t h;
	uint8_t      cmdid;
	uint32_t     lcr;
} __attribute__((packed)) cmd_sys_setuart_t;

#pragma pack()

static const uint8_t hci_reset[] = { 0x01, 0x03, 0x0c, 0x00 };
static const uint8_t hci_update_baud_rate[] = { 0x01, 0x18, 0xfc, 0x04, 0x60, 0xE3, 0x16, 0x00};
static vnd_userial_cb_t vnd_userial;

static int32_t  cmd_sync_uart(void);
static int32_t  cmd_sync_baud(uint32_t lcr);
static int32_t  cmd_write_seq(uint32_t addr, uint32_t len, uint8_t *data);
static int32_t  cmd_set_pc(uint32_t pc);
static void     userial_set_hw_fctrl(uint8_t hw_fctrl);
static uint32_t userial_read(uint8_t *p_buffer, uint32_t len, uint32_t timeout);
static uint32_t userial_write(const uint8_t *p_data, uint32_t len);

/******************************************************************************
**  Functions
******************************************************************************/
static uint16_t CheckSum16(uint8_t *data, uint32_t len)
{
	uint16_t cs = 0;
	uint16_t *p = (uint16_t *)data;

	while(len > 1) {
		cs += *p++;
		len -= 2;
	}
	if (len) {
		cs += *(uint8_t *)p;
	}
	return cs;
}

static uint64_t time_gettimeofday_us(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

static uint8_t *memsearch(uint8_t *haystack, uint32_t hlen, uint8_t *needle, uint32_t nlen)
{
	while (hlen-- >= nlen) {
		if (!memcmp(haystack, needle, nlen)) {
			return haystack;
		}
		haystack++;
	}
	return NULL;
}

static int32_t xr_raw_write(int type, uint8_t *data, uint32_t len)
{
	uint8_t  buffer[MB_CMD_HEADER_SIZE + 13] = {'B', 'R', 'O', 'M', 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t *psend = data;
	uint32_t lsend = len;
	cmd_header_t *hdr = (cmd_header_t *)buffer;;
	cmd_ack_t    *ack = (cmd_ack_t *)buffer;

	if (type != DATA_RAW) {
		psend = buffer;
		lsend = MB_CMD_HEADER_SIZE + len;
		memcpy(buffer + MB_CMD_HEADER_SIZE, data, len);
		hdr->payload_len = len;
#if ENABLE_DCS
		hdr->flags       = CMD_HFLAG_CHECK;
#endif
		hdr->checksum    = ~CheckSum16(buffer, MB_CMD_HEADER_SIZE + len);
		hdr->payload_len = SWAP32(hdr->payload_len);
		hdr->checksum    = SWAP16(hdr->checksum);
		switch (type) {
			case CMD_WRITESEQ:
				{
					cmd_seq_wr_t *cmd = (cmd_seq_wr_t *)buffer;
					cmd->addr         = SWAP32(cmd->addr);
					cmd->dlen         = SWAP32(cmd->dlen);
					cmd->dcs          = SWAP16(cmd->dcs);
				}
				break;
			case CMD_SETBAUD:
				{
					cmd_sys_setuart_t *cmd = (cmd_sys_setuart_t*)buffer;
					cmd->lcr          = SWAP32(cmd->lcr);
				}
				break;
			case CMD_SETPC:
				{
					cmd_sys_t *cmd    = (cmd_sys_t*)buffer;
					cmd->val          = SWAP32(cmd->val);
				}
				break;
			default:
				XR_ERR("%s: Unsupport type %d", __func__, type);
				return -1;
		}
	}

	ssize_t ret_w, ret_r;
	uint64_t t0, t1, t2, t3;

	tcflush(vnd_userial.fd, TCIOFLUSH);

	t0    = time_gettimeofday_us();
	ret_w = userial_write(psend, lsend);
	t1    = time_gettimeofday_us();

	memset(buffer, 0, MB_CMD_HEADER_SIZE + 1);

	t2    = time_gettimeofday_us();
	ret_r = userial_read(buffer, MB_CMD_HEADER_SIZE, 100000);
	t3    = time_gettimeofday_us();

	XR_DBG("%s, type: %d, write len: %5d, ret: %5d, time: %6lluus, read len: %2d, ret: %2d, %6lluus",
			   __func__, type, lsend, ret_w, t1 - t0, MB_CMD_HEADER_SIZE, ret_r, t3 - t2);

	uint8_t *p = (uint8_t *)memsearch(buffer, MB_CMD_HEADER_SIZE, (uint8_t *)"BROM", 4);
	if (p != buffer) {
		if (p == NULL) {
			XR_ERR("%s: invalid response", __func__);
			return -1;
		}
		uint32_t nowread  = buffer + MB_CMD_HEADER_SIZE - p;
		uint32_t needread = p - buffer;
		XR_DBG("%s: Index error, re-find header magic", __func__);
		memcpy(buffer, p, nowread);
		memset(buffer + nowread, 0x0, needread);
		userial_read(buffer + nowread, needread, 100000);
	}

	/* check response */
	if (ack->h.flags & CMD_HFLAG_ERROR) {
		userial_read(buffer + MB_CMD_HEADER_SIZE, 1, 100000);
		XR_ERR("%s: resp error flag, type %d", __func__, ack->err);
		return -ack->err;
	}

	if (ack->h.flags & CMD_HFLAG_ACK) {
		/* convert network byte order to host byte order */
		ack->h.payload_len = SWAP32(ack->h.payload_len);
		ack->h.checksum    = SWAP16(ack->h.checksum);
		if (ack->h.payload_len != 0) {
			XR_ERR("%s: data payload len %d != 0", __func__, ack->h.payload_len);
			return -1;
		}
	}

	if (ack->h.flags & CMD_HFLAG_CHECK) {
		if (CheckSum16(buffer, MB_CMD_HEADER_SIZE) != 0xffff) {
			XR_ERR("%s: write data response 0 checksum error", __func__);
			return -1;
		}
	}
	return 0;
}

static int32_t cmd_sync_uart(void)
{
	uint8_t  sync   = 0x55;
	uint8_t  ack[3] = {0};
	ssize_t  ret = -1;
	uint32_t cnt = 0;

	do {
		XR_DBG("uart sync count:%d.", cnt);
		tcflush(vnd_userial.fd, TCIOFLUSH);
		userial_write(&sync, 1);
		ret = userial_read(ack, 2, 2000);
		if (ret == 2 && ((ack[0] == 'O' && ack[1] == 'K') || (ack[0] == 'K' && ack[1] == 'O'))) {
			XR_DBG("Receive %s, uart Sync done.", ack);
			return 0;
		}
	} while (cnt++ < 50);

	XR_DBG("uart sync fail.");
	return -1;
}

static int32_t cmd_sync_baud(uint32_t lcr)
{
	uint8_t  buffer[MB_CMD_HEADER_SIZE + 5];
	cmd_sys_setuart_t *cmd = (cmd_sys_setuart_t*)buffer;

	cmd->cmdid = CMD_ID_SETUART;
	cmd->lcr   = lcr;

	uint8_t  cnt = 0;
	int ret = -1;

	do {
		XR_DBG("%s count:%d.", __func__, cnt);
		ret = xr_raw_write(CMD_SETBAUD, buffer + MB_CMD_HEADER_SIZE, 5);
		if (ret == 0) {
			set_speed(vnd_userial.fd, vnd_userial.ti, lcr & 0xffffff);
			return cmd_sync_uart();
		}
	} while (cnt++ < 3);

	XR_DBG("cmd_sync_baud fail.");
	return -1;
}

static int32_t cmd_write_seq(uint32_t addr, uint32_t len, uint8_t *data)
{
	int ret = -1;
	uint8_t buffer[MB_CMD_HEADER_SIZE + 13];
	cmd_seq_wr_t *cmd = (cmd_seq_wr_t *)buffer;

	cmd->cmdid = CMD_ID_SEQWR;
	cmd->addr  = addr;
	cmd->dlen  = len;
#if ENABLE_DCS
	cmd->dcs   = ~CheckSum16(data, len);
#endif

	ret = xr_raw_write(CMD_WRITESEQ, buffer + MB_CMD_HEADER_SIZE, 11);
	if (ret == 0) {
		return xr_raw_write(DATA_RAW, data, len);
	}
	return ret;
}

static int32_t cmd_set_pc(uint32_t pc)
{
	uint8_t buffer[MB_CMD_HEADER_SIZE + 5];
	cmd_sys_t *cmd = (cmd_sys_t*)buffer;

	cmd->cmdid = CMD_ID_SETPC;
	cmd->val   = pc;
	XR_DBG("set pc %x, val %x", pc, cmd->val);

	return xr_raw_write(CMD_SETPC, buffer + MB_CMD_HEADER_SIZE, 5);
}

static void userial_set_hw_fctrl(uint8_t hw_fctrl)
{
	if (vnd_userial.fd == -1) {
		XR_ERR("vnd_userial.fd is -1");
		return;
	}

	if (hw_fctrl) {
		XR_DBG("Set HW FlowControl On");
		vnd_userial.ti->c_cflag |= CRTSCTS;
	} else {
		XR_DBG("Set HW FlowControl Off");
		vnd_userial.ti->c_cflag &= ~CRTSCTS;
	}
	tcsetattr(vnd_userial.fd, TCSANOW, vnd_userial.ti);
	tcflush(vnd_userial.fd, TCIOFLUSH);
}

static uint32_t userial_read(uint8_t *buffer, uint32_t len, uint32_t timeout)
{
	fd_set set;
	struct timeval tv;
	int rv;

	FD_ZERO(&set);                /* clear the set */
	FD_SET(vnd_userial.fd, &set); /* add our file descriptor to the set */

	/* there was data to read */
	ssize_t r;
	uint8_t *pos = (uint8_t*)buffer;

	while (len > 0) {
		tv.tv_sec  = 0;
		tv.tv_usec = timeout;

		rv = select(vnd_userial.fd + 1, &set, NULL, NULL, &tv);
		if(rv == -1) {
			XR_ERR("select error");  /* an error accured */
			break;
		} else if(rv == 0) {
			XR_ERR("read timeout");  /* a timeout occured */
			break;
		}

		r = read(vnd_userial.fd, pos, len);
		if (r < 1)
			break;

		len -= r;
		pos += r;
	}

	return pos - buffer;
}

static uint32_t userial_write(const uint8_t *buffer, uint32_t len)
{
	ssize_t r;
	uint8_t *pos = (uint8_t*)buffer;

	while (len > 0) {
		r = write(vnd_userial.fd, pos, len);
		if (r < 1)
			break;

		len -= r;
		pos += r;
	}

	return pos - buffer;
}

static int32_t load_btfirmware(void)
{
	FILE       *fwfile_fd = NULL;
	uint32_t    len;
	uint8_t    *data = NULL;
	uint32_t    addr = BT_FW_LOAD_ADDR;
	uint32_t    section = SZ_16K;

	fwfile_fd = fopen(BT_FW_PATH_NAME, "rb");
	XR_DBG("BT firmware: %s", BT_FW_PATH_NAME);
	if(!fwfile_fd) {
		XR_ERR("Unable to open BT firmware %s", BT_FW_PATH_NAME);
		return -1;
	}

	data = (uint8_t*)malloc(section);
	if (data == NULL) {
		XR_DBG("failed to alloc %d byte memory.", section);
		fclose(fwfile_fd);
		return -1;
	}

	XR_DBG("load bt firmware starting.");
	while ((len = fread(data, 1, section, fwfile_fd)) > 0) {
		cmd_write_seq(addr, len, data);
		addr += len;
	}

	free(data);
	fclose(fwfile_fd);
	XR_DBG("load firmware done.");

	XR_DBG("Firmware run from address 0x%08X", BT_FW_JUMP_ADDR);
	cmd_set_pc(BT_FW_JUMP_ADDR);

	if (CHIP_NAME == AW1732) {
		XR_DBG("second time sync starting....");
		if (cmd_sync_uart() < 0)
			return -1;
		cmd_set_pc(BT_FW_JUMP_ADDR);
	}
	return addr;
}

static int hci_cmd_handle(const uint8_t *cmd, uint32_t cmd_len, uint32_t event_len)
{
	uint8_t buffer[256];

	XR_DBG("send hci command");
	userial_write(cmd, cmd_len);
	XR_DUMP(cmd, cmd_len);

	if (read_hci_event(vnd_userial.fd, buffer, event_len) != event_len) {
		XR_ERR("Event read error");
		return -1;
	}
	XR_DBG("Received event");
	XR_DUMP(buffer, event_len);
	return 0;
}

int xr_init(int fd, struct uart_t *u, struct termios *ti)
{
	vnd_userial.fd = fd;
	vnd_userial.ti = ti;

	XR_DBG("uart sync starting....");
	if (cmd_sync_uart() < 0)
		goto END;
	XR_DBG("set bandrate to %d.", u->speed);
	if (cmd_sync_baud(((u->speed) | (3<<24))) < 0)
		goto END;
	if (load_btfirmware() < 0)
		goto END;
	XR_DBG("bt firmware is running....");

	XR_DBG("set baudrate to %d", u->init_speed);
	set_speed(vnd_userial.fd, vnd_userial.ti, u->init_speed);
	userial_set_hw_fctrl(1);
	usleep(50000);

	XR_DBG("process hci reset...");
	if (hci_cmd_handle(hci_reset, sizeof(hci_reset), 7) < 0)
		goto END;

	XR_DBG("process hci update baud...");
	if (hci_cmd_handle(hci_update_baud_rate, sizeof(hci_update_baud_rate), 7) < 0)
		goto END;

	usleep(100000);

	return 0;

END:
	XR_DBG("device fd = %d close", fd);
	close(vnd_userial.fd);
	vnd_userial.fd = -1;
	vnd_userial.ti = NULL;

	return -1;
}

int xr_post(int fd, struct uart_t *u, struct termios *ti)
{
	XR_DBG("Done setting line discpline");
	return 0;
}

