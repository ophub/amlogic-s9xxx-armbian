/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2005-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2013-2014  Realtek Semiconductor Corp.
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
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <signal.h>
#define Config_Android 0 /*1 for android; 0 for Linux*/

#if Config_Android   //for Android
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#else //for Linux
#include <stdint.h>
#include <string.h>
#include <endian.h>
#include <byteswap.h>
#include <netinet/in.h>
#endif

#include "hciattach.h"

#define BAUDRATE_4BYTES
#define RTK_VERSION "2.3"
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_le16(d)  (d)
#define cpu_to_le32(d)  (d)
#define le16_to_cpu(d)  (d)
#define le32_to_cpu(d)  (d)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_le16(d)  bswap_16(d)
#define cpu_to_le32(d)  bswap_32(d)
#define le16_to_cpu(d)  bswap_16(d)
#define le32_to_cpu(d)  bswap_32(d)
#else
#error "Unknown byte order"
#endif

/* log related */
#define LOG_STR                    "Realtek Bluetooth"
#define DBG_ON                     1

#define RS_DBG(fmt, arg...)                                    \
	do {                                                       \
		if (DBG_ON)                                            \
			fprintf(stderr, "%s: " fmt "\n", LOG_STR, ##arg);  \
	} while (0)

#define RS_ERR(fmt, arg...)                                    \
	do {                                                       \
		fprintf(stderr, "%s ERROR: " fmt "\n", LOG_STR, ##arg);\
		perror("reason: ");                                    \
	} while (0)

#define RS_DUMP(buffer, len)                                   \
	do {                                                       \
		fprintf(stderr, "%s: ", LOG_STR);                      \
		for (int i = 0; i < len; i++) {                        \
			if (i && !(i % 16)) {                              \
				fprintf(stderr, "\n");                         \
				fprintf(stderr, "%s: ", LOG_STR);              \
			}                                                  \
			fprintf(stderr, "%02x ", buffer[i]);               \
		}                                                      \
		fprintf(stderr, "\n");                                 \
	} while (0)

struct sk_buff {
	uint32_t max_len;
	uint32_t data_len;
	uint8_t  data[0];
};

/* Skb helpers */
struct bt_skb_cb {
	uint8_t  pkt_type;
	uint8_t  incoming;
	uint16_t expect;
	uint8_t  tx_seq;
	uint8_t  retries;
	uint8_t  sar;
	uint16_t channel;
};

typedef struct {
	uint8_t index;
	uint8_t data[252];
} __attribute__ ((packed)) download_vendor_patch_cp;

struct hci_command_hdr {
	uint16_t opcode;     /* OCF & OGF */
	uint8_t  plen;
} __attribute__ ((packed));

struct hci_event_hdr {
	uint8_t  evt;
	uint8_t  plen;
} __attribute__ ((packed));

struct hci_ev_cmd_complete {
	uint8_t  ncmd;
	uint16_t opcode;
} __attribute__ ((packed));

#define HCI_COMMAND_HDR_SIZE               3
#define HCI_EVENT_HDR_SIZE                 2

#define HCI_CMD_READ_BD_ADDR               0x1009
#define HCI_VENDOR_CHANGE_BDRATE           0xfc17
#define HCI_VENDOR_READ_RTK_ROM_VERISION   0xfc6d
#define HCI_VENDOR_READ_LMP_VERISION       0x1001
#define HCI_VENDOR_READ_CHIP_TYPE          0xfc61

#define ROM_LMP_8723a                      0x1200

#define RTK_VENDOR_CONFIG_MAGIC            0x8723ab55

#define bt_cb(skb)                         ((struct bt_skb_cb *)((skb)->cb))

#if Config_Android
#define FIRMWARE_DIRECTORY                 "/system/vendor/etc/firmware"
#define BT_CONFIG_DIRECTORY                "/system/vendor/etc/firmware"
#else
#define FIRMWARE_DIRECTORY                 "/system/vendor/etc/firmware"
#define BT_CONFIG_DIRECTORY                "/system/vendor/etc/firmware"
#endif

#define BT_ADDR_DIR                        "/data/misc/bluetooth/"
#define BT_ADDR_FILE                       "/data/misc/bluetooth/btmac.txt"

#define PATCH_DATA_FIELD_MAX_SIZE          252
#define READ_DATA_SIZE                     16

/* HCI data types */
#define H5_ACK_PKT                         0x00
#define HCI_COMMAND_PKT                    0x01
#define HCI_ACLDATA_PKT                    0x02
#define HCI_SCODATA_PKT                    0x03
#define HCI_EVENT_PKT                      0x04
#define H5_VDRSPEC_PKT                     0x0E
#define H5_LINK_CTL_PKT                    0x0F


#define HCI_VERSION_MASK_10                (1<<0)        // Bluetooth Core Spec 1.0b
#define HCI_VERSION_MASK_11                (1<<1)        // Bluetooth Core Spec 1.1
#define HCI_VERSION_MASK_12                (1<<2)        // Bluetooth Core Spec 1.2
#define HCI_VERSION_MASK_20                (1<<3)        // Bluetooth Core Spec 2.0+EDR
#define HCI_VERSION_MASK_21                (1<<4)        // Bluetooth Core Spec 2.1+EDR
#define HCI_VERSION_MASK_30                (1<<5)        // Bluetooth Core Spec 3.0+HS
#define HCI_VERSION_MASK_40                (1<<6)        // Bluetooth Core Spec 4.0
#define HCI_VERSION_MASK_41                (1<<7)        // Bluetooth Core Spec 4.1
#define HCI_VERSION_MASK_42                (1<<8)        // Bluetooth Core Spec 4.2
#define HCI_VERSION_MASK_ALL               (0xFFFFFFFF)

#define HCI_REVISION_MASK_ALL              (0xFFFFFFFF)

#define LMP_SUBVERSION_NONE                (0x0)

#define CHIPTYPE_NONE                      (0x1F)        // Chip Type's range: 0x0 ~ 0xF
#define CHIP_TYPE_MASK_ALL                 (0xFFFFFFFF)

#define PROJECT_ID_MASK_ALL                (0xFFFFFFFF)  // temp used for unknow project id for a new chip

#define CONFIG_MAC_OFFSET_GEN_1_2          (0x3C)        // MAC's OFFSET in config/efuse for realtek generation 1~2 bluetooth chip
#define CONFIG_MAC_OFFSET_GEN_3PLUS        (0x44)        // MAC's OFFSET in config/efuse for rtk generation 3+ bluetooth chip
#define CONFIG_MAC_OFFSET_GEN_4PLUS        (0x30)        // MAC's OFFSET in config/efuse for rtk generation 4+ bluetooth chip

#define PATCH_OPTIONAL_MATCH_FLAG_CHIPTYPE (0x1)

#define MAX_PATCH_SIZE_24K                 (1024*24)     // 24K
#define MAX_PATCH_SIZE_40K                 (1024*40)     // 40K

struct rtk_bt_vendor_config_entry {
	uint16_t offset;
	uint8_t entry_len;
	uint8_t entry_data[0];
} __attribute__ ((packed));

struct rtk_bt_vendor_config {
	uint32_t signature;
	uint16_t data_len;
	struct rtk_bt_vendor_config_entry entry[0];
} __attribute__ ((packed));

struct rtk_epatch_entry {
	uint16_t chipID;
	uint16_t patch_length;
	uint32_t start_offset;
} __attribute__ ((packed));

struct rtk_epatch {
	uint8_t signature[8];
	uint32_t fm_version;
	uint16_t number_of_total_patch;
	struct rtk_epatch_entry entry[0];
} __attribute__ ((packed));

typedef enum _RTK_ROM_VERSION_CMD_STATE {
	cmd_not_send,
	cmd_has_sent,
	event_received
} RTK_ROM_VERSION_CMD_STATE;

#pragma pack(1)
#if __BYTE_ORDER == __LITTLE_ENDIAN
typedef struct _H5_PKT_HEADER {
	uint8_t SeqNumber:3;
	uint8_t AckNumber:3;
	uint8_t DicPresent:1; /* Data Integrity Check Present */
	uint8_t ReliablePkt:1;
	uint16_t PktType:4;
	uint16_t PayloadLen:12;
	uint8_t HdrChecksum;
} H5_PKT_HEADER;
#else
typedef struct _H5_PKT_HEADER {
	uint8_t ReliablePkt:1;
	uint8_t DicPresent:1; /* Data Integrity Check Present */
	uint8_t AckNumber:3;
	uint8_t SeqNumber:3;
	uint16_t PayloadLen:12;
	uint16_t PktType:4;
	uint8_t HdrChecksum;
} H5_PKT_HEADER;
#endif

typedef enum _H5_RX_STATE {
	H5_W4_PKT_DELIMITER,
	H5_W4_PKT_START,
	H5_W4_HDR,
	H5_W4_DATA,
	H5_W4_CRC
} H5_RX_STATE;

typedef enum _H5_RX_ESC_STATE {
	H5_ESCSTATE_NOESC,
	H5_ESCSTATE_ESC
} H5_RX_ESC_STATE;

typedef enum _H5_LINK_STATE {
	H5_SYNC,
	H5_CONFIG,
	H5_INIT,
	H5_PATCH,
	H5_ACTIVE
} H5_LINK_STATE;

typedef enum _PATCH_PROTOCOL {
	PATCH_PROTOCAL_H4,
	PATCH_PROTOCAL_H5
} PATCH_PROTOCOL;

struct rtk_h5_struct {
	uint8_t  rxseq_txack;    /* rxseq == txack. expected rx SeqNumber */
	uint8_t  rxack;          /* Last packet sent by us that the peer ack'ed */
	uint8_t  use_crc;
	uint8_t  is_txack_req;   /* txack required? Do we need to send ack's to the peer? */
	/* Reliable packet sequence number - used to assign seq to each rel pkt. */
	uint8_t  msgq_txseq;     /* next pkt seq */
	uint16_t message_crc;
	uint32_t rx_count;       /* expected pkts to recv */
	H5_RX_STATE     rx_state;
	H5_RX_ESC_STATE rx_esc_state;
	H5_LINK_STATE   link_estab_state;
	struct sk_buff *rx_skb;
	struct sk_buff *host_last_cmd;
};

struct patch_struct {
	int nTxIndex;          /* current sending pkt number */
	int nTotal;            /* total pkt number */
	int nRxIndex;          /* ack index from board */
	int nNeedRetry;        /* if no response from board */
};

typedef struct {
	uint16_t    lmp_subversion;
	uint32_t    hci_version_mask;
	uint32_t    hci_revision_mask;
	uint32_t    chip_type_mask;
	uint32_t    project_id_mask;
	char       *patch_name;
	char       *config_name;
	uint16_t    mac_offset;
	uint32_t    max_patch_size;
} patch_info;

/* h/w config control block */
typedef struct {
	uint32_t    max_patch_size;
	uint32_t    baudrate;
	uint16_t    lmp_subversion;
	uint16_t    lmp_subversion_default;
	uint16_t    lmp_sub_current;
	uint8_t     state;          /* Hardware configuration state */
	uint8_t     eversion;
	uint32_t    project_id_mask;
	uint8_t     hci_version;
	uint8_t     hci_revision;
	uint8_t     chip_type;
} bt_hw_cfg_cb_t;

static patch_info patch_table[] = {
/*   lmp_subv    hci_version_mask        hci_revision_mask    chip_type_mask  project_id_mask  fw name          config name                    mac offset           max_patch_size  */
	{0x1200,   HCI_VERSION_MASK_ALL,   HCI_REVISION_MASK_ALL, CHIP_TYPE_MASK_ALL,  1<<0,  "rtl8723as_fw",     "rtl8723as_config",     CONFIG_MAC_OFFSET_GEN_1_2,   MAX_PATCH_SIZE_24K},  //Rtl8723AS
#ifdef RTL_8723BS_BT_USED
	{0x8723, ~(HCI_VERSION_MASK_21), ~(1<<0xd),               CHIP_TYPE_MASK_ALL,  1<<1,  "rtl8723bs_fw",     "rtl8723bs_config",     CONFIG_MAC_OFFSET_GEN_1_2,   MAX_PATCH_SIZE_24K},  //Rtl8723BS
#else
	{0x8723, ~(HCI_VERSION_MASK_21), ~(1<<0xd),               CHIP_TYPE_MASK_ALL,  1<<1,  "rtl8723bs_VQ0_fw", "rtl8723bs_VQ0_config", CONFIG_MAC_OFFSET_GEN_1_2,   MAX_PATCH_SIZE_24K},  //Rtl8723BS_VQ0
#endif
	{0x8821,   HCI_VERSION_MASK_ALL, ~(1<<0xc),               CHIP_TYPE_MASK_ALL,  1<<2,  "rtl8821as_fw",     "rtl8821as_config",     CONFIG_MAC_OFFSET_GEN_1_2,   MAX_PATCH_SIZE_24K},  //Rtl8821AS
/*  {0x8761,   HCI_VERSION_MASK_ALL,   HCI_REVISION_MASK_ALL, CHIP_TYPE_MASK_ALL,  1<<3,  "rtl8761at_fw",     "rtl8761at_config",     CONFIG_MAC_OFFSET_GEN_1_2,   MAX_PATCH_SIZE_24K},  //Rtl8761AW */
	{0x8761,   HCI_VERSION_MASK_ALL, ~(1<<0xb),               CHIP_TYPE_MASK_ALL,  1<<3,  "rtl8761at_fw",     "rtl8761at_config",     CONFIG_MAC_OFFSET_GEN_1_2,   MAX_PATCH_SIZE_24K},  //Rtl8761AW
	{0x8761,   HCI_VERSION_MASK_ALL,  (1<<0xb),               CHIP_TYPE_MASK_ALL,  1<<14, "rtl8761bt_fw",     "rtl8761bt_config",     CONFIG_MAC_OFFSET_GEN_4PLUS, MAX_PATCH_SIZE_40K},  //Rtl8761BW

	{0x8723,   HCI_VERSION_MASK_21,    HCI_REVISION_MASK_ALL, CHIP_TYPE_MASK_ALL,  1<<4,  "rtl8703as_fw",     "rtl8703as_config",     CONFIG_MAC_OFFSET_GEN_1_2,   MAX_PATCH_SIZE_24K},  //Rtl8703AS

	{0x8703,   HCI_VERSION_MASK_ALL,   HCI_REVISION_MASK_ALL, 1<<7,                1<<6,  "rtl8703bs_fw",     "rtl8703bs_config",     CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_24K},  //Rtl8703BS
	{0x8703,   HCI_VERSION_MASK_ALL,   HCI_REVISION_MASK_ALL, 1<<5,                1<<7,  "rtl8723cs_xx_fw",  "rtl8723cs_xx_config",  CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_24K},  //rtl8723cs_xx
	{0x8703,   HCI_VERSION_MASK_ALL,   HCI_REVISION_MASK_ALL, 1<<3,                1<<7,  "rtl8723cs_cg_fw",  "rtl8723cs_cg_config",  CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_24K},  //rtl8723cs_cg
	{0x8703,   HCI_VERSION_MASK_ALL,   HCI_REVISION_MASK_ALL, 1<<4,                1<<7,  "rtl8723cs_vf_fw",  "rtl8723cs_vf_config",  CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_24K},  //rtl8723cs_vf
/*  {0x8822,   HCI_VERSION_MASK_ALL,   HCI_REVISION_MASK_ALL, CHIP_TYPE_MASK_ALL,  1<<8,  "rtl8822bs_fw",     "rtl8822bs_config",     CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_24K},  //Rtl8822BS */
	{0x8822,   HCI_VERSION_MASK_ALL, ~(1<<0xc),               CHIP_TYPE_MASK_ALL,  1<<8,  "rtl8822bs_fw",     "rtl8822bs_config",     CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_24K},  //Rtl8822BS
	{0x8822,   HCI_VERSION_MASK_ALL,  (1<<0xc),               CHIP_TYPE_MASK_ALL,  1<<13, "rtl8822cs_fw",     "rtl8822cs_config",     CONFIG_MAC_OFFSET_GEN_4PLUS, MAX_PATCH_SIZE_40K},  //Rtl8822CS

	{0x8723,   HCI_VERSION_MASK_ALL,  (1<<0xd),               ~(1<<7),             1<<9,  "rtl8723ds_fw",     "rtl8723ds_config",     CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_40K},  //Rtl8723ds
	{0x8723,   HCI_VERSION_MASK_ALL,  (1<<0xd),               1<<7,                1<<9,  "rtl8703cs_fw",     "rtl8703cs_config",     CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_40K},  //Rtl8703cs
	{0x8821,   HCI_VERSION_MASK_ALL,  (1<<0xc),               CHIP_TYPE_MASK_ALL,  1<<10, "rtl8821cs_fw",     "rtl8821cs_config",     CONFIG_MAC_OFFSET_GEN_3PLUS, MAX_PATCH_SIZE_40K},  //RTL8821CS
/*  todo: RTL8703CS */
	{LMP_SUBVERSION_NONE, HCI_VERSION_MASK_ALL, HCI_REVISION_MASK_ALL, CHIP_TYPE_MASK_ALL, PROJECT_ID_MASK_ALL, "rtl_none_fw", "rtl_none_config", CONFIG_MAC_OFFSET_GEN_1_2, MAX_PATCH_SIZE_24K}
};

static bt_hw_cfg_cb_t hw_cfg_cb;
static uint8_t gEVersion;
static RTK_ROM_VERSION_CMD_STATE gRom_version_cmd_state;
static RTK_ROM_VERSION_CMD_STATE ghci_version_cmd_state;
static RTK_ROM_VERSION_CMD_STATE gchip_type_cmd_state;
static int gHwFlowControlEnable;
static int gFinalSpeed;

/* signature: Realtech */
static const uint8_t RTK_EPATCH_SIGNATURE[8] = {0x52, 0x65, 0x61, 0x6C, 0x74, 0x65, 0x63, 0x68};
/* Extension Section IGNATURE:0x77FD0451 */
static const uint8_t Extension_Section_SIGNATURE[4] = {0x51, 0x04, 0xFD, 0x77};

static int serial_fd;
static int h5_max_retries = 40;
static struct rtk_h5_struct rtk_h5;
static struct patch_struct rtk_patch;

/* bite reverse in bytes
   00000001 -> 10000000
   00000100 -> 00100000
 */
static const uint8_t byte_rev_table[256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

/* reverse bit */
static __inline uint8_t bit_rev8(uint8_t byte)
{
	return byte_rev_table[byte];
}

/* reverse bit */
static __inline uint16_t bit_rev16(uint16_t x)
{
	return (bit_rev8(x & 0xff) << 8) | bit_rev8(x >> 8);
}

static const uint16_t crc_table[] = {
	0x0000, 0x1081, 0x2102, 0x3183,
	0x4204, 0x5285, 0x6306, 0x7387,
	0x8408, 0x9489, 0xa50a, 0xb58b,
	0xc60c, 0xd68d, 0xe70e, 0xf78f
};

/**
* Malloc the socket buffer
*
* @param skb socket buffer
* @return the point to the malloc buffer
*/
static __inline struct sk_buff *skb_alloc(uint32_t len)
{
	struct sk_buff *skb = NULL;
	skb = malloc(len + 8);
	if (skb) {
		skb->max_len  = len;
		skb->data_len = 0;
		memset(skb->data, 0, len);
	} else {
		RS_ERR("Allocate skb fails!!!");
		skb = NULL;
	}
	return skb;
}

/**
* Free the socket buffer
*
* @param skb socket buffer
*/
static __inline void skb_free(struct sk_buff *skb)
{
	free(skb);
	return;
}

/**
* Increase the date length in sk_buffer by len,
* and return the increased header pointer
*
* @param skb socket buffer
* @param len length want to increase
* @return the pointer to increased header
*/
static uint8_t *skb_put(struct sk_buff *skb, uint32_t len)
{
	uint32_t old_len = skb->data_len;
	if ((skb->data_len + len) > (skb->max_len)) {
		RS_ERR("Buffer too small");
		return NULL;
	}
	skb->data_len += len;
	return (skb->data + old_len);
}

/**
* decrease data length in sk_buffer by to len by cut the tail
*
* @warning len should be less than skb->len
*
* @param skb socket buffer
* @param len length want to be changed
*/
static void skb_trim(struct sk_buff *skb,  int len)
{
	if (skb->data_len > len) {
		skb->data_len = len;
	} else {
		RS_ERR("Error: skb->data_len(%ld) < len(%d)", (long int)skb->data_len, len);
	}
}

/**
* Decrease the data length in sk_buffer by len,
* and move the content forward to the header.
* the data in header will be removed.
*
* @param skb socket buffer
* @param len length of data
* @return new data
*/
static uint8_t *skb_pull(struct sk_buff *skb, uint32_t len)
{
	skb->data_len -= len;
	char *buf;
	buf = malloc(skb->data_len);
	if (!buf) {
		RS_ERR("Unable to allocate file buffer");
		exit(1);
	}
	memcpy(buf, skb->data+len, skb->data_len);
	memcpy(skb->data, buf, skb->data_len);
	free(buf);
	return skb->data;
}

/**
* Add "d" into crc scope, caculate the new crc value
*
* @param crc crc data
* @param d one byte data
*/
static void h5_crc_update(uint16_t *crc, uint8_t d)
{
	uint16_t reg = *crc;

	reg = (reg >> 4) ^ crc_table[(reg ^ d) & 0x000f];
	reg = (reg >> 4) ^ crc_table[(reg ^ (d >> 4)) & 0x000f];

	*crc = reg;
}

struct __una_u16 { uint16_t x; };

static __inline uint16_t __get_unaligned_cpu16(const void *p)
{
	const struct __una_u16 *ptr = (const struct __una_u16 *)p;
	return ptr->x;
}

static __inline uint16_t get_unaligned_be16(const void *p)
{
	return __get_unaligned_cpu16((const uint8_t *)p);
}

/**
* Get crc data.
*
* @param h5 realtek h5 struct
* @return crc data
*/
static uint16_t h5_get_crc(struct rtk_h5_struct *h5)
{
	uint16_t crc = 0;
	uint8_t *data = h5->rx_skb->data + h5->rx_skb->data_len - 2;
	crc = data[1] + (data[0] << 8);
	return crc;
}

/**
* Just add 0xc0 at the end of skb,
* we can also use this to add 0xc0 at start while there is no data in skb
*
* @param skb socket buffer
*/
static void h5_slip_msgdelim(struct sk_buff *skb)
{
	const char pkt_delim = 0xc0;
	memcpy(skb_put(skb, 1), &pkt_delim, 1);
}

/**
* Slip ecode one byte in h5 proto, as follows:
* 0xc0 -> 0xdb, 0xdc
* 0xdb -> 0xdb, 0xdd
* 0x11 -> 0xdb, 0xde
* 0x13 -> 0xdb, 0xdf
* others will not change
*
* @param skb socket buffer
* @c pure data in the one byte
*/
static void h5_slip_one_byte(struct sk_buff *skb, uint8_t c)
{
	const int8_t esc_c0[2] = { 0xdb, 0xdc };
	const int8_t esc_db[2] = { 0xdb, 0xdd };
	const int8_t esc_11[2] = { 0xdb, 0xde };
	const int8_t esc_13[2] = { 0xdb, 0xdf };

	switch (c) {
	case 0xc0:
		memcpy(skb_put(skb, 2), &esc_c0, 2);
		break;
	case 0xdb:
		memcpy(skb_put(skb, 2), &esc_db, 2);
		break;
	case 0x11:
		memcpy(skb_put(skb, 2), &esc_11, 2);
		break;
	case 0x13:
		memcpy(skb_put(skb, 2), &esc_13, 2);
		break;
	default:
		memcpy(skb_put(skb, 1), &c, 1);
	}
}

/**
* Decode one byte in h5 proto, as follows:
* 0xdb, 0xdc -> 0xc0
* 0xdb, 0xdd -> 0xdb
* 0xdb, 0xde -> 0x11
* 0xdb, 0xdf -> 0x13
* others will not change
*
* @param h5 realtek h5 struct
* @byte pure data in the one byte
*/
static void h5_unslip_one_byte(struct rtk_h5_struct *h5,  char byte)
{
	const uint8_t c0 = 0xc0, db = 0xdb;
	const uint8_t oof1 = 0x11, oof2 = 0x13;
	//RS_DBG("HCI 3wire h5_unslip_one_byte");

	if (H5_ESCSTATE_NOESC == h5->rx_esc_state) {
		if (0xdb == byte) {
			h5->rx_esc_state = H5_ESCSTATE_ESC;
		} else {
			memcpy(skb_put(h5->rx_skb, 1), &byte, 1);
			//Check Pkt Header's CRC enable bit
			if ((h5->rx_skb->data[0] & 0x40) != 0 && h5->rx_state != H5_W4_CRC) {
				h5_crc_update(&h5->message_crc, byte);
			}
			h5->rx_count--;
		}
	} else if (H5_ESCSTATE_ESC == h5->rx_esc_state) {
		switch (byte) {
		case 0xdc:
			memcpy(skb_put(h5->rx_skb, 1), &c0, 1);
			if ((h5->rx_skb->data[0] & 0x40) != 0 && h5->rx_state != H5_W4_CRC)
				h5_crc_update(&h5->message_crc, 0xc0);
			h5->rx_esc_state = H5_ESCSTATE_NOESC;
			h5->rx_count--;
			break;
		case 0xdd:
			memcpy(skb_put(h5->rx_skb, 1), &db, 1);
			if ((h5->rx_skb->data[0] & 0x40) != 0 && h5->rx_state != H5_W4_CRC)
				h5_crc_update(&h5->message_crc, 0xdb);
			h5->rx_esc_state = H5_ESCSTATE_NOESC;
			h5->rx_count--;
			break;
		case 0xde:
			memcpy(skb_put(h5->rx_skb, 1), &oof1, 1);
			if ((h5->rx_skb->data[0] & 0x40) != 0 && h5->rx_state != H5_W4_CRC)
				h5_crc_update(&h5->message_crc, oof1);
			h5->rx_esc_state = H5_ESCSTATE_NOESC;
			h5->rx_count--;
			break;
		case 0xdf:
			memcpy(skb_put(h5->rx_skb, 1), &oof2, 1);
			if ((h5->rx_skb->data[0] & 0x40) != 0 && h5->rx_state != H5_W4_CRC)
				h5_crc_update(&h5->message_crc, oof2);
			h5->rx_esc_state = H5_ESCSTATE_NOESC;
			h5->rx_count--;
			break;
		default:
			RS_ERR("Error: Invalid byte %02x after esc byte", byte);
			skb_free(h5->rx_skb);
			h5->rx_skb = NULL;
			h5->rx_state = H5_W4_PKT_DELIMITER;
			h5->rx_count = 0;
			break;
		}
	}
}

/**
* Prepare h5 packet, packet format as follow:
*  | LSB 4 octets | 0 ~ 4095 | 2 MSB
*  |packet header | payload  | data integrity check |
*
* pakcket header fromat is show below:
*  | LSB 3 bits     | 3 bits                 | 1 bits                       | 1 bits          |
*  | 4 bits         | 12 bits                | 8 bits MSB
*  |sequence number | acknowledgement number | data integrity check present | reliable packet |
*  |packet type     | payload length         | header checksum
*
* @param h5 realtek h5 struct
* @param data pure data
* @param len the length of data
* @param pkt_type packet type
* @return socket buff after prepare in h5 proto
*/
static struct sk_buff *h5_prepare_pkt(struct rtk_h5_struct *h5, uint8_t *data, int32_t len, int32_t pkt_type)
{
	struct sk_buff *nskb;
	uint8_t hdr[4];
	uint16_t h5_txmsg_crc = 0xffff;
	int rel, i;

	switch (pkt_type) {
	case HCI_ACLDATA_PKT:
	case HCI_COMMAND_PKT:
	case HCI_EVENT_PKT:
		rel = 1;    /* reliable */
		break;
	case H5_ACK_PKT:
	case H5_VDRSPEC_PKT:
	case H5_LINK_CTL_PKT:
		rel = 0;	/* unreliable */
		break;
	default:
		RS_ERR("Unknown packet type");
		return NULL;
	}

	/* Max len of packet: (original len +4(h5 hdr) + 2(crc)) * 2
	 * (because bytes 0xc0 and 0xdb are escaped, worst case is
	 * when the packet is all made of 0xc0 and 0xdb :))
	 * + 2 (0xc0 delimiters at start and end). */
	nskb = skb_alloc((len + 6) * 2 + 2);
	if (!nskb)
		return NULL;

	/* Add SLIP start byte: 0xc0 */
	h5_slip_msgdelim(nskb);
	/* set AckNumber in SlipHeader */
	hdr[0] = h5->rxseq_txack << 3;
	h5->is_txack_req = 0;

	if (rel) {
		/* set reliable pkt bit and SeqNumber */
		hdr[0] |= 0x80 + h5->msgq_txseq;
		++(h5->msgq_txseq);
		h5->msgq_txseq = (h5->msgq_txseq) & 0x07;
	}

	/* set DicPresent bit */
	if (h5->use_crc)
		hdr[0] |= 0x40;

	/* set packet type and payload length */
	hdr[1] = ((len << 4) & 0xff) | pkt_type;
	hdr[2] = (uint8_t)(len >> 4);
	/* set checksum */
	hdr[3] = ~(hdr[0] + hdr[1] + hdr[2]);

	/* Put h5 header */
	for (i = 0; i < 4; i++) {
		h5_slip_one_byte(nskb, hdr[i]);

		if (h5->use_crc)
			h5_crc_update(&h5_txmsg_crc, hdr[i]);
	}

	/* Put payload */
	for (i = 0; i < len; i++) {
		h5_slip_one_byte(nskb, data[i]);

		if (h5->use_crc)
			h5_crc_update(&h5_txmsg_crc, data[i]);
	}

	/* Put CRC */
	if (h5->use_crc) {
		h5_txmsg_crc = bit_rev16(h5_txmsg_crc);
		h5_slip_one_byte(nskb, (uint8_t) ((h5_txmsg_crc >> 8) & 0x00ff));
		h5_slip_one_byte(nskb, (uint8_t) (h5_txmsg_crc & 0x00ff));
	}

	/* Add SLIP end byte: 0xc0 */
	h5_slip_msgdelim(nskb);
	return nskb;
}

/**
* Removed controller acked packet from Host's unacked lists
*
* @param h5 realtek h5 struct
*/
static void h5_remove_acked_pkt(struct rtk_h5_struct *h5)
{
	int pkts_to_be_removed = 0;
	int seqno = 0;
	int i = 0;

	seqno = h5->msgq_txseq;

	while (pkts_to_be_removed) {
		if (h5->rxack == seqno)
			break;

		pkts_to_be_removed--;
		seqno = (seqno - 1) & 0x07;
	}

	if (h5->rxack != seqno) {
		RS_DBG("Peer acked invalid packet");
	}

	i = 0;
	for (i = 0; i < 5; ++i) {
		if (i >= pkts_to_be_removed)
			break;
		i++;
	}

	if (i != pkts_to_be_removed)
		RS_DBG("Removed only (%d) out of (%d) pkts", i, pkts_to_be_removed);
}

/**
* Realtek send pure ack, send a packet only with an ack
*
* @param fd uart file descriptor
*
*/
static void rtk_send_pure_ack_down(int fd)
{
	struct sk_buff *nskb = h5_prepare_pkt(&rtk_h5, NULL, 0, H5_ACK_PKT);
	write(fd, nskb->data, nskb->data_len);
	skb_free(nskb);
	return;
}

/**
* Parse hci event command complete, pull the cmd complete event header
*
* @param skb socket buffer
*
*/
static void hci_event_cmd_complete(struct sk_buff *skb)
{
	struct hci_ev_cmd_complete *ev = NULL;
	uint16_t opcode = 0;
	uint8_t status = 0;

	/* omit length check */
	/* pull hdr */
	skb_pull(skb, HCI_EVENT_HDR_SIZE);
	ev = (struct hci_ev_cmd_complete *)skb->data;
	opcode = le16_to_cpu(ev->opcode);

	RS_DBG("receive hci command complete event with command: %x", opcode);
	if (DBG_ON) {
		RS_DBG("Dump event data with event header (header size %d):", sizeof(struct hci_ev_cmd_complete));
		RS_DUMP(skb->data, skb->data_len);
	}

	/* pull command complete event header */
	skb_pull(skb, sizeof(struct hci_ev_cmd_complete));

	switch (opcode) {
	case HCI_VENDOR_CHANGE_BDRATE:
		status = skb->data[0];
		RS_DBG("Change BD Rate with status:%x", status);
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
		rtk_h5.link_estab_state = H5_PATCH;
		break;
	case HCI_CMD_READ_BD_ADDR:
		status = skb->data[0];
		RS_DBG("Read BD Address with Status:%x", status);
		if (!status) {
			RS_DBG("BD Address: %8x%8x", *(int *)&skb->data[1], *(int *)&skb->data[5]);
		}
		break;
	case HCI_VENDOR_READ_LMP_VERISION:
		ghci_version_cmd_state = event_received;
		status = skb->data[0];
		RS_DBG("Read RTK LMP version with Status:%x", status);
		if (0 == status) {
			hw_cfg_cb.hci_version    = *(uint8_t *)(&skb->data[1]);
			hw_cfg_cb.hci_revision   = *(uint8_t *)(&skb->data[2]);
			hw_cfg_cb.lmp_subversion = le16_to_cpu(*(uint16_t *)(&skb->data[7]));
		} else {
			RS_ERR("READ_RTK_ROM_VERISION return status error!");
			/* Need to do more */
		}
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
		break;
	case HCI_VENDOR_READ_CHIP_TYPE:
		gchip_type_cmd_state = event_received;
		status = skb->data[0];
		RS_DBG("Read RTK CHIP TYPE with Status:%x", status);
		if (0 == status) {
			hw_cfg_cb.chip_type = *(uint8_t *)(&skb->data[1]);
		} else {
			RS_ERR("READ_RTK_CHIP_TYPE return status error!");
		}
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
		break;
	case HCI_VENDOR_READ_RTK_ROM_VERISION:
		gRom_version_cmd_state = event_received;
		status = skb->data[0];
		RS_DBG("Read RTK rom version with Status:%x", status);
		if (0 == status)
			gEVersion = skb->data[1];
		else if (1 == status)
			gEVersion = 0;
		else {
			gEVersion = 0;
			RS_ERR("READ_RTK_ROM_VERISION return status error!");
			/* Need to do more */
		}
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
		break;
	}
}

/**
* Check if it's a hci frame, if it is, complete it with response or parse the cmd complete event
*
* @param skb socket buffer
*
*/
static void hci_recv_frame(struct sk_buff *skb)
{
	 char h5sync[2] = {0x01, 0x7E},
	h5syncresp[2] = {0x02, 0x7D},
	h5_sync_resp_pkt[0x8] = {0xc0, 0x00, 0x2F, 0x00, 0xD0, 0x02, 0x7D, 0xc0},
	h5_conf_resp_pkt_to_Ctrl[0x8] = {0xc0, 0x00, 0x2F, 0x00, 0xD0, 0x04, 0x7B, 0xc0},
	h5conf[3] = {0x03, 0xFC, 0x10},
	h5confresp[3] = {0x04, 0x7B, 0x10},
	cmd_complete_evt_code = 0xe;

	if (rtk_h5.link_estab_state == H5_SYNC) {  /* sync */
		if (!memcmp(skb->data, h5sync, 2)) {
			RS_DBG("Get Sync Pkt\n");
			write(serial_fd, &h5_sync_resp_pkt, 0x8);
		} else if (!memcmp(skb->data, h5syncresp, 2)) {
			RS_DBG("Get Sync Resp Pkt\n");
			rtk_h5.link_estab_state = H5_CONFIG;
		}
		skb_free(skb);
	} else if (rtk_h5.link_estab_state == H5_CONFIG) {  /* config */
		if (!memcmp(skb->data, h5sync, 0x2)) {
			write(serial_fd, &h5_sync_resp_pkt, 0x8);
			RS_DBG("Get SYNC pkt-active mode\n");
		} else if (!memcmp(skb->data, h5conf, 0x2)) {
			write(serial_fd, &h5_conf_resp_pkt_to_Ctrl, 0x8);
			RS_DBG("Get CONFG pkt-active mode\n");
		} else if (!memcmp(skb->data, h5confresp,  0x2)) {
			RS_DBG("Get CONFG resp pkt-active mode\n");
			rtk_h5.link_estab_state = H5_INIT;//H5_PATCH;
			//rtk_send_pure_ack_down(serial_fd);
		} else {
			RS_DBG("H5_CONFIG receive event\n");
			rtk_send_pure_ack_down(serial_fd);
		}
		skb_free(skb);
	} else if (rtk_h5.link_estab_state == H5_INIT) {
		if (skb->data[0] == cmd_complete_evt_code) {
			hci_event_cmd_complete(skb);
		}

		rtk_send_pure_ack_down(serial_fd);
		usleep(10000);
		rtk_send_pure_ack_down(serial_fd);
		usleep(10000);
		rtk_send_pure_ack_down(serial_fd);
		skb_free(skb);
	} else if (rtk_h5.link_estab_state == H5_PATCH) {  /* patch */
		rtk_patch.nRxIndex = skb->data[6];
		if (rtk_patch.nRxIndex & 0x80)
			rtk_patch.nRxIndex &= ~0x80;

		RS_DBG("rtk_patch.nRxIndex %d\n", rtk_patch.nRxIndex);
		if (rtk_patch.nRxIndex == rtk_patch.nTotal)
			rtk_h5.link_estab_state = H5_ACTIVE;
		skb_free(skb);
	} else {
		RS_ERR("receive packets in active state");
		skb_free(skb);
	}
}

/**
* after rx data is parsed, and we got a rx frame saved in h5->rx_skb,
* this routinue is called.
* things todo in this function:
* 1. check if it's a hci frame, if it is, complete it with response or ack
* 2. see the ack number, free acked frame in queue
* 3. reset h5->rx_state, set rx_skb to null.
*
* @param h5 realtek h5 struct
*
*/
static void h5_complete_rx_pkt(struct rtk_h5_struct *h5)
{
	int pass_up = 1;
	uint16_t *valuep, value_t;
	H5_PKT_HEADER *h5_hdr = NULL;
	/* 1 is offset of uint16_t in H5_PKT_HEADER */
	valuep  = (uint16_t *)(h5->rx_skb->data+1);
	value_t = le16_to_cpu(*valuep);
	*valuep = value_t;
	h5_hdr  = (H5_PKT_HEADER *)(h5->rx_skb->data);
	if (h5_hdr->ReliablePkt) {
		RS_DBG("Received reliable seqno %u from card", h5->rxseq_txack);
		h5->rxseq_txack = h5_hdr->SeqNumber + 1;
		h5->rxseq_txack %= 8;
		h5->is_txack_req = 1;
		/* send down an empty ack if needed. */
	}

	h5->rxack = h5_hdr->AckNumber;

	switch (h5_hdr->PktType) {
	case HCI_ACLDATA_PKT:
	case HCI_EVENT_PKT:
	case HCI_SCODATA_PKT:
	case HCI_COMMAND_PKT:
	case H5_LINK_CTL_PKT:
		pass_up = 1;
		break;
	default:
		pass_up = 0;
	}

	h5_remove_acked_pkt(h5);

	/* decide if we need to pass up. */
	if (pass_up) {
		/* remove h5 header and send packet to hci */
		skb_pull(h5->rx_skb, sizeof(H5_PKT_HEADER));
		hci_recv_frame(h5->rx_skb);
		/* should skb be freed here? */
	} else {
		/* free skb buffer */
		skb_free(h5->rx_skb);
	}

	h5->rx_state = H5_W4_PKT_DELIMITER;
	h5->rx_skb = NULL;
}

/**
* Parse the receive data in h5 proto.
*
* @param h5 realtek h5 struct
* @param data point to data received before parse
* @param count num of data
* @return reserved count
*/
static int h5_recv(struct rtk_h5_struct *h5, void *data, int count)
{
	char *ptr;
	ptr = (char *)data;

	while (count) {
		if (h5->rx_count) {
			if (*ptr == 0xc0) {
				RS_ERR("short h5 packet");
				skb_free(h5->rx_skb);
				h5->rx_state = H5_W4_PKT_START;
				h5->rx_count = 0;
			} else {
				h5_unslip_one_byte(h5, *ptr);
			}
			ptr++;
			count--;
			continue;
		}

		switch (h5->rx_state) {
		case H5_W4_HDR:
			/* check header checksum. see Core Spec V4 "3-wire uart" page 67 */
			if ((0xff & (uint8_t) ~(h5->rx_skb->data[0] + h5->rx_skb->data[1] +
					h5->rx_skb->data[2])) != h5->rx_skb->data[3]) {
				RS_ERR("h5 hdr checksum error!!!");
				skb_free(h5->rx_skb);
				h5->rx_state = H5_W4_PKT_DELIMITER;
				h5->rx_count = 0;
				continue;
			}

			if (h5->rx_skb->data[0] & 0x80	/* reliable pkt */
					&& (h5->rx_skb->data[0] & 0x07) != h5->rxseq_txack) {
				RS_ERR("Out-of-order packet arrived, got(%d)expected(%u)",
				h5->rx_skb->data[0] & 0x07, h5->rxseq_txack);
				h5->is_txack_req = 1;
				skb_free(h5->rx_skb);
				h5->rx_state = H5_W4_PKT_DELIMITER;
				h5->rx_count = 0;

				if (rtk_patch.nTxIndex == rtk_patch.nTotal) {
					/* depend on weather remote will reset ack numb or not!!!!!!!!!!!!!!!special */
					rtk_h5.rxseq_txack = h5->rx_skb->data[0] & 0x07;
				}
				continue;
			}

			h5->rx_state = H5_W4_DATA;

			/* payload length: May be 0 */
			h5->rx_count = (h5->rx_skb->data[1] >> 4) + (h5->rx_skb->data[2] << 4);
			continue;
		case H5_W4_DATA:
			if (h5->rx_skb->data[0] & 0x40) {  /* pkt with crc */
				h5->rx_state = H5_W4_CRC;
				h5->rx_count = 2;
			} else {
				h5_complete_rx_pkt(h5);        /* Send ACK */
			}
			continue;

		case H5_W4_CRC:
			if (bit_rev16(h5->message_crc) != h5_get_crc(h5)) {
				RS_ERR("Checksum failed, computed(%04x)received(%04x)",
				bit_rev16(h5->message_crc), h5_get_crc(h5));
				skb_free(h5->rx_skb);
				h5->rx_state = H5_W4_PKT_DELIMITER;
				h5->rx_count = 0;
				continue;
			}
			skb_trim(h5->rx_skb, h5->rx_skb->data_len - 2);
			h5_complete_rx_pkt(h5);
			continue;

		case H5_W4_PKT_DELIMITER:
			switch (*ptr) {
			case 0xc0:
				h5->rx_state = H5_W4_PKT_START;
				break;
			default:
				break;
			}
			ptr++;
			count--;
			break;

		case H5_W4_PKT_START:
			switch (*ptr) {
			case 0xc0:
				ptr++;
				count--;
				break;
			default:
				h5->rx_state = H5_W4_HDR;
				h5->rx_count = 4;
				h5->rx_esc_state = H5_ESCSTATE_NOESC;
				h5->message_crc = 0xffff;

				/* Do not increment ptr or decrement count
				 * Allocate packet. Max len of a H5 pkt=
				 * 0xFFF (payload) +4 (header) +2 (crc) */
				h5->rx_skb = skb_alloc(0x1005);
				if (!h5->rx_skb) {
					h5->rx_state = H5_W4_PKT_DELIMITER;
					h5->rx_count = 0;
					return 0;
				}
				break;
			}
			break;
		}
	}
	return count;
}

/**
* Read data to buf from uart.
*
* @param fd uart file descriptor
* @param buf point to the addr where read data stored
* @param count num of data want to read
* @return num of data successfully read
*/
static int read_check(int fd, void *buf, int count)
{
	int res;
	do {
		res = read(fd, buf, count);
		if (res != -1) {
			buf = (uint8_t *)buf + res;
			count -= res;
			return res;
		}
	} while (count && (errno == 0 || errno == EINTR));
	return res;
}

/**
* Retry to sync when timeout in h5 proto, max retry times is 10.
*
* @warning Each time to retry, the time for timeout will be set as 1s.
*
* @param sig signaction for timeout
*
*/
static void h5_tshy_sig_alarm(int sig)
{
	uint8_t h5sync[2] = {0x01, 0x7E};
	static int retries;
	struct itimerval value;

	if (retries < h5_max_retries) {
		retries++;
		struct sk_buff *nskb = h5_prepare_pkt(&rtk_h5, h5sync, sizeof(h5sync), H5_LINK_CTL_PKT);
		int len = write(serial_fd, nskb->data, nskb->data_len);
		RS_DBG("3-wire sync pattern resend : %d, len: %d\n", retries, len);
		skb_free(nskb);

		/* retry per 250ms */
		value.it_value.tv_sec = 0;
		value.it_value.tv_usec = 250000;
		value.it_interval.tv_sec = 0;
		value.it_interval.tv_usec = 250000;
		setitimer(ITIMER_REAL, &value, NULL);

		return;
	}

	tcflush(serial_fd, TCIOFLUSH);
	RS_ERR("H5 sync timed out\n");
	exit(1);
}

/**
* Retry to config when timeout in h5 proto, max retry times is 10.
*
* @warning Each time to retry, the time for timeout will be set as 1s.
*
* @param sig signaction for timeout
*
*/
static void h5_tconf_sig_alarm(int sig)
{
	uint8_t h5conf[3] = {0x03, 0xFC, 0x14};
	static int retries;
	struct itimerval value;

	if (retries < h5_max_retries) {
		retries++;
		struct sk_buff *nskb = h5_prepare_pkt(&rtk_h5, h5conf, 3, H5_LINK_CTL_PKT);
		int len = write(serial_fd,  nskb->data, nskb->data_len);
		RS_DBG("3-wire config pattern resend : %d , len: %d", retries, len);
		skb_free(nskb);

		/* retry per 250ms */
		value.it_value.tv_sec = 0;
		value.it_value.tv_usec = 250000;
		value.it_interval.tv_sec = 0;
		value.it_interval.tv_usec = 250000;
		setitimer(ITIMER_REAL, &value, NULL);

		return;
	}

	tcflush(serial_fd, TCIOFLUSH);
	RS_ERR("H5 config timed out\n");
	exit(1);
}

/**
* Retry to init when timeout in h5 proto, max retry times is 10.
*
* @warning Each time to retry, the time for timeout will be set as 1s.
*
* @param sig signaction for timeout
*
*/
static void h5_tinit_sig_alarm(int sig)
{
	static int retries;
	if (retries < h5_max_retries) {
		retries++;
		if (rtk_h5.host_last_cmd) {
			int len = write(serial_fd, rtk_h5.host_last_cmd->data, rtk_h5.host_last_cmd->data_len);
			RS_DBG("3-wire change baudrate re send:%d, len:%d", retries, len);
			alarm(1);
			return;
		} else {
			RS_DBG("3-wire init timeout without last command stored\n");
		}
	}

	tcflush(serial_fd, TCIOFLUSH);
	RS_ERR("H5 init process timed out");
	exit(1);
}

/**
* Retry to download patch when timeout in h5 proto, max retry times is 10.
*
* @warning Each time to retry, the time for timeout will be set as 3s.
*
* @param sig signaction for timeout
*
*/
static void h5_tpatch_sig_alarm(int sig)
{
	static int retries;
	if (retries < h5_max_retries) {
		RS_DBG("patch timerout, retry:\n");
		if (rtk_h5.host_last_cmd) {
			write(serial_fd, rtk_h5.host_last_cmd->data, rtk_h5.host_last_cmd->data_len);
			RS_DBG("3-wire download patch re send:%d", retries);
		}
		retries++;
		alarm(3);
		return;
	}
	RS_ERR("H5 patch timed out\n");
	exit(1);
}

/**
* Download patch using hci. For h5 proto, not recv reply for 2s will timeout.
* Call h5_tpatch_sig_alarm for retry.
*
* @param dd uart file descriptor
* @param index current index
* @param data point to the config file
* @param len current buf length
* @return #0 on success
*
*/
static int hci_download_patch(int dd, int index, uint8_t *data, int len, struct termios *ti)
{
	uint8_t hcipatch[256] = {0x20, 0xfc, 00};
	char bytes[READ_DATA_SIZE];
	int retlen;
	struct sigaction sa;

	sa.sa_handler = h5_tpatch_sig_alarm;
	sigaction(SIGALRM, &sa, NULL);
	alarm(2);

	download_vendor_patch_cp cp;
	memset(&cp, 0, sizeof(cp));
	cp.index = index;
	if (data != NULL) {
		memcpy(cp.data, data, len);
	}

	int nValue = rtk_patch.nTotal|0x80;
	if (index == nValue) {
		rtk_patch.nTxIndex = rtk_patch.nTotal;
	} else {
		rtk_patch.nTxIndex = index;
	}
	hcipatch[2] = len+1;
	memcpy(hcipatch+3, &cp, len+1);

	struct sk_buff *nskb = h5_prepare_pkt(&rtk_h5, hcipatch, len+4, HCI_COMMAND_PKT); /* data: len + head: 4 */

	if (rtk_h5.host_last_cmd) {
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
	}

	rtk_h5.host_last_cmd = nskb;

	len = write(dd, nskb->data, nskb->data_len);
	RS_DBG("hci_download_patch nTxIndex:%d nRxIndex: %d\n", rtk_patch.nTxIndex, rtk_patch.nRxIndex);

	if (index & 0x80) {
		RS_DBG("Hw Flow Control enable after last command sent before last event recv ! ");
		if (tcsetattr(dd, TCSADRAIN, ti) < 0) {
			RS_ERR("Can't set port settings");
			return -1;
		}
	}

	while (rtk_patch.nRxIndex != rtk_patch.nTxIndex) {  /* receive data and wait last pkt */
		retlen = read_check(dd, &bytes, READ_DATA_SIZE);
		if (retlen == -1) {
			perror("read fail");
			return -1;
		}
		h5_recv(&rtk_h5, &bytes, retlen);
	}

	alarm(0);
	return 0;
}

/**
* Download h4 patch
*
* @param dd uart file descriptor
* @param index current index
* @param data point to the config file
* @param len current buf length
* @return ret_index
*
*/
static int hci_download_patch_h4(int dd, int index, uint8_t *data, int len)
{
	 char bytes[257] = {0};
	 char buf[257] = {0x01, 0x20, 0xfc, 00};

	RS_DBG("dd:%d, index:%d, len:%d", dd, index, len);
	if (NULL != data) {
		memcpy(&buf[5], data, len);
	}

	int cur_index = index;
	int ret_Index = -1;

	/* Set data struct. */
	buf[3] = len + 1; /* add index */
	buf[4] = cur_index;
	size_t total_len = len + 5;

	/* write */
	uint16_t w_len;
	w_len = write(dd, buf, total_len);
	RS_DBG("h4 write success with len: %d.\n", w_len);

	uint16_t res;
	res = read(dd, bytes, 8);

	if (DBG_ON) {
		RS_DBG("h4 read success with len: %d.\n", res);
		int i = 0;
		for (i = 0; i < 8; i++) {
			fprintf(stderr, "byte[%d] = 0x%x\n", i, bytes[i]);
		}
	}

	uint8_t rstatus;
	if ((0x04 == bytes[0]) && (0x20 == bytes[4]) && (0xfc == bytes[5])) {
		ret_Index = bytes[7];
		rstatus = bytes[6];
		RS_DBG("---->ret_Index:%d, ----->rstatus:%d\n", ret_Index, rstatus);
		if (0x00 != rstatus) {
			RS_ERR("---->read event status is wrong.\n");
			return -1;
		}
	} else {
		RS_ERR("==========>Didn't read curret data.\n");
		return -1;
	}

	return ret_Index;
}

/**
* Realtek change speed with h4 proto. Using vendor specified command packet to achieve this.
*
* @warning before write, need to wait 1s for device up
*
* @param fd uart file descriptor
* @param baudrate the speed want to change
* @return #0 on success
*/
static int rtk_vendor_change_speed_h4(int fd, uint32_t baudrate)
{
	 char bytes[257];
	uint8_t cmd[8] = {0};

	cmd[0] = 1;    /* cmd */
	cmd[1] = 0x17; /* ocf */
	cmd[2] = 0xfc; /* ogf, vendor specified */

	cmd[3] = 4;    /* length */
#ifdef BAUDRATE_4BYTES
	memcpy((uint16_t *)&cmd[4], &baudrate, 4);
#else
	memcpy((uint16_t *)&cmd[4], &baudrate, 2);

	cmd[6] = 0;
	cmd[7] = 0;
#endif

	/* wait for a while for device to up, just h4 need it */
	sleep(1);

	if (write(fd, cmd, 8) != 8) {
		RS_ERR("H4 change uart speed error when writing vendor command");
		return -1;
	}
	RS_DBG("H4 Change uart Baudrate after write ");
	int res;
	res = read(fd, bytes, sizeof(bytes));

	if (DBG_ON) {
		RS_DBG("Realtek Receving H4 change uart speed event:%x", res);
		RS_DUMP(bytes, res);
	}
	if ((0x04 == bytes[0]) && (0x17 == bytes[4]) && (0xfc == bytes[5])) {
		RS_DBG("H4 change uart speed success, receving status:%x", bytes[6]);
		if (bytes[6] == 0)
			return 0;
	}
	return -1;
}

/**
* Parse realtek Bluetooth config file.
* The config file if begin with vendor magic: RTK_VENDOR_CONFIG_MAGIC(8723ab55)
* bt_addr is followed by 0x3c offset, it will be changed by bt_addr param
* proto, baudrate and flow control is followed by 0xc offset,
*
* @param config_buf point to config file content
* @param filelen length of config file
* @param bt_addr where bt addr is stored
* @return baudrate in config file
*
*/

static uint32_t rtk_parse_config_file(uint8_t *config_buf, size_t filelen, char bt_addr[6])
{
	struct rtk_bt_vendor_config *config = (struct rtk_bt_vendor_config *) config_buf;
	uint16_t config_len = le16_to_cpu(config->data_len), temp = 0;
	struct rtk_bt_vendor_config_entry *entry = config->entry;

	uint16_t i;
	uint32_t baudrate = 0;

	if (le32_to_cpu(config->signature) != RTK_VENDOR_CONFIG_MAGIC) {
		RS_ERR("config signature magic number(%x) is not set to RTK_VENDOR_CONFIG_MAGIC", (int)config->signature);
		return 0;
	}

	if (config_len != filelen - sizeof(struct rtk_bt_vendor_config)) {
		RS_ERR("config len(%x) is not right(%x)", config_len, filelen-sizeof(struct rtk_bt_vendor_config));
		return 0;
	}

	for (i = 0; i < config_len;) {
		switch (le16_to_cpu(entry->offset)) {
		case 0x3c:
			{
				int j = 0;
				for (j = 0; j < entry->entry_len; j++)
					entry->entry_data[j] = bt_addr[entry->entry_len - 1 - j];
			}
			break;
		case 0xc:
#ifdef BAUDRATE_4BYTES
			baudrate = *(uint32_t *)entry->entry_data;
#else
			baudrate = *(uint16_t *)entry->entry_data;
#endif
			gHwFlowControlEnable = 0;
			if (entry->entry_len >= 12) { //0ffset 0x18 - 0xc
				gHwFlowControlEnable = (entry->entry_data[12] & 0x4) ? 1:0; //0x18 byte bit2
			}
			RS_DBG("config baud rate to :%x, hwflowcontrol:%x, %x", (int)baudrate, entry->entry_data[12], gHwFlowControlEnable);
			break;
		default:
			RS_DBG("config offset(%x),length(%x)", entry->offset, entry->entry_len);
			break;
		}
		temp = entry->entry_len + sizeof(struct rtk_bt_vendor_config_entry);
		i += temp;
		entry = (struct rtk_bt_vendor_config_entry *)((uint8_t *)entry + temp);
	}

	return baudrate;
}

/**
* get random realtek Bluetooth addr.
*
* @param bt_addr where bt addr is stored
*
*/
static void rtk_get_ram_addr(char bt_addr[0])
{
	srand(time(NULL)+getpid()+getpid()*987654+rand());

	uint32_t addr = rand();
	memcpy(bt_addr, &addr, sizeof(uint8_t));
}

/**
* Write the random bt addr to the file /data/misc/bluetooth/btmac.txt.
*
* @param bt_addr where bt addr is stored
*
*/
static void rtk_write_btmac2file(char bt_addr[6])
{
	int fd;
	mkdir(BT_ADDR_DIR, 0777);
	fd = open(BT_ADDR_FILE, O_CREAT|O_RDWR|O_TRUNC, 0666);

	if (fd > 0) {
		chmod(BT_ADDR_FILE, 0666);
		char addr[18] = {0};
		addr[17] = '\0';
		sprintf(addr, "%2x:%2x:%2x:%2x:%2x:%2x", bt_addr[0], bt_addr[1], bt_addr[2], bt_addr[3], bt_addr[4], bt_addr[5]);
		write(fd, addr, strlen(addr));
		close(fd);
	} else {
		RS_ERR("open file error:%s\n", BT_ADDR_FILE);
	}
}

/**
* Get realtek Bluetooth config file. The bt addr arg is stored in /data/btmac.txt, if there is not this file,
* change to /data/misc/bluetooth/btmac.txt. If both of them are not found, using
* random bt addr.
*
* The config file is rtk8723_bt_config whose bt addr will be changed by the one read previous
*
* @param config_buf point to the content of realtek Bluetooth config file
* @param config_baud_rate the baudrate set in the config file
* @return file_len the length of config file
*/
static int rtk_get_bt_config(uint8_t **config_buf, uint32_t *config_baud_rate, patch_info *info)
{
	char bt_config_file_name[PATH_MAX] = {0};
	char bt_addr[6] = {0x00, 0xe0, 0x4c, 0x88, 0x88, 0x88};
	struct stat st;
	size_t filelen;
	int fd;
	FILE *file = NULL;
	int i = 0;

	sprintf(bt_config_file_name, "%s/%s", BT_CONFIG_DIRECTORY, "btmac.txt");
	RS_DBG("BT mac addr file: %s", bt_config_file_name);
	if (stat(bt_config_file_name, &st) < 0) {
		RS_ERR("can't access bt bt_mac_addr file:%s, try use another path", bt_config_file_name);
		sprintf(bt_config_file_name, BT_ADDR_FILE);
		if (stat(bt_config_file_name, &st) < 0) {
			RS_ERR("can't access bt bt_mac_addr file:%s, try use ramdom BT Addr", bt_config_file_name);

			for (i = 0; i < 6; i++)
				rtk_get_ram_addr(&bt_addr[i]);
			rtk_write_btmac2file(bt_addr);
			goto GET_CONFIG;
		}
	}

	filelen = st.st_size;
	file = fopen(bt_config_file_name, "rb");
	if (file == NULL) {
		RS_ERR("Can't open bt btaddr file, just use preset BT Addr");
	} else {
		fscanf(file, "%2x:%2x:%2x:%2x:%2x:%2x", (int *)&bt_addr[0], (uint32_t *)&bt_addr[1], (uint32_t *)&bt_addr[2], (uint32_t *)&bt_addr[3], (uint32_t *)&bt_addr[4], (uint32_t *)&bt_addr[5]);
		/* do not set bt_add[0] to zero */
		/* reserve LAP addr from 0x9e8b00 to 0x9e8b3f, change to 0x008b */
		if (0x9e == bt_addr[3] && 0x8b == bt_addr[4] && (bt_addr[5] <= 0x3f)) {
			/* get random value */
			bt_addr[3] = 0x00;
		}
		RS_DBG("BT MAC IS : %02X:%02X:%02X:%02X:%02X:%02X", bt_addr[0], bt_addr[1], bt_addr[2], bt_addr[3], bt_addr[4], bt_addr[5]);
	}

GET_CONFIG:
	if (info != NULL) {
		 sprintf(bt_config_file_name, "%s/%s", BT_CONFIG_DIRECTORY, info->config_name);
	} else {
		 sprintf(bt_config_file_name, "%s/%s", BT_CONFIG_DIRECTORY, "rtlbt_config");
	}
	RS_DBG("Final bt config file: %s", bt_config_file_name);

	if (stat(bt_config_file_name, &st) < 0) {
		RS_ERR("Can't access firmware, errno:%d", errno);
		return -1;
	}

	filelen = st.st_size;

	fd = open(bt_config_file_name, O_RDONLY);
	if (fd < 0) {
		perror("Can't open bt config file");
		return -1;
	}

	*config_buf = malloc(filelen);
	if (*config_buf == NULL) {
		RS_DBG("malloc buffer for config file fail(%x)\n", filelen);
		close(fd);
		return -1;
	}

	/* we may need to parse this config file. */
	/* for easy debug, only get need data. */
	if (read(fd, *config_buf, filelen) < (ssize_t)filelen) {
		perror("Can't load bt config file");
		free(*config_buf);
		*config_buf = NULL;
		close(fd);
		return -1;
	}

	*config_baud_rate = rtk_parse_config_file(*config_buf, filelen, bt_addr);
	RS_DBG("Get config baud rate(4 bytes) from config file:%x", (int)*config_baud_rate);

	close(fd);
	return filelen;
}

/**
* Realtek change speed with h5 proto. Using vendor specified command packet to achieve this.
*
* @warning it will waiting 2s for reply.
*
* @param fd uart file descriptor
* @param baudrate the speed want to change
*
*/
static int rtk_vendor_change_speed_h5(int fd, uint32_t baudrate)
{
	struct  sk_buff *cmd_change_bdrate = NULL;
	uint8_t cmd[7] = {0};
	int     retlen;
	char    bytes[READ_DATA_SIZE];
	struct  sigaction sa;

	sa.sa_handler = h5_tinit_sig_alarm;
	sigaction(SIGALRM, &sa, NULL);

	cmd[0] = 0x17; /* ocf */
	cmd[1] = 0xfc; /* ogf, vendor specified */

	cmd[2] = 4;    /* length */
#ifdef BAUDRATE_4BYTES
	memcpy((uint16_t *)&cmd[3], &baudrate, 4);
#else
	memcpy((uint16_t *)&cmd[3], &baudrate, 2);
	cmd[5] = 0;
	cmd[6] = 0;
#endif

	if (DBG_ON) {
		RS_DUMP(cmd, 7);
		RS_DBG("change speed command ready baudrate=%d n", baudrate);
	}
	cmd_change_bdrate = h5_prepare_pkt(&rtk_h5, cmd, 7, HCI_COMMAND_PKT);
	if (!cmd_change_bdrate) {
		RS_ERR("Prepare command packet for change speed fail");
		return -1;
	}

	rtk_h5.host_last_cmd = cmd_change_bdrate;
	alarm(1);
	write(fd, cmd_change_bdrate->data, cmd_change_bdrate->data_len);

	while (rtk_h5.link_estab_state == H5_INIT) {
		retlen = read_check(fd, &bytes, READ_DATA_SIZE);
		if (retlen == -1) {
			perror("read fail");
			return -1;
		}

		/* add pure ack check */
		h5_recv(&rtk_h5, &bytes, retlen);
	}

	alarm(0);
	return 0;
}

/**
* Init realtek Bluetooth h5 proto. h5 proto is added by realtek in the right kernel.
* Generally there are two steps: h5 sync and h5 config
*
* @param fd uart file descriptor
* @param ti termios struct
*
*/
static int rtk_init_h5(int fd, struct termios *ti)
{
	 char bytes[READ_DATA_SIZE];
	struct sigaction sa;
	int retlen;
	struct itimerval value;

	/* set even parity here */
	ti->c_cflag |= PARENB;
	ti->c_cflag &= ~(PARODD);
	if (tcsetattr(fd, TCSANOW, ti) < 0) {
		RS_ERR("Can't set port settings");
		return -1;
	}

	alarm(0);
	serial_fd = fd;
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = h5_tshy_sig_alarm;
	sigaction(SIGALRM, &sa, NULL);

	/* h5 sync */
	h5_tshy_sig_alarm(0);
	memset(&rtk_h5, 0, sizeof(rtk_h5));
	rtk_h5.link_estab_state = H5_SYNC;
	while (rtk_h5.link_estab_state == H5_SYNC) {
		retlen = read_check(fd, &bytes, READ_DATA_SIZE);
		if (retlen == -1) {
			RS_ERR("H5 Read Sync Response Failed");
			/* retry per 250ms */
			value.it_value.tv_sec = 0;
			value.it_value.tv_usec = 0;
			value.it_interval.tv_sec = 0;
			value.it_interval.tv_usec = 0;
			setitimer(ITIMER_REAL, &value, NULL);
			return -1;
		}
		h5_recv(&rtk_h5, &bytes, retlen);
	}

	/* retry per 250ms */
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, NULL);

	/* h5 config */
	sa.sa_handler = h5_tconf_sig_alarm;
	sigaction(SIGALRM, &sa, NULL);
	h5_tconf_sig_alarm(0);
	while (rtk_h5.link_estab_state == H5_CONFIG) {
		retlen = read_check(fd, &bytes, READ_DATA_SIZE);
		if (retlen == -1) {
			RS_ERR("H5 Read Config Response Failed");
			/* retry per 250ms */
			value.it_value.tv_sec = 0;
			value.it_value.tv_usec = 0;
			value.it_interval.tv_sec = 0;
			value.it_interval.tv_usec = 0;
			setitimer(ITIMER_REAL, &value, NULL);
			return -1;
		}
		h5_recv(&rtk_h5, &bytes, retlen);
	}
	/* retry per 250ms */
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, NULL);

	rtk_send_pure_ack_down(fd);
	RS_DBG("H5 init finished\n");
	return 0;
}

/**
* Download realtek firmware and config file from uart with the proto.
* Parse the content to serval packets follow the proto and then write the packets from uart
*
* @param fd uart file descriptor
* @param buf addr where stor the content of firmware and config file
* @param filesize length of buf
* @param is_sent_changerate if baudrate need to be changed
* @param proto realtek Bluetooth protocol, shall be either HCI_UART_H4 or HCI_UART_3WIRE
*
*/
static void rtk_download_fw_config(int fd, uint8_t *buf, size_t filesize, int is_sent_changerate, int proto, struct termios *ti)
{
	uint8_t iCurIndex = 0;
	uint8_t iCurLen = 0;
	uint8_t iEndIndex = 0;
	uint8_t iLastPacketLen = 0;
	uint8_t iAdditionPkt = 0;
	uint8_t iTotalIndex = 0;
	uint8_t iCmdSentNum = 0;   /* the number of CMDs which have been sent */
	uint8_t *bufpatch;

	iEndIndex = (uint8_t)((filesize-1)/PATCH_DATA_FIELD_MAX_SIZE);
	iLastPacketLen = (filesize)%PATCH_DATA_FIELD_MAX_SIZE;

	if (is_sent_changerate)
		iCmdSentNum++;
	if (gRom_version_cmd_state >= cmd_has_sent)
		iCmdSentNum++;
	if (ghci_version_cmd_state >= cmd_has_sent)
		iCmdSentNum++;
	if (gchip_type_cmd_state   >= cmd_has_sent)
		iCmdSentNum++;

	iAdditionPkt = (iEndIndex+1+iCmdSentNum)%8?(8-(iEndIndex+1+iCmdSentNum)%8):0;
	iTotalIndex = iAdditionPkt + iEndIndex;
	rtk_patch.nTotal = iTotalIndex; /* init TotalIndex */

	RS_DBG("iEndIndex:%d  iLastPacketLen:%d iAdditionpkt:%d\n", iEndIndex, iLastPacketLen, iAdditionPkt);

	if (iLastPacketLen == 0)
		iLastPacketLen = PATCH_DATA_FIELD_MAX_SIZE;

	bufpatch = buf;

	int i;
	for (i = 0; i <= iTotalIndex; i++) {
		if (iCurIndex < iEndIndex) {
			iCurIndex = iCurIndex&0x7F;
			iCurLen = PATCH_DATA_FIELD_MAX_SIZE;
		} else if (iCurIndex == iEndIndex) { /* send last data packet */
			if (iCurIndex == iTotalIndex)
				iCurIndex = iCurIndex | 0x80;
			else
				iCurIndex = iCurIndex&0x7F;
			iCurLen = iLastPacketLen;
		} else if (iCurIndex < iTotalIndex) {
			iCurIndex = iCurIndex&0x7F;
			bufpatch = NULL;
			iCurLen = 0;
		} else {                             /* send end packet */
			bufpatch = NULL;
			iCurLen = 0;
			iCurIndex = iCurIndex|0x80;
		}

		if (iCurIndex & 0x80)
			RS_DBG("Send FW last command");

		if (proto == HCI_UART_H4) {
			iCurIndex = hci_download_patch_h4(fd, iCurIndex, bufpatch, iCurLen);
			if ((iCurIndex != i) && (i != rtk_patch.nTotal)) {
				/* check index but ignore last pkt */
				RS_DBG("index mismatch i:%d iCurIndex:%d, patch fail\n", i, iCurIndex);
				return;
			}
		} else if (proto == HCI_UART_3WIRE)
			hci_download_patch(fd, iCurIndex, bufpatch, iCurLen, ti);

		if (iCurIndex < iEndIndex) {
			bufpatch += PATCH_DATA_FIELD_MAX_SIZE;
		}
		iCurIndex++;
	}

	/* set last ack packet down */
	if (proto == HCI_UART_3WIRE) {
		rtk_send_pure_ack_down(fd);
	}
}

/**
* Get realtek Bluetooth firmaware file. The content will be saved in *fw_buf which is malloc here.
* The length malloc here will be lager than length of firmware file if there is a config file.
* The content of config file will copy to the tail of *fw_buf in rtk_config.
*
* @param fw_buf point to the addr where stored the content of firmware.
* @param addi_len length of config file.
* @return length of *fw_buf.
*
*/
static int rtk_get_bt_firmware(uint8_t **fw_buf, size_t addi_len, patch_info *info)
{
	char filename[PATH_MAX] = {0};
	struct stat st;
	int fd = -1;
	size_t fwsize, buf_size;

	if (info != NULL) {
		 sprintf(filename, "%s/%s", FIRMWARE_DIRECTORY, info->patch_name);
	} else {
		 sprintf(filename, "%s/%s", FIRMWARE_DIRECTORY, "rtlbt_fw");
	}

	RS_DBG("Final bt firmware file: %s", filename);

	if (stat(filename, &st) < 0) {
		RS_ERR("Can't access firmware, errno:%d", errno);
		return -1;
	}

	fwsize = st.st_size;
	buf_size = fwsize + addi_len;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		RS_ERR("Can't open firmware, errno:%d", errno);
		return -1;
	}

	*fw_buf = malloc(buf_size);
	if (!(*fw_buf)) {
		RS_ERR("Can't alloc memory for fw&config, errno:%d", errno);
		close(fd);
		return -1;
	}

	if (read(fd, *fw_buf, fwsize) < (ssize_t) fwsize) {
		free(*fw_buf);
		*fw_buf = NULL;
		close(fd);
		return -1;
	}
	RS_DBG("Load FW OK");
	close(fd);
	return buf_size;
}

