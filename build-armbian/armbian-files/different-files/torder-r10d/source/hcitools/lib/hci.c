/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2000-2001  Qualcomm Incorporated
 *  Copyright (C) 2002-2003  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2002-2010  Marcel Holtmann <marcel@holtmann.org>
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

#include <sys/param.h>
#include <sys/uio.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "bluetooth.h"
#include "hci.h"
#include "hci_lib.h"

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

typedef struct {
	char *str;
	unsigned int val;
} hci_map;

static char *hci_bit2str(hci_map *m, unsigned int val)
{
	char *str = malloc(120);
	char *ptr = str;

	if (!str)
		return NULL;

	*ptr = 0;
	while (m->str) {
		if ((unsigned int) m->val & val)
			ptr += sprintf(ptr, "%s ", m->str);
		m++;
	}
	return str;
}

static int hci_str2bit(hci_map *map, char *str, unsigned int *val)
{
	char *t, *ptr;
	hci_map *m;
	int set;

	if (!str || !(str = ptr = strdup(str)))
		return 0;

	*val = set = 0;

	while ((t = strsep(&ptr, ","))) {
		for (m = map; m->str; m++) {
			if (!strcasecmp(m->str, t)) {
				*val |= (unsigned int) m->val;
				set = 1;
			}
		}
	}
	free(str);

	return set;
}

static char *hci_uint2str(hci_map *m, unsigned int val)
{
	char *str = malloc(50);
	char *ptr = str;

	if (!str)
		return NULL;

	*ptr = 0;
	while (m->str) {
		if ((unsigned int) m->val == val) {
			ptr += sprintf(ptr, "%s", m->str);
			break;
		}
		m++;
	}
	return str;
}

static int hci_str2uint(hci_map *map, char *str, unsigned int *val)
{
	char *t, *ptr;
	hci_map *m;
	int set = 0;

	if (!str)
		return 0;

	str = ptr = strdup(str);

	while ((t = strsep(&ptr, ","))) {
		for (m = map; m->str; m++) {
			if (!strcasecmp(m->str,t)) {
				*val = (unsigned int) m->val;
				set = 1;
				break;
			}
		}
	}
	free(str);

	return set;
}

char *hci_bustostr(int bus)
{
	switch (bus) {
	case HCI_VIRTUAL:
		return "VIRTUAL";
	case HCI_USB:
		return "USB";
	case HCI_PCCARD:
		return "PCCARD";
	case HCI_UART:
		return "UART";
	case HCI_RS232:
		return "RS232";
	case HCI_PCI:
		return "PCI";
	case HCI_SDIO:
		return "SDIO";
	default:
		return "UNKNOWN";
	}
}

char *hci_dtypetostr(int type)
{
	return hci_bustostr(type & 0x0f);
}

char *hci_typetostr(int type)
{
	switch (type) {
	case HCI_BREDR:
		return "BR/EDR";
	case HCI_AMP:
		return "AMP";
	default:
		return "UNKNOWN";
	}
}

/* HCI dev flags mapping */
static hci_map dev_flags_map[] = {
	{ "UP",      HCI_UP      },
	{ "INIT",    HCI_INIT    },
	{ "RUNNING", HCI_RUNNING },
	{ "RAW",     HCI_RAW     },
	{ "PSCAN",   HCI_PSCAN   },
	{ "ISCAN",   HCI_ISCAN   },
	{ "INQUIRY", HCI_INQUIRY },
	{ "AUTH",    HCI_AUTH    },
	{ "ENCRYPT", HCI_ENCRYPT },
	{ NULL }
};

char *hci_dflagstostr(uint32_t flags)
{
	char *str = bt_malloc(50);
	char *ptr = str;
	hci_map *m = dev_flags_map;

	if (!str)
		return NULL;

	*ptr = 0;

	if (!hci_test_bit(HCI_UP, &flags))
		ptr += sprintf(ptr, "DOWN ");

	while (m->str) {
		if (hci_test_bit(m->val, &flags))
			ptr += sprintf(ptr, "%s ", m->str);
		m++;
	}
	return str;
}

/* HCI packet type mapping */
static hci_map pkt_type_map[] = {
	{ "DM1",   HCI_DM1  },
	{ "DM3",   HCI_DM3  },
	{ "DM5",   HCI_DM5  },
	{ "DH1",   HCI_DH1  },
	{ "DH3",   HCI_DH3  },
	{ "DH5",   HCI_DH5  },
	{ "HV1",   HCI_HV1  },
	{ "HV2",   HCI_HV2  },
	{ "HV3",   HCI_HV3  },
	{ "2-DH1", HCI_2DH1 },
	{ "2-DH3", HCI_2DH3 },
	{ "2-DH5", HCI_2DH5 },
	{ "3-DH1", HCI_3DH1 },
	{ "3-DH3", HCI_3DH3 },
	{ "3-DH5", HCI_3DH5 },
	{ NULL }
};

static hci_map sco_ptype_map[] = {
	{ "HV1",   0x0001   },
	{ "HV2",   0x0002   },
	{ "HV3",   0x0004   },
	{ "EV3",   HCI_EV3  },
	{ "EV4",   HCI_EV4  },
	{ "EV5",   HCI_EV5  },
	{ "2-EV3", HCI_2EV3 },
	{ "2-EV5", HCI_2EV5 },
	{ "3-EV3", HCI_3EV3 },
	{ "3-EV5", HCI_3EV5 },
	{ NULL }
};

char *hci_ptypetostr(unsigned int ptype)
{
	return hci_bit2str(pkt_type_map, ptype);
}

int hci_strtoptype(char *str, unsigned int *val)
{
	return hci_str2bit(pkt_type_map, str, val);
}

char *hci_scoptypetostr(unsigned int ptype)
{
	return hci_bit2str(sco_ptype_map, ptype);
}

int hci_strtoscoptype(char *str, unsigned int *val)
{
	return hci_str2bit(sco_ptype_map, str, val);
}

/* Link policy mapping */
static hci_map link_policy_map[] = {
	{ "NONE",	0		},
	{ "RSWITCH",	HCI_LP_RSWITCH	},
	{ "HOLD",	HCI_LP_HOLD	},
	{ "SNIFF",	HCI_LP_SNIFF	},
	{ "PARK",	HCI_LP_PARK	},
	{ NULL }
};

char *hci_lptostr(unsigned int lp)
{
	return hci_bit2str(link_policy_map, lp);
}

int hci_strtolp(char *str, unsigned int *val)
{
	return hci_str2bit(link_policy_map, str, val);
}

/* Link mode mapping */
static hci_map link_mode_map[] = {
	{ "NONE",	0		},
	{ "ACCEPT",	HCI_LM_ACCEPT	},
	{ "MASTER",	HCI_LM_MASTER	},
	{ "AUTH",	HCI_LM_AUTH	},
	{ "ENCRYPT",	HCI_LM_ENCRYPT	},
	{ "TRUSTED",	HCI_LM_TRUSTED	},
	{ "RELIABLE",	HCI_LM_RELIABLE	},
	{ "SECURE",	HCI_LM_SECURE	},
	{ NULL }
};

char *hci_lmtostr(unsigned int lm)
{
	char *s, *str = bt_malloc(50);
	if (!str)
		return NULL;

	*str = 0;
	if (!(lm & HCI_LM_MASTER))
		strcpy(str, "SLAVE ");

	s = hci_bit2str(link_mode_map, lm);
	if (!s) {
		bt_free(str);
		return NULL;
	}

	strcat(str, s);
	free(s);
	return str;
}

int hci_strtolm(char *str, unsigned int *val)
{
	return hci_str2bit(link_mode_map, str, val);
}

