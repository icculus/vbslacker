/*
 * Declarations for stream support for use with FileIOFunctions.c module.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICFILESTREAM_H_
#define _INCLUDE_BASICFILESTREAM_H_

#include "FileIOFunctions.h"  /* !!! temp !!! */


#define MAX_FILE_HANDLES    511

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
__PBasicFileStream __getFileStream(STATEPARAMS, short fileNumber);
__boolean __deleteFileStream(STATEPARAMS, short fileNumber);
void __initBasicFileStream(STATEPARAMS);
__boolean __invalidFileNumber(STATEPARAMS, short fileNumber);
__PBasicFileStream __createFileStream(STATEPARAMS, short fileNumber);

#endif
#endif

/* end of BasicFileStream.h ... */