/* These two function(rtk<-->uart speed transfer) need check Host uart speed at first!!!! IMPORTANT
 * add more speed if neccessary */
typedef struct _baudrate_ex {
	uint32_t rtk_speed;
	int uart_speed;
} baudrate_ex;

#ifdef BAUDRATE_4BYTES
static baudrate_ex baudrates[] = {
	{0x00006004, 921600},
	{0x05F75004, 921600},   // RTL8723BS
	{0x00004003, 1500000},
	{0x04928002, 1500000},  // RTL8723BS
	{0x00005002, 2000000},  // same as RTL8723AS
	{0x00008001, 3000000},
	{0x00009001, 3000000},  // Lory add new, t169 and t9e use 0x00009001.
	{0x06DD8001, 3000000},  // RTL8723BS, Baudrate: 2920000
	{0x036D8001, 3000000},  // RTL8723BS, Baudrate: 2929999
	{0x06B58001, 3000000},  // RTL8723BS, Baudrate: 2940000
	{0x02B58001, 3000000},  // RTL8723BS, Baudrate: 2945000
	{0x02D58001, 3000000},  // RTL8723BS, Baudrate: 2950000
	{0x05558001, 3000000},  // RTL8723BS, Baudrate: 2960000
	{0x02AA8001, 3000000},  // RTL8723BS, Baudrate: 2969999
	{0x052A8001, 3000000},  // RTL8723BS, Baudrate: 2980000
	{0x04928001, 3000000},  // RTL8723BS, Baudrate: 2998000
	{0x00007001, 3500000},
	{0x052A6001, 3500000},  // RTL8723BS
	{0x00005001, 4000000},  // same as RTL8723AS
	{0x05AD9005, 547000},
	{0x0252C00A, 230400},
	{0x0000701d, 115200},
	{0x0252C002, 115200},   // RTL8723BS
	{0x0252C014, 115200}    // RTL8723BS
};

