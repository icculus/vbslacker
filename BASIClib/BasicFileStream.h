/*
 * Declarations for stream support for use with FileIOFunctions.c module.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */
#include "Boolean.h"
#include "BasicString.h"
#include "FileIOFunctions.h"

#ifndef _INCLUDE_BASICFILESTREAM_H_
#define _INCLUDE_BASICFILESTREAM_H_

typedef struct
{
	PBasicString	pathName;
	FileModeEnum	mode;
	FileAccessEnum	access;
	FileLockEnum	lock;
	FILE			*fileStream;
	short			recLength;
} __BasicFileStream;

typedef __BasicFileStream *__PBasicFileStream;

/*** Function Declarations ***/
__PBasicFileStream __getFileStream(short fileNumber);
boolean __deleteFileStream(short fileNumber);
void __initBasicFileStream(void);
boolean __invalidFileNumber(short fileNumber);
__PBasicFileStream __createFileStream(short fileNumber);

#endif
/* end of BasicFileStream.h ... */