/* Command mapping */
static hci_map commands_map[] = {
	{ "Inquiry",					0   },
	{ "Inquiry Cancel",				1   },
	{ "Periodic Inquiry Mode",			2   },
	{ "Exit Periodic Inquiry Mode",			3   },
	{ "Create Connection",				4   },
	{ "Disconnect",					5   },
	{ "Add SCO Connection",				6   },
	{ "Cancel Create Connection",			7   },

	{ "Accept Connection Request",			8   },
	{ "Reject Connection Request",			9   },
	{ "Link Key Request Reply",			10  },
	{ "Link Key Request Negative Reply",		11  },
	{ "PIN Code Request Reply",			12  },
	{ "PIN Code Request Negative Reply",		13  },
	{ "Change Connection Packet Type",		14  },
	{ "Authentication Requested",			15  },

	{ "Set Connection Encryption",			16  },
	{ "Change Connection Link Key",			17  },
	{ "Master Link Key",				18  },
	{ "Remote Name Request",			19  },
	{ "Cancel Remote Name Request",			20  },
	{ "Read Remote Supported Features",		21  },
	{ "Read Remote Extended Features",		22  },
	{ "Read Remote Version Information",		23  },

	{ "Read Clock Offset",				24  },
	{ "Read LMP Handle",				25  },
	{ "Reserved",					26  },
	{ "Reserved",					27  },
	{ "Reserved",					28  },
	{ "Reserved",					29  },
	{ "Reserved",					30  },
	{ "Reserved",					31  },

	{ "Reserved",					32  },
	{ "Hold Mode",					33  },
	{ "Sniff Mode",					34  },
	{ "Exit Sniff Mode",				35  },
	{ "Park State",					36  },
	{ "Exit Park State",				37  },
	{ "QoS Setup",					38  },
	{ "Role Discovery",				39  },

	{ "Switch Role",				40  },
	{ "Read Link Policy Settings",			41  },
	{ "Write Link Policy Settings",			42  },
	{ "Read Default Link Policy Settings",		43  },
	{ "Write Default Link Policy Settings",		44  },
	{ "Flow Specification",				45  },
	{ "Set Event Mask",				46  },
	{ "Reset",					47  },

	{ "Set Event Filter",				48  },
	{ "Flush",					49  },
	{ "Read PIN Type",				50  },
	{ "Write PIN Type",				51  },
	{ "Create New Unit Key",			52  },
	{ "Read Stored Link Key",			53  },
	{ "Write Stored Link Key",			54  },
	{ "Delete Stored Link Key",			55  },

	{ "Write Local Name",				56  },
	{ "Read Local Name",				57  },
	{ "Read Connection Accept Timeout",		58  },
	{ "Write Connection Accept Timeout",		59  },
	{ "Read Page Timeout",				60  },
	{ "Write Page Timeout",				61  },
	{ "Read Scan Enable",				62  },
	{ "Write Scan Enable",				63  },

	{ "Read Page Scan Activity",			64  },
	{ "Write Page Scan Activity",			65  },
	{ "Read Inquiry Scan Activity",			66  },
	{ "Write Inquiry Scan Activity",		67  },
	{ "Read Authentication Enable",			68  },
	{ "Write Authentication Enable",		69  },
	{ "Read Encryption Mode",			70  },
	{ "Write Encryption Mode",			71  },

	{ "Read Class Of Device",			72  },
	{ "Write Class Of Device",			73  },
	{ "Read Voice Setting",				74  },
	{ "Write Voice Setting",			75  },
	{ "Read Automatic Flush Timeout",		76  },
	{ "Write Automatic Flush Timeout",		77  },
	{ "Read Num Broadcast Retransmissions",		78  },
	{ "Write Num Broadcast Retransmissions",	79  },

	{ "Read Hold Mode Activity",			80  },
	{ "Write Hold Mode Activity",			81  },
	{ "Read Transmit Power Level",			82  },
	{ "Read Synchronous Flow Control Enable",	83  },
	{ "Write Synchronous Flow Control Enable",	84  },
	{ "Set Host Controller To Host Flow Control",	85  },
	{ "Host Buffer Size",				86  },
	{ "Host Number Of Completed Packets",		87  },

	{ "Read Link Supervision Timeout",		88  },
	{ "Write Link Supervision Timeout",		89  },
	{ "Read Number of Supported IAC",		90  },
	{ "Read Current IAC LAP",			91  },
	{ "Write Current IAC LAP",			92  },
	{ "Read Page Scan Period Mode",			93  },
	{ "Write Page Scan Period Mode",		94  },
	{ "Read Page Scan Mode",			95  },

	{ "Write Page Scan Mode",			96  },
	{ "Set AFH Channel Classification",		97  },
	{ "Reserved",					98  },
	{ "Reserved",					99  },
	{ "Read Inquiry Scan Type",			100 },
	{ "Write Inquiry Scan Type",			101 },
	{ "Read Inquiry Mode",				102 },
	{ "Write Inquiry Mode",				103 },

	{ "Read Page Scan Type",			104 },
	{ "Write Page Scan Type",			105 },
	{ "Read AFH Channel Assessment Mode",		106 },
	{ "Write AFH Channel Assessment Mode",		107 },
	{ "Reserved",					108 },
	{ "Reserved",					109 },
	{ "Reserved",					110 },
	{ "Reserved",					111 },

	{ "Reserved",					112 },
	{ "Reserved",					113 },
	{ "Reserved",					114 },
	{ "Read Local Version Information",		115 },
	{ "Read Local Supported Commands",		116 },
	{ "Read Local Supported Features",		117 },
	{ "Read Local Extended Features",		118 },
	{ "Read Buffer Size",				119 },

	{ "Read Country Code",				120 },
	{ "Read BD ADDR",				121 },
	{ "Read Failed Contact Counter",		122 },
	{ "Reset Failed Contact Counter",		123 },
	{ "Get Link Quality",				124 },
	{ "Read RSSI",					125 },
	{ "Read AFH Channel Map",			126 },
	{ "Read BD Clock",				127 },

	{ "Read Loopback Mode",				128 },
	{ "Write Loopback Mode",			129 },
	{ "Enable Device Under Test Mode",		130 },
	{ "Setup Synchronous Connection",		131 },
	{ "Accept Synchronous Connection",		132 },
	{ "Reject Synchronous Connection",		133 },
	{ "Reserved",					134 },
	{ "Reserved",					135 },

	{ "Read Extended Inquiry Response",		136 },
	{ "Write Extended Inquiry Response",		137 },
	{ "Refresh Encryption Key",			138 },
	{ "Reserved",					139 },
	{ "Sniff Subrating",				140 },
	{ "Read Simple Pairing Mode",			141 },
	{ "Write Simple Pairing Mode",			142 },
	{ "Read Local OOB Data",			143 },

	{ "Read Inquiry Response Transmit Power Level",	144 },
	{ "Write Inquiry Transmit Power Level",		145 },
	{ "Read Default Erroneous Data Reporting",	146 },
	{ "Write Default Erroneous Data Reporting",	147 },
	{ "Reserved",					148 },
	{ "Reserved",					149 },
	{ "Reserved",					150 },
	{ "IO Capability Request Reply",		151 },

	{ "User Confirmation Request Reply",		152 },
	{ "User Confirmation Request Negative Reply",	153 },
	{ "User Passkey Request Reply",			154 },
	{ "User Passkey Request Negative Reply",	155 },
	{ "Remote OOB Data Request Reply",		156 },
	{ "Write Simple Pairing Debug Mode",		157 },
	{ "Enhanced Flush",				158 },
	{ "Remote OOB Data Request Negative Reply",	159 },

	{ "Reserved",					160 },
	{ "Reserved",					161 },
	{ "Send Keypress Notification",			162 },
	{ "IO Capability Request Negative Reply",	163 },
	{ "Read Encryption Key Size",			164 },
	{ "Reserved",					165 },
	{ "Reserved",					166 },
	{ "Reserved",					167 },

	{ "Create Physical Link",			168 },
	{ "Accept Physical Link",			169 },
	{ "Disconnect Physical Link",			170 },
	{ "Create Logical Link",			171 },
	{ "Accept Logical Link",			172 },
	{ "Disconnect Logical Link",			173 },
	{ "Logical Link Cancel",			174 },
	{ "Flow Specification Modify",			175 },

	{ "Read Logical Link Accept Timeout",		176 },
	{ "Write Logical Link Accept Timeout",		177 },
	{ "Set Event Mask Page 2",			178 },
	{ "Read Location Data",				179 },
	{ "Write Location Data",			180 },
	{ "Read Local AMP Info",			181 },
	{ "Read Local AMP_ASSOC",			182 },
	{ "Write Remote AMP_ASSOC",			183 },

	{ "Read Flow Control Mode",			184 },
	{ "Write Flow Control Mode",			185 },
	{ "Read Data Block Size",			186 },
	{ "Reserved",					187 },
	{ "Reserved",					188 },
	{ "Enable AMP Receiver Reports",		189 },
	{ "AMP Test End",				190 },
	{ "AMP Test Command",				191 },

	{ "Read Enhanced Transmit Power Level",		192 },
	{ "Reserved",					193 },
	{ "Read Best Effort Flush Timeout",		194 },
	{ "Write Best Effort Flush Timeout",		195 },
	{ "Short Range Mode",				196 },
	{ "Read LE Host Support",			197 },
	{ "Write LE Host Support",			198 },
	{ "Reserved",					199 },

	{ "LE Set Event Mask",				200 },
	{ "LE Read Buffer Size",			201 },
	{ "LE Read Local Supported Features",		202 },
	{ "Reserved",					203 },
	{ "LE Set Random Address",			204 },
	{ "LE Set Advertising Parameters",		205 },
	{ "LE Read Advertising Channel TX Power",	206 },
	{ "LE Set Advertising Data",			207 },

	{ "LE Set Scan Response Data",			208 },
	{ "LE Set Advertise Enable",			209 },
	{ "LE Set Scan Parameters",			210 },
	{ "LE Set Scan Enable",				211 },
	{ "LE Create Connection",			212 },
	{ "LE Create Connection Cancel",		213 },
	{ "LE Read White List Size",			214 },
	{ "LE Clear White List",			215 },

	{ "LE Add Device To White List",		216 },
	{ "LE Remove Device From White List",		217 },
	{ "LE Connection Update",			218 },
	{ "LE Set Host Channel Classification",		219 },
	{ "LE Read Channel Map",			220 },
	{ "LE Read Remote Used Features",		221 },
	{ "LE Encrypt",					222 },
	{ "LE Rand",					223 },

	{ "LE Start Encryption",			224 },
	{ "LE Long Term Key Request Reply",		225 },
	{ "LE Long Term Key Request Negative Reply",	226 },
	{ "LE Read Supported States",			227 },
	{ "LE Receiver Test",				228 },
	{ "LE Transmitter Test",			229 },
	{ "LE Test End",				230 },
	{ "Reserved",					231 },

	{ NULL }
};

