/*
 * File I/O functions for BASIC library.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdarg.h>
#include "FileIOFunctions.h"
#include "BasicFileStream.h"
#include "Variant.h"
#include "ErrorFunctions.h"
#include "Boolean.h"

/*** Private function declarations ***/
static void __VBclose(va_list argPtr);
static void __VBget(short fileNumber, PVariant recNumber, void *varName, boolean isVariant);
static void __VBput(short fileNumber, PVariant recNumber, void *varName, boolean isVariant);
static short __VBFreeFile(short rangeNumber);

/*** Close Statement ***/
void __VBclose(va_list argPtr)
/*
 * Closes file I/O for the specified handle(s).
 *
 *   params : firstFileHandle   ==  Integer value representing a previously
 *                                   opened file handle.
 *            argPtr            ==  Additional arguments passed from VBclose
 *                                  "overloaded" functions.
 *  returns : void
 */
{
}
void VBclose_Params(short firstFileHandle, ...)
{
    va_list pMarker;
    va_start(pMarker, firstFileHandle);
    __VBclose(pMarker);             /* Close handles specified in arguments */
}
void VBclose_NoParams(void)
{
    __VBclose(NULL);                /* Close all file handles */
}
/*** End Close Statement ***/


/*** Open Statement ***/
void _VBopen(PBasicString pathName, FileModeEnum *mode, FileAccessEnum *access, 
             FileLockEnum *lock, short fileNumber, short *recLength)
/*
 * Enables I/O to a file.  mode, access, and recLength are set to NULL by
 *  the appropriate function overload.  This indicates that the parameter is
 *  NOT SPECIFIED!
 *
 *   params : pathName      ==  String expression that specifies a path and
 *                               file name to open.
 *            mode          ==  Specifies the file mode (see FileModeEnum for
 *                               available modes.
 *            access        ==  Specifies the operations permitted on the open
 *                               file (see FileAccessEnum for available access
 *                               permissions).
 *            lock          ==  Specifies permissions available on the open
 *                               file to other processes (see FileLockEnum for
 *                               available lock permissions).
 *            fileNumber    ==  A valid file number in the range of 1 to 511,
 *                               inclusive.
 *            recLength     ==  Number less than or equal to 32,767.  For files
 *                               opened for random access, this value is the
 *                               record length.  For sequential files, this
 *                               value is the number of characters buffered.
 *  returns : void
 */
{
                                    /* Contents of OPTIONAL values */
    FileModeEnum    enuMode;
    FileAccessEnum  enuAccess;
    FileLockEnum    enuLock;

									/* File number was invalid */
	if(__invalidFileNumber(fileNumber))
	{
		__runtimeError(ERR_BAD_FILE_NAME_OR_NUMBER);
		return;
	}

                                    /* Set parameters that weren't specified */
                                    /*  to their default values */
    if(mode == NULL)    enuMode = Random;
    else                enuMode = *mode;
    if(access == NULL)  enuAccess = Read;
    else                enuAccess = *access;
    if(lock == NULL)    enuLock = Shared;
    else                enuLock = *lock;

    /* We're all good...let's go */

}
void VBopen_NoAccess_NoLock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
									 short fileNumber)
{
}
void VBopen_NoAccess_NoLock_RecLen(PBasicString pathName, FileModeEnum mode, 
                                   short fileNumber, short recLength)
{
}
void VBopen_NoAccess_Lock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
								   FileLockEnum lock, short fileNumber)
{
}
void VBopen_NoAccess_Lock_RecLen(PBasicString pathName, FileModeEnum mode, 
								 FileAccessEnum access, FileLockEnum lock,
                                 short fileNumber, short recLength)
{
}
void VBopen_Access_NoLock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
								   FileAccessEnum access, short fileNumber)
{
}
void VBopen_Access_NoLock_RecLen(PBasicString pathName, FileModeEnum mode, 
								 FileAccessEnum access, short fileNumber,
                                 short recLength)
{
}
void VBopen_Access_Lock_NoRecLen(PBasicString pathName, FileModeEnum mode, 
								 FileAccessEnum access, FileLockEnum lock,
                                 short fileNumber)
{
}

void VBopen_Access_Lock_RecLen(PBasicString pathName, FileModeEnum mode, 
							   FileAccessEnum access, FileLockEnum lock,
                               short fileNumber, short recLength)
{
}
/*** End Open Statement ***/


