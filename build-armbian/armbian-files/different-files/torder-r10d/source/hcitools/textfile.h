/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
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

#ifndef __TEXTFILE_H
#define __TEXTFILE_H

int create_file(const char *filename, const mode_t mode);
int create_name(char *buf, size_t size, const char *path,
				const char *address, const char *name);

int textfile_put(const char *pathname, const char *key, const char *value);
int textfile_del(const char *pathname, const char *key);
char *textfile_get(const char *pathname, const char *key);

typedef void (*textfile_cb) (char *key, char *value, void *data);

int textfile_foreach(const char *pathname, textfile_cb func, void *data);

#endif /* __TEXTFILE_H */
