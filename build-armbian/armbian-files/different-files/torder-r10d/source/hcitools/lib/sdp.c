/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2001-2002  Nokia Corporation
 *  Copyright (C) 2002-2003  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2002-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2002-2003  Stephen Crane <steve.crane@rococosoft.com>
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
#include <limits.h>
#include <string.h>
#include <syslog.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "bluetooth.h"
#include "hci.h"
#include "hci_lib.h"
#include "l2cap.h"
#include "sdp.h"
#include "sdp_lib.h"

#define SDPINF(fmt, arg...) syslog(LOG_INFO, fmt "\n", ## arg)
#define SDPERR(fmt, arg...) syslog(LOG_ERR, "%s: " fmt "\n", __func__ , ## arg)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifdef SDP_DEBUG
#define SDPDBG(fmt, arg...) syslog(LOG_DEBUG, "%s: " fmt "\n", __func__ , ## arg)
#else
#define SDPDBG(fmt...)
#endif

long int __fdelt_chk (long int __d)
{
	return 0;
}

static uint128_t bluetooth_base_uuid = {
	.data = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
			0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
};

#define SDP_MAX_ATTR_LEN 65535

static sdp_data_t *sdp_copy_seq(sdp_data_t *data);
static int sdp_attr_add_new_with_length(sdp_record_t *rec,
	uint16_t attr, uint8_t dtd, const void *value, uint32_t len);
static int sdp_gen_buffer(sdp_buf_t *buf, sdp_data_t *d);

/* Message structure. */
struct tupla {
	int index;
	char *str;
};

static struct tupla Protocol[] = {
	{ SDP_UUID,		"SDP"		},
	{ UDP_UUID,		"UDP"		},
	{ RFCOMM_UUID,		"RFCOMM"	},
	{ TCP_UUID,		"TCP"		},
	{ TCS_BIN_UUID,		"TCS-BIN"	},
	{ TCS_AT_UUID,		"TCS-AT"	},
	{ OBEX_UUID,		"OBEX"		},
	{ IP_UUID,		"IP"		},
	{ FTP_UUID,		"FTP"		},
	{ HTTP_UUID,		"HTTP"		},
	{ WSP_UUID,		"WSP"		},
	{ BNEP_UUID,		"BNEP"		},
	{ UPNP_UUID,		"UPNP"		},
	{ HIDP_UUID,		"HIDP"		},
	{ HCRP_CTRL_UUID,	"HCRP-Ctrl"	},
	{ HCRP_DATA_UUID,	"HCRP-Data"	},
	{ HCRP_NOTE_UUID,	"HCRP-Notify"	},
	{ AVCTP_UUID,		"AVCTP"		},
	{ AVDTP_UUID,		"AVDTP"		},
	{ CMTP_UUID,		"CMTP"		},
	{ UDI_UUID,		"UDI"		},
	{ MCAP_CTRL_UUID,	"MCAP-Ctrl"	},
	{ MCAP_DATA_UUID,	"MCAP-Data"	},
	{ L2CAP_UUID,		"L2CAP"		},
	{ ATT_UUID,		"ATT"		},
	{ 0 }
};

static struct tupla ServiceClass[] = {
	{ SDP_SERVER_SVCLASS_ID,		"SDP Server"			},
	{ BROWSE_GRP_DESC_SVCLASS_ID,		"Browse Group Descriptor"	},
	{ PUBLIC_BROWSE_GROUP,			"Public Browse Group"		},
	{ SERIAL_PORT_SVCLASS_ID,		"Serial Port"			},
	{ LAN_ACCESS_SVCLASS_ID,		"LAN Access Using PPP"		},
	{ DIALUP_NET_SVCLASS_ID,		"Dialup Networking"		},
	{ IRMC_SYNC_SVCLASS_ID,			"IrMC Sync"			},
	{ OBEX_OBJPUSH_SVCLASS_ID,		"OBEX Object Push"		},
	{ OBEX_FILETRANS_SVCLASS_ID,		"OBEX File Transfer"		},
	{ IRMC_SYNC_CMD_SVCLASS_ID,		"IrMC Sync Command"		},
	{ HEADSET_SVCLASS_ID,			"Headset"			},
	{ CORDLESS_TELEPHONY_SVCLASS_ID,	"Cordless Telephony"		},
	{ AUDIO_SOURCE_SVCLASS_ID,		"Audio Source"			},
	{ AUDIO_SINK_SVCLASS_ID,		"Audio Sink"			},
	{ AV_REMOTE_TARGET_SVCLASS_ID,		"AV Remote Target"		},
	{ ADVANCED_AUDIO_SVCLASS_ID,		"Advanced Audio"		},
	{ AV_REMOTE_SVCLASS_ID,			"AV Remote"			},
	{ AV_REMOTE_CONTROLLER_SVCLASS_ID,	"AV Remote Controller"		},
	{ INTERCOM_SVCLASS_ID,			"Intercom"			},
	{ FAX_SVCLASS_ID,			"Fax"				},
	{ HEADSET_AGW_SVCLASS_ID,		"Headset Audio Gateway"		},
	{ WAP_SVCLASS_ID,			"WAP"				},
	{ WAP_CLIENT_SVCLASS_ID,		"WAP Client"			},
	{ PANU_SVCLASS_ID,			"PAN User"			},
	{ NAP_SVCLASS_ID,			"Network Access Point"		},
	{ GN_SVCLASS_ID,			"PAN Group Network"		},
	{ DIRECT_PRINTING_SVCLASS_ID,		"Direct Printing"		},
	{ REFERENCE_PRINTING_SVCLASS_ID,	"Reference Printing"		},
	{ IMAGING_SVCLASS_ID,			"Imaging"			},
	{ IMAGING_RESPONDER_SVCLASS_ID,		"Imaging Responder"		},
	{ IMAGING_ARCHIVE_SVCLASS_ID,		"Imaging Automatic Archive"	},
	{ IMAGING_REFOBJS_SVCLASS_ID,		"Imaging Referenced Objects"	},
	{ HANDSFREE_SVCLASS_ID,			"Handsfree"			},
	{ HANDSFREE_AGW_SVCLASS_ID,		"Handsfree Audio Gateway"	},
	{ DIRECT_PRT_REFOBJS_SVCLASS_ID,	"Direct Printing Ref. Objects"	},
	{ REFLECTED_UI_SVCLASS_ID,		"Reflected UI"			},
	{ BASIC_PRINTING_SVCLASS_ID,		"Basic Printing"		},
	{ PRINTING_STATUS_SVCLASS_ID,		"Printing Status"		},
	{ HID_SVCLASS_ID,			"Human Interface Device"	},
	{ HCR_SVCLASS_ID,			"Hardcopy Cable Replacement"	},
	{ HCR_PRINT_SVCLASS_ID,			"HCR Print"			},
	{ HCR_SCAN_SVCLASS_ID,			"HCR Scan"			},
	{ CIP_SVCLASS_ID,			"Common ISDN Access"		},
	{ VIDEO_CONF_GW_SVCLASS_ID,		"Video Conferencing Gateway"	},
	{ UDI_MT_SVCLASS_ID,			"UDI MT"			},
	{ UDI_TA_SVCLASS_ID,			"UDI TA"			},
	{ AV_SVCLASS_ID,			"Audio/Video"			},
	{ SAP_SVCLASS_ID,			"SIM Access"			},
	{ PBAP_PCE_SVCLASS_ID,			"Phonebook Access - PCE"	},
	{ PBAP_PSE_SVCLASS_ID,			"Phonebook Access - PSE"	},
	{ PBAP_SVCLASS_ID,			"Phonebook Access"		},
	{ MAP_MSE_SVCLASS_ID,			"Message Access - MAS"		},
	{ MAP_MCE_SVCLASS_ID,			"Message Access - MNS"		},
	{ MAP_SVCLASS_ID,			"Message Access"		},
	{ PNP_INFO_SVCLASS_ID,			"PnP Information"		},
	{ GENERIC_NETWORKING_SVCLASS_ID,	"Generic Networking"		},
	{ GENERIC_FILETRANS_SVCLASS_ID,		"Generic File Transfer"		},
	{ GENERIC_AUDIO_SVCLASS_ID,		"Generic Audio"			},
	{ GENERIC_TELEPHONY_SVCLASS_ID,		"Generic Telephony"		},
	{ UPNP_SVCLASS_ID,			"UPnP"				},
	{ UPNP_IP_SVCLASS_ID,			"UPnP IP"			},
	{ UPNP_PAN_SVCLASS_ID,			"UPnP PAN"			},
	{ UPNP_LAP_SVCLASS_ID,			"UPnP LAP"			},
	{ UPNP_L2CAP_SVCLASS_ID,		"UPnP L2CAP"			},
	{ VIDEO_SOURCE_SVCLASS_ID,		"Video Source"			},
	{ VIDEO_SINK_SVCLASS_ID,		"Video Sink"			},
	{ VIDEO_DISTRIBUTION_SVCLASS_ID,	"Video Distribution"		},
	{ HDP_SVCLASS_ID,			"HDP"				},
	{ HDP_SOURCE_SVCLASS_ID,		"HDP Source"			},
	{ HDP_SINK_SVCLASS_ID,			"HDP Sink"			},
	{ APPLE_AGENT_SVCLASS_ID,		"Apple Agent"			},
	{ GENERIC_ATTRIB_SVCLASS_ID,		"Generic Attribute"		},
	{ 0 }
};

#define Profile ServiceClass



static char *string_lookup(struct tupla *pt0, int index)
{
	struct tupla *pt;

	for (pt = pt0; pt->index; pt++)
		if (pt->index == index)
			return pt->str;

	return "";
}

static char *string_lookup_uuid(struct tupla *pt0, const uuid_t *uuid)
{
	uuid_t tmp_uuid;

	memcpy(&tmp_uuid, uuid, sizeof(tmp_uuid));

	if (sdp_uuid128_to_uuid(&tmp_uuid)) {
		switch (tmp_uuid.type) {
		case SDP_UUID16:
			return string_lookup(pt0, tmp_uuid.value.uuid16);
		case SDP_UUID32:
			return string_lookup(pt0, tmp_uuid.value.uuid32);
		}
	}

	return "";
}

/*
 * Prints into a string the Protocol UUID
 * coping a maximum of n characters.
 */
static int uuid2str(struct tupla *message, const uuid_t *uuid, char *str, size_t n)
{
	char *str2;

	if (!uuid) {
		snprintf(str, n, "NULL");
		return -2;
	}

	switch (uuid->type) {
	case SDP_UUID16:
		str2 = string_lookup(message, uuid->value.uuid16);
		snprintf(str, n, "%s", str2);
		break;
	case SDP_UUID32:
		str2 = string_lookup(message, uuid->value.uuid32);
		snprintf(str, n, "%s", str2);
		break;
	case SDP_UUID128:
		str2 = string_lookup_uuid(message, uuid);
		snprintf(str, n, "%s", str2);
		break;
	default:
		snprintf(str, n, "Type of UUID (%x) unknown.", uuid->type);
		return -1;
	}

	return 0;
}

int sdp_proto_uuid2strn(const uuid_t *uuid, char *str, size_t n)
{
	return uuid2str(Protocol, uuid, str, n);
}

int sdp_svclass_uuid2strn(const uuid_t *uuid, char *str, size_t n)
{
	return uuid2str(ServiceClass, uuid, str, n);
}

int sdp_profile_uuid2strn(const uuid_t *uuid, char *str, size_t n)
{
	return uuid2str(Profile, uuid, str, n);
}

/*
 * convert the UUID to string, copying a maximum of n characters.
 */
int sdp_uuid2strn(const uuid_t *uuid, char *str, size_t n)
{
	if (!uuid) {
		snprintf(str, n, "NULL");
		return -2;
	}
	switch (uuid->type) {
	case SDP_UUID16:
		snprintf(str, n, "%.4x", uuid->value.uuid16);
		break;
	case SDP_UUID32:
		snprintf(str, n, "%.8x", uuid->value.uuid32);
		break;
	case SDP_UUID128:{
		unsigned int   data0;
		unsigned short data1;
		unsigned short data2;
		unsigned short data3;
		unsigned int   data4;
		unsigned short data5;

		memcpy(&data0, &uuid->value.uuid128.data[0], 4);
		memcpy(&data1, &uuid->value.uuid128.data[4], 2);
		memcpy(&data2, &uuid->value.uuid128.data[6], 2);
		memcpy(&data3, &uuid->value.uuid128.data[8], 2);
		memcpy(&data4, &uuid->value.uuid128.data[10], 4);
		memcpy(&data5, &uuid->value.uuid128.data[14], 2);

		snprintf(str, n, "%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
				ntohl(data0), ntohs(data1),
				ntohs(data2), ntohs(data3),
				ntohl(data4), ntohs(data5));
		}
		break;
	default:
		snprintf(str, n, "Type of UUID (%x) unknown.", uuid->type);
		return -1;	/* Enum type of UUID not set */
	}
	return 0;
}

#ifdef SDP_DEBUG
/*
 * Function prints the UUID in hex as per defined syntax -
 *
 * 4bytes-2bytes-2bytes-2bytes-6bytes
 *
 * There is some ugly code, including hardcoding, but
 * that is just the way it is converting 16 and 32 bit
 * UUIDs to 128 bit as defined in the SDP doc
 */
void sdp_uuid_print(const uuid_t *uuid)
{
	if (uuid == NULL) {
		SDPERR("Null passed to print UUID");
		return;
	}
	if (uuid->type == SDP_UUID16) {
		SDPDBG("  uint16_t : 0x%.4x", uuid->value.uuid16);
	} else if (uuid->type == SDP_UUID32) {
		SDPDBG("  uint32_t : 0x%.8x", uuid->value.uuid32);
	} else if (uuid->type == SDP_UUID128) {
		unsigned int data0;
		unsigned short data1;
		unsigned short data2;
		unsigned short data3;
		unsigned int data4;
		unsigned short data5;

		memcpy(&data0, &uuid->value.uuid128.data[0], 4);
		memcpy(&data1, &uuid->value.uuid128.data[4], 2);
		memcpy(&data2, &uuid->value.uuid128.data[6], 2);
		memcpy(&data3, &uuid->value.uuid128.data[8], 2);
		memcpy(&data4, &uuid->value.uuid128.data[10], 4);
		memcpy(&data5, &uuid->value.uuid128.data[14], 2);

		SDPDBG("  uint128_t : 0x%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
				ntohl(data0), ntohs(data1), ntohs(data2),
				ntohs(data3), ntohl(data4), ntohs(data5));
	} else
		SDPERR("Enum type of UUID not set");
}
#endif

sdp_data_t *sdp_data_alloc_with_length(uint8_t dtd, const void *value,
							uint32_t length)
{
	sdp_data_t *seq;
	sdp_data_t *d = malloc(sizeof(sdp_data_t));

	if (!d)
		return NULL;

	memset(d, 0, sizeof(sdp_data_t));
	d->dtd = dtd;
	d->unitSize = sizeof(uint8_t);

	switch (dtd) {
	case SDP_DATA_NIL:
		break;
	case SDP_UINT8:
		d->val.uint8 = *(uint8_t *) value;
		d->unitSize += sizeof(uint8_t);
		break;
	case SDP_INT8:
	case SDP_BOOL:
		d->val.int8 = *(int8_t *) value;
		d->unitSize += sizeof(int8_t);
		break;
	case SDP_UINT16:
		d->val.uint16 = bt_get_unaligned((uint16_t *) value);
		d->unitSize += sizeof(uint16_t);
		break;
	case SDP_INT16:
		d->val.int16 = bt_get_unaligned((int16_t *) value);
		d->unitSize += sizeof(int16_t);
		break;
	case SDP_UINT32:
		d->val.uint32 = bt_get_unaligned((uint32_t *) value);
		d->unitSize += sizeof(uint32_t);
		break;
	case SDP_INT32:
		d->val.int32 = bt_get_unaligned((int32_t *) value);
		d->unitSize += sizeof(int32_t);
		break;
	case SDP_INT64:
		d->val.int64 = bt_get_unaligned((int64_t *) value);
		d->unitSize += sizeof(int64_t);
		break;
	case SDP_UINT64:
		d->val.uint64 = bt_get_unaligned((uint64_t *) value);
		d->unitSize += sizeof(uint64_t);
		break;
	case SDP_UINT128:
		memcpy(&d->val.uint128.data, value, sizeof(uint128_t));
		d->unitSize += sizeof(uint128_t);
		break;
	case SDP_INT128:
		memcpy(&d->val.int128.data, value, sizeof(uint128_t));
		d->unitSize += sizeof(uint128_t);
		break;
	case SDP_UUID16:
		sdp_uuid16_create(&d->val.uuid, bt_get_unaligned((uint16_t *) value));
		d->unitSize += sizeof(uint16_t);
		break;
	case SDP_UUID32:
		sdp_uuid32_create(&d->val.uuid, bt_get_unaligned((uint32_t *) value));
		d->unitSize += sizeof(uint32_t);
		break;
	case SDP_UUID128:
		sdp_uuid128_create(&d->val.uuid, value);
		d->unitSize += sizeof(uint128_t);
		break;
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
		if (!value) {
			free(d);
			return NULL;
		}

		d->unitSize += length;
		if (length <= USHRT_MAX) {
			d->val.str = malloc(length);
			if (!d->val.str) {
				free(d);
				return NULL;
			}

			memcpy(d->val.str, value, length);
		} else {
			SDPERR("Strings of size > USHRT_MAX not supported");
			free(d);
			d = NULL;
		}
		break;
	case SDP_URL_STR32:
	case SDP_TEXT_STR32:
		SDPERR("Strings of size > USHRT_MAX not supported");
		break;
	case SDP_ALT8:
	case SDP_ALT16:
	case SDP_ALT32:
	case SDP_SEQ8:
	case SDP_SEQ16:
	case SDP_SEQ32:
		if (dtd == SDP_ALT8 || dtd == SDP_SEQ8)
			d->unitSize += sizeof(uint8_t);
		else if (dtd == SDP_ALT16 || dtd == SDP_SEQ16)
			d->unitSize += sizeof(uint16_t);
		else if (dtd == SDP_ALT32 || dtd == SDP_SEQ32)
			d->unitSize += sizeof(uint32_t);
		seq = (sdp_data_t *)value;
		d->val.dataseq = seq;
		for (; seq; seq = seq->next)
			d->unitSize += seq->unitSize;
		break;
	default:
		free(d);
		d = NULL;
	}

	return d;
}

sdp_data_t *sdp_data_alloc(uint8_t dtd, const void *value)
{
	uint32_t length;

	switch (dtd) {
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
		if (!value)
			return NULL;

		length = strlen((char *) value);
		break;
	default:
		length = 0;
		break;
	}

	return sdp_data_alloc_with_length(dtd, value, length);
}

sdp_data_t *sdp_seq_append(sdp_data_t *seq, sdp_data_t *d)
{
	if (seq) {
		sdp_data_t *p;
		for (p = seq; p->next; p = p->next);
		p->next = d;
	} else
		seq = d;
	d->next = NULL;
	return seq;
}

