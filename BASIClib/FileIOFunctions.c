/*
 * File I/O functions for BASIC library.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "FileIOFunctions.h"
#include "BasicFileStream.h"
#include "Variant.h"
#include "ErrorFunctions.h"
#include "Boolean.h"
#include "InternalMemManager.h"

/*** Private function declarations ***/
static void __VBopen(STATEPARAMS, PBasicString pathName, FileModeEnum mode,
                     FileAccessEnum *access, FileLockEnum *lock,
                     short fileNumber, short *recLength);
static void __VBclose(STATEPARAMS, short handleCount, short *fileHandles);
static void __VBget(STATEPARAMS, short fileNumber, PVariant recNumber,
                    void *varName, boolean isVariant);
static void __VBput(STATEPARAMS, short fileNumber, PVariant recNumber,
                    void *varName, boolean isVariant);
static short __VBFreeFile(STATEPARAMS, short rangeNumber);
static void __GetFileOpenMode(STATEPARAMS, char *Dest, const char *pathName,
                              FileModeEnum mode, FileAccessEnum access);
static boolean __FileExist(STATEPARAMS, const char *fileName);

/*** Private constants ***/
#define MAX_FILE_MODE_LENGTH    5   /* Length of 'mode' parameter for fopen */

/*** Close Statement ***/
void __VBclose(STATEPARAMS, short handleCount, short *fileHandles)
/*
 * Closes file I/O for the specified handle(s).
 *
 *   params : handleCount       ==  Number of handles contained in fileHandles.
 *            fileHandles       ==  Pointer to array of file handles.
 *                            
 *  returns : void
 */
{
    short i;                        /* Generic counter */
    short *pTemp;                   /* Array increment pointer */
    __PBasicFileStream pFileStream; /* Pointer to file stream */

    if(handleCount == 0)            /* Process all file handles */
    {
                                    /* Cycle through all handles */
        for(i = 1; i < MAX_FILE_HANDLES; i++)
        {
            if((pFileStream = __getFileStream(STATEARGS, i)) != NULL)
            {
                                    /* Reset locking on file */
                /* !!! LOCKING NOT SUPPORTED YET !!! */
                                    /* Close associated file stream */
                fclose(pFileStream->fileStream);
                                    /* Delete file stream object */
                __deleteFileStream(STATEARGS, i);
            }
        }
    }
    else                            /* Process specified handles */
    {
        pTemp = fileHandles;        /* Set pointer to beginngin of array */

                                    /* Cycle through all handles */
        for(i = 0; i < handleCount; i++)
        {
            if((pFileStream = __getFileStream(STATEARGS, *pTemp)) != NULL)
            {
                                    /* Close associated file stream */
                fclose(pFileStream->fileStream);
                                    /* Delete file stream object */
                                    /* Increment to next array element */
                __deleteFileStream(STATEARGS, *pTemp++);
            }
        }
    }
}


void VBclose_Params(STATEPARAMS, short handleCount, short firstFileHandle, ...)
{
    short *pFileHandles;            /* Array of file handles */
    va_list pArgs;                   /* Function argument pointer */
    short i;                        /* Generic loop counter */
    short *pTemp;                   /* Incrementing pointer */

                                    /* Don't do anything if no handles */
    if(handleCount <= 0)
        return;

                                    /* Allocate memory for handle array */
    pFileHandles = (short *) __memAlloc(STATEARGS, handleCount * sizeof(short));

    va_start(pArgs, firstFileHandle);
    
    pTemp = pFileHandles;           /* Save pointer to start of array */
    *pTemp = firstFileHandle;       /* Get first handle specified */
    for(i = 1; i < handleCount; i++)
    {
        pTemp++;                    /* Increment to next element */
        *pTemp = va_arg(pArgs, short);
    }

    va_end(pArgs);                  /* Reset variable arguments. */

                                    /* Close handles specified in arguments */
    __VBclose(STATEARGS, handleCount, pFileHandles);
}


void VBclose_NoParams(STATEPARAMS)
{
    __VBclose(STATEARGS, 0, NULL);             /* Close all file handles */
}
/*** End Close Statement ***/


