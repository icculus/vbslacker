/*
 * BASIC support library: File I/O functions. This handles actual reading and
 *  writing to files. Access to the filesystem is done through the
 *  FileSystemFunctions.c module.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "FileIOFunctions.h"

#warning Is this limit (255) still in effect?
#define VB_MAX_FILE_HANDLES  255

typedef struct
{
    int fd;                     /* system-dependent file descriptor.      */
    __boolean isOpen;           /* is this handle currently being used?   */
    _vbFileAccess accessType;   /* read, write, read/write?               */
    _vbFileMode fileMode;       /* random, binary, input, output, append? */
    _vbFileLock lockType;       /* shared, read, write, read/write?       */
    __integer recordLength;     /* len of buffer or record size.          */
    __boolean isEOF;            /* Is this the end of the file?           */
    char *readAhead;            /* read ahead buffer.                     */
} BasicFileHandle;

    /*
     * This holds the unix open() flags for each corresponding BASIC
     *  file mode. It's set up in __initFileIOFunctions()...
     */
static int modeTable[(int) ((_vbFileMode) _vbFileModeTERMINATION)];

    /*
     * File handle threadlock. Grab this before altering the
     *  fileHandleTable in any way.
     */
static ThreadLock fileIOThreadLock;

    /*
     * The table of BASIC file handles. Be sure to gain possession
     *  of fileIOThreadLock before modifying this table or its structures.
     */
static BasicFileHandle fileHandleTable[VB_MAX_FILE_HANDLES];


static inline int FileIOErrorFromErrno(void)
{
    return(10);  // !!!
} /* FileIOErrorFromErrno */


void __initFileIOFunctions(void)
{
    modeTable[(int) ((_vbFileMode) _vbFileModeDefault)] = O_RDWR;
    modeTable[(int) ((_vbFileMode) _vbFileModeRandom)]  = O_RDWR;
    modeTable[(int) ((_vbFileMode) _vbFileModeInput)]   = O_RDONLY;
    modeTable[(int) ((_vbFileMode) _vbFileModeOutput)]  = O_WRONLY;
    modeTable[(int) ((_vbFileMode) _vbFileModeBinary)]  = O_RDWR;
    modeTable[(int) ((_vbFileMode) _vbFileModeAppend)]  = O_WRONLY;

    memset(fileHandleTable, '\0', sizeof (fileHandleTable));
    __createThreadLock(&fileIOThreadLock);
} /* __initFileIOFunctions */


void __deinitFileIOFunctions(void)
{
    _vbp_close();
    __destroyThreadLock(&fileIOThreadLock);
} /* __deinitFileIOFunctions */


static inline BasicFileHandle *getFileStructAndLock(__integer fNumber)
{
    fNumber--;  /* make it option base 0. */

    if ((fNumber >= VB_MAX_FILE_HANDLES) || (fNumber < 0))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);

    __obtainThreadLock(&fileIOThreadLock);
    return(&fileHandleTable[fNumber]);
} // getFileStructAndLock


static void attemptOpenFailureRecovery(BasicFileHandle *handle,
                                       _vbFileMode fMode,
                                       _vbFileAccess fAccess,
                                       char *ascizFileName,
                                       int flags,
                                       int mode)
{
    if ((fMode == _vbFileModeRandom) && (errno == EACCES) &&
        (fAccess == _vbFileAccessDefault))
    {
        flags &= ~ O_RDWR;
        flags |= O_WRONLY;
        handle->fd = open(ascizFileName, flags, mode);
        if (handle->fd == -1)
        {
            flags &= ~O_WRONLY;
            flags |= O_RDONLY;
            handle->fd = open(ascizFileName, flags, mode);
        } /* if */
    } /* if */
} /* attemptOpenFailureRecovery */


    /* ...I will never take C++'s automatic name mangling for granted again. */
void _vbpSi_open(BasicString *fName, __integer fNumber)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, _vbFileAccessDefault,
                    _vbFileLockDefault, fNumber, 1);
} /* _vbpSi_open */