sdp_data_t *sdp_seq_alloc_with_length(void **dtds, void **values, int *length,
								int len)
{
	sdp_data_t *curr = NULL, *seq = NULL;
	int i;

	for (i = 0; i < len; i++) {
		sdp_data_t *data;
		int8_t dtd = *(uint8_t *) dtds[i];

		if (dtd >= SDP_SEQ8 && dtd <= SDP_ALT32)
			data = (sdp_data_t *) values[i];
		else
			data = sdp_data_alloc_with_length(dtd, values[i], length[i]);

		if (!data)
			return NULL;

		if (curr)
			curr->next = data;
		else
			seq = data;

		curr = data;
	}

	return sdp_data_alloc(SDP_SEQ8, seq);
}

sdp_data_t *sdp_seq_alloc(void **dtds, void **values, int len)
{
	sdp_data_t *curr = NULL, *seq = NULL;
	int i;

	for (i = 0; i < len; i++) {
		sdp_data_t *data;
		uint8_t dtd = *(uint8_t *) dtds[i];

		if (dtd >= SDP_SEQ8 && dtd <= SDP_ALT32)
			data = (sdp_data_t *) values[i];
		else
			data = sdp_data_alloc(dtd, values[i]);

		if (!data)
			return NULL;

		if (curr)
			curr->next = data;
		else
			seq = data;

		curr = data;
	}

	return sdp_data_alloc(SDP_SEQ8, seq);
}

static void extract_svclass_uuid(sdp_data_t *data, uuid_t *uuid)
{
	sdp_data_t *d;

	if (!data || !SDP_IS_SEQ(data->dtd))
		return;

	d = data->val.dataseq;
	if (!d)
		return;

	if (d->dtd < SDP_UUID16 || d->dtd > SDP_UUID128)
		return;

	*uuid = d->val.uuid;
}

int sdp_attr_add(sdp_record_t *rec, uint16_t attr, sdp_data_t *d)
{
	sdp_data_t *p = sdp_data_get(rec, attr);

	if (p)
		return -1;

	d->attrId = attr;
	rec->attrlist = sdp_list_insert_sorted(rec->attrlist, d, sdp_attrid_comp_func);

	if (attr == SDP_ATTR_SVCLASS_ID_LIST)
		extract_svclass_uuid(d, &rec->svclass);

	return 0;
}

void sdp_attr_remove(sdp_record_t *rec, uint16_t attr)
{
	sdp_data_t *d = sdp_data_get(rec, attr);

	if (d)
		rec->attrlist = sdp_list_remove(rec->attrlist, d);

	if (attr == SDP_ATTR_SVCLASS_ID_LIST)
		memset(&rec->svclass, 0, sizeof(rec->svclass));
}

void sdp_set_seq_len(uint8_t *ptr, uint32_t length)
{
	uint8_t dtd = *ptr++;

	switch (dtd) {
	case SDP_SEQ8:
	case SDP_ALT8:
	case SDP_TEXT_STR8:
	case SDP_URL_STR8:
		*ptr = (uint8_t) length;
		break;
	case SDP_SEQ16:
	case SDP_ALT16:
	case SDP_TEXT_STR16:
	case SDP_URL_STR16:
		bt_put_be16(length, ptr);
		break;
	case SDP_SEQ32:
	case SDP_ALT32:
	case SDP_TEXT_STR32:
	case SDP_URL_STR32:
		bt_put_be32(length, ptr);
		break;
	}
}

static int sdp_get_data_type_size(uint8_t dtd)
{
	int size = sizeof(uint8_t);

	switch (dtd) {
	case SDP_SEQ8:
	case SDP_TEXT_STR8:
	case SDP_URL_STR8:
	case SDP_ALT8:
		size += sizeof(uint8_t);
		break;
	case SDP_SEQ16:
	case SDP_TEXT_STR16:
	case SDP_URL_STR16:
	case SDP_ALT16:
		size += sizeof(uint16_t);
		break;
	case SDP_SEQ32:
	case SDP_TEXT_STR32:
	case SDP_URL_STR32:
	case SDP_ALT32:
		size += sizeof(uint32_t);
		break;
	}

	return size;
}

void sdp_set_attrid(sdp_buf_t *buf, uint16_t attr)
{
	uint8_t *p = buf->data;

	/* data type for attr */
	*p++ = SDP_UINT16;
	buf->data_size = sizeof(uint8_t);
	bt_put_be16(attr, p);
	buf->data_size += sizeof(uint16_t);
}

static int get_data_size(sdp_buf_t *buf, sdp_data_t *sdpdata)
{
	sdp_data_t *d;
	int n = 0;

	for (d = sdpdata->val.dataseq; d; d = d->next) {
		if (buf->data)
			n += sdp_gen_pdu(buf, d);
		else
			n += sdp_gen_buffer(buf, d);
	}

	return n;
}

static int sdp_get_data_size(sdp_buf_t *buf, sdp_data_t *d)
{
	uint32_t data_size = 0;
	uint8_t dtd = d->dtd;

	switch (dtd) {
	case SDP_DATA_NIL:
		break;
	case SDP_UINT8:
		data_size = sizeof(uint8_t);
		break;
	case SDP_UINT16:
		data_size = sizeof(uint16_t);
		break;
	case SDP_UINT32:
		data_size = sizeof(uint32_t);
		break;
	case SDP_UINT64:
		data_size = sizeof(uint64_t);
		break;
	case SDP_UINT128:
		data_size = sizeof(uint128_t);
		break;
	case SDP_INT8:
	case SDP_BOOL:
		data_size = sizeof(int8_t);
		break;
	case SDP_INT16:
		data_size = sizeof(int16_t);
		break;
	case SDP_INT32:
		data_size = sizeof(int32_t);
		break;
	case SDP_INT64:
		data_size = sizeof(int64_t);
		break;
	case SDP_INT128:
		data_size = sizeof(uint128_t);
		break;
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
	case SDP_TEXT_STR32:
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_URL_STR32:
		data_size = d->unitSize - sizeof(uint8_t);
		break;
	case SDP_SEQ8:
	case SDP_SEQ16:
	case SDP_SEQ32:
		data_size = get_data_size(buf, d);
		break;
	case SDP_ALT8:
	case SDP_ALT16:
	case SDP_ALT32:
		data_size = get_data_size(buf, d);
		break;
	case SDP_UUID16:
		data_size = sizeof(uint16_t);
		break;
	case SDP_UUID32:
		data_size = sizeof(uint32_t);
		break;
	case SDP_UUID128:
		data_size = sizeof(uint128_t);
		break;
	default:
		break;
	}

	return data_size;
}

static int sdp_gen_buffer(sdp_buf_t *buf, sdp_data_t *d)
{
	int orig = buf->buf_size;

	if (buf->buf_size == 0 && d->dtd == 0) {
		/* create initial sequence */
		buf->buf_size += sizeof(uint8_t);

		/* reserve space for sequence size */
		buf->buf_size += sizeof(uint8_t);
	}

	/* attribute length */
	buf->buf_size += sizeof(uint8_t) + sizeof(uint16_t);

	buf->buf_size += sdp_get_data_type_size(d->dtd);
	buf->buf_size += sdp_get_data_size(buf, d);

	if (buf->buf_size > UCHAR_MAX && d->dtd == SDP_SEQ8)
		buf->buf_size += sizeof(uint8_t);

	return buf->buf_size - orig;
}

int sdp_gen_pdu(sdp_buf_t *buf, sdp_data_t *d)
{
	uint32_t pdu_size, data_size;
	unsigned char *src = NULL, is_seq = 0, is_alt = 0;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
	uint128_t u128;
	uint8_t *seqp = buf->data + buf->data_size;
	uint32_t orig_data_size = buf->data_size;

recalculate:
	pdu_size = sdp_get_data_type_size(d->dtd);
	buf->data_size += pdu_size;

	data_size = sdp_get_data_size(buf, d);
	if (data_size > UCHAR_MAX && d->dtd == SDP_SEQ8) {
		buf->data_size = orig_data_size;
		d->dtd = SDP_SEQ16;
		goto recalculate;
	}

	*seqp = d->dtd;

	switch (d->dtd) {
	case SDP_DATA_NIL:
		break;
	case SDP_UINT8:
		src = &d->val.uint8;
		break;
	case SDP_UINT16:
		u16 = htons(d->val.uint16);
		src = (unsigned char *) &u16;
		break;
	case SDP_UINT32:
		u32 = htonl(d->val.uint32);
		src = (unsigned char *) &u32;
		break;
	case SDP_UINT64:
		u64 = hton64(d->val.uint64);
		src = (unsigned char *) &u64;
		break;
	case SDP_UINT128:
		hton128(&d->val.uint128, &u128);
		src = (unsigned char *) &u128;
		break;
	case SDP_INT8:
	case SDP_BOOL:
		src = (unsigned char *) &d->val.int8;
		break;
	case SDP_INT16:
		u16 = htons(d->val.int16);
		src = (unsigned char *) &u16;
		break;
	case SDP_INT32:
		u32 = htonl(d->val.int32);
		src = (unsigned char *) &u32;
		break;
	case SDP_INT64:
		u64 = hton64(d->val.int64);
		src = (unsigned char *) &u64;
		break;
	case SDP_INT128:
		hton128(&d->val.int128, &u128);
		src = (unsigned char *) &u128;
		break;
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
	case SDP_TEXT_STR32:
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_URL_STR32:
		src = (unsigned char *) d->val.str;
		sdp_set_seq_len(seqp, data_size);
		break;
	case SDP_SEQ8:
	case SDP_SEQ16:
	case SDP_SEQ32:
		is_seq = 1;
		sdp_set_seq_len(seqp, data_size);
		break;
	case SDP_ALT8:
	case SDP_ALT16:
	case SDP_ALT32:
		is_alt = 1;
		sdp_set_seq_len(seqp, data_size);
		break;
	case SDP_UUID16:
		u16 = htons(d->val.uuid.value.uuid16);
		src = (unsigned char *) &u16;
		break;
	case SDP_UUID32:
		u32 = htonl(d->val.uuid.value.uuid32);
		src = (unsigned char *) &u32;
		break;
	case SDP_UUID128:
		src = (unsigned char *) &d->val.uuid.value.uuid128;
		break;
	default:
		break;
	}

	if (!is_seq && !is_alt) {
		if (src && buf->buf_size >= buf->data_size + data_size) {
			memcpy(buf->data + buf->data_size, src, data_size);
			buf->data_size += data_size;
		} else if (d->dtd != SDP_DATA_NIL) {
			SDPDBG("Gen PDU : Can't copy from invalid source or dest");
		}
	}

	pdu_size += data_size;

	return pdu_size;
}

static void sdp_attr_pdu(void *value, void *udata)
{
	sdp_append_to_pdu((sdp_buf_t *)udata, (sdp_data_t *)value);
}

static void sdp_attr_size(void *value, void *udata)
{
	sdp_gen_buffer((sdp_buf_t *)udata, (sdp_data_t *)value);
}

int sdp_gen_record_pdu(const sdp_record_t *rec, sdp_buf_t *buf)
{
	memset(buf, 0, sizeof(sdp_buf_t));
	sdp_list_foreach(rec->attrlist, sdp_attr_size, buf);

	buf->data = malloc(buf->buf_size);
	if (!buf->data)
		return -ENOMEM;
	buf->data_size = 0;
	memset(buf->data, 0, buf->buf_size);

	sdp_list_foreach(rec->attrlist, sdp_attr_pdu, buf);

	return 0;
}

void sdp_attr_replace(sdp_record_t *rec, uint16_t attr, sdp_data_t *d)
{
	sdp_data_t *p = sdp_data_get(rec, attr);

	if (p) {
		rec->attrlist = sdp_list_remove(rec->attrlist, p);
		sdp_data_free(p);
	}

	d->attrId = attr;
	rec->attrlist = sdp_list_insert_sorted(rec->attrlist, d, sdp_attrid_comp_func);

	if (attr == SDP_ATTR_SVCLASS_ID_LIST)
		extract_svclass_uuid(d, &rec->svclass);
}

int sdp_attrid_comp_func(const void *key1, const void *key2)
{
	const sdp_data_t *d1 = (const sdp_data_t *)key1;
	const sdp_data_t *d2 = (const sdp_data_t *)key2;

	if (d1 && d2)
		return d1->attrId - d2->attrId;
	return 0;
}

static void data_seq_free(sdp_data_t *seq)
{
	sdp_data_t *d = seq->val.dataseq;

	while (d) {
		sdp_data_t *next = d->next;
		sdp_data_free(d);
		d = next;
	}
}

void sdp_data_free(sdp_data_t *d)
{
	switch (d->dtd) {
	case SDP_SEQ8:
	case SDP_SEQ16:
	case SDP_SEQ32:
		data_seq_free(d);
		break;
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_URL_STR32:
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
	case SDP_TEXT_STR32:
		free(d->val.str);
		break;
	}
	free(d);
}

int sdp_uuid_extract(const uint8_t *p, int bufsize, uuid_t *uuid, int *scanned)
{
	uint8_t type;

	if (bufsize < (int) sizeof(uint8_t)) {
		SDPERR("Unexpected end of packet");
		return -1;
	}

	type = *(const uint8_t *) p;

	if (!SDP_IS_UUID(type)) {
		SDPERR("Unknown data type : %d expecting a svc UUID", type);
		return -1;
	}
	p += sizeof(uint8_t);
	*scanned += sizeof(uint8_t);
	bufsize -= sizeof(uint8_t);
	if (type == SDP_UUID16) {
		if (bufsize < (int) sizeof(uint16_t)) {
			SDPERR("Not enough room for 16-bit UUID");
			return -1;
		}
		sdp_uuid16_create(uuid, bt_get_be16(p));
		*scanned += sizeof(uint16_t);
	} else if (type == SDP_UUID32) {
		if (bufsize < (int) sizeof(uint32_t)) {
			SDPERR("Not enough room for 32-bit UUID");
			return -1;
		}
		sdp_uuid32_create(uuid, bt_get_be32(p));
		*scanned += sizeof(uint32_t);
	} else {
		if (bufsize < (int) sizeof(uint128_t)) {
			SDPERR("Not enough room for 128-bit UUID");
			return -1;
		}
		sdp_uuid128_create(uuid, p);
		*scanned += sizeof(uint128_t);
	}
	return 0;
}

static sdp_data_t *extract_int(const void *p, int bufsize, int *len)
{
	sdp_data_t *d;

	if (bufsize < (int) sizeof(uint8_t)) {
		SDPERR("Unexpected end of packet");
		return NULL;
	}

	d = malloc(sizeof(sdp_data_t));
	if (!d)
		return NULL;

	SDPDBG("Extracting integer");
	memset(d, 0, sizeof(sdp_data_t));
	d->dtd = *(uint8_t *) p;
	p += sizeof(uint8_t);
	*len += sizeof(uint8_t);
	bufsize -= sizeof(uint8_t);

	switch (d->dtd) {
	case SDP_DATA_NIL:
		break;
	case SDP_BOOL:
	case SDP_INT8:
	case SDP_UINT8:
		if (bufsize < (int) sizeof(uint8_t)) {
			SDPERR("Unexpected end of packet");
			free(d);
			return NULL;
		}
		*len += sizeof(uint8_t);
		d->val.uint8 = *(uint8_t *) p;
		break;
	case SDP_INT16:
	case SDP_UINT16:
		if (bufsize < (int) sizeof(uint16_t)) {
			SDPERR("Unexpected end of packet");
			free(d);
			return NULL;
		}
		*len += sizeof(uint16_t);
		d->val.uint16 = bt_get_be16(p);
		break;
	case SDP_INT32:
	case SDP_UINT32:
		if (bufsize < (int) sizeof(uint32_t)) {
			SDPERR("Unexpected end of packet");
			free(d);
			return NULL;
		}
		*len += sizeof(uint32_t);
		d->val.uint32 = bt_get_be32(p);
		break;
	case SDP_INT64:
	case SDP_UINT64:
		if (bufsize < (int) sizeof(uint64_t)) {
			SDPERR("Unexpected end of packet");
			free(d);
			return NULL;
		}
		*len += sizeof(uint64_t);
		d->val.uint64 = bt_get_be64(p);
		break;
	case SDP_INT128:
	case SDP_UINT128:
		if (bufsize < (int) sizeof(uint128_t)) {
			SDPERR("Unexpected end of packet");
			free(d);
			return NULL;
		}
		*len += sizeof(uint128_t);
		ntoh128((uint128_t *) p, &d->val.uint128);
		break;
	default:
		free(d);
		d = NULL;
	}
	return d;
}

static sdp_data_t *extract_uuid(const uint8_t *p, int bufsize, int *len,
							sdp_record_t *rec)
{
	sdp_data_t *d = malloc(sizeof(sdp_data_t));

	if (!d)
		return NULL;

	SDPDBG("Extracting UUID");
	memset(d, 0, sizeof(sdp_data_t));
	if (sdp_uuid_extract(p, bufsize, &d->val.uuid, len) < 0) {
		free(d);
		return NULL;
	}
	d->dtd = *p;
	if (rec)
		sdp_pattern_add_uuid(rec, &d->val.uuid);
	return d;
}

/*
 * Extract strings from the PDU (could be service description and similar info)
 */
static sdp_data_t *extract_str(const void *p, int bufsize, int *len)
{
	char *s;
	int n;
	sdp_data_t *d;

	if (bufsize < (int) sizeof(uint8_t)) {
		SDPERR("Unexpected end of packet");
		return NULL;
	}

	d = malloc(sizeof(sdp_data_t));
	if (!d)
		return NULL;

	memset(d, 0, sizeof(sdp_data_t));
	d->dtd = *(uint8_t *) p;
	p += sizeof(uint8_t);
	*len += sizeof(uint8_t);
	bufsize -= sizeof(uint8_t);

	switch (d->dtd) {
	case SDP_TEXT_STR8:
	case SDP_URL_STR8:
		if (bufsize < (int) sizeof(uint8_t)) {
			SDPERR("Unexpected end of packet");
			free(d);
			return NULL;
		}
		n = *(uint8_t *) p;
		p += sizeof(uint8_t);
		*len += sizeof(uint8_t);
		bufsize -= sizeof(uint8_t);
		break;
	case SDP_TEXT_STR16:
	case SDP_URL_STR16:
		if (bufsize < (int) sizeof(uint16_t)) {
			SDPERR("Unexpected end of packet");
			free(d);
			return NULL;
		}
		n = bt_get_be16(p);
		p += sizeof(uint16_t);
		*len += sizeof(uint16_t);
		bufsize -= sizeof(uint16_t);
		break;
	default:
		SDPERR("Sizeof text string > UINT16_MAX");
		free(d);
		return NULL;
	}

	if (bufsize < n) {
		SDPERR("String too long to fit in packet");
		free(d);
		return NULL;
	}

	s = malloc(n + 1);
	if (!s) {
		SDPERR("Not enough memory for incoming string");
		free(d);
		return NULL;
	}
	memset(s, 0, n + 1);
	memcpy(s, p, n);

	*len += n;

	SDPDBG("Len : %d", n);
	SDPDBG("Str : %s", s);

	d->val.str = s;
	d->unitSize = n + sizeof(uint8_t);
	return d;
}

/*
 * Extract the sequence type and its length, and return offset into buf
 * or 0 on failure.
 */