#else
static baudrate_ex baudrates[] = {
	{0x7001, 3500000},
	{0x6004, 921600},
	{0x4003, 1500000},
	{0x5001, 4000000},
	{0x5002, 2000000},
	{0x8001, 3000000},
	{0x9001, 3000000},
	{0x701d, 115200}
};

#endif


/**
* Change realtek Bluetooth speed to uart speed. It is matching in the struct baudrates:
*
* @code
* baudrate_ex baudrates[] =
* {
*  	{0x7001, 3500000},
*	{0x6004, 921600},
*	{0x4003, 1500000},
*	{0x5001, 4000000},
*	{0x5002, 2000000},
*	{0x8001, 3000000},
*	{0x701d, 115200}
* };
* @endcode
*
* If there is no match in baudrates, uart speed will be set as #115200.
*
* @param rtk_speed realtek Bluetooth speed
* @param uart_speed uart speed
*
*/
static void rtk_speed_to_uart_speed(uint32_t rtk_speed, uint32_t *uart_speed)
{
	*uart_speed = 115200;

	 int i;
	for (i = 0; i < sizeof(baudrates)/sizeof(baudrate_ex); i++) {
		if (baudrates[i].rtk_speed == le32_to_cpu(rtk_speed)) {
			*uart_speed = baudrates[i].uart_speed;
			return;
		}
	}
	return;
}

