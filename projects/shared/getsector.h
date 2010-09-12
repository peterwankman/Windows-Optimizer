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

#ifndef GETSECTOR_H_
#define GETSECTOR_H_

#include <Windows.h>

typedef struct {
	int SectorsPerCluster;
	int BytesPerSector;
} fsinfo_t;

typedef struct {
	BOOL Fragmented;
	LONGLONG LCN;
	LONGLONG StartOffset;
} fileinfo_t;

fsinfo_t GetFsInfo(LPCWSTR File);
fileinfo_t GetPosition(LPCWSTR File, fsinfo_t FsInfo);

#endif