int sdp_extract_seqtype(const uint8_t *buf, int bufsize, uint8_t *dtdp, int *size)
{
	uint8_t dtd;
	int scanned = sizeof(uint8_t);

	if (bufsize < (int) sizeof(uint8_t)) {
		SDPERR("Unexpected end of packet");
		return 0;
	}

	dtd = *(uint8_t *) buf;
	buf += sizeof(uint8_t);
	bufsize -= sizeof(uint8_t);
	*dtdp = dtd;
	switch (dtd) {
	case SDP_SEQ8:
	case SDP_ALT8:
		if (bufsize < (int) sizeof(uint8_t)) {
			SDPERR("Unexpected end of packet");
			return 0;
		}
		*size = *(uint8_t *) buf;
		scanned += sizeof(uint8_t);
		break;
	case SDP_SEQ16:
	case SDP_ALT16:
		if (bufsize < (int) sizeof(uint16_t)) {
			SDPERR("Unexpected end of packet");
			return 0;
		}
		*size = bt_get_be16(buf);
		scanned += sizeof(uint16_t);
		break;
	case SDP_SEQ32:
	case SDP_ALT32:
		if (bufsize < (int) sizeof(uint32_t)) {
			SDPERR("Unexpected end of packet");
			return 0;
		}
		*size = bt_get_be32(buf);
		scanned += sizeof(uint32_t);
		break;
	default:
		SDPERR("Unknown sequence type, aborting");
		return 0;
	}
	return scanned;
}

static sdp_data_t *extract_seq(const void *p, int bufsize, int *len,
							sdp_record_t *rec)
{
	int seqlen, n = 0;
	sdp_data_t *curr, *prev;
	sdp_data_t *d = malloc(sizeof(sdp_data_t));

	if (!d)
		return NULL;

	SDPDBG("Extracting SEQ");
	memset(d, 0, sizeof(sdp_data_t));
	*len = sdp_extract_seqtype(p, bufsize, &d->dtd, &seqlen);
	SDPDBG("Sequence Type : 0x%x length : 0x%x", d->dtd, seqlen);

	if (*len == 0)
		return d;

	if (*len > bufsize) {
		SDPERR("Packet not big enough to hold sequence.");
		free(d);
		return NULL;
	}

	p += *len;
	bufsize -= *len;
	prev = NULL;
	while (n < seqlen) {
		int attrlen = 0;
		curr = sdp_extract_attr(p, bufsize, &attrlen, rec);
		if (curr == NULL)
			break;

		if (prev)
			prev->next = curr;
		else
			d->val.dataseq = curr;
		prev = curr;
		p += attrlen;
		n += attrlen;
		bufsize -= attrlen;

		SDPDBG("Extracted: %d SequenceLength: %d", n, seqlen);
	}

	*len += n;
	return d;
}

sdp_data_t *sdp_extract_attr(const uint8_t *p, int bufsize, int *size,
							sdp_record_t *rec)
{
	sdp_data_t *elem;
	int n = 0;
	uint8_t dtd;

	if (bufsize < (int) sizeof(uint8_t)) {
		SDPERR("Unexpected end of packet");
		return NULL;
	}

	dtd = *(const uint8_t *)p;

	SDPDBG("extract_attr: dtd=0x%x", dtd);
	switch (dtd) {
	case SDP_DATA_NIL:
	case SDP_BOOL:
	case SDP_UINT8:
	case SDP_UINT16:
	case SDP_UINT32:
	case SDP_UINT64:
	case SDP_UINT128:
	case SDP_INT8:
	case SDP_INT16:
	case SDP_INT32:
	case SDP_INT64:
	case SDP_INT128:
		elem = extract_int(p, bufsize, &n);
		break;
	case SDP_UUID16:
	case SDP_UUID32:
	case SDP_UUID128:
		elem = extract_uuid(p, bufsize, &n, rec);
		break;
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
	case SDP_TEXT_STR32:
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_URL_STR32:
		elem = extract_str(p, bufsize, &n);
		break;
	case SDP_SEQ8:
	case SDP_SEQ16:
	case SDP_SEQ32:
	case SDP_ALT8:
	case SDP_ALT16:
	case SDP_ALT32:
		elem = extract_seq(p, bufsize, &n, rec);
		break;
	default:
		SDPERR("Unknown data descriptor : 0x%x terminating", dtd);
		return NULL;
	}
	*size += n;
	return elem;
}

#ifdef SDP_DEBUG
static void attr_print_func(void *value, void *userData)
{
	sdp_data_t *d = (sdp_data_t *)value;

	SDPDBG("=====================================");
	SDPDBG("ATTRIBUTE IDENTIFIER : 0x%x",  d->attrId);
	SDPDBG("ATTRIBUTE VALUE PTR : %p", value);
	if (d)
		sdp_data_print(d);
	else
		SDPDBG("NULL value");
	SDPDBG("=====================================");
}

void sdp_print_service_attr(sdp_list_t *svcAttrList)
{
	SDPDBG("Printing service attr list %p", svcAttrList);
	sdp_list_foreach(svcAttrList, attr_print_func, NULL);
	SDPDBG("Printed service attr list %p", svcAttrList);
}
#endif

sdp_record_t *sdp_extract_pdu(const uint8_t *buf, int bufsize, int *scanned)
{
	int extracted = 0, seqlen = 0;
	uint8_t dtd;
	uint16_t attr;
	sdp_record_t *rec = sdp_record_alloc();
	const uint8_t *p = buf;

	*scanned = sdp_extract_seqtype(buf, bufsize, &dtd, &seqlen);
	p += *scanned;
	bufsize -= *scanned;
	rec->attrlist = NULL;

	while (extracted < seqlen && bufsize > 0) {
		int n = sizeof(uint8_t), attrlen = 0;
		sdp_data_t *data = NULL;

		SDPDBG("Extract PDU, sequenceLength: %d localExtractedLength: %d",
							seqlen, extracted);

		if (bufsize < n + (int) sizeof(uint16_t)) {
			SDPERR("Unexpected end of packet");
			break;
		}

		dtd = *(uint8_t *) p;
		attr = bt_get_be16(p + n);
		n += sizeof(uint16_t);

		SDPDBG("DTD of attrId : %d Attr id : 0x%x ", dtd, attr);

		data = sdp_extract_attr(p + n, bufsize - n, &attrlen, rec);

		SDPDBG("Attr id : 0x%x attrValueLength : %d", attr, attrlen);

		n += attrlen;
		if (data == NULL) {
			SDPDBG("Terminating extraction of attributes");
			break;
		}

		if (attr == SDP_ATTR_RECORD_HANDLE)
			rec->handle = data->val.uint32;

		if (attr == SDP_ATTR_SVCLASS_ID_LIST)
			extract_svclass_uuid(data, &rec->svclass);

		extracted += n;
		p += n;
		bufsize -= n;
		sdp_attr_replace(rec, attr, data);

		SDPDBG("Extract PDU, seqLength: %d localExtractedLength: %d",
							seqlen, extracted);
	}
#ifdef SDP_DEBUG
	SDPDBG("Successful extracting of Svc Rec attributes");
	sdp_print_service_attr(rec->attrlist);
#endif
	*scanned += seqlen;
	return rec;
}

static void sdp_copy_pattern(void *value, void *udata)
{
	uuid_t *uuid = value;
	sdp_record_t *rec = udata;

	sdp_pattern_add_uuid(rec, uuid);
}

static void *sdp_data_value(sdp_data_t *data, uint32_t *len)
{
	void *val = NULL;

	switch (data->dtd) {
	case SDP_DATA_NIL:
		break;
	case SDP_UINT8:
		val = &data->val.uint8;
		break;
	case SDP_INT8:
	case SDP_BOOL:
		val = &data->val.int8;
		break;
	case SDP_UINT16:
		val = &data->val.uint16;
		break;
	case SDP_INT16:
		val = &data->val.int16;
		break;
	case SDP_UINT32:
		val = &data->val.uint32;
		break;
	case SDP_INT32:
		val = &data->val.int32;
		break;
	case SDP_INT64:
		val = &data->val.int64;
		break;
	case SDP_UINT64:
		val = &data->val.uint64;
		break;
	case SDP_UINT128:
		val = &data->val.uint128;
		break;
	case SDP_INT128:
		val = &data->val.int128;
		break;
	case SDP_UUID16:
		val = &data->val.uuid.value.uuid16;
		break;
	case SDP_UUID32:
		val = &data->val.uuid.value.uuid32;
		break;
	case SDP_UUID128:
		val = &data->val.uuid.value.uuid128;
		break;
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
	case SDP_URL_STR32:
	case SDP_TEXT_STR32:
		val = data->val.str;
		if (len)
			*len = data->unitSize - 1;
		break;
	case SDP_ALT8:
	case SDP_ALT16:
	case SDP_ALT32:
	case SDP_SEQ8:
	case SDP_SEQ16:
	case SDP_SEQ32:
		val = sdp_copy_seq(data->val.dataseq);
		break;
	}

	return val;
}

static sdp_data_t *sdp_copy_seq(sdp_data_t *data)
{
	sdp_data_t *tmp, *seq = NULL, *cur = NULL;

	for (tmp = data; tmp; tmp = tmp->next) {
		sdp_data_t *datatmp;
		void *value;

		value = sdp_data_value(tmp, NULL);
		datatmp = sdp_data_alloc_with_length(tmp->dtd, value,
								tmp->unitSize);

		if (cur)
			cur->next = datatmp;
		else
			seq = datatmp;

		cur = datatmp;
	}

	return seq;
}

static void sdp_copy_attrlist(void *value, void *udata)
{
	sdp_data_t *data = value;
	sdp_record_t *rec = udata;
	void *val;
	uint32_t len = 0;

	val = sdp_data_value(data, &len);

	if (!len)
		sdp_attr_add_new(rec, data->attrId, data->dtd, val);
	else
		sdp_attr_add_new_with_length(rec, data->attrId,
							data->dtd, val, len);
}

sdp_record_t *sdp_copy_record(sdp_record_t *rec)
{
	sdp_record_t *cpy;

	cpy = sdp_record_alloc();

	cpy->handle = rec->handle;

	sdp_list_foreach(rec->pattern, sdp_copy_pattern, cpy);
	sdp_list_foreach(rec->attrlist, sdp_copy_attrlist, cpy);

	cpy->svclass = rec->svclass;

	return cpy;
}

#ifdef SDP_DEBUG
static void print_dataseq(sdp_data_t *p)
{
	sdp_data_t *d;

	for (d = p; d; d = d->next)
		sdp_data_print(d);
}
#endif

void sdp_record_print(const sdp_record_t *rec)
{
	sdp_data_t *d = sdp_data_get(rec, SDP_ATTR_SVCNAME_PRIMARY);
	if (d && SDP_IS_TEXT_STR(d->dtd))
		printf("Service Name: %.*s", d->unitSize, d->val.str);
	d = sdp_data_get(rec, SDP_ATTR_SVCDESC_PRIMARY);
	if (d && SDP_IS_TEXT_STR(d->dtd))
		printf("Service Description: %.*s", d->unitSize, d->val.str);
	d = sdp_data_get(rec, SDP_ATTR_PROVNAME_PRIMARY);
	if (d && SDP_IS_TEXT_STR(d->dtd))
		printf("Service Provider: %.*s", d->unitSize, d->val.str);
}

#ifdef SDP_DEBUG
void sdp_data_print(sdp_data_t *d)
{
	switch (d->dtd) {
	case SDP_DATA_NIL:
		SDPDBG("NIL");
		break;
	case SDP_BOOL:
	case SDP_UINT8:
	case SDP_UINT16:
	case SDP_UINT32:
	case SDP_UINT64:
	case SDP_UINT128:
	case SDP_INT8:
	case SDP_INT16:
	case SDP_INT32:
	case SDP_INT64:
	case SDP_INT128:
		SDPDBG("Integer : 0x%x", d->val.uint32);
		break;
	case SDP_UUID16:
	case SDP_UUID32:
	case SDP_UUID128:
		SDPDBG("UUID");
		sdp_uuid_print(&d->val.uuid);
		break;
	case SDP_TEXT_STR8:
	case SDP_TEXT_STR16:
	case SDP_TEXT_STR32:
		SDPDBG("Text : %s", d->val.str);
		break;
	case SDP_URL_STR8:
	case SDP_URL_STR16:
	case SDP_URL_STR32:
		SDPDBG("URL : %s", d->val.str);
		break;
	case SDP_SEQ8:
	case SDP_SEQ16:
	case SDP_SEQ32:
		print_dataseq(d->val.dataseq);
		break;
	case SDP_ALT8:
	case SDP_ALT16:
	case SDP_ALT32:
		SDPDBG("Data Sequence Alternates");
		print_dataseq(d->val.dataseq);
		break;
	}
}
#endif

sdp_data_t *sdp_data_get(const sdp_record_t *rec, uint16_t attrId)
{
	if (rec->attrlist) {
		sdp_data_t sdpTemplate;
		sdp_list_t *p;

		sdpTemplate.attrId = attrId;
		p = sdp_list_find(rec->attrlist, &sdpTemplate, sdp_attrid_comp_func);
		if (p)
			return p->data;
	}
	return NULL;
}

static int sdp_send_req(sdp_session_t *session, uint8_t *buf, uint32_t size)
{
	uint32_t sent = 0;

	while (sent < size) {
		int n = send(session->sock, buf + sent, size - sent, 0);
		if (n < 0)
			return -1;
		sent += n;
	}
	return 0;
}

static int sdp_read_rsp(sdp_session_t *session, uint8_t *buf, uint32_t size)
{
	fd_set readFds;
	struct timeval timeout = { SDP_RESPONSE_TIMEOUT, 0 };

	FD_ZERO(&readFds);
	FD_SET(session->sock, &readFds);
	SDPDBG("Waiting for response");
	if (select(session->sock + 1, &readFds, NULL, NULL, &timeout) == 0) {
		SDPERR("Client timed out");
		errno = ETIMEDOUT;
		return -1;
	}
	return recv(session->sock, buf, size, 0);
}

/*
 * generic send request, wait for response method.
 */
int sdp_send_req_w4_rsp(sdp_session_t *session, uint8_t *reqbuf,
			uint8_t *rspbuf, uint32_t reqsize, uint32_t *rspsize)
{
	int n;
	sdp_pdu_hdr_t *reqhdr = (sdp_pdu_hdr_t *) reqbuf;
	sdp_pdu_hdr_t *rsphdr = (sdp_pdu_hdr_t *) rspbuf;

	SDPDBG("");
	if (0 > sdp_send_req(session, reqbuf, reqsize)) {
		SDPERR("Error sending data:%m");
		return -1;
	}
	n = sdp_read_rsp(session, rspbuf, SDP_RSP_BUFFER_SIZE);
	if (0 > n)
		return -1;
	SDPDBG("Read : %d", n);
	if (n == 0 || reqhdr->tid != rsphdr->tid) {
		errno = EPROTO;
		return -1;
	}
	*rspsize = n;
	return 0;
}

/*
 * singly-linked lists (after openobex implementation)
 */
sdp_list_t *sdp_list_append(sdp_list_t *p, void *d)
{
	sdp_list_t *q, *n = malloc(sizeof(sdp_list_t));

	if (!n)
		return NULL;

	n->data = d;
	n->next = 0;

	if (!p)
		return n;

	for (q = p; q->next; q = q->next);
	q->next = n;

	return p;
}

sdp_list_t *sdp_list_remove(sdp_list_t *list, void *d)
{
	sdp_list_t *p, *q;

	for (q = 0, p = list; p; q = p, p = p->next)
		if (p->data == d) {
			if (q)
				q->next = p->next;
			else
				list = p->next;
			free(p);
			break;
		}

	return list;
}

sdp_list_t *sdp_list_insert_sorted(sdp_list_t *list, void *d,
							sdp_comp_func_t f)
{
	sdp_list_t *q, *p, *n;

	n = malloc(sizeof(sdp_list_t));
	if (!n)
		return NULL;
	n->data = d;
	for (q = 0, p = list; p; q = p, p = p->next)
		if (f(p->data, d) >= 0)
			break;
	/* insert between q and p; if !q insert at head */
	if (q)
		q->next = n;
	else
		list = n;
	n->next = p;
	return list;
}

/*
 * Every element of the list points to things which need
 * to be free()'d. This method frees the list's contents
 */
void sdp_list_free(sdp_list_t *list, sdp_free_func_t f)
{
	sdp_list_t *next;
	while (list) {
		next = list->next;
		if (f)
			f(list->data);
		free(list);
		list = next;
	}
}

static inline int __find_port(sdp_data_t *seq, int proto)
{
	if (!seq || !seq->next)
		return 0;

	if (SDP_IS_UUID(seq->dtd) && sdp_uuid_to_proto(&seq->val.uuid) == proto) {
		seq = seq->next;
		switch (seq->dtd) {
		case SDP_UINT8:
			return seq->val.uint8;
		case SDP_UINT16:
			return seq->val.uint16;
		}
	}
	return 0;
}

int sdp_get_proto_port(const sdp_list_t *list, int proto)
{
	if (proto != L2CAP_UUID && proto != RFCOMM_UUID) {
		errno = EINVAL;
		return -1;
	}

	for (; list; list = list->next) {
		sdp_list_t *p;
		for (p = list->data; p; p = p->next) {
			sdp_data_t *seq = p->data;
			int port = __find_port(seq, proto);
			if (port)
				return port;
		}
	}
	return 0;
}

sdp_data_t *sdp_get_proto_desc(sdp_list_t *list, int proto)
{
	for (; list; list = list->next) {
		sdp_list_t *p;
		for (p = list->data; p; p = p->next) {
			sdp_data_t *seq = p->data;
			if (SDP_IS_UUID(seq->dtd) &&
					sdp_uuid_to_proto(&seq->val.uuid) == proto)
				return seq->next;
		}
	}
	return NULL;
}

static int sdp_get_proto_descs(uint16_t attr_id, const sdp_record_t *rec,
							sdp_list_t **pap)
{
	sdp_data_t *pdlist, *curr;
	sdp_list_t *ap = NULL;

	pdlist = sdp_data_get(rec, attr_id);
	if (pdlist == NULL) {
		errno = ENODATA;
		return -1;
	}

	SDPDBG("Attribute value type: 0x%02x", pdlist->dtd);

	if (attr_id == SDP_ATTR_ADD_PROTO_DESC_LIST) {
		if (!SDP_IS_SEQ(pdlist->dtd)) {
			errno = EINVAL;
			return -1;
		}
		pdlist = pdlist->val.dataseq;
	}

	for (; pdlist; pdlist = pdlist->next) {
		sdp_list_t *pds = NULL;

		if (!SDP_IS_SEQ(pdlist->dtd) && !SDP_IS_ALT(pdlist->dtd))
			goto failed;

		for (curr = pdlist->val.dataseq; curr; curr = curr->next) {
			if (!SDP_IS_SEQ(curr->dtd)) {
				sdp_list_free(pds, NULL);
				goto failed;
			}
			pds = sdp_list_append(pds, curr->val.dataseq);
		}

		ap = sdp_list_append(ap, pds);
	}

	*pap = ap;

	return 0;

failed:
	sdp_list_foreach(ap, (sdp_list_func_t) sdp_list_free, NULL);
	sdp_list_free(ap, NULL);
	errno = EINVAL;

	return -1;
}

