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

#include "getsector.h"

#include <stdio.h>
#include <Windows.h>
#include <WinIoCtl.h>

fsinfo_t GetFsInfo(LPCWSTR File) {
    fsinfo_t Output;
    BOOL RetVal;
    wchar_t VolumeName[MAX_PATH];
    int NumberOfFreeClusters, TotalNumberOfClusters;

    RetVal = GetVolumePathName(File, VolumeName, sizeof(VolumeName));
    if(!RetVal) {
        Output.BytesPerSector = Output.SectorsPerCluster = -1;
        return Output;
    }

    RetVal = GetDiskFreeSpace(VolumeName,
        &Output.SectorsPerCluster,
        &Output.BytesPerSector,
        &NumberOfFreeClusters,
        &TotalNumberOfClusters);

    if(!RetVal) {
        Output.BytesPerSector = Output.SectorsPerCluster = -1;
        return Output;
    }

    return Output;
}

fileinfo_t GetPosition(LPCWSTR File, fsinfo_t FsInfo) {
    fileinfo_t Output;
    HANDLE FileHandle, DriveHandle;
    STARTING_VCN_INPUT_BUFFER InputBuffer;
    RETRIEVAL_POINTERS_BUFFER OutputBuffer;
    VOLUME_DISK_EXTENTS DiskExtents;

    DWORD BytesReturned, Error;
    BOOL RetVal;
    Output.Fragmented = 0;
    Output.LCN = 0;
    
	FileHandle = CreateFile(File,
        GENERIC_ALL,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if(FileHandle == INVALID_HANDLE_VALUE) {
        return Output;
    }

    InputBuffer.StartingVcn.QuadPart = 0;
    
    do {
        RetVal = DeviceIoControl(FileHandle,
            FSCTL_GET_RETRIEVAL_POINTERS,
            &InputBuffer, sizeof(STARTING_VCN_INPUT_BUFFER),
            &OutputBuffer, sizeof(RETRIEVAL_POINTERS_BUFFER),
            &BytesReturned,
            NULL);

        Error = GetLastError();

        switch(Error) {
        case ERROR_HANDLE_EOF:				
                break;
            case ERROR_MORE_DATA:
                InputBuffer.StartingVcn = OutputBuffer.Extents[0].NextVcn;
                Output.Fragmented = 1;
            case NO_ERROR:
                if(Output.LCN == 0)
                    Output.LCN = OutputBuffer.Extents[0].Lcn.QuadPart;
                break;
            default:                
                break;
        }
    } while (Error == ERROR_MORE_DATA);

	DriveHandle = CreateFile(TEXT("\\\\.\\C:"),
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_WRITE|FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    BytesReturned = 0;
    RetVal = DeviceIoControl(DriveHandle,
        IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
        NULL, 0,
        &DiskExtents, sizeof(VOLUME_DISK_EXTENTS),
        &BytesReturned,
        NULL);
	
    Output.StartOffset = Output.LCN * FsInfo.SectorsPerCluster + DiskExtents.Extents[0].StartingOffset.QuadPart / FsInfo.BytesPerSector;

	CloseHandle(FileHandle);	
	CloseHandle(DriveHandle);
		
	return Output;
}