void _vbpSii_open(BasicString *fName, __integer fNumber, __integer fRecLen)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, _vbFileAccessDefault,
                    _vbFileLockDefault, fNumber, fRecLen);
} /* _vbpSii_open */


void _vbpS1i_open(BasicString *fName, _vbFileMode fMode, __integer fNumber)
{
    _vbpS123ii_open(fName, fMode, _vbFileAccessDefault,
                    _vbFileLockDefault, fNumber, 1);
} /* _vbpS1i_open */


void _vbpS1ii_open(BasicString *fName, _vbFileMode fMode,
                    __integer fNumber, __integer fRecLen)
{
    _vbpS123ii_open(fName, fMode, _vbFileAccessDefault,
                    _vbFileLockDefault, fNumber, fRecLen);
} /* _vbpS1ii_open */


void _vbpS12i_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileAccess fAccess, __integer fNumber)
{
    _vbpS123ii_open(fName, fMode, fAccess, _vbFileLockDefault, fNumber, 1);
} /* _vbpS12i_open */

void _vbpS12ii_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileAccess fAccess, __integer fNumber, __integer frl)
{
    _vbpS123ii_open(fName, fMode, fAccess, _vbFileLockDefault, fNumber, frl);
} /* _vbpS12ii_open */

void _vbpS13i_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileLock fLock, __integer fNumber)
{
    _vbpS123ii_open(fName, fMode, _vbFileAccessDefault, fLock, fNumber, 1);
} /* _vbpS13i_open */

void _vbpS13ii_open(BasicString *fName, _vbFileMode fMode,
                    _vbFileLock fLock, __integer fNumber, __integer frl)
{
    _vbpS123ii_open(fName, fMode, _vbFileAccessDefault, fLock, fNumber, frl);
} /* _vbpS13ii_open */

void _vbpS2i_open(BasicString *fName, _vbFileAccess fAccess, __integer fNumber)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, fAccess,
                    _vbFileLockDefault, fNumber, 1);
} /* _vbpS2i_open */

void _vbpS2ii_open(BasicString *fName, _vbFileAccess fAccess,
                    __integer fNumber, __integer fRecLen)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, fAccess,
                    _vbFileLockDefault, fNumber, fRecLen);
} /* _vbpS2ii_open */

void _vbpS23i_open(BasicString *fName, _vbFileAccess fAccess,
                    _vbFileLock fLock, __integer fNumber)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, fAccess, fLock, fNumber, 1);
} /* _vbpS23i_open */

void _vbpS23ii_open(BasicString *fName, _vbFileAccess fAccess,
                    _vbFileLock fLock, __integer fNumber, __integer fRLen)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, fAccess, fLock, fNumber, fRLen);
} /* _vbpS23ii_open */

void _vbpS3i_open(BasicString *fName, _vbFileLock fLock, __integer fNumber)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, _vbFileAccessDefault,
                    fLock, fNumber, 1);
} /* _vbpS3i_open */

void _vbpS3ii_open(BasicString *fName, _vbFileLock fLock,
                    __integer fNumber, __integer fRLen)
{
    _vbpS123ii_open(fName, _vbFileModeDefault, _vbFileAccessDefault,
                    fLock, fNumber, fRLen);
} /* _vbpS3ii_open */

void _vbpS123i_open(BasicString *fName, _vbFileMode fMode,
                     _vbFileAccess fAccess, _vbFileLock fLock, __integer fNum)
{
    _vbpS123ii_open(fName, fMode, fAccess, fLock, fNum, 1);
} /* _vbpS123i_open */