int sdp_get_access_protos(const sdp_record_t *rec, sdp_list_t **pap)
{
	return sdp_get_proto_descs(SDP_ATTR_PROTO_DESC_LIST, rec, pap);
}

int sdp_get_add_access_protos(const sdp_record_t *rec, sdp_list_t **pap)
{
	return sdp_get_proto_descs(SDP_ATTR_ADD_PROTO_DESC_LIST, rec, pap);
}

int sdp_get_uuidseq_attr(const sdp_record_t *rec, uint16_t attr,
							sdp_list_t **seqp)
{
	sdp_data_t *sdpdata = sdp_data_get(rec, attr);

	*seqp = NULL;
	if (sdpdata && SDP_IS_SEQ(sdpdata->dtd)) {
		sdp_data_t *d;
		for (d = sdpdata->val.dataseq; d; d = d->next) {
			uuid_t *u;
			if (d->dtd < SDP_UUID16 || d->dtd > SDP_UUID128) {
				errno = EINVAL;
				goto fail;
			}

			u = malloc(sizeof(uuid_t));
			if (!u)
				goto fail;

			*u = d->val.uuid;
			*seqp = sdp_list_append(*seqp, u);
		}
		return 0;
	}
fail:
	sdp_list_free(*seqp, free);
	*seqp = NULL;
	return -1;
}

int sdp_set_uuidseq_attr(sdp_record_t *rec, uint16_t aid, sdp_list_t *seq)
{
	int status = 0, i, len;
	void **dtds, **values;
	uint8_t uuid16 = SDP_UUID16;
	uint8_t uuid32 = SDP_UUID32;
	uint8_t uuid128 = SDP_UUID128;
	sdp_list_t *p;

	len = sdp_list_len(seq);
	if (!seq || len == 0)
		return -1;
	dtds = malloc(len * sizeof(void *));
	if (!dtds)
		return -1;

	values = malloc(len * sizeof(void *));
	if (!values) {
		free(dtds);
		return -1;
	}

	for (p = seq, i = 0; i < len; i++, p = p->next) {
		uuid_t *uuid = p->data;
		if (uuid)
			switch (uuid->type) {
			case SDP_UUID16:
				dtds[i] = &uuid16;
				values[i] = &uuid->value.uuid16;
				break;
			case SDP_UUID32:
				dtds[i] = &uuid32;
				values[i] = &uuid->value.uuid32;
				break;
			case SDP_UUID128:
				dtds[i] = &uuid128;
				values[i] = &uuid->value.uuid128;
				break;
			default:
				status = -1;
				break;
			}
		else {
			status = -1;
			break;
		}
	}
	if (status == 0) {
		sdp_data_t *data = sdp_seq_alloc(dtds, values, len);
		sdp_attr_replace(rec, aid, data);
		sdp_pattern_add_uuidseq(rec, seq);
	}
	free(dtds);
	free(values);
	return status;
}

int sdp_get_lang_attr(const sdp_record_t *rec, sdp_list_t **langSeq)
{
	sdp_lang_attr_t *lang;
	sdp_data_t *sdpdata, *curr_data;

	*langSeq = NULL;
	sdpdata = sdp_data_get(rec, SDP_ATTR_LANG_BASE_ATTR_ID_LIST);
	if (sdpdata == NULL) {
		errno = ENODATA;
		return -1;
	}

	if (!SDP_IS_SEQ(sdpdata->dtd))
		goto invalid;
	curr_data = sdpdata->val.dataseq;

	while (curr_data) {
		sdp_data_t *pCode, *pEncoding, *pOffset;

		pCode = curr_data;
		if (pCode->dtd != SDP_UINT16)
			goto invalid;

		/* LanguageBaseAttributeIDList entries are always grouped as
		 * triplets */
		if (!pCode->next || !pCode->next->next)
			goto invalid;

		pEncoding = pCode->next;
		if (pEncoding->dtd != SDP_UINT16)
			goto invalid;

		pOffset = pEncoding->next;
		if (pOffset->dtd != SDP_UINT16)
			goto invalid;

		lang = malloc(sizeof(sdp_lang_attr_t));
		if (!lang) {
			sdp_list_free(*langSeq, free);
			*langSeq = NULL;
			return -1;
		}
		lang->code_ISO639 = pCode->val.uint16;
		lang->encoding = pEncoding->val.uint16;
		lang->base_offset = pOffset->val.uint16;
		SDPDBG("code_ISO639 :  0x%02x", lang->code_ISO639);
		SDPDBG("encoding :     0x%02x", lang->encoding);
		SDPDBG("base_offfset : 0x%02x", lang->base_offset);
		*langSeq = sdp_list_append(*langSeq, lang);

		curr_data = pOffset->next;
	}

	return 0;

invalid:
	sdp_list_free(*langSeq, free);
	*langSeq = NULL;
	errno = EINVAL;

	return -1;
}

int sdp_get_profile_descs(const sdp_record_t *rec, sdp_list_t **profDescSeq)
{
	sdp_profile_desc_t *profDesc;
	sdp_data_t *sdpdata, *seq;

	*profDescSeq = NULL;
	sdpdata = sdp_data_get(rec, SDP_ATTR_PFILE_DESC_LIST);
	if (sdpdata == NULL) {
		errno = ENODATA;
		return -1;
	}

	if (!SDP_IS_SEQ(sdpdata->dtd) || sdpdata->val.dataseq == NULL)
		goto invalid;

	for (seq = sdpdata->val.dataseq; seq; seq = seq->next) {
		uuid_t *uuid = NULL;
		uint16_t version = 0x100;

		if (SDP_IS_UUID(seq->dtd)) {
			/* Mac OS X 10.7.3 and old Samsung phones do not comply
			 * to the SDP specification for
			 * BluetoothProfileDescriptorList. This workaround
			 * allows to properly parse UUID/version from SDP
			 * record published by these systems. */
			sdp_data_t *next = seq->next;
			uuid = &seq->val.uuid;
			if (next && next->dtd == SDP_UINT16) {
				version = next->val.uint16;
				seq = next;
			}
		} else if (SDP_IS_SEQ(seq->dtd)) {
			sdp_data_t *puuid, *pVnum;

			puuid = seq->val.dataseq;
			if (puuid == NULL || !SDP_IS_UUID(puuid->dtd))
				goto invalid;

			uuid = &puuid->val.uuid;

			pVnum = puuid->next;
			if (pVnum == NULL || pVnum->dtd != SDP_UINT16)
				goto invalid;

			version = pVnum->val.uint16;
		} else
			goto invalid;

		if (uuid != NULL) {
			profDesc = malloc(sizeof(sdp_profile_desc_t));
			if (!profDesc) {
				sdp_list_free(*profDescSeq, free);
				*profDescSeq = NULL;
				return -1;
			}
			profDesc->uuid = *uuid;
			profDesc->version = version;
#ifdef SDP_DEBUG
			sdp_uuid_print(&profDesc->uuid);
			SDPDBG("Vnum : 0x%04x", profDesc->version);
#endif
			*profDescSeq = sdp_list_append(*profDescSeq, profDesc);
		}
	}
	return 0;

invalid:
	sdp_list_free(*profDescSeq, free);
	*profDescSeq = NULL;
	errno = EINVAL;

	return -1;
}

int sdp_get_server_ver(const sdp_record_t *rec, sdp_list_t **u16)
{
	sdp_data_t *d, *curr;

	*u16 = NULL;
	d = sdp_data_get(rec, SDP_ATTR_VERSION_NUM_LIST);
	if (d == NULL) {
		errno = ENODATA;
		return -1;
	}

	if (!SDP_IS_SEQ(d->dtd) || d->val.dataseq == NULL)
		goto invalid;

	for (curr = d->val.dataseq; curr; curr = curr->next) {
		if (curr->dtd != SDP_UINT16)
			goto invalid;
		*u16 = sdp_list_append(*u16, &curr->val.uint16);
	}

	return 0;

invalid:
	sdp_list_free(*u16, NULL);
	*u16 = NULL;
	errno = EINVAL;

	return -1;
}

/* flexible extraction of basic attributes - Jean II */
/* How do we expect caller to extract predefined data sequences? */
int sdp_get_int_attr(const sdp_record_t *rec, uint16_t attrid, int *value)
{
	sdp_data_t *sdpdata = sdp_data_get(rec, attrid);

	if (sdpdata)
		/* Verify that it is what the caller expects */
		if (sdpdata->dtd == SDP_BOOL || sdpdata->dtd == SDP_UINT8 ||
		sdpdata->dtd == SDP_UINT16 || sdpdata->dtd == SDP_UINT32 ||
		sdpdata->dtd == SDP_INT8 || sdpdata->dtd == SDP_INT16 ||
		sdpdata->dtd == SDP_INT32) {
			*value = sdpdata->val.uint32;
			return 0;
		}
	errno = EINVAL;
	return -1;
}

int sdp_get_string_attr(const sdp_record_t *rec, uint16_t attrid, char *value,
								int valuelen)
{
	sdp_data_t *sdpdata = sdp_data_get(rec, attrid);
	if (sdpdata)
		/* Verify that it is what the caller expects */
		if (SDP_IS_TEXT_STR(sdpdata->dtd))
			if ((int) strlen(sdpdata->val.str) < valuelen) {
				strcpy(value, sdpdata->val.str);
				return 0;
			}
	errno = EINVAL;
	return -1;
}

#define get_basic_attr(attrID, pAttrValue, fieldName)		\
	sdp_data_t *data = sdp_data_get(rec, attrID);		\
	if (data) {						\
		*pAttrValue = data->val.fieldName;		\
		return 0;					\
	}							\
	errno = EINVAL;						\
	return -1;

int sdp_get_service_id(const sdp_record_t *rec, uuid_t *uuid)
{
	get_basic_attr(SDP_ATTR_SERVICE_ID, uuid, uuid);
}

int sdp_get_group_id(const sdp_record_t *rec, uuid_t *uuid)
{
	get_basic_attr(SDP_ATTR_GROUP_ID, uuid, uuid);
}

int sdp_get_record_state(const sdp_record_t *rec, uint32_t *svcRecState)
{
	get_basic_attr(SDP_ATTR_RECORD_STATE, svcRecState, uint32);
}

int sdp_get_service_avail(const sdp_record_t *rec, uint8_t *svcAvail)
{
	get_basic_attr(SDP_ATTR_SERVICE_AVAILABILITY, svcAvail, uint8);
}

int sdp_get_service_ttl(const sdp_record_t *rec, uint32_t *svcTTLInfo)
{
	get_basic_attr(SDP_ATTR_SVCINFO_TTL, svcTTLInfo, uint32);
}

int sdp_get_database_state(const sdp_record_t *rec, uint32_t *svcDBState)
{
	get_basic_attr(SDP_ATTR_SVCDB_STATE, svcDBState, uint32);
}

/*
 * NOTE that none of the setXXX() functions below will
 * actually update the SDP server, unless the
 * {register, update}sdp_record_t() function is invoked.
 */

int sdp_attr_add_new(sdp_record_t *rec, uint16_t attr, uint8_t dtd,
							const void *value)
{
	sdp_data_t *d = sdp_data_alloc(dtd, value);
	if (d) {
		sdp_attr_replace(rec, attr, d);
		return 0;
	}
	return -1;
}

static int sdp_attr_add_new_with_length(sdp_record_t *rec,
	uint16_t attr, uint8_t dtd, const void *value, uint32_t len)
{
	sdp_data_t *d;

	d = sdp_data_alloc_with_length(dtd, value, len);
	if (!d)
		return -1;

	sdp_attr_replace(rec, attr, d);

	return 0;
}

/*
 * Set the information attributes of the service
 * pointed to by rec. The attributes are
 * service name, description and provider name
 */
void sdp_set_info_attr(sdp_record_t *rec, const char *name, const char *prov,
							const char *desc)
{
	if (name)
		sdp_attr_add_new(rec, SDP_ATTR_SVCNAME_PRIMARY,
							SDP_TEXT_STR8, name);
	if (prov)
		sdp_attr_add_new(rec, SDP_ATTR_PROVNAME_PRIMARY,
							SDP_TEXT_STR8, prov);
	if (desc)
		sdp_attr_add_new(rec, SDP_ATTR_SVCDESC_PRIMARY,
							SDP_TEXT_STR8, desc);
}

static sdp_data_t *access_proto_to_dataseq(sdp_record_t *rec, sdp_list_t *proto)
{
	sdp_data_t *seq = NULL;
	void *dtds[10], *values[10];
	void **seqDTDs, **seqs;
	int i, seqlen;
	sdp_list_t *p;

	seqlen = sdp_list_len(proto);
	seqDTDs = malloc(seqlen * sizeof(void *));
	if (!seqDTDs)
		return NULL;

	seqs = malloc(seqlen * sizeof(void *));
	if (!seqs) {
		free(seqDTDs);
		return NULL;
	}

	for (i = 0, p = proto; p; p = p->next, i++) {
		sdp_list_t *elt = p->data;
		sdp_data_t *s;
		uuid_t *uuid = NULL;
		unsigned int pslen = 0;
		for (; elt && pslen < ARRAY_SIZE(dtds); elt = elt->next, pslen++) {
			sdp_data_t *d = elt->data;
			dtds[pslen] = &d->dtd;
			switch (d->dtd) {
			case SDP_UUID16:
				uuid = (uuid_t *) d;
				values[pslen] = &uuid->value.uuid16;
				break;
			case SDP_UUID32:
				uuid = (uuid_t *) d;
				values[pslen] = &uuid->value.uuid32;
				break;
			case SDP_UUID128:
				uuid = (uuid_t *) d;
				values[pslen] = &uuid->value.uuid128;
				break;
			case SDP_UINT8:
				values[pslen] = &d->val.uint8;
				break;
			case SDP_UINT16:
				values[pslen] = &d->val.uint16;
				break;
			case SDP_SEQ8:
			case SDP_SEQ16:
			case SDP_SEQ32:
				values[pslen] = d;
				break;
			/* FIXME: more */
			}
		}
		s = sdp_seq_alloc(dtds, values, pslen);
		if (s) {
			seqDTDs[i] = &s->dtd;
			seqs[i] = s;
			if (uuid)
				sdp_pattern_add_uuid(rec, uuid);
		}
	}
	seq = sdp_seq_alloc(seqDTDs, seqs, seqlen);
	free(seqDTDs);
	free(seqs);
	return seq;
}

/*
 * sets the access protocols of the service specified
 * to the value specified in "access_proto"
 *
 * Note that if there are alternate mechanisms by
 * which the service is accessed, then they should
 * be specified as sequences
 *
 * Using a value of NULL for accessProtocols has
 * effect of removing this attribute (if previously set)
 *
 * This function replaces the existing sdp_access_proto_t
 * structure (if any) with the new one specified.
 *
 * returns 0 if successful or -1 if there is a failure.
 */
int sdp_set_access_protos(sdp_record_t *rec, const sdp_list_t *ap)
{
	const sdp_list_t *p;
	sdp_data_t *protos = NULL;

	for (p = ap; p; p = p->next) {
		sdp_data_t *seq = access_proto_to_dataseq(rec, p->data);
		protos = sdp_seq_append(protos, seq);
	}

	sdp_attr_add(rec, SDP_ATTR_PROTO_DESC_LIST, protos);

	return 0;
}

int sdp_set_add_access_protos(sdp_record_t *rec, const sdp_list_t *ap)
{
	const sdp_list_t *p;
	sdp_data_t *protos = NULL;

	for (p = ap; p; p = p->next) {
		sdp_data_t *seq = access_proto_to_dataseq(rec, p->data);
		protos = sdp_seq_append(protos, seq);
	}

	sdp_attr_add(rec, SDP_ATTR_ADD_PROTO_DESC_LIST,
			protos ? sdp_data_alloc(SDP_SEQ8, protos) : NULL);

	return 0;
}

/*
 * set the "LanguageBase" attributes of the service record
 * record to the value specified in "langAttrList".
 *
 * "langAttrList" is a linked list of "sdp_lang_attr_t"
 * objects, one for each language in which user visible
 * attributes are present in the service record.
 *
 * Using a value of NULL for langAttrList has
 * effect of removing this attribute (if previously set)
 *
 * This function replaces the exisiting sdp_lang_attr_t
 * structure (if any) with the new one specified.
 *
 * returns 0 if successful or -1 if there is a failure.
 */
int sdp_set_lang_attr(sdp_record_t *rec, const sdp_list_t *seq)
{
	uint8_t uint16 = SDP_UINT16;
	int status = 0, i = 0, seqlen = sdp_list_len(seq);
	void **dtds, **values;
	const sdp_list_t *p;

	dtds = malloc(3 * seqlen * sizeof(void *));
	if (!dtds)
		return -1;

	values = malloc(3 * seqlen * sizeof(void *));
	if (!values) {
		free(dtds);
		return -1;
	}

	for (p = seq; p; p = p->next) {
		sdp_lang_attr_t *lang = p->data;
		if (!lang) {
			status = -1;
			break;
		}
		dtds[i] = &uint16;
		values[i] = &lang->code_ISO639;
		i++;
		dtds[i] = &uint16;
		values[i] = &lang->encoding;
		i++;
		dtds[i] = &uint16;
		values[i] = &lang->base_offset;
		i++;
	}
	if (status == 0) {
		sdp_data_t *seq = sdp_seq_alloc(dtds, values, 3 * seqlen);
		sdp_attr_add(rec, SDP_ATTR_LANG_BASE_ATTR_ID_LIST, seq);
	}
	free(dtds);
	free(values);
	return status;
}

/*
 * set the "ServiceID" attribute of the service.
 *
 * This is the UUID of the service.
 *
 * returns 0 if successful or -1 if there is a failure.
 */
void sdp_set_service_id(sdp_record_t *rec, uuid_t uuid)
{
	switch (uuid.type) {
	case SDP_UUID16:
		sdp_attr_add_new(rec, SDP_ATTR_SERVICE_ID, SDP_UUID16,
							&uuid.value.uuid16);
		break;
	case SDP_UUID32:
		sdp_attr_add_new(rec, SDP_ATTR_SERVICE_ID, SDP_UUID32,
							&uuid.value.uuid32);
		break;
	case SDP_UUID128:
		sdp_attr_add_new(rec, SDP_ATTR_SERVICE_ID, SDP_UUID128,
							&uuid.value.uuid128);
		break;
	}
	sdp_pattern_add_uuid(rec, &uuid);
}

/*
 * set the GroupID attribute of the service record defining a group.
 *
 * This is the UUID of the group.
 *
 * returns 0 if successful or -1 if there is a failure.
 */
void sdp_set_group_id(sdp_record_t *rec, uuid_t uuid)
{
	switch (uuid.type) {
	case SDP_UUID16:
		sdp_attr_add_new(rec, SDP_ATTR_GROUP_ID, SDP_UUID16,
							&uuid.value.uuid16);
		break;
	case SDP_UUID32:
		sdp_attr_add_new(rec, SDP_ATTR_GROUP_ID, SDP_UUID32,
							&uuid.value.uuid32);
		break;
	case SDP_UUID128:
		sdp_attr_add_new(rec, SDP_ATTR_GROUP_ID, SDP_UUID128,
							&uuid.value.uuid128);
		break;
	}
	sdp_pattern_add_uuid(rec, &uuid);
}

