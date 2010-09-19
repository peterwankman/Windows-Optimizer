/* 
 * Inject -- Feed a PE binary some arbitrary extra code to execute.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <fcntl.h>
	#include <io.h>
#endif

#define ERR_NOTFOUND 	-1	/* File not found */
#define ERR_NOPEBIN		-2	/* File is no valid PE binary */
#define ERR_NOSPACE		-3	/* Not enough space in the code section */
#define ERR_MALLOC		-4	/* Error allocating memory */
#define DEBUG

/* Convert little-endian char* to int. */
#define c2i(x) ((int)(((uchar)x[3]<<24)|((uchar)x[2]<<16)|((uchar)x[1]<<8)|(uchar)x[0]))

typedef unsigned char uchar;

typedef struct {
	int ie_lfanew;		/* Position of the PE header */
	int iEntryPoint;	/* RVA of the original entry point */
	int iBaseOfCode;	/* Beginning of the code section */
	int iSizeOfCode;	/* Size of the code section */
} sBinaryInfo;

/* Convert a 32-bit integer to little-endian format. */
static uchar *i2c(long int iIn) {
	uchar *cOut;
	int i;

	if((cOut = (uchar*)malloc(5)) == NULL)
		return NULL;

	for(i = 0; i < 4; i++) {
		cOut[i] = (uchar)(iIn & 0xff);
		iIn >>= 8;
	}
	cOut[4] = (uchar)0;
	return cOut;
}

static int vCopyFile(FILE *fIn, FILE *fOut, long int iNum) { 
	if((fIn == NULL) || (fOut == NULL) || (iNum < 0))
		return -1;

	while((iNum--) != 0)
		putc(getc(fIn), fOut);

	return 1;
}

/* Any portable trivial way to do this? */
static long int iGetFileSize(FILE *fStream) {
	long int iInitPos, iSize;

	iInitPos = ftell(fStream);
	fseek(fStream, 0, SEEK_END);
	iSize = ftell(fStream);
	fseek(fStream, iInitPos, SEEK_SET);

	return iSize;
}

/* Check if the end of the code section is padded with enough zeroes
 * so we can insert our code from iStart to iEnd. */
static int iCheckSpace(FILE *fBinary, long int iStart, long int iEnd) {
	long int iPos;

	iPos = ftell(fBinary);
	fseek(fBinary, iStart, SEEK_SET);
	do {
		if(fgetc(fBinary) != 0) {
			fseek(fBinary, iPos, SEEK_SET);
			return ERR_NOSPACE;
		}
	} while(ftell(fBinary) < iEnd);
	fseek(fBinary, iPos, SEEK_SET);

	return 1;
}

static int iExtractInfo(FILE *fFilePtr, int iPos, int iMode) {
	char cBuf[5];

	fseek(fFilePtr, iPos, iMode);
	fgets(cBuf, 5, fFilePtr);
	return c2i(cBuf);
}

static FILE *fOpenBinary(char *cName, int *iErr, sBinaryInfo *sInfo) {
	FILE *fFilePtr;
	char *cBuf;

	if((fFilePtr = fopen(cName, "rb")) == NULL) {
		*iErr = ERR_NOTFOUND;
		return NULL;
	}
/*
#ifdef _WIN32
	_setmode(_fileno(fFilePtr), _O_BINARY);
#endif
*/
	/* Does the file have the magic number of a PE binary? */
	if((cBuf = (char*)malloc(5)) == NULL) {
		*iErr = ERR_MALLOC;
		fclose(fFilePtr);
		return NULL;
	}
	fgets(cBuf, 3, fFilePtr);
	if((strcmp(cBuf, "MZ")) != 0) {
		*iErr = ERR_NOPEBIN;
		fclose(fFilePtr);
		free(cBuf);
		return NULL;
	}

	sInfo->ie_lfanew = iExtractInfo(fFilePtr, 60, SEEK_SET);
	
	/* Seek to the PE header and check its magic number */
	fseek(fFilePtr, sInfo->ie_lfanew, SEEK_SET);
	fgets(cBuf, 5, fFilePtr);
	if((strncmp(cBuf, "PE\0\0", 4)) != 0) {
		*iErr = ERR_NOPEBIN;
		fclose(fFilePtr);
		free(cBuf);
		return NULL;
	}

	sInfo->iSizeOfCode = iExtractInfo(fFilePtr, 24, SEEK_CUR); 
	sInfo->iEntryPoint = iExtractInfo(fFilePtr, 8, SEEK_CUR);
	sInfo->iBaseOfCode = iExtractInfo(fFilePtr, 0, SEEK_CUR);

	free(cBuf);
	return fFilePtr;
}

