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
void VBclose_Params(short firstFileHandle, ...);
void VBclose_NoParams(void);
void VBopen_NoAccess_NoLock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
									 short fileNumber);
void VBopen_NoAccess_NoLock_RecLen(PBasicString pathName, FileModeEnum mode, 
                                   short fileNumber, short recLength);
void VBopen_NoAccess_Lock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
								   FileLockEnum lock, short fileNumber);
void VBopen_NoAccess_Lock_RecLen(PBasicString pathName, FileModeEnum mode, 
								 FileAccessEnum access, FileLockEnum lock,
                                 short fileNumber, short recLength);
void VBopen_Access_NoLock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
								   FileAccessEnum access, short fileNumber);
void VBopen_Access_NoLock_RecLen(PBasicString pathName, FileModeEnum mode, 
								 FileAccessEnum access, short fileNumber,
                                 short recLength);
void VBopen_Access_Lock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
								 FileAccessEnum access, FileLockEnum lock,
                                 short fileNumber);
void VBopen_Access_Lock_RecLen(PBasicString pathName, FileModeEnum mode, 
							   FileAccessEnum access, FileLockEnum lock,
                               short fileNumber, short recLength);
void VBget_NoRecNum_NoVar(short fileNumber, void *varName);
void VBget_NoRecNum_Var(short fileNumber, PVariant *varName);
void VBget_RecNum_NoVar(short fileNumber, PVariant recNumber, void *varName);
void VBget_RecNum_Var(short fileNumber, PVariant recNumber, PVariant *varName);
void VBput_NoRecNum_NoVar(short fileNumber, void *varName);
void VBput_NoRecNum_Var(short fileNumber, PVariant *varName);
void VBput_RecNum_NoVar(short fileNumber, PVariant recNumber, void *varName);
void VBput_RecNum_Var(short fileNumber, PVariant recNumber, PVariant *varName);
PBasicString VBfunc_input(long number, short fileNumber);
PBasicString VBproc_input(short fileNumber, PVariant varList, ...);
PBasicString VBlineInput(short fileNumber, PBasicString varName);
void VBprint(short fileNumber, PVariant outputList, ...);
void VBwrite(short fileNumber, PVariant outputList, ...);
void VB_EOF(short fileNumber);
void VB_LOF(short fileNumber);
short VBFreeFile_Range(short rangeNumber);
short VBFreeFile_NoRange(void);
VBloc(short fileNumber);
VBfunc_seek(short fileNumber);
VBproc_seek(short fileNumber, long position);

#endif
/* end of FileIOFunctions.h ... */