char *hci_cmdtostr(unsigned int cmd)
{
	return hci_uint2str(commands_map, cmd);
}

char *hci_commandstostr(uint8_t *commands, char *pref, int width)
{
	unsigned int maxwidth = width - 3;
	hci_map *m;
	char *off, *ptr, *str;
	int size = 10;

	m = commands_map;

	while (m->str) {
		if (commands[m->val / 8] & (1 << (m->val % 8)))
			size += strlen(m->str) + (pref ? strlen(pref) : 0) + 3;
		m++;
	}

	str = bt_malloc(size);
	if (!str)
		return NULL;

	ptr = str; *ptr = '\0';

	if (pref)
		ptr += sprintf(ptr, "%s", pref);

	off = ptr;

	m = commands_map;

	while (m->str) {
		if (commands[m->val / 8] & (1 << (m->val % 8))) {
			if (strlen(off) + strlen(m->str) > maxwidth) {
				ptr += sprintf(ptr, "\n%s", pref ? pref : "");
				off = ptr;
			}
			ptr += sprintf(ptr, "'%s' ", m->str);
		}
		m++;
	}

	return str;
}

/* Version mapping */
static hci_map ver_map[] = {
	{ "1.0b",	0x00 },
	{ "1.1",	0x01 },
	{ "1.2",	0x02 },
	{ "2.0",	0x03 },
	{ "2.1",	0x04 },
	{ "3.0",	0x05 },
	{ "4.0",	0x06 },
	{ NULL }
};

char *hci_vertostr(unsigned int ver)
{
	return hci_uint2str(ver_map, ver);
}

int hci_strtover(char *str, unsigned int *ver)
{
	return hci_str2uint(ver_map, str, ver);
}

char *lmp_vertostr(unsigned int ver)
{
	return hci_uint2str(ver_map, ver);
}

int lmp_strtover(char *str, unsigned int *ver)
{
	return hci_str2uint(ver_map, str, ver);
}

static hci_map pal_map[] = {
	{ "3.0",	0x01 },
	{ NULL }
};

char *pal_vertostr(unsigned int ver)
{
	return hci_uint2str(pal_map, ver);
}

int pal_strtover(char *str, unsigned int *ver)
{
	return hci_str2uint(pal_map, str, ver);
}

/* LMP features mapping */
static hci_map lmp_features_map[8][9] = {
	{	/* Byte 0 */
		{ "<3-slot packets>",	LMP_3SLOT	},	/* Bit 0 */
		{ "<5-slot packets>",	LMP_5SLOT	},	/* Bit 1 */
		{ "<encryption>",	LMP_ENCRYPT	},	/* Bit 2 */
		{ "<slot offset>",	LMP_SOFFSET	},	/* Bit 3 */
		{ "<timing accuracy>",	LMP_TACCURACY	},	/* Bit 4 */
		{ "<role switch>",	LMP_RSWITCH	},	/* Bit 5 */
		{ "<hold mode>",	LMP_HOLD	},	/* Bit 6 */
		{ "<sniff mode>",	LMP_SNIFF	},	/* Bit 7 */
		{ NULL }
	},
	{	/* Byte 1 */
		{ "<park state>",	LMP_PARK	},	/* Bit 0 */
		{ "<RSSI>",		LMP_RSSI	},	/* Bit 1 */
		{ "<channel quality>",	LMP_QUALITY	},	/* Bit 2 */
		{ "<SCO link>",		LMP_SCO		},	/* Bit 3 */
		{ "<HV2 packets>",	LMP_HV2		},	/* Bit 4 */
		{ "<HV3 packets>",	LMP_HV3		},	/* Bit 5 */
		{ "<u-law log>",	LMP_ULAW	},	/* Bit 6 */
		{ "<A-law log>",	LMP_ALAW	},	/* Bit 7 */
		{ NULL }
	},
	{	/* Byte 2 */
		{ "<CVSD>",		LMP_CVSD	},	/* Bit 0 */
		{ "<paging scheme>",	LMP_PSCHEME	},	/* Bit 1 */
		{ "<power control>",	LMP_PCONTROL	},	/* Bit 2 */
		{ "<transparent SCO>",	LMP_TRSP_SCO	},	/* Bit 3 */
		{ "<broadcast encrypt>",LMP_BCAST_ENC	},	/* Bit 7 */
		{ NULL }
	},
	{	/* Byte 3 */
		{ "<no. 24>",		0x01		},	/* Bit 0 */
		{ "<EDR ACL 2 Mbps>",	LMP_EDR_ACL_2M	},	/* Bit 1 */
		{ "<EDR ACL 3 Mbps>",	LMP_EDR_ACL_3M	},	/* Bit 2 */
		{ "<enhanced iscan>",	LMP_ENH_ISCAN	},	/* Bit 3 */
		{ "<interlaced iscan>",	LMP_ILACE_ISCAN	},	/* Bit 4 */
		{ "<interlaced pscan>",	LMP_ILACE_PSCAN	},	/* Bit 5 */
		{ "<inquiry with RSSI>",LMP_RSSI_INQ	},	/* Bit 6 */
		{ "<extended SCO>",	LMP_ESCO	},	/* Bit 7 */
		{ NULL }
	},
	{	/* Byte 4 */
		{ "<EV4 packets>",	LMP_EV4		},	/* Bit 0 */
		{ "<EV5 packets>",	LMP_EV5		},	/* Bit 1 */
		{ "<no. 34>",		0x04		},	/* Bit 2 */
		{ "<AFH cap. slave>",	LMP_AFH_CAP_SLV	},	/* Bit 3 */
		{ "<AFH class. slave>",	LMP_AFH_CLS_SLV	},	/* Bit 4 */
		{ "<BR/EDR not supp.>",	LMP_NO_BREDR	},	/* Bit 5 */
		{ "<LE support>",	LMP_LE		},	/* Bit 6 */
		{ "<3-slot EDR ACL>",	LMP_EDR_3SLOT	},	/* Bit 7 */
		{ NULL }
	},
	{	/* Byte 5 */
		{ "<5-slot EDR ACL>",	LMP_EDR_5SLOT	},	/* Bit 0 */
		{ "<sniff subrating>",	LMP_SNIFF_SUBR	},	/* Bit 1 */
		{ "<pause encryption>",	LMP_PAUSE_ENC	},	/* Bit 2 */
		{ "<AFH cap. master>",	LMP_AFH_CAP_MST	},	/* Bit 3 */
		{ "<AFH class. master>",LMP_AFH_CLS_MST	},	/* Bit 4 */
		{ "<EDR eSCO 2 Mbps>",	LMP_EDR_ESCO_2M	},	/* Bit 5 */
		{ "<EDR eSCO 3 Mbps>",	LMP_EDR_ESCO_3M	},	/* Bit 6 */
		{ "<3-slot EDR eSCO>",	LMP_EDR_3S_ESCO	},	/* Bit 7 */
		{ NULL }
	},
	{	/* Byte 6 */
		{ "<extended inquiry>",	LMP_EXT_INQ	},	/* Bit 0 */
		{ "<LE and BR/EDR>",	LMP_LE_BREDR	},	/* Bit 1 */
		{ "<no. 50>",		0x04		},	/* Bit 2 */
		{ "<simple pairing>",	LMP_SIMPLE_PAIR	},	/* Bit 3 */
		{ "<encapsulated PDU>",	LMP_ENCAPS_PDU	},	/* Bit 4 */
		{ "<err. data report>",	LMP_ERR_DAT_REP	},	/* Bit 5 */
		{ "<non-flush flag>",	LMP_NFLUSH_PKTS	},	/* Bit 6 */
		{ "<no. 55>",		0x80		},	/* Bit 7 */
		{ NULL }
	},
	{	/* Byte 7 */
		{ "<LSTO>",		LMP_LSTO	},	/* Bit 1 */
		{ "<inquiry TX power>",	LMP_INQ_TX_PWR	},	/* Bit 1 */
		{ "<EPC>",		LMP_EPC		},	/* Bit 2 */
		{ "<no. 59>",		0x08		},	/* Bit 3 */
		{ "<no. 60>",		0x10		},	/* Bit 4 */
		{ "<no. 61>",		0x20		},	/* Bit 5 */
		{ "<no. 62>",		0x40		},	/* Bit 6 */
		{ "<extended features>",LMP_EXT_FEAT	},	/* Bit 7 */
		{ NULL }
	},
};

