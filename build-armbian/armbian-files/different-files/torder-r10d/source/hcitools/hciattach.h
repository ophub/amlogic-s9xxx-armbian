/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2003-2010  Marcel Holtmann <marcel@holtmann.org>
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

#include <termios.h>

#include <bluetooth.h>

#ifndef N_HCI
#define N_HCI	15
#endif

#define HCIUARTSETPROTO		_IOW('U', 200, int)
#define HCIUARTGETPROTO		_IOR('U', 201, int)
#define HCIUARTGETDEVICE	_IOR('U', 202, int)
#define HCIUARTSETFLAGS		_IOW('U', 203, int)
#define HCIUARTGETFLAGS		_IOR('U', 204, int)

#define HCI_UART_H4	0
#define HCI_UART_BCSP	1
#define HCI_UART_3WIRE	2
#define HCI_UART_H4DS	3
#define HCI_UART_LL	4
#define HCI_UART_ATH3K  5

#define HCI_UART_RAW_DEVICE	0
#define HCI_UART_RESET_ON_INIT	1
#define HCI_UART_CREATE_AMP	2

struct uart_t {
	char *type;
	int  m_id;
	int  p_id;
	int  proto;
	int  init_speed;
	int  speed;
	int  flags;
	int  pm;
	char *bdaddr;
	int  (*init) (int fd, struct uart_t *u, struct termios *ti);
	int  (*post) (int fd, struct uart_t *u, struct termios *ti);
};

#define FLOW_CTL    0x0001
#define AMP_DEV     0x0002
#define ENABLE_PM   1
#define DISABLE_PM  0

int read_hci_event(int fd, unsigned char *buf, int size);
int set_speed(int fd, struct termios *ti, int speed);
int uart_speed(int speed);

int texas_init(int fd, int *speed, struct termios *ti);
int texas_post(int fd, struct termios *ti);
int texasalt_init(int fd, int speed, struct termios *ti);
int stlc2500_init(int fd, bdaddr_t *bdaddr);
int bgb2xx_init(int dd, bdaddr_t *bdaddr);
int ath3k_init(int fd, int speed, int init_speed, char *bdaddr,
						struct termios *ti);
int ath3k_post(int fd, int pm);
int qualcomm_init(int fd, int speed, struct termios *ti, const char *bdaddr);
int intel_init(int fd, int init_speed, int *speed, struct termios *ti);
int bcm43xx_init(int fd, int speed, struct termios *ti, const char *bdaddr);

//Realtek_add_start
//add realtek init and post process for realtek Bluetooth chip
int rtk_init(int fd, int proto, int speed, struct termios *ti);
int rtk_post(int fd, int proto, struct termios *ti);
//Realtek_add_end

// add xradio init and post process for xradio Bluetooth chip (xr829)
int xr_init(int fd, struct uart_t *u, struct termios *ti);
int xr_post(int fd, struct uart_t *u, struct termios *ti);

// add sprd init and post process for sprd Bluetooth chip (UWE5622)
int sprd_config_init(int fd, struct uart_t *u, struct termios *ti);
int sprd_config_post(int fd, struct uart_t *u, struct termios *ti);
