/* 
 * mkrand -- Generate a file full of random data.
 * 
 * Copyright (C) 2008-2010  Anonymous
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
#include <time.h>
#include <windows.h>

#define MINSIZE 524288 /* 2^19 */

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					 LPSTR lpCmdLine, int nCmdShow) {

	FILE *fp;
	char buf[128];
	int i, add;

	fp = fopen("a.out", "wb");
	if(fp == NULL)
		return -1;

	srand(time(NULL));

	do {
			add = (rand() << 5) | rand() & 31;
			sprintf(buf, "%d bytes.", MINSIZE + add);
	} while (MessageBox(0, buf, "Output:", MB_OKCANCEL) == 2);
	
	for(i = 0; i < MINSIZE + add; i++)
		fputc(rand() & 255, fp);

	fclose(fp);
	return 0;
}