void _vbpS123ii_open(BasicString *fName, _vbFileMode fMode,
                     _vbFileAccess fAccess, _vbFileLock fLock,
                     __integer fNumber, __integer fRecLen)
{
    BasicFileHandle *handle;
    int flags = 0;
    int mode = (S_IRUSR | S_IWUSR);
    char *ascizFileName = __basicStringToAsciz(fName);

    if (fMode == _vbFileModeDefault)
        fMode = _vbFileModeRandom;

        /* enums are unsigned, so we don't need to check if these are < 0. */
    if ( (fMode >= _vbFileModeTERMINATION) ||
         (fAccess >= _vbFileAccessTERMINATION) ||
         (fLock >= _vbFileLockTERMINATION) ||
         ((fMode == _vbFileModeInput) && (fAccess == _vbFileAccessWrite)) ||
         ((fMode == _vbFileModeOutput) && (fAccess == _vbFileAccessRead)) )
    {
            /*
             * These are actually illegal arguments, but since the
             *  theory is that this should only be generated by a
             *  BASIC compiler, we'll assume it's our fault and call it
             *  an Internal Error.
             */
        __runtimeError(ERR_INTERNAL_ERROR);
    } /* if */

    mode = modeTable[(int) fMode];
    if ((fNumber < 1) || (fNumber > VB_MAX_FILE_HANDLES))
        __runtimeError(ERR_BAD_FILE_NAME_OR_NUMBER);

    fNumber--;   /* make it option base 0. */

    __obtainThreadLock(&fileIOThreadLock);

    handle = getFileStructAndLock(fNumber);
    if (handle->isOpen == true)
        __vbReleaseThreadThrowError(&fileIOThreadLock,ERR_FILE_ALREADY_OPEN);

        /* safety first: (re)initialize this file handle structure... */
    memset(handle, '\0', sizeof (BasicFileHandle));

    handle->fd = open(ascizFileName, flags, mode);
    if (handle->fd == -1)  /* can't open file? */
    {
        attemptOpenFailureRecovery(handle, fMode, fAccess,
                                    ascizFileName, flags, mode);
    } /* if */

    if (handle->fd == -1)  /* still FUBAR? */
        __vbReleaseThreadThrowError(&fileIOThreadLock, FileIOErrorFromErrno());

    handle->accessType = fAccess;
    handle->fileMode = fMode;
    handle->lockType = fLock;
    handle->recordLength = fRecLen;
    handle->isOpen = true;

        /* other threads may now safely touch this file handle, now. */
    __releaseThreadLock(&fileIOThreadLock);
} /* _vbpS123ii_open */


void _vbp_close(void)
/*
 * Close all open BASIC file handles.
 *
 *    params : void.
 *   returns : void.
 */
{
    __integer i;

    for (i = 1; i <= (__integer) VB_MAX_FILE_HANDLES; i++)
        _vbpin_close(i);
} /* _vbp_close */


void _vbpin_close(__integer fNumber)
{
    BasicFileHandle *handle = getFileStructAndLock(fNumber);
    if (handle->isOpen == true)
    {
#warning Check the following comment.
            /*
             * if close fails, can I still use the file descriptor? !!!
             *  if so, this code is okay. If not, resetting handle->isOpen()
             *  needs to happen before the close, so that catching code may
             *  open another file under this BASIC file index.
             */
        if (close(handle->fd) == -1)
        {
            __vbReleaseThreadThrowError(&fileIOThreadLock,
                                        FileIOErrorFromErrno());
        } /* if */

        handle->isOpen = false;
    } /* if */

    __releaseThreadLock(&fileIOThreadLock);
} /* _vbpin_close */


__integer _vbi_freefile(void)
{
    int i;
    __integer retVal = -1;

    __obtainThreadLock(&fileIOThreadLock);

    for (i = 0; (i < VB_MAX_FILE_HANDLES) && (retVal == -1); i++)
    {
        if (fileHandleTable[i].isOpen == false)
            retVal = i;
    } /* for */

    __releaseThreadLock(&fileIOThreadLock);

    return(retVal);
} /* _vbi_freefile */


__integer _vbii_eof(__integer fNumber)
{
    BasicFileHandle *handle = getFileStructAndLock(fNumber);
    __integer retVal = (__integer) handle->isEOF;
    __releaseThreadLock(&fileIOThreadLock);
    return(retVal);
} /* _vbii_eof */

/* end of FileIOFunctions.c ... */

