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

#include <Windows.h>

#include "help.h"

LPWSTR CharToWSTR(char *Input) {
	size_t OriginalSize;
	LPWSTR WCString;
	size_t ConvertedChars = 0;

	if((WCString = malloc(strlen(Input) * 2 + 1)) == NULL) {
		return NULL;
	}

	OriginalSize = strlen(Input) + 1;
	mbstowcs_s(&ConvertedChars, WCString, OriginalSize, Input, _TRUNCATE);	

	return WCString;
}

LPSTR GetSystemVolume(void) {
	LPSTR buf;
	
	if((buf = (LPSTR)malloc(16)) == NULL)
		 return NULL;

	ExpandEnvironmentStringsA("%SystemDrive%", buf, 16);
	
	return buf;
}

LPWSTR GetSystemVolume_w(void) {
	LPWSTR buf;
	
	if((buf = (LPWSTR)malloc(16)) == NULL)
		 return NULL;

	ExpandEnvironmentStringsW(TEXT("%SystemDrive%"), buf, 16);
	
	return buf;
}