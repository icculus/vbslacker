/*
 * Declarations for File I/O functions for BASIC library.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include "BasicString.h"
#include "Variant.h"

#ifndef _INCLUDE_FILEIOFUNCTIONS_H_
#define _INCLUDE_FILEIOFUNCTIONS_H_

#include "Assembler.h"

typedef enum
{
    Append,
    Binary,
    Input,
    Output,
    Random
} FileModeEnum;

typedef enum
{
    Read,
    Write,
    ReadWrite
} FileAccessEnum;

typedef enum
{
    Shared,
    LockRead,
    LockWrite,
    LockReadWrite
} FileLockEnum;

/*** Function Declarations ***/
void VBclose_Params(STATEPARAMS, short handleCount, short firstFileHandle, ...);
void VBclose_NoParams(STATEPARAMS);
void VBopen_NoAccess_NoLock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                     FileModeEnum mode, short fileNumber);
void VBopen_NoAccess_NoLock_RecLen(STATEPARAMS, PBasicString pathName,
                                   FileModeEnum mode, short fileNumber,
                                   short recLength);
void VBopen_NoAccess_Lock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                   FileModeEnum mode, FileLockEnum lock,
                                   short fileNumber);
void VBopen_NoAccess_Lock_RecLen(STATEPARAMS, PBasicString pathName,
                                 FileModeEnum mode, FileAccessEnum access,
                                 FileLockEnum lock, short fileNumber,
                                 short recLength);
void VBopen_Access_NoLock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                   FileModeEnum mode, FileAccessEnum access,
                                   short fileNumber);
void VBopen_Access_NoLock_RecLen(STATEPARAMS, PBasicString pathName,
                                 FileModeEnum mode, FileAccessEnum access,
                                 short fileNumber, short recLength);
void VBopen_Access_Lock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                 FileModeEnum mode, FileAccessEnum access,
                                 FileLockEnum lock, short fileNumber);
void VBopen_Access_Lock_RecLen(STATEPARAMS, PBasicString pathName,
                               FileModeEnum mode, FileAccessEnum access,
                               FileLockEnum lock, short fileNumber,
                               short recLength);
void VBget_NoRecNum_NoVar(STATEPARAMS, short fileNumber, void *varName);
void VBget_NoRecNum_Var(STATEPARAMS, short fileNumber, PVariant *varName);
void VBget_RecNum_NoVar(STATEPARAMS, short fileNumber,
                        PVariant recNumber, void *varName);
void VBget_RecNum_Var(STATEPARAMS, short fileNumber,
                      PVariant recNumber, PVariant *varName);
void VBput_NoRecNum_NoVar(STATEPARAMS, short fileNumber, void *varName);
void VBput_NoRecNum_Var(STATEPARAMS, short fileNumber, PVariant *varName);
void VBput_RecNum_NoVar(STATEPARAMS, short fileNumber,
                        PVariant recNumber, void *varName);
void VBput_RecNum_Var(STATEPARAMS, short fileNumber,
                      PVariant recNumber, PVariant *varName);
PBasicString VBfunc_input(STATEPARAMS, long number, short fileNumber);
PBasicString VBproc_input(STATEPARAMS, short fileNumber, PVariant varList, ...);
PBasicString VBlineInput(STATEPARAMS, short fileNumber, PBasicString varName);
void VBprint(STATEPARAMS, short fileNumber, PVariant outputList, ...);
void VBwrite(STATEPARAMS, short fileNumber, PVariant outputList, ...);
void VB_EOF(STATEPARAMS, short fileNumber);
void VB_LOF(STATEPARAMS, short fileNumber);
short VBFreeFile_Range(STATEPARAMS, short rangeNumber);
short VBFreeFile_NoRange(STATEPARAMS);
void VBloc(STATEPARAMS, short fileNumber);
void VBfunc_seek(STATEPARAMS, short fileNumber);
void VBproc_seek(STATEPARAMS, short fileNumber, long position);

#endif

/* end of FileIOFunctions.h ... */

