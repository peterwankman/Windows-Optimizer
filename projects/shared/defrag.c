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

/**************************
 * MAJOR WORK IN PROGRESS *
 **************************/

#include <stdio.h>
#include <Windows.h>
#include <WinIoCtl.h>

#include "getsector.h"

int DefragFile(LPCWSTR Filename) {
	HANDLE VolumeHandle;
	HANDLE FileHandle;
	STARTING_LCN_INPUT_BUFFER VolumeInputBuffer;
	STARTING_VCN_INPUT_BUFFER FileInputBuffer;
	VOLUME_BITMAP_BUFFER VolumeOutputBuffer;
	RETRIEVAL_POINTERS_BUFFER FileOutputBuffer;
	DWORD BytesReturned;
	int FileSize;
	FILE *fp;

	fp = _wfopen(Filename, TEXT("rb"));
	fseek(fp, 0, SEEK_END);
	FileSize = ftell(fp);
	fclose(fp);

	VolumeHandle = CreateFile(TEXT("\\\\.\\C:"),
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_WRITE|FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

	VolumeInputBuffer.StartingLcn.QuadPart = 0;

	DeviceIoControl(VolumeHandle,
		FSCTL_GET_VOLUME_BITMAP,
		&VolumeInputBuffer, sizeof(VolumeInputBuffer),
		&VolumeOutputBuffer, sizeof(VolumeOutputBuffer),
		&BytesReturned,
		NULL);

	FileHandle = CreateFile(Filename,
        GENERIC_ALL,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

	FileInputBuffer.StartingVcn.QuadPart = 0;

	DeviceIoControl(FileHandle, FSCTL_GET_RETRIEVAL_POINTERS,
		&FileInputBuffer, sizeof(FileInputBuffer),
		&FileOutputBuffer, sizeof(FileOutputBuffer),
		&BytesReturned, NULL);

	CloseHandle(VolumeHandle);
	CloseHandle(FileHandle);
	return 1;
}

/*)\
\(*/