/*
 * set the ProfileDescriptorList attribute of the service record
 * pointed to by record to the value specified in "profileDesc".
 *
 * Each element in the list is an object of type
 * sdp_profile_desc_t which is a definition of the
 * Bluetooth profile that this service conforms to.
 *
 * Using a value of NULL for profileDesc has
 * effect of removing this attribute (if previously set)
 *
 * This function replaces the exisiting ProfileDescriptorList
 * structure (if any) with the new one specified.
 *
 * returns 0 if successful or -1 if there is a failure.
 */
int sdp_set_profile_descs(sdp_record_t *rec, const sdp_list_t *profiles)
{
	int status = 0;
	uint8_t uuid16 = SDP_UUID16;
	uint8_t uuid32 = SDP_UUID32;
	uint8_t uuid128 = SDP_UUID128;
	uint8_t uint16 = SDP_UINT16;
	int i = 0, seqlen = sdp_list_len(profiles);
	void **seqDTDs, **seqs;
	const sdp_list_t *p;
	sdp_data_t *pAPSeq;

	seqDTDs = malloc(seqlen * sizeof(void *));
	if (!seqDTDs)
		return -1;

	seqs = malloc(seqlen * sizeof(void *));
	if (!seqs) {
		free(seqDTDs);
		return -1;
	}

	for (p = profiles; p; p = p->next) {
		sdp_data_t *seq;
		void *dtds[2], *values[2];
		sdp_profile_desc_t *profile = p->data;
		if (!profile) {
			status = -1;
			goto end;
		}
		switch (profile->uuid.type) {
		case SDP_UUID16:
			dtds[0] = &uuid16;
			values[0] = &profile->uuid.value.uuid16;
			break;
		case SDP_UUID32:
			dtds[0] = &uuid32;
			values[0] = &profile->uuid.value.uuid32;
			break;
		case SDP_UUID128:
			dtds[0] = &uuid128;
			values[0] = &profile->uuid.value.uuid128;
			break;
		default:
			status = -1;
			goto end;
		}
		dtds[1] = &uint16;
		values[1] = &profile->version;
		seq = sdp_seq_alloc(dtds, values, 2);

		if (seq == NULL) {
			status = -1;
			goto end;
		}

		seqDTDs[i] = &seq->dtd;
		seqs[i] = seq;
		sdp_pattern_add_uuid(rec, &profile->uuid);
		i++;
	}

	pAPSeq = sdp_seq_alloc(seqDTDs, seqs, seqlen);
	sdp_attr_add(rec, SDP_ATTR_PFILE_DESC_LIST, pAPSeq);
end:
	free(seqDTDs);
	free(seqs);
	return status;
}

/*
 * sets various URL attributes of the service
 * pointed to by record. The URL include
 *
 * client: a URL to the client's
 *   platform specific (WinCE, PalmOS) executable
 *   code that can be used to access this service.
 *
 * doc: a URL pointing to service documentation
 *
 * icon: a URL to an icon that can be used to represent
 *   this service.
 *
 * Note that you need to pass NULL for any URLs
 * that you don't want to set or remove
 */
void sdp_set_url_attr(sdp_record_t *rec, const char *client, const char *doc,
							const char *icon)
{
	sdp_attr_add_new(rec, SDP_ATTR_CLNT_EXEC_URL, SDP_URL_STR8, client);
	sdp_attr_add_new(rec, SDP_ATTR_DOC_URL, SDP_URL_STR8, doc);
	sdp_attr_add_new(rec, SDP_ATTR_ICON_URL, SDP_URL_STR8, icon);
}

uuid_t *sdp_uuid16_create(uuid_t *u, uint16_t val)
{
	memset(u, 0, sizeof(uuid_t));
	u->type = SDP_UUID16;
	u->value.uuid16 = val;
	return u;
}

uuid_t *sdp_uuid32_create(uuid_t *u, uint32_t val)
{
	memset(u, 0, sizeof(uuid_t));
	u->type = SDP_UUID32;
	u->value.uuid32 = val;
	return u;
}

uuid_t *sdp_uuid128_create(uuid_t *u, const void *val)
{
	memset(u, 0, sizeof(uuid_t));
	u->type = SDP_UUID128;
	memcpy(&u->value.uuid128, val, sizeof(uint128_t));
	return u;
}

/*
 * UUID comparison function
 * returns 0 if uuidValue1 == uuidValue2 else -1
 */
int sdp_uuid_cmp(const void *p1, const void *p2)
{
	uuid_t *u1 = sdp_uuid_to_uuid128(p1);
	uuid_t *u2 = sdp_uuid_to_uuid128(p2);
	int ret;

	ret = sdp_uuid128_cmp(u1, u2);

	bt_free(u1);
	bt_free(u2);

	return ret;
}

/*
 * UUID comparison function
 * returns 0 if uuidValue1 == uuidValue2 else -1
 */
int sdp_uuid16_cmp(const void *p1, const void *p2)
{
	const uuid_t *u1 = p1;
	const uuid_t *u2 = p2;
	return memcmp(&u1->value.uuid16, &u2->value.uuid16, sizeof(uint16_t));
}

/*
 * UUID comparison function
 * returns 0 if uuidValue1 == uuidValue2 else -1
 */
int sdp_uuid128_cmp(const void *p1, const void *p2)
{
	const uuid_t *u1 = p1;
	const uuid_t *u2 = p2;
	return memcmp(&u1->value.uuid128, &u2->value.uuid128, sizeof(uint128_t));
}

/*
 * 128 to 16 bit and 32 to 16 bit UUID conversion functions
 * yet to be implemented. Note that the input is in NBO in
 * both 32 and 128 bit UUIDs and conversion is needed
 */
void sdp_uuid16_to_uuid128(uuid_t *uuid128, const uuid_t *uuid16)
{
	/*
	 * We have a 16 bit value, which needs to be added to
	 * bytes 3 and 4 (at indices 2 and 3) of the Bluetooth base
	 */
	unsigned short data1;

	/* allocate a 128bit UUID and init to the Bluetooth base UUID */
	uuid128->value.uuid128 = bluetooth_base_uuid;
	uuid128->type = SDP_UUID128;

	/* extract bytes 2 and 3 of 128bit BT base UUID */
	memcpy(&data1, &bluetooth_base_uuid.data[2], 2);

	/* add the given UUID (16 bits) */
	data1 += htons(uuid16->value.uuid16);

	/* set bytes 2 and 3 of the 128 bit value */
	memcpy(&uuid128->value.uuid128.data[2], &data1, 2);
}

void sdp_uuid32_to_uuid128(uuid_t *uuid128, const uuid_t *uuid32)
{
	/*
	 * We have a 32 bit value, which needs to be added to
	 * bytes 1->4 (at indices 0 thru 3) of the Bluetooth base
	 */
	unsigned int data0;

	/* allocate a 128bit UUID and init to the Bluetooth base UUID */
	uuid128->value.uuid128 = bluetooth_base_uuid;
	uuid128->type = SDP_UUID128;

	/* extract first 4 bytes */
	memcpy(&data0, &bluetooth_base_uuid.data[0], 4);

	/* add the given UUID (32bits) */
	data0 += htonl(uuid32->value.uuid32);

	/* set the 4 bytes of the 128 bit value */
	memcpy(&uuid128->value.uuid128.data[0], &data0, 4);
}

uuid_t *sdp_uuid_to_uuid128(const uuid_t *uuid)
{
	uuid_t *uuid128 = bt_malloc(sizeof(uuid_t));

	if (!uuid128)
		return NULL;

	memset(uuid128, 0, sizeof(uuid_t));
	switch (uuid->type) {
	case SDP_UUID128:
		*uuid128 = *uuid;
		break;
	case SDP_UUID32:
		sdp_uuid32_to_uuid128(uuid128, uuid);
		break;
	case SDP_UUID16:
		sdp_uuid16_to_uuid128(uuid128, uuid);
		break;
	}
	return uuid128;
}

/*
 * converts a 128-bit uuid to a 16/32-bit one if possible
 * returns true if uuid contains a 16/32-bit UUID at exit
 */
int sdp_uuid128_to_uuid(uuid_t *uuid)
{
	uint128_t *b = &bluetooth_base_uuid;
	uint128_t *u = &uuid->value.uuid128;
	uint32_t data;
	unsigned int i;

	if (uuid->type != SDP_UUID128)
		return 1;

	for (i = 4; i < sizeof(b->data); i++)
		if (b->data[i] != u->data[i])
			return 0;

	memcpy(&data, u->data, 4);
	data = htonl(data);
	if (data <= 0xffff) {
		uuid->type = SDP_UUID16;
		uuid->value.uuid16 = (uint16_t) data;
	} else {
		uuid->type = SDP_UUID32;
		uuid->value.uuid32 = data;
	}
	return 1;
}

/*
 * convert a UUID to the 16-bit short-form
 */
int sdp_uuid_to_proto(uuid_t *uuid)
{
	uuid_t u = *uuid;
	if (sdp_uuid128_to_uuid(&u)) {
		switch (u.type) {
		case SDP_UUID16:
			return u.value.uuid16;
		case SDP_UUID32:
			return u.value.uuid32;
		}
	}
	return 0;
}

/*
 * This function appends data to the PDU buffer "dst" from source "src".
 * The data length is also computed and set.
 * Should the PDU length exceed 2^8, then sequence type is
 * set accordingly and the data is memmove()'d.
 */
void sdp_append_to_buf(sdp_buf_t *dst, uint8_t *data, uint32_t len)
{
	uint8_t *p = dst->data;
	uint8_t dtd = *p;

	SDPDBG("Append src size: %d", len);
	SDPDBG("Append dst size: %d", dst->data_size);
	SDPDBG("Dst buffer size: %d", dst->buf_size);
	if (dst->data_size == 0 && dtd == 0) {
		/* create initial sequence */
		*p = SDP_SEQ8;
		dst->data_size += sizeof(uint8_t);
		/* reserve space for sequence size */
		dst->data_size += sizeof(uint8_t);
	}

	memcpy(dst->data + dst->data_size, data, len);
	dst->data_size += len;

	dtd = *(uint8_t *) dst->data;
	if (dst->data_size > UCHAR_MAX && dtd == SDP_SEQ8) {
		short offset = sizeof(uint8_t) + sizeof(uint8_t);
		memmove(dst->data + offset + 1, dst->data + offset,
						dst->data_size - offset);
		*p = SDP_SEQ16;
		dst->data_size += 1;
	}
	dtd = *(uint8_t *) p;
	p += sizeof(uint8_t);
	switch (dtd) {
	case SDP_SEQ8:
		*(uint8_t *) p = dst->data_size - sizeof(uint8_t) - sizeof(uint8_t);
		break;
	case SDP_SEQ16:
		bt_put_be16(dst->data_size - sizeof(uint8_t) - sizeof(uint16_t), p);
		break;
	case SDP_SEQ32:
		bt_put_be32(dst->data_size - sizeof(uint8_t) - sizeof(uint32_t), p);
		break;
	}
}

void sdp_append_to_pdu(sdp_buf_t *pdu, sdp_data_t *d)
{
	sdp_buf_t append;

	memset(&append, 0, sizeof(sdp_buf_t));
	sdp_gen_buffer(&append, d);
	append.data = malloc(append.buf_size);
	if (!append.data)
		return;

	sdp_set_attrid(&append, d->attrId);
	sdp_gen_pdu(&append, d);
	sdp_append_to_buf(pdu, append.data, append.data_size);
	free(append.data);
}

/*
 * Registers an sdp record.
 *
 * It is incorrect to call this method on a record that
 * has been already registered with the server.
 *
 * Returns zero on success, otherwise -1 (and sets errno).
 */
int sdp_device_record_register_binary(sdp_session_t *session, bdaddr_t *device, uint8_t *data, uint32_t size, uint8_t flags, uint32_t *handle)
{
	uint8_t *req, *rsp, *p;
	uint32_t reqsize, rspsize;
	sdp_pdu_hdr_t *reqhdr, *rsphdr;
	int status;

	SDPDBG("");

	if (!session->local) {
		errno = EREMOTE;
		return -1;
	}
	req = malloc(SDP_REQ_BUFFER_SIZE);
	rsp = malloc(SDP_RSP_BUFFER_SIZE);
	if (req == NULL || rsp == NULL) {
		status = -1;
		errno = ENOMEM;
		goto end;
	}

	reqhdr = (sdp_pdu_hdr_t *)req;
	reqhdr->pdu_id = SDP_SVC_REGISTER_REQ;
	reqhdr->tid    = htons(sdp_gen_tid(session));
	reqsize = sizeof(sdp_pdu_hdr_t) + 1;
	p = req + sizeof(sdp_pdu_hdr_t);

	if (bacmp(device, BDADDR_ANY)) {
		*p++ = flags | SDP_DEVICE_RECORD;
		bacpy((bdaddr_t *) p, device);
		p += sizeof(bdaddr_t);
		reqsize += sizeof(bdaddr_t);
	} else
		*p++ = flags;

	memcpy(p, data, size);
	reqsize += size;
	reqhdr->plen = htons(reqsize - sizeof(sdp_pdu_hdr_t));

	status = sdp_send_req_w4_rsp(session, req, rsp, reqsize, &rspsize);
	if (status < 0)
		goto end;

	if (rspsize < sizeof(sdp_pdu_hdr_t)) {
		SDPERR("Unexpected end of packet");
		errno = EPROTO;
		status = -1;
		goto end;
	}

	rsphdr = (sdp_pdu_hdr_t *) rsp;
	p = rsp + sizeof(sdp_pdu_hdr_t);

	if (rsphdr->pdu_id == SDP_ERROR_RSP) {
		/* Invalid service record */
		errno = EINVAL;
		status = -1;
	} else if (rsphdr->pdu_id != SDP_SVC_REGISTER_RSP) {
		errno = EPROTO;
		status = -1;
	} else {
		if (rspsize < sizeof(sdp_pdu_hdr_t) + sizeof(uint32_t)) {
			SDPERR("Unexpected end of packet");
			errno = EPROTO;
			status = -1;
			goto end;
		}
		if (handle)
			*handle  = bt_get_be32(p);
	}

end:
	free(req);
	free(rsp);

	return status;
}

int sdp_device_record_register(sdp_session_t *session, bdaddr_t *device, sdp_record_t *rec, uint8_t flags)
{
	sdp_buf_t pdu;
	uint32_t handle;
	int err;

	SDPDBG("");

	if (rec->handle && rec->handle != 0xffffffff) {
		uint32_t handle = rec->handle;
		sdp_data_t *data = sdp_data_alloc(SDP_UINT32, &handle);
		sdp_attr_replace(rec, SDP_ATTR_RECORD_HANDLE, data);
	}

	if (sdp_gen_record_pdu(rec, &pdu) < 0) {
		errno = ENOMEM;
		return -1;
	}

	err = sdp_device_record_register_binary(session, device,
				pdu.data, pdu.data_size, flags, &handle);

	free(pdu.data);

	if (err == 0) {
		sdp_data_t *data = sdp_data_alloc(SDP_UINT32, &handle);
		rec->handle = handle;
		sdp_attr_replace(rec, SDP_ATTR_RECORD_HANDLE, data);
	}

	return err;
}

int sdp_record_register(sdp_session_t *session, sdp_record_t *rec, uint8_t flags)
{
	return sdp_device_record_register(session, BDADDR_ANY, rec, flags);
}

/*
 * unregister a service record
 */
int sdp_device_record_unregister_binary(sdp_session_t *session, bdaddr_t *device, uint32_t handle)
{
	uint8_t *reqbuf, *rspbuf, *p;
	uint32_t reqsize = 0, rspsize = 0;
	sdp_pdu_hdr_t *reqhdr, *rsphdr;
	int status;

	SDPDBG("");

	if (handle == SDP_SERVER_RECORD_HANDLE) {
		errno = EINVAL;
		return -1;
	}

	if (!session->local) {
		errno = EREMOTE;
		return -1;
	}

	reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
	rspbuf = malloc(SDP_RSP_BUFFER_SIZE);
	if (!reqbuf || !rspbuf) {
		errno = ENOMEM;
		status = -1;
		goto end;
	}
	reqhdr = (sdp_pdu_hdr_t *) reqbuf;
	reqhdr->pdu_id = SDP_SVC_REMOVE_REQ;
	reqhdr->tid    = htons(sdp_gen_tid(session));

	p = reqbuf + sizeof(sdp_pdu_hdr_t);
	reqsize = sizeof(sdp_pdu_hdr_t);
	bt_put_be32(handle, p);
	reqsize += sizeof(uint32_t);

	reqhdr->plen = htons(reqsize - sizeof(sdp_pdu_hdr_t));
	status = sdp_send_req_w4_rsp(session, reqbuf, rspbuf, reqsize, &rspsize);
	if (status < 0)
		goto end;

	if (rspsize < sizeof(sdp_pdu_hdr_t) + sizeof(uint16_t)) {
		SDPERR("Unexpected end of packet");
		errno = EPROTO;
		status = -1;
		goto end;
	}

	rsphdr = (sdp_pdu_hdr_t *) rspbuf;
	p = rspbuf + sizeof(sdp_pdu_hdr_t);

	if (rsphdr->pdu_id == SDP_ERROR_RSP) {
		/* For this case the status always is invalid record handle */
		errno = EINVAL;
		status = -1;
	} else if (rsphdr->pdu_id != SDP_SVC_REMOVE_RSP) {
		errno = EPROTO;
		status = -1;
	} else {
		uint16_t tmp;

		memcpy(&tmp, p, sizeof(tmp));

		status = tmp;
	}
end:
	free(reqbuf);
	free(rspbuf);

	return status;
}

int sdp_device_record_unregister(sdp_session_t *session, bdaddr_t *device, sdp_record_t *rec)
{
	int err;

	err = sdp_device_record_unregister_binary(session, device, rec->handle);
	if (err == 0)
		sdp_record_free(rec);

	return err;
}

int sdp_record_unregister(sdp_session_t *session, sdp_record_t *rec)
{
	return sdp_device_record_unregister(session, BDADDR_ANY, rec);
}

/*
 * modify an existing service record
 */
int sdp_device_record_update_binary(sdp_session_t *session, bdaddr_t *device, uint32_t handle, uint8_t *data, uint32_t size)
{
	return -1;
}