/**
* Change uart speed to realtek Bluetooth speed. It is matching in the struct baudrates:
*
* @code
* baudrate_ex baudrates[] =
* {
*  	{0x7001, 3500000},
*	{0x6004, 921600},
*	{0x4003, 1500000},
*	{0x5001, 4000000},
*	{0x5002, 2000000},
*	{0x8001, 3000000},
*	{0x701d, 115200}
* };
* @endcode
*
* If there is no match in baudrates, realtek Bluetooth speed will be set as #0x701D.
*
* @param uart_speed uart speed
* @param rtk_speed realtek Bluetooth speed
*
*/
static inline void uart_speed_to_rtk_speed(int uart_speed, uint32_t *rtk_speed)
{
	*rtk_speed = 0x701D;

	int i;
	for (i = 0; i < sizeof(baudrates) / sizeof(baudrate_ex); i++) {
		if (baudrates[i].uart_speed == uart_speed) {
			*rtk_speed = baudrates[i].rtk_speed;
			return;
		}
	}

	return;
}

static void rtk_get_eversion_timeout(int sig)
{
	static int retries;
	RS_DBG("RTK get HCI_VENDOR_READ_RTK_ROM_VERISION_Command\n");
	if (retries < h5_max_retries) {
		RS_DBG("patch timerout, retry:\n");
		if (rtk_h5.host_last_cmd) {
			write(serial_fd, rtk_h5.host_last_cmd->data, rtk_h5.host_last_cmd->data_len);
			RS_DBG("3-wire download patch re send:%d", retries);
		}
		retries++;
		alarm(3);
		return;
	}
	tcflush(serial_fd, TCIOFLUSH);
	RS_ERR("rtk get eversion cmd complete event timed out\n");
	exit(1);
}

