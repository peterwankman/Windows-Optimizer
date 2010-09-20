/* 
 * bmp2bin -- Convert a Windows Bitmap file to raw data.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <fcntl.h>
	#include <io.h>
#endif

#define	REQ_WIDTH	640
#define REQ_HEIGHT	480
#define REQ_BPP		24


typedef struct {
	char magic[2];
	int size;
	int res;
	int offset;
} bmp_header_t;

typedef struct {
	int size;
	int width;
	int height;
	char planes[2];
	char bpp[2];
} dib_header_t;

typedef struct {
	char B;
	char G;
	char R;
	char reserved;
} rgbquad_t;

void writepixel(char *inpixel, rgbquad_t *palette, int offset, char *buf) {
	switch(REQ_BPP) {
		case 8:
			buf[offset] = inpixel[0];
			break;
		case 24:
			buf[offset] = inpixel[0];
			buf[offset + 1] = inpixel[1];
			buf[offset + 2] = inpixel[2];
			break;
	}
}

bmp_header_t readbmpheader (FILE *in) {
	bmp_header_t out;
	memset(&out, 0, sizeof(bmp_header_t));

	fread(&out.magic, sizeof(out.magic), 1, in);
	fread(&out.size, sizeof(out.size), 1, in);
	fread(&out.res, sizeof(out.res), 1, in);
	fread(&out.offset, sizeof(out.offset), 1, in);

	return out;
}

dib_header_t readdibheader (FILE *in) {
	dib_header_t out;
	memset(&out, 0, sizeof(bmp_header_t));

	fread(&out.size, sizeof(out.size), 1, in);
	fread(&out.width, sizeof(out.width), 1, in);
	fread(&out.height, sizeof(out.height), 1, in);
	fread(&out.planes, sizeof(out.planes), 1, in);
	fread(&out.bpp, sizeof(out.bpp), 1, in);

	return out;
}

rgbquad_t *readpalette(char *palbuf) {
	rgbquad_t *out;

	if((out = (rgbquad_t*)malloc(256 * sizeof(rgbquad_t))) == NULL)
		return NULL;

	return out;
}

int main(int argc, char **argv) {
	FILE *in, *out;
	bmp_header_t bmp_header;
	dib_header_t dib_header;
	rgbquad_t *palette;

	int i, j, k, add, bufsize, offset;	
	char buf[3], palbuf[1024];
	char *outbuf;

	if(argc < 3) {
		fprintf(stderr, "USAGE: %s <INPUT> <OUTPUT>\n", argv[0]);
		return -1;
	}

	printf("Opening input file... ");
	if((in = fopen(argv[1], "rb")) == NULL) {
		printf("ERROR: Could not open input file for reading.\n");
		return -1;
	}

	printf("OK\n");

	bmp_header = readbmpheader(in);
	dib_header = readdibheader(in);

	printf("Checking magic number... ");
	if(strncmp(bmp_header.magic, "BM", 2)) {
		printf("ERROR: Not a bitmap file.\n");
		fclose(in);
		return -1;
	}
	printf("OK\n");

	printf("Checking header size... ");
	printf("%d. ", dib_header.size);
	if(dib_header.size != 40) {
		printf("ERROR: Not Windows V3\n");
		fclose(in);
		return -1;
	}
	printf("OK\n");

	printf("Checking dimensions... ");
	printf("%dx%dx%dbbp. ", dib_header.width, dib_header.height, dib_header.bpp[0]);
	if((dib_header.width != REQ_WIDTH) || 
	   (dib_header.height != REQ_HEIGHT) ||
	   (dib_header.bpp[0] != REQ_BPP)) {
		printf("ERROR. %dx%dx%d required.\n", REQ_WIDTH, REQ_HEIGHT, REQ_BPP);
		fclose(in);
		return -1;
	}
	printf("OK\n\n");

	if(REQ_BPP == 8) {
		printf("Reading palette... ");
		fseek(in, sizeof(bmp_header_t) + dib_header.size, SEEK_SET);
		fread(palbuf, 1024, 1, in);
		palette = readpalette(palbuf);
		printf("OK\n");
	} else
		palette = NULL;

	printf("Opening output file... ");
	if((out = fopen(argv[2], "wb")) == NULL) {
		printf("ERROR: Could not open output file for writing.\n");
		fclose(in);
		return -1;
	}

	printf("OK\n");
	
	fseek(in, bmp_header.offset, SEEK_SET);
	printf("Filling output buffer... ");
	bufsize = REQ_HEIGHT * REQ_WIDTH * (REQ_BPP / 8);
	add = (REQ_BPP == 8)?1024:0;

	if((outbuf = (char*)malloc(bufsize + add)) == NULL) {
		fprintf(stderr, "ERROR: malloc(%d) failed.\n", bufsize);
		fclose(out);
		fclose(in);
		return -1;
	}

	for(i = 0; i < add; i++)
		outbuf[i] = palbuf[i];

	for(i = 0; i < REQ_HEIGHT; i++) {
		offset = (REQ_BPP / 8) * REQ_WIDTH * (REQ_HEIGHT - 1 - i) + add;
		for(j = 0; j < REQ_WIDTH; j++) {
			fread(buf, (REQ_BPP / 8), 1, in);
			writepixel(buf, palette, offset + (j * REQ_BPP / 8), outbuf);
		}
		for(k = 0; k < ((REQ_WIDTH * REQ_BPP / 8) % 4); k++)
			fgetc(in);
	}

	printf("Done.\nWriting file... ");
	fwrite(outbuf, bufsize + add, 1, out);
	printf("Done.\n");

	if(palette)
		free(palette);
	free(outbuf);
	fclose(out);
	fclose(in);

	return 0;
}