/*** Open Statement ***/
void __VBopen(STATEPARAMS, PBasicString pathName, FileModeEnum mode,
              FileAccessEnum *access, FileLockEnum *lock, short fileNumber,
              short *recLength)
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
    __PBasicFileStream pNewFileStream;
    char strOpenFileMode[MAX_FILE_MODE_LENGTH];

                                    /* Contents of OPTIONAL values */
    FileAccessEnum  enuAccess;
    FileLockEnum    enuLock;
    short           sRecLength;
    char            *strPathName;   /* C string version of pathName */

									/* File number was invalid */
	if(__invalidFileNumber(STATEARGS, fileNumber))
	{
		__runtimeError(STATEARGS, ERR_BAD_FILE_NAME_OR_NUMBER);
		return;
	}

    /* Set parameters that weren't specified to their default values */
    if(access == NULL)      enuAccess = Read;
    else                    enuAccess = *access;
    if(lock == NULL)        enuLock = Shared;
    else                    enuLock = *lock;
    if(recLength == NULL)   sRecLength = 0;
    else                    sRecLength = *recLength;

    /* Convert BASIC string to a C string */
    strPathName = (char *)__memAlloc(STATEARGS, pathName->length + 1);
    memcpy(strPathName, pathName->data, pathName->length);
                                    /* Add NULL terminator */
    strPathName[pathName->length] = 0x00;
    
    /* We're all good...let's go */
                                    /* Try to create a new stream object */
    pNewFileStream = __createFileStream(STATEARGS, fileNumber);
    if(pNewFileStream == NULL)      /* If creation failed */
    {  
        __runtimeError(STATEARGS, ERR_OUT_OF_MEMORY);
        return;
    }
    
                                    /* Get file mode string based on VB mode */
                                    /*  and access values */
    __GetFileOpenMode(STATEARGS, strOpenFileMode, strPathName, mode, enuAccess);

    /* Set share lock on file */
    /* !!! LOCKING NOT SUPPORTED YET !!! */

    /* Open der funky file */
                                    /* Attempt to open the file */
    if((pNewFileStream->fileStream = fopen(strPathName, strOpenFileMode)) == NULL)
    {
        __deleteFileStream(STATEARGS, fileNumber);
        __runtimeError(STATEARGS, ERR_PATH_FILE_ACCESS_ERROR);
        return;
    }

    /* Save stream info to stream object */
    pNewFileStream->pathName = pathName;
    pNewFileStream->mode = mode;
    pNewFileStream->access = enuAccess;
    pNewFileStream->lock = enuLock;
    pNewFileStream->recLength = sRecLength;
}


void VBopen_NoAccess_NoLock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                     FileModeEnum mode, short fileNumber)
{
    __VBopen(STATEARGS, pathName, mode, NULL, NULL, fileNumber, NULL);
}


void VBopen_NoAccess_NoLock_RecLen(STATEPARAMS, PBasicString pathName,
                                   FileModeEnum mode, short fileNumber,
                                   short recLength)
{
    __VBopen(STATEARGS, pathName, mode, NULL, NULL, fileNumber, &recLength);
}


void VBopen_NoAccess_Lock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                   FileModeEnum mode, FileLockEnum lock,
                                   short fileNumber)
{
    __VBopen(STATEARGS, pathName, mode, NULL, &lock, fileNumber, NULL);
}


void VBopen_NoAccess_Lock_RecLen(STATEPARAMS, PBasicString pathName,
                                 FileModeEnum mode, FileAccessEnum access,
                                 FileLockEnum lock, short fileNumber,
                                 short recLength)
{
    __VBopen(STATEARGS, pathName, mode, NULL, &lock, fileNumber, &recLength);
}


void VBopen_Access_NoLock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                   FileModeEnum mode, FileAccessEnum access,
                                   short fileNumber)
{
    __VBopen(STATEARGS, pathName, mode, &access, NULL, fileNumber, NULL);
}