int sdp_device_record_update(sdp_session_t *session, bdaddr_t *device, const sdp_record_t *rec)
{
	uint8_t *reqbuf, *rspbuf, *p;
	uint32_t reqsize, rspsize;
	sdp_pdu_hdr_t *reqhdr, *rsphdr;
	uint32_t handle;
	sdp_buf_t pdu;
	int status;

	SDPDBG("");

	handle = rec->handle;

	if (handle == SDP_SERVER_RECORD_HANDLE) {
		errno = EINVAL;
		return -1;
	}
	if (!session->local) {
		errno = EREMOTE;
		return -1;
	}
	reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
	rspbuf = malloc(SDP_RSP_BUFFER_SIZE);
	if (!reqbuf || !rspbuf) {
		errno = ENOMEM;
		status = -1;
		goto end;
	}
	reqhdr = (sdp_pdu_hdr_t *) reqbuf;
	reqhdr->pdu_id = SDP_SVC_UPDATE_REQ;
	reqhdr->tid    = htons(sdp_gen_tid(session));

	p = reqbuf + sizeof(sdp_pdu_hdr_t);
	reqsize = sizeof(sdp_pdu_hdr_t);

	bt_put_be32(handle, p);
	reqsize += sizeof(uint32_t);
	p += sizeof(uint32_t);

	if (sdp_gen_record_pdu(rec, &pdu) < 0) {
		errno = ENOMEM;
		status = -1;
		goto end;
	}
	memcpy(p, pdu.data, pdu.data_size);
	reqsize += pdu.data_size;
	free(pdu.data);

	reqhdr->plen = htons(reqsize - sizeof(sdp_pdu_hdr_t));
	status = sdp_send_req_w4_rsp(session, reqbuf, rspbuf, reqsize, &rspsize);
	if (status < 0)
		goto end;

	if (rspsize < sizeof(sdp_pdu_hdr_t) + sizeof(uint16_t)) {
		SDPERR("Unexpected end of packet");
		errno = EPROTO;
		status = -1;
		goto end;
	}

	SDPDBG("Send req status : %d", status);

	rsphdr = (sdp_pdu_hdr_t *) rspbuf;
	p = rspbuf + sizeof(sdp_pdu_hdr_t);

	if (rsphdr->pdu_id == SDP_ERROR_RSP) {
		/* The status can be invalid sintax or invalid record handle */
		errno = EINVAL;
		status = -1;
	} else if (rsphdr->pdu_id != SDP_SVC_UPDATE_RSP) {
		errno = EPROTO;
		status = -1;
	} else {
		uint16_t tmp;

		memcpy(&tmp, p, sizeof(tmp));

		status = tmp;
	}
end:
	free(reqbuf);
	free(rspbuf);
	return status;
}

int sdp_record_update(sdp_session_t *session, const sdp_record_t *rec)
{
	return sdp_device_record_update(session, BDADDR_ANY, rec);
}

sdp_record_t *sdp_record_alloc(void)
{
	sdp_record_t *rec = malloc(sizeof(sdp_record_t));

	if (!rec)
		return NULL;

	memset(rec, 0, sizeof(sdp_record_t));
	rec->handle = 0xffffffff;
	return rec;
}

/*
 * Free the contents of a service record
 */
void sdp_record_free(sdp_record_t *rec)
{
	sdp_list_free(rec->attrlist, (sdp_free_func_t) sdp_data_free);
	sdp_list_free(rec->pattern, free);
	free(rec);
}

void sdp_pattern_add_uuid(sdp_record_t *rec, uuid_t *uuid)
{
	uuid_t *uuid128 = sdp_uuid_to_uuid128(uuid);

	SDPDBG("Elements in target pattern : %d", sdp_list_len(rec->pattern));
	SDPDBG("Trying to add : 0x%lx", (unsigned long) uuid128);

	if (sdp_list_find(rec->pattern, uuid128, sdp_uuid128_cmp) == NULL)
		rec->pattern = sdp_list_insert_sorted(rec->pattern, uuid128, sdp_uuid128_cmp);
	else
		bt_free(uuid128);

	SDPDBG("Elements in target pattern : %d", sdp_list_len(rec->pattern));
}

void sdp_pattern_add_uuidseq(sdp_record_t *rec, sdp_list_t *seq)
{
	for (; seq; seq = seq->next) {
		uuid_t *uuid = (uuid_t *)seq->data;
		sdp_pattern_add_uuid(rec, uuid);
	}
}

/*
 * Extract a sequence of service record handles from a PDU buffer
 * and add the entries to a sdp_list_t. Note that the service record
 * handles are not in "data element sequence" form, but just like
 * an array of service handles
 */
static void extract_record_handle_seq(uint8_t *pdu, int bufsize, sdp_list_t **seq, int count, unsigned int *scanned)
{
	sdp_list_t *pSeq = *seq;
	uint8_t *pdata = pdu;
	int n;

	for (n = 0; n < count; n++) {
		uint32_t *pSvcRec;
		if (bufsize < (int) sizeof(uint32_t)) {
			SDPERR("Unexpected end of packet");
			break;
		}
		pSvcRec = malloc(sizeof(uint32_t));
		if (!pSvcRec)
			break;
		*pSvcRec = bt_get_be32(pdata);
		pSeq = sdp_list_append(pSeq, pSvcRec);
		pdata += sizeof(uint32_t);
		*scanned += sizeof(uint32_t);
		bufsize -= sizeof(uint32_t);
	}
	*seq = pSeq;
}
/*
 * Generate the attribute sequence pdu form
 * from sdp_list_t elements. Return length of attr seq
 */
static int gen_dataseq_pdu(uint8_t *dst, const sdp_list_t *seq, uint8_t dtd)
{
	sdp_data_t *dataseq;
	void **types, **values;
	sdp_buf_t buf;
	int i, seqlen = sdp_list_len(seq);

	/* Fill up the value and the dtd arrays */
	SDPDBG("");

	SDPDBG("Seq length : %d", seqlen);

	types = malloc(seqlen * sizeof(void *));
	if (!types)
		return -ENOMEM;

	values = malloc(seqlen * sizeof(void *));
	if (!values) {
		free(types);
		return -ENOMEM;
	}

	for (i = 0; i < seqlen; i++) {
		void *data = seq->data;
		types[i] = &dtd;
		if (SDP_IS_UUID(dtd))
			data = &((uuid_t *)data)->value;
		values[i] = data;
		seq = seq->next;
	}

	dataseq = sdp_seq_alloc(types, values, seqlen);
	if (!dataseq) {
		free(types);
		free(values);
		return -ENOMEM;
	}

	memset(&buf, 0, sizeof(sdp_buf_t));
	sdp_gen_buffer(&buf, dataseq);
	buf.data = malloc(buf.buf_size);

	if (!buf.data) {
		sdp_data_free(dataseq);
		free(types);
		free(values);
		return -ENOMEM;
	}

	SDPDBG("Data Seq : 0x%p", seq);
	seqlen = sdp_gen_pdu(&buf, dataseq);
	SDPDBG("Copying : %d", buf.data_size);
	memcpy(dst, buf.data, buf.data_size);

	sdp_data_free(dataseq);

	free(types);
	free(values);
	free(buf.data);
	return seqlen;
}

static int gen_searchseq_pdu(uint8_t *dst, const sdp_list_t *seq)
{
	uuid_t *uuid = seq->data;
	return gen_dataseq_pdu(dst, seq, uuid->type);
}

static int gen_attridseq_pdu(uint8_t *dst, const sdp_list_t *seq, uint8_t dataType)
{
	return gen_dataseq_pdu(dst, seq, dataType);
}

typedef struct {
	uint8_t length;
	unsigned char data[16];
} __attribute__ ((packed)) sdp_cstate_t;

static int copy_cstate(uint8_t *pdata, int pdata_len, const sdp_cstate_t *cstate)
{
	if (cstate) {
		uint8_t len = cstate->length;
		if (len >= pdata_len) {
			SDPERR("Continuation state size exceeds internal buffer");
			len = pdata_len - 1;
		}
		*pdata++ = len;
		memcpy(pdata, cstate->data, len);
		return len + 1;
	}
	*pdata = 0;
	return 1;
}

/*
 * This is a service search request.
 *
 * INPUT :
 *
 *   sdp_list_t *search
 *     Singly linked list containing elements of the search
 *     pattern. Each entry in the list is a UUID (DataTypeSDP_UUID16)
 *     of the service to be searched
 *
 *   uint16_t max_rec_num
 *      A 16 bit integer which tells the service, the maximum
 *      entries that the client can handle in the response. The
 *      server is obliged not to return > max_rec_num entries
 *
 * OUTPUT :
 *
 *   int return value
 *     0:
 *       The request completed successfully. This does not
 *       mean the requested services were found
 *     -1:
 *       On any failure and sets errno
 *
 *   sdp_list_t **rsp_list
 *     This variable is set on a successful return if there are
 *     non-zero service handles. It is a singly linked list of
 *     service record handles (uint16_t)
 */
int sdp_service_search_req(sdp_session_t *session, const sdp_list_t *search,
			uint16_t max_rec_num, sdp_list_t **rsp)
{
	int status = 0;
	uint32_t reqsize = 0, _reqsize;
	uint32_t rspsize = 0, rsplen;
	int seqlen = 0;
	int rec_count;
	unsigned scanned, pdata_len;
	uint8_t *pdata, *_pdata;
	uint8_t *reqbuf, *rspbuf;
	sdp_pdu_hdr_t *reqhdr, *rsphdr;
	sdp_cstate_t *cstate = NULL;

	reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
	rspbuf = malloc(SDP_RSP_BUFFER_SIZE);
	if (!reqbuf || !rspbuf) {
		errno = ENOMEM;
		status = -1;
		goto end;
	}
	reqhdr = (sdp_pdu_hdr_t *) reqbuf;
	reqhdr->pdu_id = SDP_SVC_SEARCH_REQ;
	pdata = reqbuf + sizeof(sdp_pdu_hdr_t);
	reqsize = sizeof(sdp_pdu_hdr_t);

	/* add service class IDs for search */
	seqlen = gen_searchseq_pdu(pdata, search);

	SDPDBG("Data seq added : %d", seqlen);

	/* set the length and increment the pointer */
	reqsize += seqlen;
	pdata += seqlen;

	/* specify the maximum svc rec count that client expects */
	bt_put_be16(max_rec_num, pdata);
	reqsize += sizeof(uint16_t);
	pdata += sizeof(uint16_t);

	_reqsize = reqsize;
	_pdata   = pdata;
	*rsp = NULL;

	do {
		/* Add continuation state or NULL (first time) */
		reqsize = _reqsize + copy_cstate(_pdata,
					SDP_REQ_BUFFER_SIZE - _reqsize, cstate);

		/* Set the request header's param length */
		reqhdr->plen = htons(reqsize - sizeof(sdp_pdu_hdr_t));

		reqhdr->tid  = htons(sdp_gen_tid(session));
		/*
		 * Send the request, wait for response and if
		 * no error, set the appropriate values and return
		 */
		status = sdp_send_req_w4_rsp(session, reqbuf, rspbuf, reqsize, &rspsize);
		if (status < 0)
			goto end;

		if (rspsize < sizeof(sdp_pdu_hdr_t)) {
			SDPERR("Unexpected end of packet");
			status = -1;
			goto end;
		}

		rsphdr = (sdp_pdu_hdr_t *) rspbuf;
		rsplen = ntohs(rsphdr->plen);

		if (rsphdr->pdu_id == SDP_ERROR_RSP) {
			SDPDBG("Status : 0x%x", rsphdr->pdu_id);
			status = -1;
			goto end;
		}
		scanned = 0;
		pdata = rspbuf + sizeof(sdp_pdu_hdr_t);
		pdata_len = rspsize - sizeof(sdp_pdu_hdr_t);

		if (pdata_len < sizeof(uint16_t) + sizeof(uint16_t)) {
			SDPERR("Unexpected end of packet");
			status = -1;
			goto end;
		}

		/* net service record match count */
		pdata += sizeof(uint16_t);
		scanned += sizeof(uint16_t);
		pdata_len -= sizeof(uint16_t);
		rec_count = bt_get_be16(pdata);
		pdata += sizeof(uint16_t);
		scanned += sizeof(uint16_t);
		pdata_len -= sizeof(uint16_t);

		SDPDBG("Current svc count: %d", rec_count);
		SDPDBG("ResponseLength: %d", rsplen);

		if (!rec_count) {
			status = -1;
			goto end;
		}
		extract_record_handle_seq(pdata, pdata_len, rsp, rec_count, &scanned);
		SDPDBG("BytesScanned : %d", scanned);

		if (rsplen > scanned) {
			uint8_t cstate_len;

			if (rspsize < sizeof(sdp_pdu_hdr_t) + scanned + sizeof(uint8_t)) {
				SDPERR("Unexpected end of packet: continuation state data missing");
				status = -1;
				goto end;
			}

			pdata = rspbuf + sizeof(sdp_pdu_hdr_t) + scanned;
			cstate_len = *(uint8_t *) pdata;
			if (cstate_len > 0) {
				cstate = (sdp_cstate_t *)pdata;
				SDPDBG("Cont state length: %d", cstate_len);
			} else
				cstate = NULL;
		}
	} while (cstate);

end:
	free(reqbuf);
	free(rspbuf);

	return status;
}

/*
 * This is a service attribute request.
 *
 * INPUT :
 *
 *   uint32_t handle
 *     The handle of the service for which the attribute(s) are
 *     requested
 *
 *   sdp_attrreq_type_t reqtype
 *     Attribute identifiers are 16 bit unsigned integers specified
 *     in one of 2 ways described below :
 *     SDP_ATTR_REQ_INDIVIDUAL - 16bit individual identifiers
 *        They are the actual attribute identifiers in ascending order
 *
 *     SDP_ATTR_REQ_RANGE - 32bit identifier range
 *        The high-order 16bits is the start of range
 *        the low-order 16bits are the end of range
 *        0x0000 to 0xFFFF gets all attributes
 *
 *   sdp_list_t *attrid
 *     Singly linked list containing attribute identifiers desired.
 *     Every element is either a uint16_t(attrSpec = SDP_ATTR_REQ_INDIVIDUAL)
 *     or a uint32_t(attrSpec=SDP_ATTR_REQ_RANGE)
 *
 * OUTPUT :
 *   return sdp_record_t *
 *     0:
 *       On any error and sets errno
 *     !0:
 *	 The service record
 */
sdp_record_t *sdp_service_attr_req(sdp_session_t *session, uint32_t handle,
			sdp_attrreq_type_t reqtype, const sdp_list_t *attrids)
{
	uint32_t reqsize = 0, _reqsize;
	uint32_t rspsize = 0, rsp_count;
	int attr_list_len = 0;
	int seqlen = 0;
	unsigned int pdata_len;
	uint8_t *pdata, *_pdata;
	uint8_t *reqbuf, *rspbuf;
	sdp_pdu_hdr_t *reqhdr, *rsphdr;
	sdp_cstate_t *cstate = NULL;
	uint8_t cstate_len = 0;
	sdp_buf_t rsp_concat_buf;
	sdp_record_t *rec = 0;

	if (reqtype != SDP_ATTR_REQ_INDIVIDUAL && reqtype != SDP_ATTR_REQ_RANGE) {
		errno = EINVAL;
		return NULL;
	}

	memset(&rsp_concat_buf, 0, sizeof(sdp_buf_t));

	reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
	rspbuf = malloc(SDP_RSP_BUFFER_SIZE);
	if (!reqbuf || !rspbuf) {
		errno = ENOMEM;
		goto end;
	}
	reqhdr = (sdp_pdu_hdr_t *) reqbuf;
	reqhdr->pdu_id = SDP_SVC_ATTR_REQ;

	pdata = reqbuf + sizeof(sdp_pdu_hdr_t);
	reqsize = sizeof(sdp_pdu_hdr_t);

	/* add the service record handle */
	bt_put_be32(handle, pdata);
	reqsize += sizeof(uint32_t);
	pdata += sizeof(uint32_t);

	/* specify the response limit */
	bt_put_be16(65535, pdata);
	reqsize += sizeof(uint16_t);
	pdata += sizeof(uint16_t);

	/* get attr seq PDU form */
	seqlen = gen_attridseq_pdu(pdata, attrids,
		reqtype == SDP_ATTR_REQ_INDIVIDUAL? SDP_UINT16 : SDP_UINT32);
	if (seqlen == -1) {
		errno = EINVAL;
		goto end;
	}
	pdata += seqlen;
	reqsize += seqlen;
	SDPDBG("Attr list length : %d", seqlen);

	/* save before Continuation State */
	_pdata = pdata;
	_reqsize = reqsize;

	do {
		int status;

		/* add NULL continuation state */
		reqsize = _reqsize + copy_cstate(_pdata,
					SDP_REQ_BUFFER_SIZE - _reqsize, cstate);

		/* set the request header's param length */
		reqhdr->tid  = htons(sdp_gen_tid(session));
		reqhdr->plen = htons(reqsize - sizeof(sdp_pdu_hdr_t));

		status = sdp_send_req_w4_rsp(session, reqbuf, rspbuf, reqsize, &rspsize);
		if (status < 0)
			goto end;

		if (rspsize < sizeof(sdp_pdu_hdr_t)) {
			SDPERR("Unexpected end of packet");
			goto end;
		}

		rsphdr = (sdp_pdu_hdr_t *) rspbuf;
		if (rsphdr->pdu_id == SDP_ERROR_RSP) {
			SDPDBG("PDU ID : 0x%x", rsphdr->pdu_id);
			goto end;
		}
		pdata = rspbuf + sizeof(sdp_pdu_hdr_t);
		pdata_len = rspsize - sizeof(sdp_pdu_hdr_t);

		if (pdata_len < sizeof(uint16_t)) {
			SDPERR("Unexpected end of packet");
			goto end;
		}

		rsp_count = bt_get_be16(pdata);
		attr_list_len += rsp_count;
		pdata += sizeof(uint16_t);
		pdata_len -= sizeof(uint16_t);

		/*
		 * if continuation state set need to re-issue request before
		 * parsing
		 */
		if (pdata_len < rsp_count + sizeof(uint8_t)) {
			SDPERR("Unexpected end of packet: continuation state data missing");
			goto end;
		}
		cstate_len = *(uint8_t *) (pdata + rsp_count);

		SDPDBG("Response id : %d", rsphdr->pdu_id);
		SDPDBG("Attrlist byte count : %d", rsp_count);
		SDPDBG("sdp_cstate_t length : %d", cstate_len);

		/*
		 * a split response: concatenate intermediate responses
		 * and the last one (which has cstate_len == 0)
		 */
		if (cstate_len > 0 || rsp_concat_buf.data_size != 0) {
			uint8_t *targetPtr = NULL;

			cstate = cstate_len > 0 ? (sdp_cstate_t *) (pdata + rsp_count) : 0;

			/* build concatenated response buffer */
			rsp_concat_buf.data = realloc(rsp_concat_buf.data, rsp_concat_buf.data_size + rsp_count);
			rsp_concat_buf.buf_size = rsp_concat_buf.data_size + rsp_count;
			targetPtr = rsp_concat_buf.data + rsp_concat_buf.data_size;
			memcpy(targetPtr, pdata, rsp_count);
			rsp_concat_buf.data_size += rsp_count;
		}
	} while (cstate);

	if (attr_list_len > 0) {
		int scanned = 0;
		if (rsp_concat_buf.data_size != 0) {
			pdata = rsp_concat_buf.data;
			pdata_len = rsp_concat_buf.data_size;
		}
		rec = sdp_extract_pdu(pdata, pdata_len, &scanned);
	}

end:
	free(reqbuf);
	free(rsp_concat_buf.data);
	free(rspbuf);
	return rec;
}

/*
 * SDP transaction structure for asynchronous search
 */
struct sdp_transaction {
	sdp_callback_t *cb;	/* called when the transaction finishes */
	void *udata;		/* client user data */
	uint8_t *reqbuf;	/* pointer to request PDU */
	sdp_buf_t rsp_concat_buf;
	uint32_t reqsize;	/* without cstate */
	int err;		/* ZERO if success or the errno if failed */
};

