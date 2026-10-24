/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2012  Intel Corporation. All rights reserved.
 *  Copyright (c) 2012  Code Aurora Forum. All rights reserved.
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

#ifndef __A2MP_H
#define __A2MP_H

#ifdef __cplusplus
extern "C" {
#endif

/* A2MP Protocol */

/* A2MP command codes */

#define A2MP_COMMAND_REJ	0x01
#define A2MP_DISCOVER_REQ	0x02
#define A2MP_DISCOVER_RSP	0x03
#define A2MP_CHANGE_NOTIFY	0x04
#define A2MP_CHANGE_RSP		0x05
#define A2MP_INFO_REQ		0x06
#define A2MP_INFO_RSP		0x07
#define A2MP_ASSOC_REQ		0x08
#define A2MP_ASSOC_RSP		0x09
#define A2MP_CREATE_REQ		0x0a
#define A2MP_CREATE_RSP		0x0b
#define A2MP_DISCONN_REQ	0x0c
#define A2MP_DISCONN_RSP	0x0d

struct a2mp_hdr {
	uint8_t		code;
	uint8_t		ident;
	uint16_t	len;
} __attribute__ ((packed));
#define A2MP_HDR_SIZE 4

struct a2mp_command_rej {
	uint16_t	reason;
} __attribute__ ((packed));

struct a2mp_discover_req {
	uint16_t	mtu;
	uint16_t	mask;
} __attribute__ ((packed));

struct a2mp_ctrl {
	uint8_t		id;
	uint8_t		type;
	uint8_t		status;
} __attribute__ ((packed));

struct a2mp_discover_rsp {
	uint16_t	mtu;
	uint16_t	mask;
	struct a2mp_ctrl ctrl_list[0];
} __attribute__ ((packed));

struct a2mp_info_req {
	uint8_t		id;
} __attribute__ ((packed));

struct a2mp_info_rsp {
	uint8_t		id;
	uint8_t		status;
	uint32_t	total_bw;
	uint32_t	max_bw;
	uint32_t	min_latency;
	uint16_t	pal_caps;
	uint16_t	assoc_size;
} __attribute__ ((packed));

struct a2mp_assoc_req {
	uint8_t		id;
} __attribute__ ((packed));

struct a2mp_assoc_rsp {
	uint8_t		id;
	uint8_t		status;
	uint8_t		assoc_data[0];
} __attribute__ ((packed));

struct a2mp_create_req {
	uint8_t		local_id;
	uint8_t		remote_id;
	uint8_t		assoc_data[0];
} __attribute__ ((packed));

struct a2mp_create_rsp {
	uint8_t		local_id;
	uint8_t		remote_id;
	uint8_t		status;
} __attribute__ ((packed));

struct a2mp_disconn_req {
	uint8_t		local_id;
	uint8_t		remote_id;
} __attribute__ ((packed));

struct a2mp_disconn_rsp {
	uint8_t		local_id;
	uint8_t		remote_id;
	uint8_t		status;
} __attribute__ ((packed));

#define A2MP_COMMAND_NOT_RECOGNIZED 0x0000

/* AMP controller status */
#define AMP_CTRL_POWERED_DOWN		0x00
#define AMP_CTRL_BLUETOOTH_ONLY		0x01
#define AMP_CTRL_NO_CAPACITY		0x02
#define AMP_CTRL_LOW_CAPACITY		0x03
#define AMP_CTRL_MEDIUM_CAPACITY	0x04
#define AMP_CTRL_HIGH_CAPACITY		0x05
#define AMP_CTRL_FULL_CAPACITY		0x06

/* A2MP response status */
#define A2MP_STATUS_SUCCESS				0x00
#define A2MP_STATUS_INVALID_CTRL_ID			0x01
#define A2MP_STATUS_UNABLE_START_LINK_CREATION		0x02
#define A2MP_STATUS_NO_PHYSICAL_LINK_EXISTS		0x02
#define A2MP_STATUS_COLLISION_OCCURED			0x03
#define A2MP_STATUS_DISCONN_REQ_RECVD			0x04
#define A2MP_STATUS_PHYS_LINK_EXISTS			0x05
#define A2MP_STATUS_SECURITY_VIOLATION			0x06

#ifdef __cplusplus
}
#endif

#endif /* __A2MP_H */