/*** Get Statement ***/
void __VBget(short fileNumber, PVariant recNumber, void *varName, boolean isVariant)
/*
 * Reads data from an open disk file into a variable
 *   params : fileNumber    -   Any valid file number
 *            recNumber     -   Record number, or byte number at which reading
 *                              begins.
 *            varName       -   Variable name into which data is read.
 *            isVariant     -   This value is usually specified by the VB
 *                              parser.  It is TRUE if 'varName' is of type
 *                              PVariant.
 *  returns : void
 */
{
}
void VBget_NoRecNum_NoVar(short fileNumber, void *varName)
{
    __VBget(fileNumber, NULL, varName, false);
}
void VBget_NoRecNum_Var(short fileNumber, PVariant *varName)
{
    __VBget(fileNumber, NULL, varName, true);
}
void VBget_RecNum_NoVar(short fileNumber, PVariant recNumber, void *varName)
{
    __VBget(fileNumber, recNumber, varName, false);
}
void VBget_RecNum_Var(short fileNumber, PVariant recNumber, PVariant *varName)
{
    __VBget(fileNumber, recNumber, varName, true);
}
/*** End Get Statement ***/


/*** Put Statement ***/
void __VBput(short fileNumber, PVariant recNumber, void *varName, boolean isVariant)
/*
 * Reads data from an open disk file into a variable
 *   params : fileNumber    -   Any valid file number
 *            recNumber     -   Record number, or byte number at which reading
 *                              begins.
 *            varName       -   Variable containing data to be written to disk.
 *            isVariant     -   This value is usually specified by the VB
 *                              parser.  It is TRUE if 'varName' is of type
 *                              PVariant.
 *  returns : void
 */
{
}
void VBput_NoRecNum_NoVar(short fileNumber, void *varName)
{
    __VBput(fileNumber, NULL, varName, false);
}
void VBput_NoRecNum_Var(short fileNumber, PVariant *varName)
{
    __VBput(fileNumber, NULL, varName, true);
}
void VBput_RecNum_NoVar(short fileNumber, PVariant recNumber, void *varName)
{
    __VBput(fileNumber, recNumber, varName, false);
}
void VBput_RecNum_Var(short fileNumber, PVariant recNumber, PVariant *varName)
{
    __VBput(fileNumber, recNumber, varName, true);
}
/*** End Put Statement ***/


PBasicString VBfunc_input(long number, short fileNumber)
/*
 * Returns a string containing characters from a file openend in Input of
 *  Binary mode.
 *
 *   params : number        -   Number of characters to return
 *            fileNumber    -   Any valid file number
 *  returns : PBasicString
 */
{
}

PBasicString VBproc_input(short fileNumber, PVariant varList, ...)
/*
 * Returns a string containing characters from a file openend in Input of
 *  Binary mode.
 *
 *   params : fileNumber    -   Any valid file number
 *            varList       -   List of variables to assign values.
 *  returns : PBasicString
 */
{
}

PBasicString VBlineInput(short fileNumber, PBasicString varName)
/*
 * Read a single line and saves it into varName.
 *
 *   params : fileNumber    -   Any valid file number
 *            varList       -   List of variables to assign values.
 *  returns : void
 */
{
}

void VBprint(short fileNumber, PVariant outputList, ...)
/*
 * Writes display-formatted data to a sequential file.
 *
 *   params : fileNumber    -   Any valid file number
 *            outputList    -   Expressions to print.
 *  returns : void
 */
{
}

void VBwrite(short fileNumber, PVariant outputList, ...)
/*
 * Writes data to a sequential file.
 *
 *   params : fileNumber    -   Any valid file number
 *            outputList    -   Expressions to print.
 *  returns : void
 */
{
}

void VB_EOF(short fileNumber)
{
}

void VB_LOF(short fileNumber)
{
}


/*** FreeFile Statement ***/
short __VBFreeFile(short rangeNumber)
{
}

short VBFreeFile_Range(short rangeNumber)
{
}

short VBFreeFile_NoRange(void)
{
}
/*** End FreeFile Statement ***/

VBloc(short fileNumber)
{
}

VBfunc_seek(short fileNumber)
{
}

VBproc_seek(short fileNumber, long position)
{
}
/* end of FileIOFunctions.c ... */