/*
 * Creates a new sdp session for asynchronous search
 * INPUT:
 *  int sk
 *     non-blocking L2CAP socket
 *
 * RETURN:
 *  sdp_session_t *
 *  NULL - On memory allocation failure
 */
sdp_session_t *sdp_create(int sk, uint32_t flags)
{
	sdp_session_t *session;
	struct sdp_transaction *t;

	session = malloc(sizeof(sdp_session_t));
	if (!session) {
		errno = ENOMEM;
		return NULL;
	}
	memset(session, 0, sizeof(*session));

	session->flags = flags;
	session->sock = sk;

	t = malloc(sizeof(struct sdp_transaction));
	if (!t) {
		errno = ENOMEM;
		free(session);
		return NULL;
	}
	memset(t, 0, sizeof(*t));

	session->priv = t;

	return session;
}

/*
 * Sets the callback function/user data used to notify the application
 * that the asynchronous transaction finished. This function must be
 * called before request an asynchronous search.
 *
 * INPUT:
 *  sdp_session_t *session
 *	Current sdp session to be handled
 *  sdp_callback_t *cb
 *      callback to be called when the transaction finishes
 *  void *udata
 *      user data passed to callback
 * RETURN:
 * 	 0 - Success
 * 	-1 - Failure
 */
int sdp_set_notify(sdp_session_t *session, sdp_callback_t *func, void *udata)
{
	struct sdp_transaction *t;

	if (!session || !session->priv)
		return -1;

	t = session->priv;
	t->cb = func;
	t->udata = udata;

	return 0;
}

/*
 * This function starts an asynchronous service search request.
 * The incoming and outgoing data are stored in the transaction structure
 * buffers. When there is incoming data the sdp_process function must be
 * called to get the data and handle the continuation state.
 *
 * INPUT :
 *  sdp_session_t *session
 *     Current sdp session to be handled
 *
 *   sdp_list_t *search
 *     Singly linked list containing elements of the search
 *     pattern. Each entry in the list is a UUID (DataTypeSDP_UUID16)
 *     of the service to be searched
 *
 *   uint16_t max_rec_num
 *      A 16 bit integer which tells the service, the maximum
 *      entries that the client can handle in the response. The
 *      server is obliged not to return > max_rec_num entries
 *
 * OUTPUT :
 *
 *   int return value
 * 	0  - if the request has been sent properly
 * 	-1 - On any failure and sets errno
 */

int sdp_service_search_async(sdp_session_t *session, const sdp_list_t *search, uint16_t max_rec_num)
{
	struct sdp_transaction *t;
	sdp_pdu_hdr_t *reqhdr;
	uint8_t *pdata;
	int cstate_len, seqlen = 0;

	if (!session || !session->priv)
		return -1;

	t = session->priv;

	/* clean possible allocated buffer */
	free(t->rsp_concat_buf.data);
	memset(&t->rsp_concat_buf, 0, sizeof(sdp_buf_t));

	if (!t->reqbuf) {
		t->reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
		if (!t->reqbuf) {
			t->err = ENOMEM;
			goto end;
		}
	}
	memset(t->reqbuf, 0, SDP_REQ_BUFFER_SIZE);

	reqhdr = (sdp_pdu_hdr_t *) t->reqbuf;
	reqhdr->tid = htons(sdp_gen_tid(session));
	reqhdr->pdu_id = SDP_SVC_SEARCH_REQ;

	/* generate PDU */
	pdata = t->reqbuf + sizeof(sdp_pdu_hdr_t);
	t->reqsize = sizeof(sdp_pdu_hdr_t);

	/* add service class IDs for search */
	seqlen = gen_searchseq_pdu(pdata, search);

	SDPDBG("Data seq added : %d", seqlen);

	/* now set the length and increment the pointer */
	t->reqsize += seqlen;
	pdata += seqlen;

	bt_put_be16(max_rec_num, pdata);
	t->reqsize += sizeof(uint16_t);
	pdata += sizeof(uint16_t);

	/* set the request header's param length */
	cstate_len = copy_cstate(pdata, SDP_REQ_BUFFER_SIZE - t->reqsize, NULL);
	reqhdr->plen = htons((t->reqsize + cstate_len) - sizeof(sdp_pdu_hdr_t));

	if (sdp_send_req(session, t->reqbuf, t->reqsize + cstate_len) < 0) {
		SDPERR("Error sendind data:%m");
		t->err = errno;
		goto end;
	}

	return 0;
end:

	free(t->reqbuf);
	t->reqbuf = NULL;

	return -1;
}

/*
 * This function starts an asynchronous service attribute request.
 * The incoming and outgoing data are stored in the transaction structure
 * buffers. When there is incoming data the sdp_process function must be
 * called to get the data and handle the continuation state.
 *
 * INPUT :
 *  sdp_session_t *session
 *	Current sdp session to be handled
 *
 *   uint32_t handle
 *     The handle of the service for which the attribute(s) are
 *     requested
 *
 *   sdp_attrreq_type_t reqtype
 *     Attribute identifiers are 16 bit unsigned integers specified
 *     in one of 2 ways described below :
 *     SDP_ATTR_REQ_INDIVIDUAL - 16bit individual identifiers
 *        They are the actual attribute identifiers in ascending order
 *
 *     SDP_ATTR_REQ_RANGE - 32bit identifier range
 *        The high-order 16bits is the start of range
 *        the low-order 16bits are the end of range
 *        0x0000 to 0xFFFF gets all attributes
 *
 *   sdp_list_t *attrid_list
 *     Singly linked list containing attribute identifiers desired.
 *     Every element is either a uint16_t(attrSpec = SDP_ATTR_REQ_INDIVIDUAL)
 *     or a uint32_t(attrSpec=SDP_ATTR_REQ_RANGE)
 *
 * OUTPUT :
 *   int return value
 * 	 0 - if the request has been sent properly
 * 	-1 - On any failure and sets errno
 */

int sdp_service_attr_async(sdp_session_t *session, uint32_t handle, sdp_attrreq_type_t reqtype, const sdp_list_t *attrid_list)
{
	struct sdp_transaction *t;
	sdp_pdu_hdr_t *reqhdr;
	uint8_t *pdata;
	int cstate_len, seqlen = 0;

	if (!session || !session->priv)
		return -1;

	t = session->priv;

	/* clean possible allocated buffer */
	free(t->rsp_concat_buf.data);
	memset(&t->rsp_concat_buf, 0, sizeof(sdp_buf_t));

	if (!t->reqbuf) {
		t->reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
		if (!t->reqbuf) {
			t->err = ENOMEM;
			goto end;
		}
	}
	memset(t->reqbuf, 0, SDP_REQ_BUFFER_SIZE);

	reqhdr = (sdp_pdu_hdr_t *) t->reqbuf;
	reqhdr->tid = htons(sdp_gen_tid(session));
	reqhdr->pdu_id = SDP_SVC_ATTR_REQ;

	/* generate PDU */
	pdata = t->reqbuf + sizeof(sdp_pdu_hdr_t);
	t->reqsize = sizeof(sdp_pdu_hdr_t);

	/* add the service record handle */
	bt_put_be32(handle, pdata);
	t->reqsize += sizeof(uint32_t);
	pdata += sizeof(uint32_t);

	/* specify the response limit */
	bt_put_be16(65535, pdata);
	t->reqsize += sizeof(uint16_t);
	pdata += sizeof(uint16_t);

	/* get attr seq PDU form */
	seqlen = gen_attridseq_pdu(pdata, attrid_list,
			reqtype == SDP_ATTR_REQ_INDIVIDUAL? SDP_UINT16 : SDP_UINT32);
	if (seqlen == -1) {
		t->err = EINVAL;
		goto end;
	}

	/* now set the length and increment the pointer */
	t->reqsize += seqlen;
	pdata += seqlen;
	SDPDBG("Attr list length : %d", seqlen);

	/* set the request header's param length */
	cstate_len = copy_cstate(pdata, SDP_REQ_BUFFER_SIZE - t->reqsize, NULL);
	reqhdr->plen = htons((t->reqsize + cstate_len) - sizeof(sdp_pdu_hdr_t));

	if (sdp_send_req(session, t->reqbuf, t->reqsize + cstate_len) < 0) {
		SDPERR("Error sendind data:%m");
		t->err = errno;
		goto end;
	}

	return 0;
end:

	free(t->reqbuf);
	t->reqbuf = NULL;

	return -1;
}

/*
 * This function starts an asynchronous service search attributes.
 * It is a service search request combined with attribute request. The incoming
 * and outgoing data are stored in the transaction structure buffers. When there
 * is incoming data the sdp_process function must be called to get the data
 * and handle the continuation state.
 *
 * INPUT:
 *  sdp_session_t *session
 *	Current sdp session to be handled
 *
 *   sdp_list_t *search
 *     Singly linked list containing elements of the search
 *     pattern. Each entry in the list is a UUID(DataTypeSDP_UUID16)
 *     of the service to be searched
 *
 *   AttributeSpecification attrSpec
 *     Attribute identifiers are 16 bit unsigned integers specified
 *     in one of 2 ways described below :
 *     SDP_ATTR_REQ_INDIVIDUAL - 16bit individual identifiers
 *        They are the actual attribute identifiers in ascending order
 *
 *     SDP_ATTR_REQ_RANGE - 32bit identifier range
 *        The high-order 16bits is the start of range
 *        the low-order 16bits are the end of range
 *        0x0000 to 0xFFFF gets all attributes
 *
 *   sdp_list_t *attrid_list
 *     Singly linked list containing attribute identifiers desired.
 *     Every element is either a uint16_t(attrSpec = SDP_ATTR_REQ_INDIVIDUAL)
 *     or a uint32_t(attrSpec=SDP_ATTR_REQ_RANGE)
 *

 * RETURN:
 * 	 0 - if the request has been sent properly
 * 	-1 - On any failure
 */
int sdp_service_search_attr_async(sdp_session_t *session, const sdp_list_t *search, sdp_attrreq_type_t reqtype, const sdp_list_t *attrid_list)
{
	struct sdp_transaction *t;
	sdp_pdu_hdr_t *reqhdr;
	uint8_t *pdata;
	int cstate_len, seqlen = 0;

	if (!session || !session->priv)
		return -1;

	t = session->priv;

	/* clean possible allocated buffer */
	free(t->rsp_concat_buf.data);
	memset(&t->rsp_concat_buf, 0, sizeof(sdp_buf_t));

	if (!t->reqbuf) {
		t->reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
		if (!t->reqbuf) {
			t->err = ENOMEM;
			goto end;
		}
	}
	memset(t->reqbuf, 0, SDP_REQ_BUFFER_SIZE);

	reqhdr = (sdp_pdu_hdr_t *) t->reqbuf;
	reqhdr->tid = htons(sdp_gen_tid(session));
	reqhdr->pdu_id = SDP_SVC_SEARCH_ATTR_REQ;

	/* generate PDU */
	pdata = t->reqbuf + sizeof(sdp_pdu_hdr_t);
	t->reqsize = sizeof(sdp_pdu_hdr_t);

	/* add service class IDs for search */
	seqlen = gen_searchseq_pdu(pdata, search);

	SDPDBG("Data seq added : %d", seqlen);

	/* now set the length and increment the pointer */
	t->reqsize += seqlen;
	pdata += seqlen;

	bt_put_be16(SDP_MAX_ATTR_LEN, pdata);
	t->reqsize += sizeof(uint16_t);
	pdata += sizeof(uint16_t);

	SDPDBG("Max attr byte count : %d", SDP_MAX_ATTR_LEN);

	/* get attr seq PDU form */
	seqlen = gen_attridseq_pdu(pdata, attrid_list,
			reqtype == SDP_ATTR_REQ_INDIVIDUAL ? SDP_UINT16 : SDP_UINT32);
	if (seqlen == -1) {
		t->err = EINVAL;
		goto end;
	}

	pdata += seqlen;
	SDPDBG("Attr list length : %d", seqlen);
	t->reqsize += seqlen;

	/* set the request header's param length */
	cstate_len = copy_cstate(pdata, SDP_REQ_BUFFER_SIZE - t->reqsize, NULL);
	reqhdr->plen = htons((t->reqsize + cstate_len) - sizeof(sdp_pdu_hdr_t));

	if (sdp_send_req(session, t->reqbuf, t->reqsize + cstate_len) < 0) {
		SDPERR("Error sendind data:%m");
		t->err = errno;
		goto end;
	}

	return 0;
end:

	free(t->reqbuf);
	t->reqbuf = NULL;

	return -1;
}

/*
 * Function used to get the error reason after sdp_callback_t function has been called
 * and the status is 0xffff or if sdp_service_{search, attr, search_attr}_async returns -1.
 * It indicates that an error NOT related to SDP_ErrorResponse happened. Get errno directly
 * is not safe because multiple transactions can be triggered.
 * This function must be used with asynchronous sdp functions only.
 *
 * INPUT:
 *  sdp_session_t *session
 *	Current sdp session to be handled
 * RETURN:
 * 	 0 = No error in the current transaction
 * 	-1 - if the session is invalid
 * 	positive value - the errno value
 *
 */
int sdp_get_error(sdp_session_t *session)
{
	struct sdp_transaction *t;

	if (!session || !session->priv) {
		SDPERR("Invalid session");
		return -1;
	}

	t = session->priv;

	return t->err;
}

/*
 * Receive the incoming SDP PDU. This function must be called when there is data
 * available to be read. On continuation state, the original request (with a new
 * transaction ID) and the continuation state data will be appended in the initial PDU.
 * If an error happens or the transaction finishes the callback function will be called.
 *
 * INPUT:
 *  sdp_session_t *session
 *	Current sdp session to be handled
 * RETURN:
 * 	0  - if the transaction is on continuation state
 * 	-1 - On any failure or the transaction finished
 */
int sdp_process(sdp_session_t *session)
{
	struct sdp_transaction *t;
	sdp_pdu_hdr_t *reqhdr, *rsphdr;
	sdp_cstate_t *pcstate;
	uint8_t *pdata, *rspbuf, *targetPtr;
	int rsp_count, err = -1;
	size_t size = 0;
	int n, plen;
	uint16_t status = 0xffff;
	uint8_t pdu_id = 0x00;

	if (!session || !session->priv) {
		SDPERR("Invalid session");
		return -1;
	}

	rspbuf = malloc(SDP_RSP_BUFFER_SIZE);
	if (!rspbuf) {
		SDPERR("Response buffer alloc failure:%m (%d)", errno);
		return -1;
	}

	memset(rspbuf, 0, SDP_RSP_BUFFER_SIZE);

	t = session->priv;
	reqhdr = (sdp_pdu_hdr_t *)t->reqbuf;
	rsphdr = (sdp_pdu_hdr_t *)rspbuf;

	pdata = rspbuf + sizeof(sdp_pdu_hdr_t);

	n = sdp_read_rsp(session, rspbuf, SDP_RSP_BUFFER_SIZE);
	if (n < 0) {
		SDPERR("Read response:%m (%d)", errno);
		t->err = errno;
		goto end;
	}

	if (n == 0 || reqhdr->tid != rsphdr->tid ||
		(n != (int) (ntohs(rsphdr->plen) + sizeof(sdp_pdu_hdr_t)))) {
		t->err = EPROTO;
		SDPERR("Protocol error.");
		goto end;
	}

	pdu_id = rsphdr->pdu_id;
	switch (rsphdr->pdu_id) {
	uint8_t *ssr_pdata;
	uint16_t tsrc, csrc;
	case SDP_SVC_SEARCH_RSP:
		/*
		 * TSRC: Total Service Record Count (2 bytes)
		 * CSRC: Current Service Record Count (2 bytes)
		 */
		ssr_pdata = pdata;
		tsrc = bt_get_be16(ssr_pdata);
		ssr_pdata += sizeof(uint16_t);
		csrc = bt_get_be16(ssr_pdata);

		/* csrc should never be larger than tsrc */
		if (csrc > tsrc) {
			t->err = EPROTO;
			SDPERR("Protocol error: wrong current service record count value.");
			goto end;
		}

		SDPDBG("Total svc count: %d", tsrc);
		SDPDBG("Current svc count: %d", csrc);

		/* parameter length without continuation state */
		plen = sizeof(tsrc) + sizeof(csrc) + csrc * 4;

		if (t->rsp_concat_buf.data_size == 0) {
			/* first fragment */
			rsp_count = sizeof(tsrc) + sizeof(csrc) + csrc * 4;
		} else if (t->rsp_concat_buf.data_size >= sizeof(uint16_t) * 2) {
			/* point to the first csrc */
			uint8_t *pcsrc = t->rsp_concat_buf.data + 2;
			uint16_t tcsrc, tcsrc2;

			/* FIXME: update the interface later. csrc doesn't need be passed to clients */

			pdata += sizeof(uint16_t); /* point to csrc */

			/* the first csrc contains the sum of partial csrc responses */
			memcpy(&tcsrc, pcsrc, sizeof(tcsrc));
			memcpy(&tcsrc2, pdata, sizeof(tcsrc2));
			tcsrc += tcsrc2;
			memcpy(pcsrc, &tcsrc, sizeof(tcsrc));

			pdata += sizeof(uint16_t); /* point to the first handle */
			rsp_count = csrc * 4;
		} else {
			t->err = EPROTO;
			SDPERR("Protocol error: invalid PDU size");
			status = SDP_INVALID_PDU_SIZE;
			goto end;
		}
		status = 0x0000;
		break;
	case SDP_SVC_ATTR_RSP:
	case SDP_SVC_SEARCH_ATTR_RSP:
		rsp_count = bt_get_be16(pdata);
		SDPDBG("Attrlist byte count : %d", rsp_count);

		/* Valid range for rsp_count is 0x0002-0xFFFF */
		if (t->rsp_concat_buf.data_size == 0 && rsp_count < 0x0002) {
			t->err = EPROTO;
			SDPERR("Protocol error: invalid AttrList size");
			status = SDP_INVALID_PDU_SIZE;
			goto end;
		}

		/*
		 * Number of bytes in the AttributeLists parameter(without
		 * continuation state) + AttributeListsByteCount field size.
		 */
		plen = sizeof(uint16_t) + rsp_count;

		pdata += sizeof(uint16_t); /* points to attribute list */
		status = 0x0000;
		break;
	case SDP_ERROR_RSP:
		status = bt_get_be16(pdata);
		size = ntohs(rsphdr->plen);

		goto end;
	default:
		t->err = EPROTO;
		SDPERR("Illegal PDU ID: 0x%x", rsphdr->pdu_id);
		goto end;
	}

	/* Out of bound check before using rsp_count as offset for
	 * continuation state, which has at least a one byte size
	 * field.
	 */
	if ((n - (int) sizeof(sdp_pdu_hdr_t)) < plen + 1) {
		t->err = EPROTO;
		SDPERR("Protocol error: invalid PDU size");
		status = SDP_INVALID_PDU_SIZE;
		goto end;
	}

	pcstate = (sdp_cstate_t *) (pdata + rsp_count);

	SDPDBG("Cstate length : %d", pcstate->length);

	/*
	 * Check out of bound. Continuation state must have at least
	 * 1 byte: ZERO to indicate that it is not a partial response.
	 */
	if ((n - (int) sizeof(sdp_pdu_hdr_t))  != (plen + pcstate->length + 1)) {
		t->err = EPROTO;
		SDPERR("Protocol error: wrong PDU size.");
		status = 0xffff;
		goto end;
	}

	/*
	 * This is a split response, need to concatenate intermediate
	 * responses and the last one which will have cstate length == 0
	 */
	t->rsp_concat_buf.data = realloc(t->rsp_concat_buf.data, t->rsp_concat_buf.data_size + rsp_count);
	targetPtr = t->rsp_concat_buf.data + t->rsp_concat_buf.data_size;
	t->rsp_concat_buf.buf_size = t->rsp_concat_buf.data_size + rsp_count;
	memcpy(targetPtr, pdata, rsp_count);
	t->rsp_concat_buf.data_size += rsp_count;

	if (pcstate->length > 0) {
		int reqsize, cstate_len;

		reqhdr->tid = htons(sdp_gen_tid(session));

		/* add continuation state */
		cstate_len = copy_cstate(t->reqbuf + t->reqsize,
				SDP_REQ_BUFFER_SIZE - t->reqsize, pcstate);

		reqsize = t->reqsize + cstate_len;

		/* set the request header's param length */
		reqhdr->plen = htons(reqsize - sizeof(sdp_pdu_hdr_t));

		if (sdp_send_req(session, t->reqbuf, reqsize) < 0) {
			SDPERR("Error sendind data:%m(%d)", errno);
			status = 0xffff;
			t->err = errno;
			goto end;
		}
		err = 0;
	}

end:
	if (err) {
		if (t->rsp_concat_buf.data_size != 0) {
			pdata = t->rsp_concat_buf.data;
			size = t->rsp_concat_buf.data_size;
		}
		if (t->cb)
			t->cb(pdu_id, status, pdata, size, t->udata);
	}

	free(rspbuf);

	return err;
}