void VBopen_Access_NoLock_RecLen(STATEPARAMS, PBasicString pathName,
                                 FileModeEnum mode, FileAccessEnum access,
                                 short fileNumber, short recLength)
{
    __VBopen(STATEARGS, pathName, mode, &access, NULL, fileNumber, &recLength);
}


void VBopen_Access_Lock_NoRecLen(STATEPARAMS, PBasicString pathName,
                                 FileModeEnum mode, FileAccessEnum access,
                                 FileLockEnum lock, short fileNumber)
{
    __VBopen(STATEARGS, pathName, mode, &access, &lock, fileNumber, NULL);
}


void VBopen_Access_Lock_RecLen(STATEPARAMS, PBasicString pathName,
                               FileModeEnum mode, FileAccessEnum access,
                               FileLockEnum lock, short fileNumber,
                               short recLength)
{
    __VBopen(STATEARGS, pathName, mode, &access, &lock, fileNumber, &recLength);
}

/*** End Open Statement ***/


/*** Get Statement ***/

void __VBget(STATEPARAMS, short fileNumber, PVariant recNumber, void *varName,
             boolean isVariant)
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


void VBget_NoRecNum_NoVar(STATEPARAMS, short fileNumber, void *varName)
{
    __VBget(STATEARGS, fileNumber, NULL, varName, false);
}


void VBget_NoRecNum_Var(STATEPARAMS, short fileNumber, PVariant *varName)
{
    __VBget(STATEARGS, fileNumber, NULL, varName, true);
}


void VBget_RecNum_NoVar(STATEPARAMS, short fileNumber,
                        PVariant recNumber, void *varName)
{
    __VBget(STATEARGS, fileNumber, recNumber, varName, false);
}


void VBget_RecNum_Var(STATEPARAMS, short fileNumber,
                      PVariant recNumber, PVariant *varName)
{
    __VBget(STATEARGS, fileNumber, recNumber, varName, true);
}

/*** End Get Statement ***/


/*** Put Statement ***/

void __VBput(STATEPARAMS, short fileNumber, PVariant recNumber, void *varName,
             boolean isVariant)
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


void VBput_NoRecNum_NoVar(STATEPARAMS, short fileNumber, void *varName)
{
    __VBput(STATEARGS, fileNumber, NULL, varName, false);
}


void VBput_NoRecNum_Var(STATEPARAMS, short fileNumber, PVariant *varName)
{
    __VBput(STATEARGS, fileNumber, NULL, varName, true);
}


void VBput_RecNum_NoVar(STATEPARAMS, short fileNumber,
                        PVariant recNumber, void *varName)
{
    __VBput(STATEARGS, fileNumber, recNumber, varName, false);
}


void VBput_RecNum_Var(STATEPARAMS, short fileNumber,
                      PVariant recNumber, PVariant *varName)
{
    __VBput(STATEARGS, fileNumber, recNumber, varName, true);
}

/*** End Put Statement ***/


PBasicString VBfunc_input(STATEPARAMS, long number, short fileNumber)
/*
 * Returns a string containing characters from a file openend in Input of
 *  Binary mode.
 *
 *   params : number        -   Number of characters to return
 *            fileNumber    -   Any valid file number
 *  returns : PBasicString
 */
{
    return NULL;
}


PBasicString VBproc_input(STATEPARAMS, short fileNumber, PVariant varList, ...)
/*
 * Returns a string containing characters from a file openend in Input of
 *  Binary mode.
 *
 *   params : fileNumber    -   Any valid file number
 *            varList       -   List of variables to assign values.
 *  returns : PBasicString
 */
{
    return NULL;
}

PBasicString VBlineInput(STATEPARAMS, short fileNumber, PBasicString varName)
/*
 * Read a single line and saves it into varName.
 *
 *   params : fileNumber    -   Any valid file number
 *            varList       -   List of variables to assign values.
 *  returns : void
 */
{
    return NULL;
}

void VBprint(STATEPARAMS, short fileNumber, PVariant outputList, ...)
/*
 * Writes display-formatted data to a sequential file.
 *
 *   params : fileNumber    -   Any valid file number
 *            outputList    -   Expressions to print.
 *  returns : void
 */
{
}