char *lmp_featurestostr(uint8_t *features, char *pref, int width)
{
	unsigned int maxwidth = width - 1;
	char *off, *ptr, *str;
	int i, size = 10;

	for (i = 0; i < 8; i++) {
		hci_map *m = lmp_features_map[i];

		while (m->str) {
			if (m->val & features[i])
				size += strlen(m->str) +
						(pref ? strlen(pref) : 0) + 1;
			m++;
		}
	}

	str = bt_malloc(size);
	if (!str)
		return NULL;

	ptr = str; *ptr = '\0';

	if (pref)
		ptr += sprintf(ptr, "%s", pref);

	off = ptr;

	for (i = 0; i < 8; i++) {
		hci_map *m = lmp_features_map[i];

		while (m->str) {
			if (m->val & features[i]) {
				if (strlen(off) + strlen(m->str) > maxwidth) {
					ptr += sprintf(ptr, "\n%s",
							pref ? pref : "");
					off = ptr;
				}
				ptr += sprintf(ptr, "%s ", m->str);
			}
			m++;
		}
	}

	return str;
}

/* HCI functions that do not require open device */
int hci_for_each_dev(int flag, int (*func)(int dd, int dev_id, long arg),
			long arg)
{
	struct hci_dev_list_req *dl;
	struct hci_dev_req *dr;
	int dev_id = -1;
	int i, sk, err = 0;

	sk = socket(AF_BLUETOOTH, SOCK_RAW | O_CLOEXEC, BTPROTO_HCI);
	if (sk < 0)
		return -1;

	dl = malloc(HCI_MAX_DEV * sizeof(*dr) + sizeof(*dl));
	if (!dl) {
		err = errno;
		goto done;
	}

	memset(dl, 0, HCI_MAX_DEV * sizeof(*dr) + sizeof(*dl));

	dl->dev_num = HCI_MAX_DEV;
	dr = dl->dev_req;

	if (ioctl(sk, HCIGETDEVLIST, (void *) dl) < 0) {
		err = errno;
		goto free;
	}

	for (i = 0; i < dl->dev_num; i++, dr++) {
		if (hci_test_bit(flag, &dr->dev_opt))
			if (!func || func(sk, dr->dev_id, arg)) {
				dev_id = dr->dev_id;
				break;
			}
	}

	if (dev_id < 0)
		err = ENODEV;

free:
	free(dl);

done:
	close(sk);
	errno = err;

	return dev_id;
}

static int __other_bdaddr(int dd, int dev_id, long arg)
{
	struct hci_dev_info di = { .dev_id = dev_id };

	if (ioctl(dd, HCIGETDEVINFO, (void *) &di))
		return 0;

	if (hci_test_bit(HCI_RAW, &di.flags))
		return 0;

	return bacmp((bdaddr_t *) arg, &di.bdaddr);
}

static int __same_bdaddr(int dd, int dev_id, long arg)
{
	struct hci_dev_info di = { .dev_id = dev_id };

	if (ioctl(dd, HCIGETDEVINFO, (void *) &di))
		return 0;

	return !bacmp((bdaddr_t *) arg, &di.bdaddr);
}

int hci_get_route(bdaddr_t *bdaddr)
{
	return hci_for_each_dev(HCI_UP, __other_bdaddr,
				(long) (bdaddr ? bdaddr : BDADDR_ANY));
}

int hci_devid(const char *str)
{
	bdaddr_t ba;
	int id = -1;

	if (!strncmp(str, "hci", 3) && strlen(str) >= 4) {
		id = atoi(str + 3);
		if (hci_devba(id, &ba) < 0)
			return -1;
	} else {
		errno = ENODEV;
		str2ba(str, &ba);
		id = hci_for_each_dev(HCI_UP, __same_bdaddr, (long) &ba);
	}

	return id;
}

int hci_devinfo(int dev_id, struct hci_dev_info *di)
{
	int dd, err, ret;

	dd = socket(AF_BLUETOOTH, SOCK_RAW | O_CLOEXEC, BTPROTO_HCI);
	if (dd < 0)
		return dd;

	memset(di, 0, sizeof(struct hci_dev_info));

	di->dev_id = dev_id;
	ret = ioctl(dd, HCIGETDEVINFO, (void *) di);

	err = errno;
	close(dd);
	errno = err;

	return ret;
}

int hci_devba(int dev_id, bdaddr_t *bdaddr)
{
	struct hci_dev_info di;

	memset(&di, 0, sizeof(di));

	if (hci_devinfo(dev_id, &di))
		return -1;

	if (!hci_test_bit(HCI_UP, &di.flags)) {
		errno = ENETDOWN;
		return -1;
	}

	bacpy(bdaddr, &di.bdaddr);

	return 0;
}

int hci_inquiry(int dev_id, int len, int nrsp, const uint8_t *lap,
		inquiry_info **ii, long flags)
{
	struct hci_inquiry_req *ir;
	uint8_t num_rsp = nrsp;
	void *buf;
	int dd, size, err, ret = -1;

	if (nrsp <= 0) {
		num_rsp = 0;
		nrsp = 255;
	}

	if (dev_id < 0) {
		dev_id = hci_get_route(NULL);
		if (dev_id < 0) {
			errno = ENODEV;
			return -1;
		}
	}

	dd = socket(AF_BLUETOOTH, SOCK_RAW | O_CLOEXEC, BTPROTO_HCI);
	if (dd < 0)
		return dd;

	buf = malloc(sizeof(*ir) + (sizeof(inquiry_info) * (nrsp)));
	if (!buf)
		goto done;

	ir = buf;
	ir->dev_id  = dev_id;
	ir->num_rsp = num_rsp;
	ir->length  = len;
	ir->flags   = flags;

	if (lap) {
		memcpy(ir->lap, lap, 3);
	} else {
		ir->lap[0] = 0x33;
		ir->lap[1] = 0x8b;
		ir->lap[2] = 0x9e;
	}

	ret = ioctl(dd, HCIINQUIRY, (unsigned long) buf);
	if (ret < 0)
		goto free;

	size = sizeof(inquiry_info) * ir->num_rsp;

	if (!*ii)
		*ii = malloc(size);

	if (*ii) {
		memcpy((void *) *ii, buf + sizeof(*ir), size);
		ret = ir->num_rsp;
	} else
		ret = -1;

free:
	free(buf);

done:
	err = errno;
	close(dd);
	errno = err;

	return ret;
}

/* Open HCI device.
 * Returns device descriptor (dd). */
int hci_open_dev(int dev_id)
{
	struct sockaddr_hci a;
	int dd, err;

	/* Create HCI socket */
	dd = socket(AF_BLUETOOTH, SOCK_RAW | O_CLOEXEC, BTPROTO_HCI);
	if (dd < 0)
		return dd;

	/* Bind socket to the HCI device */
	memset(&a, 0, sizeof(a));
	a.hci_family = AF_BLUETOOTH;
	a.hci_dev = dev_id;
	if (bind(dd, (struct sockaddr *) &a, sizeof(a)) < 0)
		goto failed;

	return dd;

failed:
	err = errno;
	close(dd);
	errno = err;

	return -1;
}