/**
* Send vendor cmd to get eversion: 0xfc6d
* If Rom code does not support this cmd, use default.
*/
static void rtk_get_eversion(int dd)
{
	char bytes[READ_DATA_SIZE];
	int retlen;
	struct sigaction sa;
	/* send HCI_VENDOR_READ_RTK_ROM_VERISION Command */
	uint8_t read_rom_patch_cmd[3] = {0x6d, 0xfc, 0x00};
	struct  sk_buff *nskb = h5_prepare_pkt(&rtk_h5, read_rom_patch_cmd, 3, HCI_COMMAND_PKT); /* data: len+head: 4 */

	if (rtk_h5.host_last_cmd) {
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
	}

	rtk_h5.host_last_cmd = nskb;

	write(dd, nskb->data, nskb->data_len);
	gRom_version_cmd_state = cmd_has_sent;
	RS_DBG("RTK send HCI_VENDOR_READ_RTK_ROM_VERISION_Command\n");

	alarm(0);
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = rtk_get_eversion_timeout;
	sigaction(SIGALRM, &sa, NULL);

	alarm(3);
	while (gRom_version_cmd_state != event_received) {
		retlen = read_check(dd, &bytes, READ_DATA_SIZE);
		if (retlen == -1) {
			perror("read fail");
			return;
		}
		h5_recv(&rtk_h5, &bytes, retlen);
	}
	alarm(0);
	return;
}