/*
 * This is a service search request combined with the service
 * attribute request. First a service class match is done and
 * for matching service, requested attributes are extracted
 *
 * INPUT :
 *
 *   sdp_list_t *search
 *     Singly linked list containing elements of the search
 *     pattern. Each entry in the list is a UUID(DataTypeSDP_UUID16)
 *     of the service to be searched
 *
 *   AttributeSpecification attrSpec
 *     Attribute identifiers are 16 bit unsigned integers specified
 *     in one of 2 ways described below :
 *     SDP_ATTR_REQ_INDIVIDUAL - 16bit individual identifiers
 *        They are the actual attribute identifiers in ascending order
 *
 *     SDP_ATTR_REQ_RANGE - 32bit identifier range
 *        The high-order 16bits is the start of range
 *        the low-order 16bits are the end of range
 *        0x0000 to 0xFFFF gets all attributes
 *
 *   sdp_list_t *attrids
 *     Singly linked list containing attribute identifiers desired.
 *     Every element is either a uint16_t(attrSpec = SDP_ATTR_REQ_INDIVIDUAL)
 *     or a uint32_t(attrSpec=SDP_ATTR_REQ_RANGE)
 *
 * OUTPUT :
 *   int return value
 *     0:
 *       The request completed successfully. This does not
 *       mean the requested services were found
 *     -1:
 *       On any error and sets errno
 *
 *   sdp_list_t **rsp
 *     This variable is set on a successful return to point to
 *     service(s) found. Each element of this list is of type
 *     sdp_record_t* (of the services which matched the search list)
 */
int sdp_service_search_attr_req(sdp_session_t *session, const sdp_list_t *search, sdp_attrreq_type_t reqtype, const sdp_list_t *attrids, sdp_list_t **rsp)
{
	int status = 0;
	uint32_t reqsize = 0, _reqsize;
	uint32_t rspsize = 0;
	int seqlen = 0, attr_list_len = 0;
	int rsp_count = 0, cstate_len = 0;
	unsigned int pdata_len;
	uint8_t *pdata, *_pdata;
	uint8_t *reqbuf, *rspbuf;
	sdp_pdu_hdr_t *reqhdr, *rsphdr;
	uint8_t dataType;
	sdp_list_t *rec_list = NULL;
	sdp_buf_t rsp_concat_buf;
	sdp_cstate_t *cstate = NULL;

	if (reqtype != SDP_ATTR_REQ_INDIVIDUAL && reqtype != SDP_ATTR_REQ_RANGE) {
		errno = EINVAL;
		return -1;
	}

	memset(&rsp_concat_buf, 0, sizeof(sdp_buf_t));

	reqbuf = malloc(SDP_REQ_BUFFER_SIZE);
	rspbuf = malloc(SDP_RSP_BUFFER_SIZE);
	if (!reqbuf || !rspbuf) {
		errno = ENOMEM;
		status = -1;
		goto end;
	}

	reqhdr = (sdp_pdu_hdr_t *) reqbuf;
	reqhdr->pdu_id = SDP_SVC_SEARCH_ATTR_REQ;

	/* generate PDU */
	pdata = reqbuf + sizeof(sdp_pdu_hdr_t);
	reqsize = sizeof(sdp_pdu_hdr_t);

	/* add service class IDs for search */
	seqlen = gen_searchseq_pdu(pdata, search);
	if (seqlen < 0) {
		errno = EINVAL;
		status = -1;
		goto end;
	}

	SDPDBG("Data seq added : %d", seqlen);

	/* now set the length and increment the pointer */
	reqsize += seqlen;
	pdata += seqlen;

	bt_put_be16(SDP_MAX_ATTR_LEN, pdata);
	reqsize += sizeof(uint16_t);
	pdata += sizeof(uint16_t);

	SDPDBG("Max attr byte count : %d", SDP_MAX_ATTR_LEN);

	/* get attr seq PDU form */
	seqlen = gen_attridseq_pdu(pdata, attrids,
		reqtype == SDP_ATTR_REQ_INDIVIDUAL ? SDP_UINT16 : SDP_UINT32);
	if (seqlen == -1) {
		errno = EINVAL;
		status = -1;
		goto end;
	}
	pdata += seqlen;
	SDPDBG("Attr list length : %d", seqlen);
	reqsize += seqlen;
	*rsp = 0;

	/* save before Continuation State */
	_pdata = pdata;
	_reqsize = reqsize;

	do {
		reqhdr->tid = htons(sdp_gen_tid(session));

		/* add continuation state (can be null) */
		reqsize = _reqsize + copy_cstate(_pdata,
					SDP_REQ_BUFFER_SIZE - _reqsize, cstate);

		/* set the request header's param length */
		reqhdr->plen = htons(reqsize - sizeof(sdp_pdu_hdr_t));
		rsphdr = (sdp_pdu_hdr_t *) rspbuf;
		status = sdp_send_req_w4_rsp(session, reqbuf, rspbuf, reqsize, &rspsize);
		if (rspsize < sizeof(sdp_pdu_hdr_t)) {
			SDPERR("Unexpected end of packet");
			status = -1;
			goto end;
		}

		if (status < 0) {
			SDPDBG("Status : 0x%x", rsphdr->pdu_id);
			goto end;
		}

		if (rsphdr->pdu_id == SDP_ERROR_RSP) {
			status = -1;
			goto end;
		}

		pdata = rspbuf + sizeof(sdp_pdu_hdr_t);
		pdata_len = rspsize - sizeof(sdp_pdu_hdr_t);

		if (pdata_len < sizeof(uint16_t)) {
			SDPERR("Unexpected end of packet");
			status = -1;
			goto end;
		}

		rsp_count = bt_get_be16(pdata);
		attr_list_len += rsp_count;
		pdata += sizeof(uint16_t); /* pdata points to attribute list */
		pdata_len -= sizeof(uint16_t);

		if (pdata_len < rsp_count + sizeof(uint8_t)) {
			SDPERR("Unexpected end of packet: continuation state data missing");
			status = -1;
			goto end;
		}

		cstate_len = *(uint8_t *) (pdata + rsp_count);

		SDPDBG("Attrlist byte count : %d", attr_list_len);
		SDPDBG("Response byte count : %d", rsp_count);
		SDPDBG("Cstate length : %d", cstate_len);
		/*
		 * This is a split response, need to concatenate intermediate
		 * responses and the last one which will have cstate_len == 0
		 */
		if (cstate_len > 0 || rsp_concat_buf.data_size != 0) {
			uint8_t *targetPtr = NULL;

			cstate = cstate_len > 0 ? (sdp_cstate_t *) (pdata + rsp_count) : 0;

			/* build concatenated response buffer */
			rsp_concat_buf.data = realloc(rsp_concat_buf.data, rsp_concat_buf.data_size + rsp_count);
			targetPtr = rsp_concat_buf.data + rsp_concat_buf.data_size;
			rsp_concat_buf.buf_size = rsp_concat_buf.data_size + rsp_count;
			memcpy(targetPtr, pdata, rsp_count);
			rsp_concat_buf.data_size += rsp_count;
		}
	} while (cstate);

	if (attr_list_len > 0) {
		int scanned = 0;

		if (rsp_concat_buf.data_size != 0) {
			pdata = rsp_concat_buf.data;
			pdata_len = rsp_concat_buf.data_size;
		}

		/*
		 * Response is a sequence of sequence(s) for one or
		 * more data element sequence(s) representing services
		 * for which attributes are returned
		 */
		scanned = sdp_extract_seqtype(pdata, pdata_len, &dataType, &seqlen);

		SDPDBG("Bytes scanned : %d", scanned);
		SDPDBG("Seq length : %d", seqlen);

		if (scanned && seqlen) {
			pdata += scanned;
			pdata_len -= scanned;
			do {
				int recsize = 0;
				sdp_record_t *rec = sdp_extract_pdu(pdata, pdata_len, &recsize);
				if (rec == NULL) {
					SDPERR("SVC REC is null");
					status = -1;
					goto end;
				}
				if (!recsize) {
					sdp_record_free(rec);
					break;
				}
				scanned += recsize;
				pdata += recsize;
				pdata_len -= recsize;

				SDPDBG("Loc seq length : %d", recsize);
				SDPDBG("Svc Rec Handle : 0x%x", rec->handle);
				SDPDBG("Bytes scanned : %d", scanned);
				SDPDBG("Attrlist byte count : %d", attr_list_len);
				rec_list = sdp_list_append(rec_list, rec);
			} while (scanned < attr_list_len && pdata_len > 0);

			SDPDBG("Successful scan of service attr lists");
			*rsp = rec_list;
		}
	}
end:
	free(rsp_concat_buf.data);
	free(reqbuf);
	free(rspbuf);
	return status;
}

/*
 * Find devices in the piconet.
 */
int sdp_general_inquiry(inquiry_info *ii, int num_dev, int duration, uint8_t *found)
{
	int n = hci_inquiry(-1, 10, num_dev, NULL, &ii, 0);
	if (n < 0) {
		SDPERR("Inquiry failed:%m");
		return -1;
	}
	*found = n;
	return 0;
}

int sdp_close(sdp_session_t *session)
{
	struct sdp_transaction *t;
	int ret;

	if (!session)
		return -1;

	ret = close(session->sock);

	t = session->priv;

	if (t) {
		free(t->reqbuf);

		free(t->rsp_concat_buf.data);

		free(t);
	}
	free(session);
	return ret;
}

static inline int sdp_is_local(const bdaddr_t *device)
{
	return memcmp(device, BDADDR_LOCAL, sizeof(bdaddr_t)) == 0;
}

static int sdp_connect_local(sdp_session_t *session)
{
	struct sockaddr_un sa;

	session->sock = socket(PF_UNIX, SOCK_STREAM | O_CLOEXEC, 0);
	if (session->sock < 0)
		return -1;
	session->local = 1;

	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, SDP_UNIX_PATH);

	return connect(session->sock, (struct sockaddr *) &sa, sizeof(sa));
}

static int sdp_connect_l2cap(const bdaddr_t *src,
		const bdaddr_t *dst, sdp_session_t *session)
{
	uint32_t flags = session->flags;
	struct sockaddr_l2 sa;
	int sk;
	int sockflags = SOCK_SEQPACKET | O_CLOEXEC;

	if (flags & SDP_NON_BLOCKING)
		sockflags |= O_NONBLOCK;

	session->sock = socket(PF_BLUETOOTH, sockflags, BTPROTO_L2CAP);
	if (session->sock < 0)
		return -1;
	session->local = 0;

	sk = session->sock;

	memset(&sa, 0, sizeof(sa));

	sa.l2_family = AF_BLUETOOTH;
	sa.l2_psm = 0;

	if (bacmp(src, BDADDR_ANY)) {
		sa.l2_bdaddr = *src;
		if (bind(sk, (struct sockaddr *) &sa, sizeof(sa)) < 0)
			return -1;
	}

	if (flags & SDP_WAIT_ON_CLOSE) {
		struct linger l = { .l_onoff = 1, .l_linger = 1 };
		setsockopt(sk, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
	}

	sa.l2_psm = htobs(SDP_PSM);
	sa.l2_bdaddr = *dst;

	do {
		int ret = connect(sk, (struct sockaddr *) &sa, sizeof(sa));
		if (!ret)
			return 0;
		if (ret < 0 && (flags & SDP_NON_BLOCKING) &&
				(errno == EAGAIN || errno == EINPROGRESS))
			return 0;
	} while (errno == EBUSY && (flags & SDP_RETRY_IF_BUSY));

	return -1;
}

sdp_session_t *sdp_connect(const bdaddr_t *src,
		const bdaddr_t *dst, uint32_t flags)
{
	sdp_session_t *session;
	int err;

	if ((flags & SDP_RETRY_IF_BUSY) && (flags & SDP_NON_BLOCKING)) {
		errno = EINVAL;
		return NULL;
	}

	session = sdp_create(-1, flags);
	if (!session)
		return NULL;

	if (sdp_is_local(dst)) {
		if (sdp_connect_local(session) < 0)
			goto fail;
	} else {
		if (sdp_connect_l2cap(src, dst, session) < 0)
			goto fail;
	}

	return session;

fail:
	err = errno;
	if (session->sock >= 0)
		close(session->sock);
	free(session->priv);
	free(session);
	errno = err;

	return NULL;
}

int sdp_get_socket(const sdp_session_t *session)
{
	return session->sock;
}

uint16_t sdp_gen_tid(sdp_session_t *session)
{
	return session->tid++;
}

/*
 * Set the supported features
 */
int sdp_set_supp_feat(sdp_record_t *rec, const sdp_list_t *sf)
{
	const sdp_list_t *p, *r;
	sdp_data_t *feat, *seq_feat;
	int seqlen, i;
	void **seqDTDs, **seqVals;

	seqlen = sdp_list_len(sf);
	seqDTDs = malloc(seqlen * sizeof(void *));
	if (!seqDTDs)
		return -1;
	seqVals = malloc(seqlen * sizeof(void *));
	if (!seqVals) {
		free(seqDTDs);
		return -1;
	}

	for (p = sf, i = 0; p; p = p->next, i++) {
		int plen, j;
		void **dtds, **vals;
		int *lengths;

		plen = sdp_list_len(p->data);
		dtds = malloc(plen * sizeof(void *));
		if (!dtds)
			goto fail;
		vals = malloc(plen * sizeof(void *));
		if (!vals) {
			free(dtds);
			goto fail;
		}
		lengths = malloc(plen * sizeof(int *));
		if (!lengths) {
			free(dtds);
			free(vals);
			goto fail;
		}
		for (r = p->data, j = 0; r; r = r->next, j++) {
			sdp_data_t *data = (sdp_data_t *) r->data;
			dtds[j] = &data->dtd;
			switch (data->dtd) {
			case SDP_URL_STR8:
			case SDP_URL_STR16:
			case SDP_TEXT_STR8:
			case SDP_TEXT_STR16:
				vals[j] = data->val.str;
				lengths[j] = data->unitSize - sizeof(uint8_t);
				break;
			case SDP_ALT8:
			case SDP_ALT16:
			case SDP_ALT32:
			case SDP_SEQ8:
			case SDP_SEQ16:
			case SDP_SEQ32:
				vals[j] = data->val.dataseq;
				lengths[j] = 0;
				break;
			default:
				vals[j] = &data->val;
				lengths[j] = 0;
				break;
			}
		}
		feat = sdp_seq_alloc_with_length(dtds, vals, lengths, plen);
		free(dtds);
		free(vals);
		free(lengths);
		if (!feat)
			goto fail;
		seqDTDs[i] = &feat->dtd;
		seqVals[i] = feat;
	}
	seq_feat = sdp_seq_alloc(seqDTDs, seqVals, seqlen);
	if (!seq_feat)
		goto fail;
	sdp_attr_replace(rec, SDP_ATTR_SUPPORTED_FEATURES_LIST, seq_feat);

	free(seqVals);
	free(seqDTDs);
	return 0;

fail:
	free(seqVals);
	free(seqDTDs);
	return -1;
}

/*
 * Get the supported features
 * If an error occurred -1 is returned and errno is set
 */
int sdp_get_supp_feat(const sdp_record_t *rec, sdp_list_t **seqp)
{
	sdp_data_t *sdpdata, *d;
	sdp_list_t *tseq;
	tseq = NULL;

	sdpdata = sdp_data_get(rec, SDP_ATTR_SUPPORTED_FEATURES_LIST);

	if (!sdpdata || !SDP_IS_SEQ(sdpdata->dtd))
		return sdp_get_uuidseq_attr(rec,
					SDP_ATTR_SUPPORTED_FEATURES_LIST, seqp);

	for (d = sdpdata->val.dataseq; d; d = d->next) {
		sdp_data_t *dd;
		sdp_list_t *subseq;

		if (!SDP_IS_SEQ(d->dtd))
			goto fail;

		subseq = NULL;

		for (dd = d->val.dataseq; dd; dd = dd->next) {
			sdp_data_t *data;
			void *val;
			int length;

			switch (dd->dtd) {
			case SDP_URL_STR8:
			case SDP_URL_STR16:
			case SDP_TEXT_STR8:
			case SDP_TEXT_STR16:
				val = dd->val.str;
				length = dd->unitSize - sizeof(uint8_t);
				break;
			case SDP_UINT8:
			case SDP_UINT16:
				val = &dd->val;
				length = 0;
				break;
			default:
				goto fail;
			}

			data = sdp_data_alloc_with_length(dd->dtd, val, length);
			if (data)
				subseq = sdp_list_append(subseq, data);
		}
		tseq = sdp_list_append(tseq, subseq);
	}
	*seqp = tseq;
	return 0;

fail:
	while (tseq) {
		sdp_list_t * next;

		next = tseq->next;
		sdp_list_free(tseq, free);
		tseq = next;
	}
	errno = EINVAL;
	return -1;
}

void sdp_add_lang_attr(sdp_record_t *rec)
{
	sdp_lang_attr_t base_lang;
	sdp_list_t *langs;

	base_lang.code_ISO639 = (0x65 << 8) | 0x6e;
	base_lang.encoding = 106;
	base_lang.base_offset = SDP_PRIMARY_LANG_BASE;

	langs = sdp_list_append(0, &base_lang);
	sdp_set_lang_attr(rec, langs);
	sdp_list_free(langs, NULL);
}