int hci_close_dev(int dd)
{
	return close(dd);
}

/* HCI functions that require open device
 * dd - Device descriptor returned by hci_open_dev. */

int hci_send_cmd(int dd, uint16_t ogf, uint16_t ocf, uint8_t plen, void *param)
{
	uint8_t type = HCI_COMMAND_PKT;
	hci_command_hdr hc;
	struct iovec iv[3];
	int ivn;

	hc.opcode = htobs(cmd_opcode_pack(ogf, ocf));
	hc.plen= plen;

	iv[0].iov_base = &type;
	iv[0].iov_len  = 1;
	iv[1].iov_base = &hc;
	iv[1].iov_len  = HCI_COMMAND_HDR_SIZE;
	ivn = 2;

	if (plen) {
		iv[2].iov_base = param;
		iv[2].iov_len  = plen;
		ivn = 3;
	}

	while (writev(dd, iv, ivn) < 0) {
		if (errno == EAGAIN || errno == EINTR)
			continue;
		return -1;
	}
	return 0;
}

int hci_send_req(int dd, struct hci_request *r, int to)
{
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	uint16_t opcode = htobs(cmd_opcode_pack(r->ogf, r->ocf));
	struct hci_filter nf, of;
	socklen_t olen;
	hci_event_hdr *hdr;
	int err, try;

	olen = sizeof(of);
	if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0)
		return -1;

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT,  &nf);
	hci_filter_set_event(EVT_CMD_STATUS, &nf);
	hci_filter_set_event(EVT_CMD_COMPLETE, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	hci_filter_set_event(r->event, &nf);
	hci_filter_set_opcode(opcode, &nf);
	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
		return -1;

	if (hci_send_cmd(dd, r->ogf, r->ocf, r->clen, r->cparam) < 0)
		goto failed;

	try = 10;
	while (try--) {
		evt_cmd_complete *cc;
		evt_cmd_status *cs;
		evt_remote_name_req_complete *rn;
		evt_le_meta_event *me;
		remote_name_req_cp *cp;
		int len;

		if (to) {
			struct pollfd p;
			int n;

			p.fd = dd; p.events = POLLIN;
			while ((n = poll(&p, 1, to)) < 0) {
				if (errno == EAGAIN || errno == EINTR)
					continue;
				goto failed;
			}

			if (!n) {
				errno = ETIMEDOUT;
				goto failed;
			}

			to -= 10;
			if (to < 0)
				to = 0;

		}

		while ((len = read(dd, buf, sizeof(buf))) < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			goto failed;
		}

		hdr = (void *) (buf + 1);
		ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
		len -= (1 + HCI_EVENT_HDR_SIZE);

		switch (hdr->evt) {
		case EVT_CMD_STATUS:
			cs = (void *) ptr;

			if (cs->opcode != opcode)
				continue;

			if (r->event != EVT_CMD_STATUS) {
				if (cs->status) {
					errno = EIO;
					goto failed;
				}
				break;
			}

			r->rlen = MIN(len, r->rlen);
			memcpy(r->rparam, ptr, r->rlen);
			goto done;

		case EVT_CMD_COMPLETE:
			cc = (void *) ptr;

			if (cc->opcode != opcode)
				continue;

			ptr += EVT_CMD_COMPLETE_SIZE;
			len -= EVT_CMD_COMPLETE_SIZE;

			r->rlen = MIN(len, r->rlen);
			memcpy(r->rparam, ptr, r->rlen);
			goto done;

		case EVT_REMOTE_NAME_REQ_COMPLETE:
			if (hdr->evt != r->event)
				break;

			rn = (void *) ptr;
			cp = r->cparam;

			if (bacmp(&rn->bdaddr, &cp->bdaddr))
				continue;

			r->rlen = MIN(len, r->rlen);
			memcpy(r->rparam, ptr, r->rlen);
			goto done;

		case EVT_LE_META_EVENT:
			me = (void *) ptr;

			if (me->subevent != r->event)
				continue;

			len -= 1;
			r->rlen = MIN(len, r->rlen);
			memcpy(r->rparam, me->data, r->rlen);
			goto done;

		default:
			if (hdr->evt != r->event)
				break;

			r->rlen = MIN(len, r->rlen);
			memcpy(r->rparam, ptr, r->rlen);
			goto done;
		}
	}
	errno = ETIMEDOUT;

failed:
	err = errno;
	setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));
	errno = err;
	return -1;

done:
	setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));
	return 0;
}

int hci_create_connection(int dd, const bdaddr_t *bdaddr, uint16_t ptype,
				uint16_t clkoffset, uint8_t rswitch,
				uint16_t *handle, int to)
{
	evt_conn_complete rp;
	create_conn_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.bdaddr, bdaddr);
	cp.pkt_type       = ptype;
	cp.pscan_rep_mode = 0x02;
	cp.clock_offset   = clkoffset;
	cp.role_switch    = rswitch;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_CREATE_CONN;
	rq.event  = EVT_CONN_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = CREATE_CONN_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_CONN_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*handle = rp.handle;
	return 0;
}

int hci_disconnect(int dd, uint16_t handle, uint8_t reason, int to)
{
	evt_disconn_complete rp;
	disconnect_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle = handle;
	cp.reason = reason;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_DISCONNECT;
	rq.event  = EVT_DISCONN_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = DISCONNECT_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_DISCONN_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}
	return 0;
}

int hci_le_add_white_list(int dd, const bdaddr_t *bdaddr, uint8_t type, int to)
{
	struct hci_request rq;
	le_add_device_to_white_list_cp cp;
	uint8_t status;

	memset(&cp, 0, sizeof(cp));
	cp.bdaddr_type = type;
	bacpy(&cp.bdaddr, bdaddr);

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_ADD_DEVICE_TO_WHITE_LIST;
	rq.cparam = &cp;
	rq.clen = LE_ADD_DEVICE_TO_WHITE_LIST_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_le_rm_white_list(int dd, const bdaddr_t *bdaddr, uint8_t type, int to)
{
	struct hci_request rq;
	le_remove_device_from_white_list_cp cp;
	uint8_t status;

	memset(&cp, 0, sizeof(cp));
	cp.bdaddr_type = type;
	bacpy(&cp.bdaddr, bdaddr);

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_REMOVE_DEVICE_FROM_WHITE_LIST;
	rq.cparam = &cp;
	rq.clen = LE_REMOVE_DEVICE_FROM_WHITE_LIST_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_le_read_white_list_size(int dd, uint8_t *size, int to)
{
	struct hci_request rq;
	le_read_white_list_size_rp rp;

	memset(&rp, 0, sizeof(rp));
	memset(&rq, 0, sizeof(rq));

	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_READ_WHITE_LIST_SIZE;
	rq.rparam = &rp;
	rq.rlen = LE_READ_WHITE_LIST_SIZE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	if (size)
		*size = rp.size;

	return 0;
}

int hci_le_clear_white_list(int dd, int to)
{
	struct hci_request rq;
	uint8_t status;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_CLEAR_WHITE_LIST;
	rq.rparam = &status;
	rq.rlen = 1;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_local_name(int dd, int len, char *name, int to)
{
	read_local_name_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_LOCAL_NAME;
	rq.rparam = &rp;
	rq.rlen   = READ_LOCAL_NAME_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	rp.name[247] = '\0';
	strncpy(name, (char *) rp.name, len);
	return 0;
}

int hci_write_local_name(int dd, const char *name, int to)
{
	change_local_name_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	strncpy((char *) cp.name, name, sizeof(cp.name));

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_CHANGE_LOCAL_NAME;
	rq.cparam = &cp;
	rq.clen   = CHANGE_LOCAL_NAME_CP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	return 0;
}

int hci_read_remote_name_with_clock_offset(int dd, const bdaddr_t *bdaddr,
						uint8_t pscan_rep_mode,
						uint16_t clkoffset,
						int len, char *name, int to)
{
	evt_remote_name_req_complete rn;
	remote_name_req_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.bdaddr, bdaddr);
	cp.pscan_rep_mode = pscan_rep_mode;
	cp.clock_offset   = clkoffset;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_REMOTE_NAME_REQ;
	rq.cparam = &cp;
	rq.clen   = REMOTE_NAME_REQ_CP_SIZE;
	rq.event  = EVT_REMOTE_NAME_REQ_COMPLETE;
	rq.rparam = &rn;
	rq.rlen   = EVT_REMOTE_NAME_REQ_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rn.status) {
		errno = EIO;
		return -1;
	}

	rn.name[247] = '\0';
	strncpy(name, (char *) rn.name, len);
	return 0;
}

int hci_read_remote_name(int dd, const bdaddr_t *bdaddr, int len, char *name,
				int to)
{
	return hci_read_remote_name_with_clock_offset(dd, bdaddr, 0x02, 0x0000,
							len, name, to);
}

int hci_read_remote_name_cancel(int dd, const bdaddr_t *bdaddr, int to)
{
	remote_name_req_cancel_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.bdaddr, bdaddr);

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_REMOTE_NAME_REQ_CANCEL;
	rq.cparam = &cp;
	rq.clen   = REMOTE_NAME_REQ_CANCEL_CP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	return 0;
}

