/*
 * Header file for BASIClib's file i/o API.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _INCLUDE_FILEIOFUNCTIONS_H_
#define _INCLUDE_FILEIOFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    _vbFileModeRandom,
    _vbFileModeBinary,
    _vbFileModeInput,
    _vbFileModeOutput,
    _vbFileModeAppend,
    _vbFileModeTERMINATION,     /* don't use any entries from here down. */
    _vbFileModeDefault
} _vbFileMode;

typedef enum
{
    _vbFileAccessRead,
    _vbFileAccessWrite,
    _vbFileAccessReadWrite,
    _vbFileAccessTERMINATION,   /* don't use any entries from here down. */
    _vbFileAccessDefault
} _vbFileAccess;

typedef enum
{
    _vbFileLockShared,
    _vbFileLockRead,
    _vbFileLockWrite,
    _vbFileLockReadWrite,
    _vbFileLockTERMINATION,     /* don't use any entries from here down. */
    _vbFileLockDefault
} _vbFileLock;

void __initFileIOFunctions(void);
void __deinitFileIOFunctions(void);

void _vbpSi_open(BasicString *fName, __integer fNumber);
void _vbpSii_open(BasicString *fName, __integer fNumber, __integer fRecLen);
void _vbpS1i_open(BasicString *fName, _vbFileMode fMode, __integer fNumber);
void _vbpS1ii_open(BasicString *fName, _vbFileMode fMode,
                    __integer fNumber, __integer fRecLen);
void _vbpS12i_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileAccess fAccess, __integer fNumber);
void _vbpS12ii_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileAccess fAccess, __integer fNumber, __integer frl);
void _vbpS13i_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileLock fLock, __integer fNumber);
void _vbpS13ii_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileLock fLock, __integer fNumber, __integer frl);
void _vbpS2i_open(BasicString *fName, _vbFileAccess fAccess, __integer fNumber);
void _vbpS2ii_open(BasicString *fName, _vbFileAccess fAccess,
                    __integer fNumber, __integer fRecLen);
void _vbpS23i_open(BasicString *fName, _vbFileAccess fAccess,
                    _vbFileLock fLock, __integer fNumber);
void _vbpS23ii_open(BasicString *fName, _vbFileAccess fAccess,
                    _vbFileLock fLock, __integer fNumber, __integer fRLen);
void _vbpS3i_open(BasicString *fName, _vbFileLock fLock, __integer fNumber);
void _vbpS3ii_open(BasicString *fName, _vbFileLock fLock,
                    __integer fNumber, __integer fRLen);
void _vbpS123i_open(BasicString *fName, _vbFileMode fMode,
                     _vbFileAccess fAccess, _vbFileLock fLock, __integer fNum);
void _vbpS123ii_open(BasicString *fName, _vbFileMode fMode,
                     _vbFileAccess fAccess, _vbFileLock fLock,
                     __integer fNumber, __integer fRecLen);

void _vbp_close(void);
void _vbpin_close(__integer fNumber);

__integer _vbi_freefile(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_FILEIOFUNCTIONS_H_ */

/* end of FileIOFunctions.h ... */

