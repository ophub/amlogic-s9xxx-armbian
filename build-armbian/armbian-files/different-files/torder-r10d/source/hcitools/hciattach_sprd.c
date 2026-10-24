#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
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
#define LOG_STR "SPRD Bluetooth"
#define DBG_ON   1

#define SPRD_DBG(fmt, arg...)                                \
  do {                                                     \
    if (DBG_ON)                                            \
      fprintf(stderr, "%s: " fmt "\n" , LOG_STR, ##arg);   \
  } while(0)

#define SPRD_ERR(fmt, arg...)                                \
  do {                                                     \
    fprintf(stderr, "%s ERROR: " fmt "\n", LOG_STR, ##arg);\
    perror(LOG_STR" ERROR reason");                        \
  } while(0)

#define SPRD_DUMP(buffer, len)                               \
  fprintf(stderr, "%s: ", LOG_STR);                        \
  do {                                                     \
    int i = 0;                                             \
    for (i = 0; i < len; i++) {                        \
      if (i && !(i % 16)) {                                \
        fprintf(stderr, "\n");                             \
        fprintf(stderr, "%s: ", LOG_STR);                  \
      }                                                    \
      fprintf(stderr, "%02x ", buffer[i]);                 \
    }                                                      \
    fprintf(stderr, "\n");                                 \
  } while (0)

#define CONF_ITEM_TABLE(ITEM, ACTION, BUF, LEN) \
  { #ITEM, ACTION, &(BUF.ITEM), LEN, (sizeof(BUF.ITEM) / LEN) }

#define UINT8_TO_STREAM(p, u8) \
  { *(p)++ = (uint8_t)(u8); }

#define STREAM_TO_UINT8(u8, p) \
  {                            \
    (u8) = (uint8_t)(*(p));    \
    (p) += 1;                  \
  }

#define UINT16_TO_STREAM(p, u16)    \
  {                                 \
    *(p)++ = (uint8_t)(u16);        \
    *(p)++ = (uint8_t)((u16) >> 8); \
  }

#define STREAM_TO_UINT16(u16, p)                                  \
  {                                                               \
    (u16) = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); \
    (p) += 2;                                                     \
  }

#define UINT32_TO_STREAM(p, u32)     \
  {                                  \
    *(p)++ = (uint8_t)(u32);         \
    *(p)++ = (uint8_t)((u32) >> 8);  \
    *(p)++ = (uint8_t)((u32) >> 16); \
    *(p)++ = (uint8_t)((u32) >> 24); \
  }

#define CONF_COMMENT               '#'
#define CONF_DELIMITERS            " =\n\r\t"
#define CONF_VALUES_DELIMITERS     "=\n\r\t#"
#define CONF_VALUES_PARTITION      " ,=\n\r\t#"
#define CONF_MAX_LINE_LEN          255

#define HCI_PSKEY                  0xFCA0
#define HCI_VSC_ENABLE_COMMMAND    0xFCA1
#define HCI_RF_PARA                0xFCA2

#define RESPONSE_LENGTH            100
#define HCI_CMD_MAX_LEN            258
#define HCI_EVT_CMD_CMPL_OPCODE    3
#define HCI_PACKET_TYPE_COMMAND    1
#define HCI_CMD_PREAMBLE_SIZE      3

#define FW_NODE_BYTE               6
#define FW_DATE_D_BYTE             8
#define FW_DATE_M_BYTE             9
#define FW_DATE_Y_BYTE             10

#define BT_CONFIG_PATH             "/lib/firmware"
#define BT_HC_HDR_SIZE             (sizeof(HC_BT_HDR))
#define BT_VND_OP_RESULT_SUCCESS   0
#define BT_VND_OP_RESULT_FAIL      1
#define MSG_STACK_TO_HC_HCI_CMD    0x2000
#define START_STOP_CMD_SIZE        3
#define DUAL_MODE                  0
#define DISABLE_BT                 0
#define ENABLE_BT                  1

typedef void (*hci_cback)(void *);
typedef int  (conf_action_t)(char *p_conf_name, char *p_conf_value, void *buf, int len, int size);

typedef struct {
	uint16_t event;
	uint16_t len;
	uint16_t offset;
	uint16_t layer_specific;
	uint8_t data[];
} HC_BT_HDR;

typedef struct {
	uint32_t  device_class;
	uint8_t  feature_set[16];
	uint8_t  device_addr[6];
	uint16_t  comp_id;
	uint8_t g_sys_uart0_communication_supported;
	uint8_t cp2_log_mode;
	uint8_t LogLevel;
	uint8_t g_central_or_perpheral;
	uint16_t Log_BitMask;
	uint8_t super_ssp_enable;
	uint8_t common_rfu_b3;
	uint32_t common_rfu_w[2];
	uint32_t le_rfu_w[2];
	uint32_t lmp_rfu_w[2];
	uint32_t lc_rfu_w[2];
	uint16_t g_wbs_nv_117;
	uint16_t g_wbs_nv_118;
	uint16_t g_nbv_nv_117;
	uint16_t g_nbv_nv_118;
	uint8_t g_sys_sco_transmit_mode;
	uint8_t audio_rfu_b1;
	uint8_t audio_rfu_b2;
	uint8_t audio_rfu_b3;
	uint32_t audio_rfu_w[2];
	uint8_t g_sys_sleep_in_standby_supported;
	uint8_t g_sys_sleep_master_supported;
	uint8_t g_sys_sleep_slave_supported;
	uint8_t power_rfu_b1;
	uint32_t power_rfu_w[2];
	uint32_t win_ext;
	uint8_t edr_tx_edr_delay;
	uint8_t edr_rx_edr_delay;
	uint8_t tx_delay;
	uint8_t rx_delay;
	uint32_t bb_rfu_w[2];
	uint8_t agc_mode;
	uint8_t diff_or_eq;
	uint8_t ramp_mode;
	uint8_t modem_rfu_b1;
	uint32_t modem_rfu_w[2];
	uint32_t BQB_BitMask_1;
	uint32_t BQB_BitMask_2;
	uint16_t bt_coex_threshold[8];
	uint32_t other_rfu_w[6];
} pskey_config_t;

typedef struct {
	uint16_t g_GainValue_A[6];
	uint16_t g_ClassicPowerValue_A[10];
	uint16_t g_LEPowerValue_A[16];
	uint16_t g_BRChannelpwrvalue_A[8];
	uint16_t g_EDRChannelpwrvalue_A[8];
	uint16_t g_LEChannelpwrvalue_A[8];
	uint16_t g_GainValue_B[6];
	uint16_t g_ClassicPowerValue_B[10];
	uint16_t g_LEPowerValue_B[16];
	uint16_t g_BRChannelpwrvalue_B[8];
	uint16_t g_EDRChannelpwrvalue_B[8];
	uint16_t g_LEChannelpwrvalue_B[8];
	uint16_t LE_fix_powerword;
	uint8_t Classic_pc_by_channel;
	uint8_t LE_pc_by_channel;
	uint8_t RF_switch_mode;
	uint8_t Data_Capture_Mode;
	uint8_t Analog_IQ_Debug_Mode;
	uint8_t RF_common_rfu_b3;
	uint32_t RF_common_rfu_w[5];
} rf_config_t;

typedef struct {
	const char *conf_entry;
	conf_action_t *p_action;
	void *buf;
	int len;
	int size;
} conf_entry_t;

static uint8_t local_bdaddr[6]={0x10, 0x11, 0x12, 0x13, 0x14, 0x15};
static pskey_config_t marlin3_pskey;
static rf_config_t marlin3_rf_config;
static int s_bt_fd = -1;

static const conf_entry_t marlin3_pksey_table[] = {
	CONF_ITEM_TABLE(device_class, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(feature_set, 0, marlin3_pskey, 16),
	CONF_ITEM_TABLE(device_addr, 0, marlin3_pskey, 6),
	CONF_ITEM_TABLE(comp_id, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_sys_uart0_communication_supported, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(cp2_log_mode, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(LogLevel, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_central_or_perpheral, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(Log_BitMask, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(super_ssp_enable, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(common_rfu_b3, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(common_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(le_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(lmp_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(lc_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(g_wbs_nv_117, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_wbs_nv_118, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_nbv_nv_117, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_nbv_nv_118, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_sys_sco_transmit_mode, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(audio_rfu_b1, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(audio_rfu_b2, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(audio_rfu_b3, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(audio_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(g_sys_sleep_in_standby_supported, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_sys_sleep_master_supported, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(g_sys_sleep_slave_supported, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(power_rfu_b1, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(power_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(win_ext, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(edr_tx_edr_delay, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(edr_rx_edr_delay, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(tx_delay, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(rx_delay, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(bb_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(agc_mode, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(diff_or_eq, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(ramp_mode, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(modem_rfu_b1, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(modem_rfu_w, 0, marlin3_pskey, 2),
	CONF_ITEM_TABLE(BQB_BitMask_1, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(BQB_BitMask_2, 0, marlin3_pskey, 1),
	CONF_ITEM_TABLE(bt_coex_threshold, 0, marlin3_pskey, 8),
	CONF_ITEM_TABLE(other_rfu_w, 0, marlin3_pskey, 6),
	{0, 0, 0, 0, 0}
};

static const conf_entry_t marlin3_rf_table[] = {
	CONF_ITEM_TABLE(g_GainValue_A, 0, marlin3_rf_config, 6),
	CONF_ITEM_TABLE(g_ClassicPowerValue_A, 0, marlin3_rf_config, 10),
	CONF_ITEM_TABLE(g_LEPowerValue_A, 0, marlin3_rf_config, 16),
	CONF_ITEM_TABLE(g_BRChannelpwrvalue_A, 0, marlin3_rf_config, 8),
	CONF_ITEM_TABLE(g_EDRChannelpwrvalue_A, 0, marlin3_rf_config, 8),
	CONF_ITEM_TABLE(g_LEChannelpwrvalue_A, 0, marlin3_rf_config, 8),
	CONF_ITEM_TABLE(g_GainValue_B, 0, marlin3_rf_config, 6),
	CONF_ITEM_TABLE(g_ClassicPowerValue_B, 0, marlin3_rf_config, 10),
	CONF_ITEM_TABLE(g_LEPowerValue_B, 0, marlin3_rf_config, 16),
	CONF_ITEM_TABLE(g_BRChannelpwrvalue_B, 0, marlin3_rf_config, 8),
	CONF_ITEM_TABLE(g_EDRChannelpwrvalue_B, 0, marlin3_rf_config, 8),
	CONF_ITEM_TABLE(g_LEChannelpwrvalue_B, 0, marlin3_rf_config, 8),
	CONF_ITEM_TABLE(LE_fix_powerword, 0, marlin3_rf_config, 1),
	CONF_ITEM_TABLE(Classic_pc_by_channel, 0, marlin3_rf_config, 1),
	CONF_ITEM_TABLE(LE_pc_by_channel, 0, marlin3_rf_config, 1),
	CONF_ITEM_TABLE(RF_switch_mode, 0, marlin3_rf_config, 1),
	CONF_ITEM_TABLE(Data_Capture_Mode, 0, marlin3_rf_config, 1),
	CONF_ITEM_TABLE(Analog_IQ_Debug_Mode, 0, marlin3_rf_config, 1),
	CONF_ITEM_TABLE(RF_common_rfu_b3, 0, marlin3_rf_config, 1),
	CONF_ITEM_TABLE(RF_common_rfu_w, 0, marlin3_rf_config, 5),
	{0, 0, 0, 0, 0}
};

static void log_bin_to_hexstr(uint8_t *bin, uint8_t binsz, const char *log_tag)
{
	SPRD_DBG("%s", log_tag);
	SPRD_DUMP(bin, binsz);
}

static void parse_number(char *p_conf_name, char *p_conf_value, void *buf, int len, int size)
{
	uint8_t *dest = (uint8_t *)buf;
	char *sub_value, *p;
	uint32_t value;
	(void)p_conf_name;
	sub_value = strtok_r(p_conf_value, CONF_VALUES_PARTITION, &p);
	do {
		if (sub_value == NULL)
			break;

		if (sub_value[0] == '0' && (sub_value[1] == 'x' || sub_value[1] == 'X'))
			value = strtoul(sub_value, 0, 16) & 0xFFFFFFFF;
		else
			value = strtoul(sub_value, 0, 10) & 0xFFFFFFFF;

		switch (size) {
		case sizeof(uint8_t):
			*dest = value & 0xFF;
			dest += size;
			break;

		case sizeof(uint16_t):
			*((uint16_t *)dest) = value & 0xFFFF;
			dest += size;
			break;

		case sizeof(uint32_t):
			*((uint32_t *)dest) = value & 0xFFFFFFFF;
			dest += size;
			break;

		default:
			break;
		}
		sub_value = strtok_r(NULL, CONF_VALUES_PARTITION, &p);
	} while (--len);
}

static unsigned char compare_char(unsigned char ch)
{
		unsigned char data = 0x0;

		switch(ch)
		{
			case 0:
			case '0':
				data = 0x0;
				break;
			case 1:
			case '1':
				data = 0x1;
				break;
			case 2:
			case '2':
				data = 0x2;
				break;
			case 3:
			case '3':
				data = 0x3;
				break;
			case 4:
			case '4':
				data = 0x4;
				break;
			case 5:
			case '5':
				data = 0x5;
				break;
			case 6:
			case '6':
				data = 0x6;
				break;
			case 7:
			case '7':
				data = 0x7;
				break;
			case 8:
			case '8':
				data = 0x8;
				break;
			case 9:
			case '9':
				data = 0x9;
				break;
			case 10:
			case 'a':
			case 'A':
				data = 0xA;
				break;
			case 11:
			case 'b':
			case 'B':
				data = 0xB;
				break;
			case 12:
			case 'c':
			case 'C':
				data = 0xC;
				break;
			case 13:
			case 'd':
			case 'D':
				data = 0xD;
				break;
			case 14:
			case 'e':
			case 'E':
				data = 0xE;
				break;
			case 15:
			case 'f':
			case 'F':
				data = 0xF;
				break;
		}
		return data;
}

static int parse_mac_address(const char *text, uint8_t *addr)
{
	unsigned int octet[6];
	int length = 0;
	int i;

	if (!text || strlen(text) != 17 ||
	    sscanf(text, "%2x:%2x:%2x:%2x:%2x:%2x%n",
		   &octet[0], &octet[1], &octet[2], &octet[3],
		   &octet[4], &octet[5], &length) != 6 || length != 17)
		return 0;
	for (i = 0; i < 6; i++) {
		if (octet[i] > 255 ||
		    !isxdigit((unsigned char)text[3 * i]) ||
		    !isxdigit((unsigned char)text[3 * i + 1]))
			return 0;
	}
	for (i = 0; i < 6; i++)
		addr[5 - i] = (uint8_t)octet[i];
	return 1;
}

static void set_mac_address(uint8_t *addr, const char *explicit_addr)
{
	static const char *const paths[] = {
		"/sys/class/addr_mgt/addr_bt",
		"/sys/class/net/wlan0/address",
		"/lib/firmware/wifi_mac.txt"
	};
	char text[64];
	size_t i;

	if (parse_mac_address(explicit_addr, addr))
		return;
	for (i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
		FILE *fp = fopen(paths[i], "r");
		int valid;

		if (!fp)
			continue;
		valid = fscanf(fp, "%63s", text) == 1;
		fclose(fp);
		if (valid && parse_mac_address(text, addr))
			return;
	}
	SPRD_DBG("No usable MAC source; retaining configured device address");
}

static void vnd_load_configure(const char *p_path, const conf_entry_t *entry)
{
	FILE *p_file;
	char *p_name, *p_value, *p;
	conf_entry_t *p_entry;
	char line[CONF_MAX_LINE_LEN + 1]; /* add 1 for \0 char */

	SPRD_DBG("Attempt to load conf from %s", p_path);

	if ((p_file = fopen(p_path, "r")) != NULL) {
		/* read line by line */
		while (fgets(line, CONF_MAX_LINE_LEN + 1, p_file) != NULL) {
			if (line[0] == CONF_COMMENT) continue;

			p_name = strtok_r(line, CONF_DELIMITERS, &p);

			if (NULL == p_name) {
				continue;
			}

			p_value = strtok_r(NULL, CONF_VALUES_DELIMITERS, &p);

			if (NULL == p_value) {
				SPRD_DBG("vnd_load_conf: missing value for name: %s", p_name);
				continue;
			}

			p_entry = (conf_entry_t*)entry;

			while (p_entry->conf_entry != NULL) {
				if (strcmp(p_entry->conf_entry, (const char *)p_name) == 0) {
					if (p_entry->p_action) {
						p_entry->p_action(p_name, p_value, p_entry->buf, p_entry->len,
									p_entry->size);
					} else {
						SPRD_DBG("%s -> %s", p_name, p_value);
						parse_number(p_name, p_value, p_entry->buf, p_entry->len,
									p_entry->size);
					}
					break;
				}

				p_entry++;
			}
		}

		fclose(p_file);
	} else {
		SPRD_DBG("vnd_load_conf file >%s< not found", p_path);
	}
}

static size_t H4Protocol_Send(uint8_t type, const uint8_t* data, size_t length)
{
	struct iovec iov[] = {
		{&type, sizeof(type)},
		{(uint8_t *)data, length}};

	ssize_t ret = 0;
	do {
		ret = writev(s_bt_fd, iov, sizeof(iov) / sizeof(iov[0]));
	} while (-1 == ret && EAGAIN == errno);

	if (ret == -1) {
		SPRD_ERR("%s error writing to UART (%s)", __func__, strerror(errno));
	} else if (ret < length + 1) {
		SPRD_ERR("%s: %d / %d bytes written - something went wrong...", __func__, ret, length + 1);
	}

	return ret;
}

static void *bt_vendor_alloc(int size)
{
	void *p = (uint8_t *)malloc(size);
	return p;
}

static void bt_vendor_free(void *buffer)
{
	free(buffer);
}

static uint8_t bt_vendor_xmit(uint16_t opcode, void* buffer, hci_cback callback)
{
	uint8_t type = HCI_PACKET_TYPE_COMMAND;
	(void)opcode;
	HC_BT_HDR* bt_hdr = (HC_BT_HDR *)buffer;
	H4Protocol_Send(type, bt_hdr->data, bt_hdr->len);
	return BT_VND_OP_RESULT_SUCCESS;
}

static uint8_t sprd_vnd_send_hci_vsc(uint16_t cmd, uint8_t *payload, uint8_t len, hci_cback cback)
{
	HC_BT_HDR   *p_buf;
	uint8_t     *p, ret;

	p_buf = (HC_BT_HDR *)bt_vendor_alloc(
		BT_HC_HDR_SIZE + HCI_CMD_PREAMBLE_SIZE + len);
	if (p_buf) {
		p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
		p_buf->offset = 0;
		p_buf->layer_specific = 0;
		p_buf->len = HCI_CMD_PREAMBLE_SIZE + len;
		p = (uint8_t *)(p_buf + 1);

		UINT16_TO_STREAM(p, cmd);
		*p++ = len;
		memcpy(p, payload, len);
		log_bin_to_hexstr((uint8_t *)(p_buf + 1), HCI_CMD_PREAMBLE_SIZE + len, __FUNCTION__);
		ret = bt_vendor_xmit(cmd, p_buf, cback);
		bt_vendor_free(p_buf);
		return ret;
	}
	return BT_VND_OP_RESULT_FAIL;
}

static void hw_core_cback(void *p_mem)
{
	uint8_t *p_evt_buf = (uint8_t *)p_mem;
	uint8_t *p, status;
	uint16_t opcode, mode;

	p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
	STREAM_TO_UINT16(opcode,p);
	STREAM_TO_UINT16(mode,p);
	STREAM_TO_UINT8(status,p);
	SPRD_DBG("%s hw_core_cback response: [0x%04X, 0x%04X, 0x%02X]", __func__, opcode, mode, status);
	bt_vendor_free(p_evt_buf);
}

static void hw_core_enable(unsigned char enable)
{
	uint8_t *p, msg_req[HCI_CMD_MAX_LEN];
	p = msg_req;
	UINT16_TO_STREAM(p, DUAL_MODE);
	UINT8_TO_STREAM(p, enable ? ENABLE_BT : DISABLE_BT);
	sprd_vnd_send_hci_vsc(HCI_VSC_ENABLE_COMMMAND, msg_req, (uint8_t)(p - msg_req), NULL);
}

static void hw_rf_cback(void *p_mem)
{
	uint8_t *p_evt_buf = (uint8_t *)p_mem, len;
	uint8_t *p, status;
	uint16_t opcode, mode = 0;

	p = (uint8_t *)(p_evt_buf + 1) + 1;
	STREAM_TO_UINT8(len, p);

	p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
	STREAM_TO_UINT16(opcode, p);
	if (len == 6)
		STREAM_TO_UINT16(mode, p);

	STREAM_TO_UINT8(status, p);

	SPRD_DBG("%s hw_rf_cback response: [0x%04X, 0x%04X, 0x%02X]", __func__, opcode, mode, status);
	/* Must free the RX event buffer */
	bt_vendor_free(p_evt_buf);
}

static int marlin3_rf_preload()
{
	uint8_t *p, msg_req[HCI_CMD_MAX_LEN];
	int i;

	SPRD_DBG("yujian.qin %s", __FUNCTION__);
	p = msg_req;

	for (i = 0; i < 6; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_GainValue_A[i]);

	for (i = 0; i < 10; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_ClassicPowerValue_A[i]);

	for (i = 0; i < 16; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_LEPowerValue_A[i]);

	for (i = 0; i < 8; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_BRChannelpwrvalue_A[i]);

	for (i = 0; i < 8; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_EDRChannelpwrvalue_A[i]);

	for (i = 0; i < 8; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_LEChannelpwrvalue_A[i]);

	for (i = 0; i < 6; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_GainValue_B[i]);

	for (i = 0; i < 10; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_ClassicPowerValue_B[i]);

	for (i = 0; i < 16; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_LEPowerValue_B[i]);

	for (i = 0; i < 8; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_BRChannelpwrvalue_B[i]);

	for (i = 0; i < 8; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_EDRChannelpwrvalue_B[i]);

	for (i = 0; i < 8; i++)
		UINT16_TO_STREAM(p, marlin3_rf_config.g_LEChannelpwrvalue_B[i]);

	UINT16_TO_STREAM(p, marlin3_rf_config.LE_fix_powerword);

	UINT8_TO_STREAM(p, marlin3_rf_config.Classic_pc_by_channel);
	UINT8_TO_STREAM(p, marlin3_rf_config.LE_pc_by_channel);
	UINT8_TO_STREAM(p, marlin3_rf_config.RF_switch_mode);
	UINT8_TO_STREAM(p, marlin3_rf_config.Data_Capture_Mode);
	UINT8_TO_STREAM(p, marlin3_rf_config.Analog_IQ_Debug_Mode);
	UINT8_TO_STREAM(p, marlin3_rf_config.RF_common_rfu_b3);

	for (i = 0; i < 5; i++)
		UINT32_TO_STREAM(p, marlin3_rf_config.RF_common_rfu_w[i]);

	sprd_vnd_send_hci_vsc(HCI_RF_PARA, msg_req, (uint8_t)(p - msg_req), NULL);
	return 0;
}

static void marlin3_pskey_cback(void *p_mem)
{
	uint8_t *p_evt_buf = (uint8_t *)p_mem;

	uint16_t opcode, node, year;
	uint8_t *p, month, day;
	(void)opcode;

	p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
	STREAM_TO_UINT16(opcode, p);

	p = (uint8_t *)(p_evt_buf + 1) + FW_NODE_BYTE;
	STREAM_TO_UINT16(node, p);
	p = (uint8_t *)(p_evt_buf + 1) + FW_DATE_Y_BYTE;
	STREAM_TO_UINT16(year, p);
	p = (uint8_t *)(p_evt_buf + 1) + FW_DATE_M_BYTE;
	STREAM_TO_UINT8(month, p);
	p = (uint8_t *)(p_evt_buf + 1) + FW_DATE_D_BYTE;
	STREAM_TO_UINT8(day, p);

	SPRD_DBG("Bluetooth Firmware Node: %04X Date: %04x-%02x-%02x", node, year, month, day);

	/* Must free the RX event buffer */
	bt_vendor_free(p_evt_buf);
}

static int marlin3_pskey_preload(void *arg)
{
	uint8_t *p, msg_req[HCI_CMD_MAX_LEN];
	int i;
	(void)arg;

	SPRD_DBG("%s", __FUNCTION__);
	p = msg_req;
	UINT32_TO_STREAM(p, marlin3_pskey.device_class);

	for (i = 0; i < 16; i++)
		UINT8_TO_STREAM(p, marlin3_pskey.feature_set[i]);

	for (i = 0; i < 6; i++)
		UINT8_TO_STREAM(p, marlin3_pskey.device_addr[i]);

	UINT16_TO_STREAM(p, marlin3_pskey.comp_id);
	UINT8_TO_STREAM(p, marlin3_pskey.g_sys_uart0_communication_supported);
	UINT8_TO_STREAM(p, marlin3_pskey.cp2_log_mode);
	UINT8_TO_STREAM(p, marlin3_pskey.LogLevel);
	UINT8_TO_STREAM(p, marlin3_pskey.g_central_or_perpheral);

	UINT16_TO_STREAM(p, marlin3_pskey.Log_BitMask);
	UINT8_TO_STREAM(p, marlin3_pskey.super_ssp_enable);
	UINT8_TO_STREAM(p, marlin3_pskey.common_rfu_b3);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.common_rfu_w[i]);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.le_rfu_w[i]);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.lmp_rfu_w[i]);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.lc_rfu_w[i]);

	UINT16_TO_STREAM(p, marlin3_pskey.g_wbs_nv_117);
	UINT16_TO_STREAM(p, marlin3_pskey.g_wbs_nv_118);
	UINT16_TO_STREAM(p, marlin3_pskey.g_nbv_nv_117);
	UINT16_TO_STREAM(p, marlin3_pskey.g_nbv_nv_118);

	UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sco_transmit_mode);
	UINT8_TO_STREAM(p, marlin3_pskey.audio_rfu_b1);
	UINT8_TO_STREAM(p, marlin3_pskey.audio_rfu_b2);
	UINT8_TO_STREAM(p, marlin3_pskey.audio_rfu_b3);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.audio_rfu_w[i]);

	UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sleep_in_standby_supported);
	UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sleep_master_supported);
	UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sleep_slave_supported);
	UINT8_TO_STREAM(p, marlin3_pskey.power_rfu_b1);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.power_rfu_w[i]);

	UINT32_TO_STREAM(p, marlin3_pskey.win_ext);

	UINT8_TO_STREAM(p, marlin3_pskey.edr_tx_edr_delay);
	UINT8_TO_STREAM(p, marlin3_pskey.edr_rx_edr_delay);
	UINT8_TO_STREAM(p, marlin3_pskey.tx_delay);
	UINT8_TO_STREAM(p, marlin3_pskey.rx_delay);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.bb_rfu_w[i]);

	UINT8_TO_STREAM(p, marlin3_pskey.agc_mode);
	UINT8_TO_STREAM(p, marlin3_pskey.diff_or_eq);
	UINT8_TO_STREAM(p, marlin3_pskey.ramp_mode);
	UINT8_TO_STREAM(p, marlin3_pskey.modem_rfu_b1);

	for (i = 0; i < 2; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.modem_rfu_w[i]);

	UINT32_TO_STREAM(p, marlin3_pskey.BQB_BitMask_1);
	UINT32_TO_STREAM(p, marlin3_pskey.BQB_BitMask_2);
	for (i = 0; i < 8; i++)
		UINT16_TO_STREAM(p, marlin3_pskey.bt_coex_threshold[i]);

	for (i = 0; i < 6; i++)
		UINT32_TO_STREAM(p, marlin3_pskey.other_rfu_w[i]);

	sprd_vnd_send_hci_vsc(HCI_PSKEY, msg_req, (uint8_t)(p - msg_req), NULL);
	return 0;
}


int sprd_config_init(int fd, struct uart_t *u, struct termios *ti)
{
	uint8_t *recv = NULL;
	int len = 0;

	s_bt_fd = fd;

	memset(&marlin3_pskey, 0, sizeof(marlin3_pskey));
	memset(&marlin3_rf_config, 0, sizeof(marlin3_rf_config));
	vnd_load_configure(BT_CONFIG_PATH "/bt_configure_pskey.ini", &marlin3_pksey_table[0]);
	vnd_load_configure(BT_CONFIG_PATH "/bt_configure_rf.ini", &marlin3_rf_table[0]);
	set_mac_address(marlin3_pskey.device_addr, u->bdaddr);

	marlin3_pskey_preload(NULL);
	recv = bt_vendor_alloc(RESPONSE_LENGTH);
	len = read_hci_event(s_bt_fd, recv, RESPONSE_LENGTH);
	SPRD_DBG("Received event, len: %d", len);
	SPRD_DUMP(recv, len);
	marlin3_pskey_cback(recv);

	marlin3_rf_preload();
	recv = bt_vendor_alloc(RESPONSE_LENGTH);
	len = read_hci_event(s_bt_fd, recv, RESPONSE_LENGTH);
	SPRD_DBG("Received event, len: %d", len);
	SPRD_DUMP(recv, len);
	hw_rf_cback(recv);

	hw_core_enable(1);
	recv = bt_vendor_alloc(RESPONSE_LENGTH);
	len = read_hci_event(s_bt_fd, recv, RESPONSE_LENGTH);
	SPRD_DBG("Received event, len: %d", len);
	SPRD_DUMP(recv, len);
	hw_core_cback(recv);

	return 0;
}

int sprd_config_post(int fd, struct uart_t *u, struct termios *ti)
{
	SPRD_DBG("Done setting line discpline");
	return 0;
}