int hci_read_remote_version(int dd, uint16_t handle, struct hci_version *ver,
				int to)
{
	evt_read_remote_version_complete rp;
	read_remote_version_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle = handle;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_READ_REMOTE_VERSION;
	rq.event  = EVT_READ_REMOTE_VERSION_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = READ_REMOTE_VERSION_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_READ_REMOTE_VERSION_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	ver->manufacturer = btohs(rp.manufacturer);
	ver->lmp_ver      = rp.lmp_ver;
	ver->lmp_subver   = btohs(rp.lmp_subver);
	return 0;
}

int hci_read_remote_features(int dd, uint16_t handle, uint8_t *features, int to)
{
	evt_read_remote_features_complete rp;
	read_remote_features_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle = handle;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_READ_REMOTE_FEATURES;
	rq.event  = EVT_READ_REMOTE_FEATURES_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = READ_REMOTE_FEATURES_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_READ_REMOTE_FEATURES_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	if (features)
		memcpy(features, rp.features, 8);

	return 0;
}

int hci_read_remote_ext_features(int dd, uint16_t handle, uint8_t page,
					uint8_t *max_page, uint8_t *features,
					int to)
{
	evt_read_remote_ext_features_complete rp;
	read_remote_ext_features_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle   = handle;
	cp.page_num = page;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_READ_REMOTE_EXT_FEATURES;
	rq.event  = EVT_READ_REMOTE_EXT_FEATURES_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = READ_REMOTE_EXT_FEATURES_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_READ_REMOTE_EXT_FEATURES_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	if (max_page)
		*max_page = rp.max_page_num;

	if (features)
		memcpy(features, rp.features, 8);

	return 0;
}

int hci_read_clock_offset(int dd, uint16_t handle, uint16_t *clkoffset, int to)
{
	evt_read_clock_offset_complete rp;
	read_clock_offset_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle = handle;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_READ_CLOCK_OFFSET;
	rq.event  = EVT_READ_CLOCK_OFFSET_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = READ_CLOCK_OFFSET_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_READ_CLOCK_OFFSET_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*clkoffset = rp.clock_offset;
	return 0;
}

int hci_read_local_version(int dd, struct hci_version *ver, int to)
{
	read_local_version_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_INFO_PARAM;
	rq.ocf    = OCF_READ_LOCAL_VERSION;
	rq.rparam = &rp;
	rq.rlen   = READ_LOCAL_VERSION_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	ver->manufacturer = btohs(rp.manufacturer);
	ver->hci_ver      = rp.hci_ver;
	ver->hci_rev      = btohs(rp.hci_rev);
	ver->lmp_ver      = rp.lmp_ver;
	ver->lmp_subver   = btohs(rp.lmp_subver);
	return 0;
}

int hci_read_local_commands(int dd, uint8_t *commands, int to)
{
	read_local_commands_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_INFO_PARAM;
	rq.ocf    = OCF_READ_LOCAL_COMMANDS;
	rq.rparam = &rp;
	rq.rlen   = READ_LOCAL_COMMANDS_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	if (commands)
		memcpy(commands, rp.commands, 64);

	return 0;
}

int hci_read_local_features(int dd, uint8_t *features, int to)
{
	read_local_features_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_INFO_PARAM;
	rq.ocf    = OCF_READ_LOCAL_FEATURES;
	rq.rparam = &rp;
	rq.rlen   = READ_LOCAL_FEATURES_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	if (features)
		memcpy(features, rp.features, 8);

	return 0;
}

int hci_read_local_ext_features(int dd, uint8_t page, uint8_t *max_page,
				uint8_t *features, int to)
{
	read_local_ext_features_cp cp;
	read_local_ext_features_rp rp;
	struct hci_request rq;

	cp.page_num = page;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_INFO_PARAM;
	rq.ocf    = OCF_READ_LOCAL_EXT_FEATURES;
	rq.cparam = &cp;
	rq.clen   = READ_LOCAL_EXT_FEATURES_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = READ_LOCAL_EXT_FEATURES_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	if (max_page)
		*max_page = rp.max_page_num;

	if (features)
		memcpy(features, rp.features, 8);

	return 0;
}

int hci_read_bd_addr(int dd, bdaddr_t *bdaddr, int to)
{
	read_bd_addr_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_INFO_PARAM;
	rq.ocf    = OCF_READ_BD_ADDR;
	rq.rparam = &rp;
	rq.rlen   = READ_BD_ADDR_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	if (bdaddr)
		bacpy(bdaddr, &rp.bdaddr);

	return 0;
}

int hci_read_class_of_dev(int dd, uint8_t *cls, int to)
{
	read_class_of_dev_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_CLASS_OF_DEV;
	rq.rparam = &rp;
	rq.rlen   = READ_CLASS_OF_DEV_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	memcpy(cls, rp.dev_class, 3);
	return 0;
}

int hci_write_class_of_dev(int dd, uint32_t cls, int to)
{
	write_class_of_dev_cp cp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	cp.dev_class[0] = cls & 0xff;
	cp.dev_class[1] = (cls >> 8) & 0xff;
	cp.dev_class[2] = (cls >> 16) & 0xff;
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_CLASS_OF_DEV;
	rq.cparam = &cp;
	rq.clen   = WRITE_CLASS_OF_DEV_CP_SIZE;
	return hci_send_req(dd, &rq, to);
}

int hci_read_voice_setting(int dd, uint16_t *vs, int to)
{
	read_voice_setting_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_VOICE_SETTING;
	rq.rparam = &rp;
	rq.rlen   = READ_VOICE_SETTING_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*vs = rp.voice_setting;
	return 0;
}

int hci_write_voice_setting(int dd, uint16_t vs, int to)
{
	write_voice_setting_cp cp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	cp.voice_setting = vs;
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_VOICE_SETTING;
	rq.cparam = &cp;
	rq.clen   = WRITE_VOICE_SETTING_CP_SIZE;

	return hci_send_req(dd, &rq, to);
}

int hci_read_current_iac_lap(int dd, uint8_t *num_iac, uint8_t *lap, int to)
{
	read_current_iac_lap_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_CURRENT_IAC_LAP;
	rq.rparam = &rp;
	rq.rlen   = READ_CURRENT_IAC_LAP_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*num_iac = rp.num_current_iac;
	memcpy(lap, rp.lap, rp.num_current_iac * 3);
	return 0;
}

int hci_write_current_iac_lap(int dd, uint8_t num_iac, uint8_t *lap, int to)
{
	write_current_iac_lap_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.num_current_iac = num_iac;
	memcpy(&cp.lap, lap, num_iac * 3);

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_CURRENT_IAC_LAP;
	rq.cparam = &cp;
	rq.clen   = num_iac * 3 + 1;

	return hci_send_req(dd, &rq, to);
}

int hci_read_stored_link_key(int dd, bdaddr_t *bdaddr, uint8_t all, int to)
{
	read_stored_link_key_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.bdaddr, bdaddr);
	cp.read_all = all;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_STORED_LINK_KEY;
	rq.cparam = &cp;
	rq.clen   = READ_STORED_LINK_KEY_CP_SIZE;

	return hci_send_req(dd, &rq, to);
}

int hci_write_stored_link_key(int dd, bdaddr_t *bdaddr, uint8_t *key, int to)
{
	unsigned char cp[WRITE_STORED_LINK_KEY_CP_SIZE + 6 + 16];
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp[0] = 1;
	bacpy((bdaddr_t *) (cp + 1), bdaddr);
	memcpy(cp + 7, key, 16);

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_STORED_LINK_KEY;
	rq.cparam = &cp;
	rq.clen   = WRITE_STORED_LINK_KEY_CP_SIZE + 6 + 16;

	return hci_send_req(dd, &rq, to);
}

