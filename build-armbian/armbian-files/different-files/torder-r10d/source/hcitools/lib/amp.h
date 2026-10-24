/*
 *
 *	BlueZ - Bluetooth protocol stack for Linux
 *
 *	Copyright (C) 2010-2011 Code Aurora Forum.  All rights reserved.
 *	Copyright (C) 2012 Intel Corporation.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 and
 *	only version 2 as published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 */

#ifndef __AMP_H
#define __AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#define AMP_MGR_CID 0x03

/* AMP manager codes */
#define AMP_COMMAND_REJ		0x01
#define AMP_DISCOVER_REQ	0x02
#define AMP_DISCOVER_RSP	0x03
#define AMP_CHANGE_NOTIFY	0x04
#define AMP_CHANGE_RSP		0x05
#define AMP_INFO_REQ		0x06
#define AMP_INFO_RSP		0x07
#define AMP_ASSOC_REQ		0x08
#define AMP_ASSOC_RSP		0x09
#define AMP_LINK_REQ		0x0a
#define AMP_LINK_RSP		0x0b
#define AMP_DISCONN_REQ		0x0c
#define AMP_DISCONN_RSP		0x0d

typedef struct {
	uint8_t		code;
	uint8_t		ident;
	uint16_t	len;
} __attribute__ ((packed)) amp_mgr_hdr;
#define AMP_MGR_HDR_SIZE 4

/* AMP ASSOC structure */
typedef struct {
	uint8_t		type_id;
	uint16_t	len;
	uint8_t		data[0];
} __attribute__ ((packed)) amp_assoc_tlv;

typedef struct {
	uint16_t	reason;
} __attribute__ ((packed)) amp_cmd_rej_parms;

typedef struct {
	uint16_t	mtu;
	uint16_t	mask;
} __attribute__ ((packed)) amp_discover_req_parms;

typedef struct {
	uint16_t	mtu;
	uint16_t	mask;
	uint8_t		controller_list[0];
} __attribute__ ((packed)) amp_discover_rsp_parms;

typedef struct {
	uint8_t		id;
} __attribute__ ((packed)) amp_info_req_parms;

typedef struct {
	uint8_t		id;
	uint8_t		status;
	uint32_t	total_bandwidth;
	uint32_t	max_bandwidth;
	uint32_t	min_latency;
	uint16_t	pal_caps;
	uint16_t	assoc_size;
} __attribute__ ((packed)) amp_info_rsp_parms;

typedef struct {
	uint8_t		id;
	uint8_t		status;
	amp_assoc_tlv	assoc;
} __attribute__ ((packed)) amp_assoc_rsp_parms;

typedef struct {
	uint8_t		local_id;
	uint8_t		remote_id;
	amp_assoc_tlv	assoc;
} __attribute__ ((packed)) amp_link_req_parms;

typedef struct {
	uint8_t		local_id;
	uint8_t		remote_id;
	uint8_t		status;
} __attribute__ ((packed)) amp_link_rsp_parms;

typedef struct {
	uint8_t		local_id;
	uint8_t		remote_id;
} __attribute__ ((packed)) amp_disconn_req_parms;

#define A2MP_MAC_ADDR_TYPE		1
#define A2MP_PREF_CHANLIST_TYPE		2
#define A2MP_CONNECTED_CHAN		3
#define A2MP_PAL_CAP_TYPE		4
#define A2MP_PAL_VER_INFO		5

struct amp_tlv {
	uint8_t type;
	uint16_t len;
	uint8_t val[0];
} __attribute__ ((packed));

struct amp_pal_ver {
	uint8_t ver;
	uint16_t company_id;
	uint16_t sub_ver;
} __attribute__ ((packed));

struct amp_country_triplet {
	union {
		struct {
			uint8_t first_channel;
			uint8_t num_channels;
			int8_t max_power;
		} __attribute__ ((packed)) chans;
		struct {
			uint8_t reg_extension_id;
			uint8_t reg_class;
			uint8_t coverage_class;
		} __attribute__ ((packed)) ext;
	};
} __attribute__ ((packed));

struct amp_chan_list {
	uint8_t country_code[3];
	struct amp_country_triplet triplets[0];
} __attribute__ ((packed));

#define AMP_COMMAND_NOT_RECOGNIZED 0x0000

/* AMP controller status */
#define AMP_CT_POWERED_DOWN		0x00
#define AMP_CT_BLUETOOTH_ONLY		0x01
#define AMP_CT_NO_CAPACITY		0x02
#define AMP_CT_LOW_CAPACITY		0x03
#define AMP_CT_MEDIUM_CAPACITY		0x04
#define AMP_CT_HIGH_CAPACITY		0x05
#define AMP_CT_FULL_CAPACITY		0x06

/* AMP response status */
#define AMP_STATUS_SUCCESS				0x00
#define AMP_STATUS_INVALID_CTRL_ID			0x01
#define AMP_STATUS_UNABLE_START_LINK_CREATION		0x02
#define AMP_STATUS_NO_PHYSICAL_LINK_EXISTS		0x02
#define AMP_STATUS_COLLISION_OCCURED			0x03
#define AMP_STATUS_DISCONN_REQ_RECVD			0x04
#define AMP_STATUS_PHYS_LINK_EXISTS			0x05
#define AMP_STATUS_SECURITY_VIOLATION			0x06

#ifdef __cplusplus
}
#endif

#endif /* __AMP_H */
