/*
 * Declarations for stream support for use with FileIOFunctions.c module.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICFILESTREAM_H_
#define _INCLUDE_BASICFILESTREAM_H_

#include "FileIOFunctions.h"  /* !!! temp !!! */

#ifdef __cplusplus
extern "C" {
#endif

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
__PBasicFileStream __getFileStream(short fileNumber);
__boolean __deleteFileStream(short fileNumber);
void __initBasicFileStream(void);
__boolean __invalidFileNumber(short fileNumber);
__PBasicFileStream __createFileStream(short fileNumber);

#ifdef __cplusplus
}
#endif

#endif
#endif

/* end of BasicFileStream.h ... */