static void rtk_get_lmp_version_timeout(int sig)
{
	static int retries;
	RS_DBG("RTK get HCI_VENDOR_READ_RTK_LMP_VERISION_Command\n");
	if (retries < h5_max_retries) {
		RS_DBG("patch timerout, retry:\n");
		if (rtk_h5.host_last_cmd) {
			write(serial_fd, rtk_h5.host_last_cmd->data, rtk_h5.host_last_cmd->data_len);
			RS_DBG("3-wire download patch re send:%d", retries);
		}
		retries++;
		alarm(3);
		return;
	}
	tcflush(serial_fd, TCIOFLUSH);
	RS_ERR("rtk get lmp version cmd complete event timed out\n");
	exit(1);
}

static void rtk_get_lmp_version(int dd)
{
	char bytes[READ_DATA_SIZE];
	int retlen;
	struct sigaction sa;
	/* send HCI_VENDOR_READ_RTK_ROM_VERISION_Command */
	uint8_t read_rom_patch_cmd[3] = {0x01, 0x10, 00};
	struct  sk_buff *nskb = h5_prepare_pkt(&rtk_h5, read_rom_patch_cmd, 3, HCI_COMMAND_PKT); /* data: len+head: 4 */

	if (rtk_h5.host_last_cmd) {
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
	}

	rtk_h5.host_last_cmd = nskb;

	write(dd, nskb->data, nskb->data_len);
	ghci_version_cmd_state = cmd_has_sent;
	RS_DBG("RTK send HCI_VENDOR_READ_RTK_ROM_VERISION_Command\n");

	alarm(0);
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = rtk_get_lmp_version_timeout;
	sigaction(SIGALRM, &sa, NULL);

	alarm(3);
	while (ghci_version_cmd_state != event_received) {
		retlen = read_check(dd, &bytes, READ_DATA_SIZE);
		if (retlen == -1) {
			perror("read fail");
			return;
		}
		h5_recv(&rtk_h5, &bytes, retlen);
	}
	alarm(0);
	return;
}