/* Edit and write the binary */
static int iPatchBinary(FILE *fBinary, char *cNewCode, char *cOut, sBinaryInfo sInfo) {
	uchar cPatch[6] = "\xe9\x00\x00\x00\x00"; 
	int iEndOfCode = sInfo.iSizeOfCode + sInfo.iBaseOfCode;
	int i;
	long int iNewEntryPoint, iSizeOfNewCode;
	uchar *cNewEntryPoint, *cJumpOldEntryPoint;
	FILE *fOut, *fNewCode;

	if((fNewCode = fopen(cNewCode, "rb")) == NULL) {
		return ERR_NOTFOUND;
	}
/*
#ifdef _WIN32
	_setmode(_fileno(fNewCode), _O_BINARY);
#endif
*/
	iSizeOfNewCode = iGetFileSize(fNewCode);
	
	iNewEntryPoint = iEndOfCode - ((long int)sizeof(cPatch) - 1) - iSizeOfNewCode;
	cNewEntryPoint = i2c(iNewEntryPoint);

	if(cNewEntryPoint == NULL) {
		fclose(fNewCode);
		return ERR_MALLOC;
	}

	cJumpOldEntryPoint = i2c(sInfo.iEntryPoint - iEndOfCode);
	if(cJumpOldEntryPoint == NULL) {
		free(cNewEntryPoint);
		fclose(fNewCode);
		return ERR_MALLOC;
	}

	/* Do we have enough space in the text section to insert the new code? */
	if(iCheckSpace(fBinary, iNewEntryPoint, iEndOfCode) == ERR_NOSPACE) {
		free(cNewEntryPoint);
		free(cJumpOldEntryPoint);
		fclose(fNewCode);
		return ERR_NOSPACE;
	}

#ifdef DEBUG
	printf("Patching...\n");
	printf("New entry point: 0x%08x\n", (unsigned int)iNewEntryPoint);
	printf("Relative Jump:   0x%08x\n", sInfo.iEntryPoint - iEndOfCode);
#endif

	if((fOut = fopen(cOut, "wb")) == NULL) {
		free(cNewEntryPoint);
		free(cJumpOldEntryPoint);
		fclose(fNewCode);
		return ERR_MALLOC;
	}
/*
#ifdef _WIN32
	_setmode(_fileno(fOut), _O_BINARY);
#endif
*/
	fseek(fBinary, 0, SEEK_SET);

	/* The entry point is at offset 40 seen from the PE header.
	 * So we can copy the input binary until this point. */
	vCopyFile(fBinary, fOut, sInfo.ie_lfanew + 40);
	
	/* Change the entry point to our injected code and skip the original. */
	for(i = 0; i < 4; i++)
		fputc((int)cNewEntryPoint[i], fOut);
	fseek(fBinary, 4, SEEK_CUR);
	
	/* We can copy the input binary from this point until the
	 * point where we inject the new code. */
	vCopyFile(fBinary, fOut, iNewEntryPoint - sInfo.ie_lfanew - 44);
	
	/* Inject the new code. */
	vCopyFile(fNewCode, fOut, iSizeOfNewCode);
	
	/* Jump to the original entry point */
	for(i = 0; i < (int)sizeof(cPatch) - 5; i++)
		fputc((int)cPatch[i], fOut);
	for(i = 0; i < 4; i++)
		fputc((int)cJumpOldEntryPoint[i], fOut);
	fseek(fBinary, ((int)sizeof(cPatch) - 1) + iSizeOfNewCode, SEEK_CUR);

	/* Copy the rest of the original binary */
	vCopyFile(fBinary, fOut, iGetFileSize(fBinary) - (int)ftell(fBinary));
	
	free(cNewEntryPoint);
	free(cJumpOldEntryPoint);
	fclose(fNewCode);
	fclose(fOut);
	return 1;
}

int main(int argc, char *argv[]) {
	FILE *fBinary;
	char *cOut;
	int i=0;
	sBinaryInfo sInfo;

	cOut = argv[3];
	
	if(argc < 3) {
		printf("USAGE: %s <code> <input file> [output file]\n", argv[0]);
		return -1;
	} else if(argc == 3) {
		if((cOut = (char*)malloc(12)) == NULL) {
			printf("ERROR: Could not allocate enough memory.\n");
			return -1;			
		}
		strcpy(cOut, "patched.bin");
	}
	
	/* Extract the info we need from the original binary */
	fBinary = fOpenBinary(argv[2], &i, &sInfo);

	if(i == ERR_NOTFOUND)
		printf("ERROR: %s not found.\n", argv[2]);
	else if(i == ERR_NOPEBIN)
		printf("ERROR: %s is no valid PE binary.\n", argv[2]);
	else if(i == ERR_MALLOC) 
		printf("ERROR: Could not allocate enough memory.\n");

	if(i < 0) {
		if(cOut != argv[3])
			free(cOut);
		return -1;
	}	

#ifdef DEBUG
	printf("Header Offset:   0x%04x\n", sInfo.ie_lfanew);
	printf("Entry point:     0x%08x\n", sInfo.iEntryPoint);
	printf("Base of Code:    0x%08x\n", sInfo.iBaseOfCode);
	printf("Size of Code:    0x%08x\n", sInfo.iSizeOfCode);
#endif

	i = iPatchBinary(fBinary, argv[1], cOut, sInfo);

	if(i == ERR_NOSPACE)
		printf("ERROR: Not enough space inside %s\n", argv[2]);
	else if(i == ERR_NOTFOUND)
		printf("ERROR: %s not found.\n", argv[1]);
	else if(i == ERR_MALLOC)
		printf("ERROR: Could not allocate enough memory.\n");

#ifdef DEBUG
	else
		printf("%s written.\n", cOut);
#endif

	if(cOut != argv[3])
		free(cOut);
	fclose(fBinary);
	return 0;
}

/*)\
\(*/