void VBwrite(STATEPARAMS, short fileNumber, PVariant outputList, ...)
/*
 * Writes data to a sequential file.
 *
 *   params : fileNumber    -   Any valid file number
 *            outputList    -   Expressions to print.
 *  returns : void
 */
{
}

void VB_EOF(STATEPARAMS, short fileNumber)
{
}

void VB_LOF(STATEPARAMS, short fileNumber)
{
}


/*** FreeFile Statement ***/
short __VBFreeFile(STATEPARAMS, short rangeNumber)
{
    short sStart;                   /* Start and end of search loop */
    short sEnd;
    short i;                        /* Generic loop counter */
    short sReturnedFileNumber = -1; /* File number to return */

    if(rangeNumber == 0)             /* Private file numbers */
    {
        sStart = 1;
        sEnd = 255;
    }
    else                            /* Public file number */
    {
        sStart = 256;
        sEnd = 511;
    }

    for(i = sStart; i <= sEnd; i++)
    {
                                    /* If we found our man...err...number */
        if(__getFileStream(STATEARGS, i) == NULL)
        {
            sReturnedFileNumber = i;
            break;                  /* Break out of loop */
        }
    }

                                    /* If all of the file handles were taken */
    if(sReturnedFileNumber == -1)
    {
        __runtimeError(STATEARGS, ERR_TOO_MANY_FILES);
        return 0;
    }
    
    return sReturnedFileNumber;     /* Return the free file handle */
}


short VBFreeFile_Range(STATEPARAMS, short rangeNumber)
{
    return __VBFreeFile(STATEARGS, rangeNumber);
}


short VBFreeFile_NoRange(STATEPARAMS)
{
    return __VBFreeFile(STATEARGS, 0);         /* Specify default range */
}

/*** End FreeFile Statement ***/


void VBloc(STATEPARAMS, short fileNumber)
{
}

void VBfunc_seek(STATEPARAMS, short fileNumber)
{
}

void VBproc_seek(STATEPARAMS, short fileNumber, long position)
{
}


/*** Miscellaneous Private Methods ***/

void __GetFileOpenMode(STATEPARAMS, char *Dest, const char *pathName,
                       FileModeEnum mode, FileAccessEnum access)
/*
 * Gets the 'fopen' equivelent file mode string for the specified VB mode and access.
 *
 *   params : Dest          -   String to write mode too.  Length must be
 *                               MAX_FILE_MODE_LENGTH.
 *            pathName      -   Name of file to get mode of.
 *            mode          -   File open mode
 *            access        -   File open access
 *
 *  returns : void
 */
{
    switch(mode)
    {
        case Append:
            strcpy(Dest, "at");
            /* 'access' variable is ignored */
            break;
        case Input:
            strcpy(Dest, "rt");
            /* 'access' variable is ignored */
            break;
        case Output:
            strcpy(Dest, "wt");
            /* 'access' variable is ignored */
            break;
        case Random:
        case Binary:
            switch(access)
            {
                case Read:
                    strcpy(Dest, "rb");

                /* Write & ReadWrite are opened the same way.  'fopen' does */
                /*  not allow you to open a file for 'write' only unless you */
                /*  append or destroy the file. */
                /* Reading must be restricted appropriately from the */
                /*  BasicLIB file routines. */
                case Write:
                case ReadWrite:
                                    /* If file exists, we don't want to */
                                    /*  overwrite it. */
                    if(__FileExist(STATEARGS, pathName))
                        strcpy(Dest, "r+b");
                    else
                        strcpy(Dest, "w+b");
            }
            strcpy(Dest, "b");
    }
}

boolean __FileExist(STATEPARAMS, const char *fileName)
/*
 * Checks to see if specified file exists.
 *
 *   params : fileName      -   File to check existence of.
 *
 *   returns: TRUE if file exists, otherwise FALSE.
 */
{
    FILE *tempStream;
    boolean bReturnValue;

    if((tempStream = fopen(fileName, "rb")) == NULL)
        bReturnValue = false;
    else
        bReturnValue = true;

    fclose(tempStream);
    return bReturnValue;
}

/* end of FileIOFunctions.c ... */