static void rtk_get_chip_type(int dd)
{
	char bytes[READ_DATA_SIZE];
	int retlen;
	struct sigaction sa;
	uint8_t read_rom_patch_cmd[8] = {0x61, 0xfc, 0x05, 0x00, 0x94, 0xa0, 0x00, 0xb0};
	struct  sk_buff *nskb = h5_prepare_pkt(&rtk_h5, read_rom_patch_cmd, 8, HCI_COMMAND_PKT); /* data: len+head: 4 */

	if (rtk_h5.host_last_cmd) {
		skb_free(rtk_h5.host_last_cmd);
		rtk_h5.host_last_cmd = NULL;
	}

	rtk_h5.host_last_cmd = nskb;

	write(dd, nskb->data, nskb->data_len);
	gchip_type_cmd_state = cmd_has_sent;
	RS_DBG("RTK send HCI_VENDOR_READ_CHIP_TYPE Command");

	alarm(0);
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = rtk_get_lmp_version_timeout;
	sigaction(SIGALRM, &sa, NULL);

	alarm(3);
	while (gchip_type_cmd_state != event_received) {
		retlen = read_check(dd, &bytes, READ_DATA_SIZE);
		if (retlen == -1) {
			perror("read fail");
			return;
		}
		h5_recv(&rtk_h5, &bytes, retlen);
	}
	alarm(0);
	return;
}