int hci_delete_stored_link_key(int dd, bdaddr_t *bdaddr, uint8_t all, int to)
{
	delete_stored_link_key_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	bacpy(&cp.bdaddr, bdaddr);
	cp.delete_all = all;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_DELETE_STORED_LINK_KEY;
	rq.cparam = &cp;
	rq.clen   = DELETE_STORED_LINK_KEY_CP_SIZE;

	return hci_send_req(dd, &rq, to);
}

int hci_authenticate_link(int dd, uint16_t handle, int to)
{
	auth_requested_cp cp;
	evt_auth_complete rp;
	struct hci_request rq;

	cp.handle = handle;

	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_AUTH_REQUESTED;
	rq.event  = EVT_AUTH_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = AUTH_REQUESTED_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_AUTH_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_encrypt_link(int dd, uint16_t handle, uint8_t encrypt, int to)
{
	set_conn_encrypt_cp cp;
	evt_encrypt_change rp;
	struct hci_request rq;

	cp.handle  = handle;
	cp.encrypt = encrypt;

	rq.ogf     = OGF_LINK_CTL;
	rq.ocf     = OCF_SET_CONN_ENCRYPT;
	rq.event   = EVT_ENCRYPT_CHANGE;
	rq.cparam  = &cp;
	rq.clen    = SET_CONN_ENCRYPT_CP_SIZE;
	rq.rparam  = &rp;
	rq.rlen    = EVT_ENCRYPT_CHANGE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_change_link_key(int dd, uint16_t handle, int to)
{
	change_conn_link_key_cp cp;
	evt_change_conn_link_key_complete rp;
	struct hci_request rq;

	cp.handle = handle;

	rq.ogf    = OGF_LINK_CTL;
	rq.ocf    = OCF_CHANGE_CONN_LINK_KEY;
	rq.event  = EVT_CHANGE_CONN_LINK_KEY_COMPLETE;
	rq.cparam = &cp;
	rq.clen   = CHANGE_CONN_LINK_KEY_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_CHANGE_CONN_LINK_KEY_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_switch_role(int dd, bdaddr_t *bdaddr, uint8_t role, int to)
{
	switch_role_cp cp;
	evt_role_change rp;
	struct hci_request rq;

	bacpy(&cp.bdaddr, bdaddr);
	cp.role   = role;
	rq.ogf    = OGF_LINK_POLICY;
	rq.ocf    = OCF_SWITCH_ROLE;
	rq.cparam = &cp;
	rq.clen   = SWITCH_ROLE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_ROLE_CHANGE_SIZE;
	rq.event  = EVT_ROLE_CHANGE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_park_mode(int dd, uint16_t handle, uint16_t max_interval,
			uint16_t min_interval, int to)
{
	park_mode_cp cp;
	evt_mode_change rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof (cp));
	cp.handle       = handle;
	cp.max_interval = max_interval;
	cp.min_interval = min_interval;

	memset(&rq, 0, sizeof (rq));
	rq.ogf    = OGF_LINK_POLICY;
	rq.ocf    = OCF_PARK_MODE;
	rq.event  = EVT_MODE_CHANGE;
	rq.cparam = &cp;
	rq.clen   = PARK_MODE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_MODE_CHANGE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_exit_park_mode(int dd, uint16_t handle, int to)
{
	exit_park_mode_cp cp;
	evt_mode_change rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof (cp));
	cp.handle = handle;

	memset (&rq, 0, sizeof (rq));
	rq.ogf    = OGF_LINK_POLICY;
	rq.ocf    = OCF_EXIT_PARK_MODE;
	rq.event  = EVT_MODE_CHANGE;
	rq.cparam = &cp;
	rq.clen   = EXIT_PARK_MODE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = EVT_MODE_CHANGE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_inquiry_scan_type(int dd, uint8_t *type, int to)
{
	read_inquiry_scan_type_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_INQUIRY_SCAN_TYPE;
	rq.rparam = &rp;
	rq.rlen   = READ_INQUIRY_SCAN_TYPE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*type = rp.type;
	return 0;
}

int hci_write_inquiry_scan_type(int dd, uint8_t type, int to)
{
	write_inquiry_scan_type_cp cp;
	write_inquiry_scan_type_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.type = type;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_INQUIRY_SCAN_TYPE;
	rq.cparam = &cp;
	rq.clen   = WRITE_INQUIRY_SCAN_TYPE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_INQUIRY_SCAN_TYPE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_inquiry_mode(int dd, uint8_t *mode, int to)
{
	read_inquiry_mode_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_INQUIRY_MODE;
	rq.rparam = &rp;
	rq.rlen   = READ_INQUIRY_MODE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*mode = rp.mode;
	return 0;
}

int hci_write_inquiry_mode(int dd, uint8_t mode, int to)
{
	write_inquiry_mode_cp cp;
	write_inquiry_mode_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.mode = mode;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_INQUIRY_MODE;
	rq.cparam = &cp;
	rq.clen   = WRITE_INQUIRY_MODE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_INQUIRY_MODE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_afh_mode(int dd, uint8_t *mode, int to)
{
	read_afh_mode_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_AFH_MODE;
	rq.rparam = &rp;
	rq.rlen   = READ_AFH_MODE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*mode = rp.mode;
	return 0;
}

int hci_write_afh_mode(int dd, uint8_t mode, int to)
{
	write_afh_mode_cp cp;
	write_afh_mode_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.mode = mode;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_AFH_MODE;
	rq.cparam = &cp;
	rq.clen   = WRITE_AFH_MODE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_AFH_MODE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_ext_inquiry_response(int dd, uint8_t *fec, uint8_t *data, int to)
{
	read_ext_inquiry_response_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_EXT_INQUIRY_RESPONSE;
	rq.rparam = &rp;
	rq.rlen   = READ_EXT_INQUIRY_RESPONSE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*fec = rp.fec;
	memcpy(data, rp.data, HCI_MAX_EIR_LENGTH);

	return 0;
}

int hci_write_ext_inquiry_response(int dd, uint8_t fec, uint8_t *data, int to)
{
	write_ext_inquiry_response_cp cp;
	write_ext_inquiry_response_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.fec = fec;
	memcpy(cp.data, data, HCI_MAX_EIR_LENGTH);

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_EXT_INQUIRY_RESPONSE;
	rq.cparam = &cp;
	rq.clen   = WRITE_EXT_INQUIRY_RESPONSE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_EXT_INQUIRY_RESPONSE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_simple_pairing_mode(int dd, uint8_t *mode, int to)
{
	read_simple_pairing_mode_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_SIMPLE_PAIRING_MODE;
	rq.rparam = &rp;
	rq.rlen   = READ_SIMPLE_PAIRING_MODE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*mode = rp.mode;
	return 0;
}

int hci_write_simple_pairing_mode(int dd, uint8_t mode, int to)
{
	write_simple_pairing_mode_cp cp;
	write_simple_pairing_mode_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.mode = mode;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_SIMPLE_PAIRING_MODE;
	rq.cparam = &cp;
	rq.clen   = WRITE_SIMPLE_PAIRING_MODE_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_SIMPLE_PAIRING_MODE_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_local_oob_data(int dd, uint8_t *hash, uint8_t *randomizer, int to)
{
	read_local_oob_data_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_LOCAL_OOB_DATA;
	rq.rparam = &rp;
	rq.rlen   = READ_LOCAL_OOB_DATA_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	memcpy(hash, rp.hash, 16);
	memcpy(randomizer, rp.randomizer, 16);
	return 0;
}

int hci_read_inq_response_tx_power_level(int dd, int8_t *level, int to)
{
	read_inq_response_tx_power_level_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_INQ_RESPONSE_TX_POWER_LEVEL;
	rq.rparam = &rp;
	rq.rlen   = READ_INQ_RESPONSE_TX_POWER_LEVEL_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*level = rp.level;
	return 0;
}

int hci_read_inquiry_transmit_power_level(int dd, int8_t *level, int to)
{
	return hci_read_inq_response_tx_power_level(dd, level, to);
}

int hci_write_inquiry_transmit_power_level(int dd, int8_t level, int to)
{
	write_inquiry_transmit_power_level_cp cp;
	write_inquiry_transmit_power_level_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.level = level;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL;
	rq.cparam = &cp;
	rq.clen   = WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_transmit_power_level(int dd, uint16_t handle, uint8_t type,
					int8_t *level, int to)
{
	read_transmit_power_level_cp cp;
	read_transmit_power_level_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle = handle;
	cp.type   = type;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_TRANSMIT_POWER_LEVEL;
	rq.cparam = &cp;
	rq.clen   = READ_TRANSMIT_POWER_LEVEL_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = READ_TRANSMIT_POWER_LEVEL_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*level = rp.level;
	return 0;
}

int hci_read_link_policy(int dd, uint16_t handle, uint16_t *policy, int to)
{
	read_link_policy_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_POLICY;
	rq.ocf    = OCF_READ_LINK_POLICY;
	rq.cparam = &handle;
	rq.clen   = 2;
	rq.rparam = &rp;
	rq.rlen   = READ_LINK_POLICY_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*policy = rp.policy;
	return 0;
}

int hci_write_link_policy(int dd, uint16_t handle, uint16_t policy, int to)
{
	write_link_policy_cp cp;
	write_link_policy_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle = handle;
	cp.policy = policy;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_LINK_POLICY;
	rq.ocf    = OCF_WRITE_LINK_POLICY;
	rq.cparam = &cp;
	rq.clen   = WRITE_LINK_POLICY_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_LINK_POLICY_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_link_supervision_timeout(int dd, uint16_t handle,
					uint16_t *timeout, int to)
{
	read_link_supervision_timeout_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_READ_LINK_SUPERVISION_TIMEOUT;
	rq.cparam = &handle;
	rq.clen   = 2;
	rq.rparam = &rp;
	rq.rlen   = READ_LINK_SUPERVISION_TIMEOUT_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*timeout = rp.timeout;
	return 0;
}

int hci_write_link_supervision_timeout(int dd, uint16_t handle,
					uint16_t timeout, int to)
{
	write_link_supervision_timeout_cp cp;
	write_link_supervision_timeout_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle  = handle;
	cp.timeout = timeout;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_WRITE_LINK_SUPERVISION_TIMEOUT;
	rq.cparam = &cp;
	rq.clen   = WRITE_LINK_SUPERVISION_TIMEOUT_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = WRITE_LINK_SUPERVISION_TIMEOUT_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_set_afh_classification(int dd, uint8_t *map, int to)
{
	set_afh_classification_cp cp;
	set_afh_classification_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	memcpy(cp.map, map, 10);

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_HOST_CTL;
	rq.ocf    = OCF_SET_AFH_CLASSIFICATION;
	rq.cparam = &cp;
	rq.clen   = SET_AFH_CLASSIFICATION_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = SET_AFH_CLASSIFICATION_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_read_link_quality(int dd, uint16_t handle, uint8_t *link_quality,
				int to)
{
	read_link_quality_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_STATUS_PARAM;
	rq.ocf    = OCF_READ_LINK_QUALITY;
	rq.cparam = &handle;
	rq.clen   = 2;
	rq.rparam = &rp;
	rq.rlen   = READ_LINK_QUALITY_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*link_quality = rp.link_quality;
	return 0;
}

int hci_read_rssi(int dd, uint16_t handle, int8_t *rssi, int to)
{
	read_rssi_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_STATUS_PARAM;
	rq.ocf    = OCF_READ_RSSI;
	rq.cparam = &handle;
	rq.clen   = 2;
	rq.rparam = &rp;
	rq.rlen   = READ_RSSI_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*rssi = rp.rssi;
	return 0;
}

int hci_read_afh_map(int dd, uint16_t handle, uint8_t *mode, uint8_t *map,
			int to)
{
	read_afh_map_rp rp;
	struct hci_request rq;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_STATUS_PARAM;
	rq.ocf    = OCF_READ_AFH_MAP;
	rq.cparam = &handle;
	rq.clen   = 2;
	rq.rparam = &rp;
	rq.rlen   = READ_AFH_MAP_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*mode = rp.mode;
	memcpy(map, rp.map, 10);
	return 0;
}

int hci_read_clock(int dd, uint16_t handle, uint8_t which, uint32_t *clock,
			uint16_t *accuracy, int to)
{
	read_clock_cp cp;
	read_clock_rp rp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle      = handle;
	cp.which_clock = which;

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_STATUS_PARAM;
	rq.ocf    = OCF_READ_CLOCK;
	rq.cparam = &cp;
	rq.clen   = READ_CLOCK_CP_SIZE;
	rq.rparam = &rp;
	rq.rlen   = READ_CLOCK_RP_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (rp.status) {
		errno = EIO;
		return -1;
	}

	*clock    = rp.clock;
	*accuracy = rp.accuracy;
	return 0;
}

int hci_le_set_scan_enable(int dd, uint8_t enable, uint8_t filter_dup, int to)
{
	struct hci_request rq;
	le_set_scan_enable_cp scan_cp;
	uint8_t status;

	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable = enable;
	scan_cp.filter_dup = filter_dup;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_SCAN_ENABLE;
	rq.cparam = &scan_cp;
	rq.clen = LE_SET_SCAN_ENABLE_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_le_set_scan_parameters(int dd, uint8_t type,
					uint16_t interval, uint16_t window,
					uint8_t own_type, uint8_t filter, int to)
{
	struct hci_request rq;
	le_set_scan_parameters_cp param_cp;
	uint8_t status;

	memset(&param_cp, 0, sizeof(param_cp));
	param_cp.type = type;
	param_cp.interval = interval;
	param_cp.window = window;
	param_cp.own_bdaddr_type = own_type;
	param_cp.filter = filter;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_SCAN_PARAMETERS;
	rq.cparam = &param_cp;
	rq.clen = LE_SET_SCAN_PARAMETERS_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_le_set_advertise_enable(int dd, uint8_t enable, int to)
{
	struct hci_request rq;
	le_set_advertise_enable_cp adv_cp;
	uint8_t status;

	memset(&adv_cp, 0, sizeof(adv_cp));
	adv_cp.enable = enable;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
	rq.cparam = &adv_cp;
	rq.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (status) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int hci_le_create_conn(int dd, uint16_t interval, uint16_t window,
		uint8_t initiator_filter, uint8_t peer_bdaddr_type,
		bdaddr_t peer_bdaddr, uint8_t own_bdaddr_type,
		uint16_t min_interval, uint16_t max_interval,
		uint16_t latency, uint16_t supervision_timeout,
		uint16_t min_ce_length, uint16_t max_ce_length,
		uint16_t *handle, int to)
{
	struct hci_request rq;
	le_create_connection_cp create_conn_cp;
	evt_le_connection_complete conn_complete_rp;

	memset(&create_conn_cp, 0, sizeof(create_conn_cp));
	create_conn_cp.interval = interval;
	create_conn_cp.window = window;
	create_conn_cp.initiator_filter = initiator_filter;
	create_conn_cp.peer_bdaddr_type = peer_bdaddr_type;
	create_conn_cp.peer_bdaddr = peer_bdaddr;
	create_conn_cp.own_bdaddr_type = own_bdaddr_type;
	create_conn_cp.min_interval = min_interval;
	create_conn_cp.max_interval = max_interval;
	create_conn_cp.latency = latency;
	create_conn_cp.supervision_timeout = supervision_timeout;
	create_conn_cp.min_ce_length = min_ce_length;
	create_conn_cp.max_ce_length = max_ce_length;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_CREATE_CONN;
	rq.event = EVT_LE_CONN_COMPLETE;
	rq.cparam = &create_conn_cp;
	rq.clen = LE_CREATE_CONN_CP_SIZE;
	rq.rparam = &conn_complete_rp;
	rq.rlen = EVT_CONN_COMPLETE_SIZE;

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (conn_complete_rp.status) {
		errno = EIO;
		return -1;
	}

	if (handle)
		*handle = conn_complete_rp.handle;

	return 0;
}

int hci_le_conn_update(int dd, uint16_t handle, uint16_t min_interval,
			uint16_t max_interval, uint16_t latency,
			uint16_t supervision_timeout, int to)
{
	evt_le_connection_update_complete evt;
	le_connection_update_cp cp;
	struct hci_request rq;

	memset(&cp, 0, sizeof(cp));
	cp.handle = handle;
	cp.min_interval = min_interval;
	cp.max_interval = max_interval;
	cp.latency = latency;
	cp.supervision_timeout = supervision_timeout;
	cp.min_ce_length = htobs(0x0001);
	cp.max_ce_length = htobs(0x0001);

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_CONN_UPDATE;
	rq.cparam = &cp;
	rq.clen = LE_CONN_UPDATE_CP_SIZE;
	rq.event = EVT_LE_CONN_UPDATE_COMPLETE;
	rq.rparam = &evt;
	rq.rlen = sizeof(evt);

	if (hci_send_req(dd, &rq, to) < 0)
		return -1;

	if (evt.status) {
		errno = EIO;
		return -1;
	}

	return 0;
}
