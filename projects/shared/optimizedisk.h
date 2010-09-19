/* 
 * Optimizer -- Speed up Windows
 * 
 * Copyright (C) 2007-2010  Anonymous
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to 
 * the Free Software Foundation, Inc.
 * 51 Franklin Street, Fifth Floor
 * Boston, MA  02110-1301, USA
 * 
 */

#ifndef _OPTIMIZEDISK_H_
#define _OPTIMIZEDISK_H_

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

typedef struct {
	uchar h, s;
	short c;
} chs_t;

typedef struct {
	uchar boot;			// 1 byte
	chs_t start_chs;	// 3 bytes */
	uchar type;			// 1 byte
	chs_t end_chs;		// 3 bytes */
	uint start_lba;		// 4 bytes
	uint size;			// 4 bytes
} partinfo_t;

void OptimizeDisk(void);

#endif