static int check_match_state(bt_hw_cfg_cb_t *cfg, uint32_t mask)
{
	patch_info *patch_entry;
	int res = 0;

	for (patch_entry = patch_table; patch_entry->lmp_subversion != LMP_SUBVERSION_NONE; patch_entry++) {
		if (patch_entry->lmp_subversion != cfg->lmp_subversion)
			continue;
		if ((patch_entry->hci_version_mask != HCI_VERSION_MASK_ALL) && ((patch_entry->hci_version_mask & (1 << cfg->hci_version)) == 0))
			 continue;
		if ((patch_entry->hci_revision_mask != HCI_REVISION_MASK_ALL) && ((patch_entry->hci_revision_mask & (1 << cfg->hci_revision)) == 0))
			 continue;
		if ((mask & PATCH_OPTIONAL_MATCH_FLAG_CHIPTYPE) && (patch_entry->chip_type_mask != CHIP_TYPE_MASK_ALL) && ((patch_entry->chip_type_mask & (1 << cfg->chip_type)) == 0))
			 continue;
		res++;
	}
	RS_DBG("check_match_state return %d(cfg->lmp_subversion:0x%x cfg->hci_vesion:0x%x cfg->hci_revision:0x%x cfg->chip_type:0x%x mask:%08x)",
			res, cfg->lmp_subversion, cfg->hci_version, cfg->hci_revision, cfg->chip_type, mask);
	return res;
}

static patch_info *get_patch_entry(bt_hw_cfg_cb_t *cfg)
{
	patch_info *patch_entry;

	RS_DBG("get_patch_entry(lmp_subversion:0x%x hci_vesion:0x%x cfg->hci_revision:0x%x chip_type:0x%x)",
			cfg->lmp_subversion, cfg->hci_version, cfg->hci_revision, cfg->chip_type);
	for (patch_entry = patch_table; patch_entry->lmp_subversion != LMP_SUBVERSION_NONE; patch_entry++) {
		if (patch_entry->lmp_subversion != cfg->lmp_subversion)
			continue;
		if ((patch_entry->hci_version_mask != HCI_VERSION_MASK_ALL) && ((patch_entry->hci_version_mask & (1 << cfg->hci_version)) == 0))
			 continue;
		if ((patch_entry->hci_revision_mask != HCI_REVISION_MASK_ALL) && ((patch_entry->hci_revision_mask & (1<<cfg->hci_revision)) == 0))
			 continue;
		if ((patch_entry->chip_type_mask != CHIP_TYPE_MASK_ALL) && ((patch_entry->chip_type_mask & (1<<cfg->chip_type)) == 0))
			 continue;
		break;
	}
	RS_DBG("get_patch_entry return(patch_name:%s config_name:%s mac_offset:0x%x)",
			patch_entry->patch_name, patch_entry->config_name, patch_entry->mac_offset);
	return patch_entry;
}

/**
* Config realtek Bluetooth. The configuration parameter is get from config file and fw.
*
* @warning maybe only one of config file and fw file exists. The bt_addr arg is stored in "/data/btmac.txt"
* or "/data/misc/bluetoothd/bt_mac/btmac.txt",
*
* @param fd uart file descriptor
* @param proto realtek Bluetooth protocol, shall be either HCI_UART_H4 or HCI_UART_3WIRE
* @param speed init_speed in uart struct
* @param ti termios struct
* @returns #0 on success
*/
static int rtk_config(int fd, int proto, int speed, struct termios *ti)
{
	int config_len = -1, buf_len = -1, final_speed = 0;
	uint8_t *config_file_buf = NULL;
	uint8_t *buf = NULL;
	uint32_t baudrate = 0;

	uint8_t *epatch_buf = NULL;
	struct rtk_epatch *epatch_info = NULL;
	struct rtk_epatch_entry current_entry;
	uint8_t need_download_fw = 1;
	patch_info *prtk_patch_file_info = NULL;

	current_entry.start_offset = 0;
	current_entry.patch_length = 0;
	current_entry.chipID       = 0;

	/*
	* 1. if both config file and fw exists, use it and change rate according to config file
	* 2. if config file not exists while fw does, not change baudrate and only download fw
	* 3. if fw doesnot exist, only change rate to 3.25M or from config file if it exist. This case is only for early debug before any efuse is setting.
	*/

	/* Get version from ROM */
	rtk_get_lmp_version(fd);
	if (check_match_state(&hw_cfg_cb, 0) > 1) {
		rtk_get_chip_type(fd);
		RS_DBG("lmp_subversion = 0x%x, hci_version = 0x%x, hci_revision = 0x%x, chip_type = 0x%x",
				hw_cfg_cb.lmp_subversion, hw_cfg_cb.hci_version, hw_cfg_cb.hci_revision, hw_cfg_cb.chip_type);
	}

	prtk_patch_file_info = get_patch_entry(&hw_cfg_cb);

	if (prtk_patch_file_info == NULL) {
		RS_ERR("lmp_version is %x, no matched project found!", hw_cfg_cb.lmp_subversion);
		need_download_fw = 0;
		goto FETCH_DONE;
	}

	config_len = rtk_get_bt_config(&config_file_buf, &baudrate, prtk_patch_file_info);
	if (config_len < 0) {
		RS_ERR("Get Config file error, just use efuse settings");
		config_len = 0;
	}

	buf_len = rtk_get_bt_firmware(&epatch_buf, config_len, prtk_patch_file_info);
	if (buf_len < 0) {
		RS_ERR("Get BT firmware error, continue without bt firmware");
		goto FETCH_DONE;
	}

	if (hw_cfg_cb.lmp_subversion == ROM_LMP_8723a) {
		if (memcmp(epatch_buf, RTK_EPATCH_SIGNATURE, 8) == 0) {
			RS_ERR("8723as Check signature error!");
			need_download_fw = 0;
			goto FETCH_DONE;
		}

		buf = malloc(buf_len);
		if (!buf) {
			RS_ERR("Can't alloc memory for fw&config, errno:%d", errno);
			buf_len = -1;
			goto FETCH_DONE;
		}

		RS_DBG("8723as, fw copy direct");
		memcpy(buf, epatch_buf, buf_len);
	} else {
		/* Get version from ROM */
		rtk_get_eversion(fd);  /* gEVersion is set. */
		RS_DBG("gEVersion=%d", gEVersion);
		/* check Extension Section Field */
		if (memcmp(epatch_buf + buf_len - config_len - 4, Extension_Section_SIGNATURE, 4) != 0) {
			RS_ERR("Check Extension_Section_SIGNATURE error! do not download fw");
			need_download_fw = 0;
			goto FETCH_DONE;
		}

		if (memcmp(epatch_buf, RTK_EPATCH_SIGNATURE, 8) != 0) {
			RS_DBG("Check signature error!");
			need_download_fw = 0;
			goto FETCH_DONE;
		}

		int i = 0;
    	uint32_t value_t = 0;
		epatch_info = (struct rtk_epatch *)epatch_buf;
		value_t = le32_to_cpu(epatch_info->fm_version);
		epatch_info->fm_version = (uint32_t)value_t;
		value_t = le16_to_cpu(epatch_info->number_of_total_patch);
		epatch_info->number_of_total_patch = (uint16_t)value_t;
		RS_DBG("fm_version = 0x%x", epatch_info->fm_version);
		RS_DBG("number_of_total_patch = %d", epatch_info->number_of_total_patch);
		/* get right epatch entry */
		for (i = 0; i < epatch_info->number_of_total_patch; i++) {
			if (le16_to_cpu(*(uint16_t *)(epatch_buf + 14 + 2 * i)) == gEVersion + 1) {
				current_entry.chipID = gEVersion + 1;
				current_entry.patch_length = le16_to_cpu(*(uint16_t *)(epatch_buf + 14 + 2 * epatch_info->number_of_total_patch + 2 * i));
				current_entry.start_offset = le32_to_cpu(*(uint32_t *)(epatch_buf + 14 + 4 * epatch_info->number_of_total_patch + 4 * i));
				break;
			}
		}
		RS_DBG("chipID = %d", current_entry.chipID);
		RS_DBG("patch_length = 0x%x", current_entry.patch_length);
		RS_DBG("start_offset = 0x%x", current_entry.start_offset);
		/* get right version patch: buf, buf_len */
		buf_len = current_entry.patch_length + config_len;
		RS_DBG("buf_len = 0x%x", buf_len);
		buf = malloc(buf_len);
		if (!buf) {
			RS_ERR("Can't alloc memory for multi fw&config, errno:%d", errno);
			buf_len = -1;
			goto FETCH_DONE;
		}
		memcpy(buf, &epatch_buf[current_entry.start_offset], current_entry.patch_length);
		value_t = cpu_to_le32(epatch_info->fm_version);
		epatch_info->fm_version = (uint32_t)value_t;
		memcpy(&buf[current_entry.patch_length-4], &epatch_info->fm_version, 4);
		value_t = cpu_to_le32(epatch_info->fm_version);
		epatch_info->fm_version = value_t;
	}

	if (config_len) {
		memcpy(&buf[buf_len - config_len], config_file_buf, config_len);
	}

FETCH_DONE:
	free(epatch_buf);
	epatch_buf = NULL;
	if (config_file_buf)
		free(config_file_buf);

	RS_DBG("Fw:%s exists, config file:%s exists", (buf_len > 0) ? "" : "not", (config_len > 0) ? "" : "not");
	if ((buf_len > 0) && (config_len == 0)) {
		rtk_h5.link_estab_state = H5_PATCH;
		goto DOWNLOAD_FW;
	}

	/* change baudrate if needed */
	if (baudrate == 0) {
		uart_speed_to_rtk_speed(speed, &baudrate);
		RS_DBG("Since no config file to set uart baudrate, so use user input parameters:%x, %x", (int)speed, (uint32_t)baudrate);
	} else
		rtk_speed_to_uart_speed(baudrate, (uint32_t *)&gFinalSpeed);

	if (proto == HCI_UART_3WIRE)
		rtk_vendor_change_speed_h5(fd, baudrate);
	else
		rtk_vendor_change_speed_h4(fd, baudrate);

	usleep(50000);
	final_speed = gFinalSpeed ? gFinalSpeed : speed;
	RS_DBG("final_speed %d\n", final_speed);
	if (set_speed(fd, ti, final_speed) < 0) {
		RS_ERR("Can't set baud rate:%x, %x, %x", final_speed, gFinalSpeed, speed);
		return -1;
	}

	if (gHwFlowControlEnable) {
		RS_DBG("Hw Flow Control enable");
		ti->c_cflag |= CRTSCTS;
	} else {
		RS_DBG("Hw Flow Control disable");
		ti->c_cflag &= ~CRTSCTS;
	}
	RS_DBG("Hw Flow Control do not enable before download fw! ");

	/* wait for while for controller to setup */
	usleep(10000);

DOWNLOAD_FW:
	if (buf && (buf_len > 0) && (need_download_fw)) {
		/* baudrate 0 means no change baudrate send */
		memset(&rtk_patch, 0, sizeof(rtk_patch));
		rtk_patch.nRxIndex = -1;

		rtk_download_fw_config(fd, buf, buf_len, baudrate, proto, ti);
		free(buf);
	}
	RS_DBG("Init Process finished");
	return 0;
}

/**
* Init uart by realtek Bluetooth.
*
* @param fd uart file descriptor
* @param proto realtek Bluetooth protocol, shall be either HCI_UART_H4 or HCI_UART_3WIRE
* @param speed init_speed in uart struct
* @param ti termios struct
* @returns #0 on success, depend on rtk_config
*/
int rtk_init(int fd, int proto, int speed, struct termios *ti)
{
	RS_DBG("Realtek hciattach version %s \n", RTK_VERSION);

	if (proto == HCI_UART_3WIRE) { /*h4 will do nothing for init */
		rtk_init_h5(fd, ti);
	}
	return rtk_config(fd, proto, speed, ti);
}

/**
* Post uart by realtek Bluetooth. If gFinalSpeed is set, set uart speed with it.
*
* @param fd uart file descriptor
* @param proto realtek Bluetooth protocol, shall be either HCI_UART_H4 or HCI_UART_3WIRE
* @param ti termios struct
* @returns #0 on success.
*/
int rtk_post(int fd, int proto, struct termios *ti)
{
	if (gFinalSpeed) {
		return set_speed(fd, ti, gFinalSpeed);
	}
	return 0;
}
