/* 
 * Optimizer -- Speed up Windows
 * 
 * Copyright (C) 2007-2011  Anonymous
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

#include <Shlwapi.h>

#ifndef _OPTIMIZEREG_H_
#define _OPTIMIZEREG_H_

#define HKEY_BASE	0x80000000
#define OPT_ROOT	0x00
#define OPT_CURR	0x01
#define OPT_MACH	0x02
#define OPT_USER	0x03
#define OPT_PERF	0x04
#define OPT_CONF	0x05

#define makekey(x) ((HKEY)(HKEY_BASE + x))

void OptimizeKey(int hive, char *key);

#endif

/*)\
\(*/
