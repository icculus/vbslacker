/*
 * File system interfacing functions. This deals with directories,
 *  collections of files, and file hierarchy. Specific file i/o can
 *  be found in the FileIOFunctions module.
 *
 *     Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <fnmatch.h>
#include "FileSystemFunctions.h"


typedef struct _DIRLIST
{
    __byte *fileName;
    struct _DIRLIST *next;
} DirList;


ThreadLock fileSystemLock;
DirList **threadDirs = NULL;


void __initFileSystemFunctions(void)
{
    __createThreadLock(&fileSystemLock);
} /* __initFileSystemFunctions */


void __deinitFileSystemFunctions(void)
{
    __destroyThreadLock(&fileSystemLock);
} /* __deinitFileSystemFunctions */


void __initThreadFileSystemFunctions(__integer tidx)
{
    __obtainThreadLock(&fileSystemLock);
    threadDirs = __memRealloc(threadDirs,
                              __getHighestThreadIndex() * sizeof (DirList *));
    threadDirs[tidx] = NULL;
    __releaseThreadLock(&fileSystemLock);
} /* __initThreadFileSystemFunctions */


static int fileSysErrors(void)
/*
 * Convert (errno)'s value, set by a problem in FileCopy (and others),
 *   into a standard VB error.
 *
 *      params : void.
 *     returns : BASIC error number.
 */
{
    switch (errno)
    {
        case EACCES:
        case EROFS:
        case EBUSY:
            return(ERR_PERMISSION_DENIED);

        case EISDIR:
        case ELOOP:
        case ENOTDIR:
        case ENOTEMPTY:
        case EINVAL:
            return(ERR_PATH_FILE_ACCESS_ERROR);

        case ENAMETOOLONG:
            return(ERR_BAD_FILE_NAME_OR_NUMBER);

        case EEXIST:
            return(ERR_FILE_ALREADY_EXISTS);

        case EMFILE:
        case ENFILE:
        case EMLINK:
            return(ERR_TOO_MANY_FILES);

        case ENOENT:
            return(ERR_FILE_NOT_FOUND);

        case ENOSPC:
            return(ERR_DISK_FULL);

        case EIO:
        case EFBIG:
            return(DEVICE_IO_ERROR);
    } /* switch */

    return(ERR_INTERNAL_ERROR);    /* uh? */
} /* fileSysErrors */



static __byte *__convertPathWinToUnix(__byte *pathName)
/*
 * Convert "C:\path\path\filename.txt" to "/path/path/filename.txt"
 *
 *  The drive is removed if it exists, regardless of the letter.
 *  The return value is allocated from scratch, and is collectable.
 *
 *     params : pathName == path to convert.
 *    returns : see above.
 */
{
    __integer length = strlen(pathName);
    __byte *retVal = __memAllocNoPtrs(length + 1);
    __integer i;

    if ((length >= 2) && (pathName[1] == ':'))   /* drive letter? */
        pathName += 2;                           /* bump pathName past it. */

    for (i = 0; *pathName != '\0'; pathName++, i++)
        retVal[i] = ((*pathName == '\\') ? '/' : *pathName);

    retVal[i] = '\0';
    return(retVal);
} /* __convertPathWinToUnix */



static __byte *__convertPathUnixToWin(__byte *pathName)
/*
 * Convert "/path/path/filename.txt" to "C:\path\path\filename.txt"
 *
 * A "C:" is appended. The return value is allocated from scratch and is
 *  collectable.
 *
 *     params : pathName == path to convert.
 *    returns : see above.
 */
{
    __integer length = strlen(pathName);
    __byte *retVal = __memAllocNoPtrs(length + 3);
    __integer i;

    strcpy(retVal, "C:");

    for (i = 2; *pathName != '\0'; pathName++, i++)
        retVal[i] = ((*pathName == '/') ? '\\' : *pathName);

    retVal[i] = '\0';
    return(retVal);
} /* __convertPathUnixToWin */


static void parseDir(__byte *dirToParse, DIR **dirInfo,
                     __byte **fileName, __byte **path);
{
    __byte *ascizFileName = __convertPathWinToUnix(dirToParse);

    *fileName = strrchr(ascizFileName, "/");
    if (*fileName == NULL)
    {
        *fileName = ascizFileName;
        *path = __CURRENTDIRSTR;
    } /* if */
    else
    {
        *(*fileName) = '\0';   /* terminate path string.      */
        (*fileName)++;         /* point to start of filename. */
        *path = __memAllocNoPtrs(strlen(ascizFileName) + 1);
        strcpy(*path, ascizFileName);
    } /* else */

        /*
         * Even without wildcards, this call to opendir() will
         *  make sure any leading directories in the filename
         *  are valid. (So, "/home/????????/files*.txt" would fail.
         *  This is normal for Visual BASIC; wildcards must be in
         *  one directory.)
         */
    dirInfo = opendir(*path);
} /* parseDir */


static __long killWildcards(DIR *dirInfo, __byte *path, __byte *fileName)
/*
 * Called exclusively from _vbpS_kill(). Terminates a collection
 *  of files that match the pattern in (fileName). Directories are
 *  unharmed.
 *
 *     params : dirInfo  == structure of directory where files reside.
 *              path     == pathname of directory where files reside.
 *              fileName == wildcard pattern to match.
 *    returns : error code to throw. ERR_NO_ERROR if cool.
 */
{
    __long retVal = ERR_NO_ERROR;
    struct dirent dirEntry;
    struct dirent *pDirEntry;
    int rc;
    int flags = 0;
    char fullName[strlen(path) + MAX_FILENAME + 2];

    if (__getInitFlags() & INITFLAG_FILENAMES_IGNORECASE)
        flags |= FNM_CASEFOLD;

    do
    {
        pDirEntry = &dirEntry;
        rc = readdir_r(dirInfo, pDirEntry, &pDirEntry);
        if (rc == 0)
        {
            if (pDirEntry->d_type != DT_DIR)
            {
                if (fnmatch(fileName, pDirEntry->d_name, flags) == 0)
                {
                    sprintf(fullName, "%s/%s", path, pDirEntry->d_name);
                    if (unlink(fullName) == -1)
                        retVal = fileSysErrors();
                } /* if */
            } /* if */
        } /* if */
    } while (rc == 0);
    
    return(retVal);
} /* killLoop */


static inline __long killSingle(__byte *path, __byte *fileName)
/*
 * Kill just one file. This is called exclusively from _vbpS_kill().
 *  It's an error if the filename is actually a directory.
 *
 *     params : path     == pathname of directory where file resides.
 *              fileName == file name to kill.
 *    returns : error code, ERR_NO_ERROR if all is cool.
 */
{
    char fullName[strlen(path) + strlen(fileName) + 2];
    struct stat statInfo;
    __long retVal = ERR_NO_ERROR;

    sprintf(fullName, "%s/%s", path, fileName);

    if (stat(fullName, &statInfo) == -1)
        retVal = fileSysErrors();
    else
    {
        if (S_ISDIR(statInfo.st_mode))
            retVal = ERR_FILE_NOT_FOUND;
        else
        {
            if (unlink(fullName) == -1)
                retVal = fileSysErrors();
        } /* else */
    } /* else */

    return(retVal);
} /* killSingle */


void _vbpS_kill(PBasicString fileName)
/*
 * Remove a file or files from the file system. Wildcards may be
 *  specified to remove multiple files. Use with care; there is no
 *  "undelete".
 *
 *    params : fileName == Name of file to remove (wildcards accepted).
 *   returns : void.
 */
{
    __byte *ascizFileName = __basicStringToAsciz(fileName);
    __byte *path = NULL;
    __byte *fileName;
    __long errorCode;
    DIR *dirInfo;

    parseDirectory(ascizFileName, &dirInfo, &path, &fileName);
    if (dirInfo != NULL)
        errorCode = ((errno == ENOENT) ? ERR_PATH_NOT_FOUND : fileSysErrors());
    else
    {
        if ((strchr(fileName, '*') != NULL) || (strchr(fileName, '?') != NULL))
            errorCode = killWildcards(dirInfo, fileName);
        else
            errorCode = killSingle(path, fileName);
    } /* else */
    closedir(dirInfo);

    __runtimeError(errorCode);         /* continues normally if ERR_NO_ERROR */
} /* _vbpS_kill */


PBasicString _vbS_dir(void)
/*
 * Ooh, a recursive function!
 *
 * This function returns the next entry in a directory, under the
 *  parameters previously specified by _vbSS_dir() or _vbSSi_dir().
 *
 * The other DIR$() functions fill in a linked list with all the
 *  return values that DIR$ without params should return. Then, they
 *  actually call this function to return the first value.
 *
 * This function gets the first item in the linked list, removes it
 *  from the list (moving the secondary item to the primary position, if
 *  available), and checks to make sure the file still exists by stat()ing
 *  it. If the file is gone, we call ourself again (this is the recursive
 *  part), and get the next item in the list, until we either hit the
 *  end of the list, or find a stat()able filename.
 *
 * Dir$() functions do not throw errors. They just return ("") if they can't
 *  help you.
 *
 *       params : void. Must call another DIR() function prior to this
 *                 one for it to be useful, though.
 *      returns : string of next filename in specified pattern. Returns
 *                 an empty string ("") if there are no more matches, or
 *                 if the pattern was bogus, or whatnot.
 */
{
    DirList *nextFile;
    __integer tidx = __getCurrentThreadIndex();
    struct stat statInfo;
    PBasicString retVal = NULL;

    __obtainThreadLock(&fileSystemLock);
    nextFile = threadDirs[tidx];
    if (nextFile != NULL)
        threadDirs[tidx] = nextFile->next;
    __releaseThreadLock(&fileSystemLock);

    if (nextFile == NULL)
        retVal = __createString("", false);
    else
    {
        if (stat(nextFile->fileName, &statInfo) == -1)
            retVal = _vbS_dir();   /* try next file... */
        else
            retVal = __createString(nextFile->fileName, false);
    } /* else */

    return(retVal);
} /* _vbS_dir */


PBasicString _vbSS_dir(PBasicString pathname)
{
} /* _vbSS_dir */


PBasicString _vbSSi_dir(PBasicString pathname, __integer attributes)
{
} /* _vbSSi_dir */


void _vbpS_mkdir(PBasicString dirStr)
{
    __byte *ascizDirName = __basicStringToAsciz(dirStr);
    __long errorCode = ERR_NO_ERROR;

    if (mkdir(ascizDirName, S_IRWXU) == -1)
        errorCode = fileSysErrors();

    __memFree(ascizDirName);
    __runtimeError(errorCode);   /* continues normally if ERR_NO_ERROR */
} /* _vbpS_mkdir */


void _vbpS_rmdir(PBasicString dirStr)
/*
 * Remove a directory from the filesystem. Will not remove files.
 *
 *    params : dirStr == name of directory to remove.
 *   returns : void.
 */
{
    __byte *ascizDirName = __convertPathWinToUnix(__basicStringToAsciz(dirStr));
    __long errorCode = ERR_NO_ERROR;

    if (rmdir(ascizDirName) == -1)
        errorCode = ((errno == ENOENT) ? ERR_PATH_NOT_FOUND : fileSysErrors());

    __memFree(ascizDirName);
    __runtimeError(errorCode);   /* continues normally if ERR_NO_ERROR */
} /* _vbpS_rmdir */


void _vbpSS_name(PBasicString oldName, PBasicString newName)
/*
 * Rename a file or directory. Unlike unix (which overwrites when renaming),
 *  the destination file must not exist. Files that are to be renamed
 *  between file systems are copied using _vbpSS_filecopy(). Directories
 *  that are to be renamed across filesystems throw an error.
 *
 * If the file to rename is already open, ERR_FILE_ALREADY_OPEN is thrown.
 *
 *     params : oldName == string of filename to rename.
 *              newName == string of destination filename.
 *    returns : void.
 */
{
    __byte *ascizOldName = __basicStringToAsciz(oldName);
    __byte *ascizNewName = __basicStringToAsciz(newName);
    __long errorCode = ERR_NO_ERROR;
    struct stat statInfo;

    if (stat(ascizNewName, &statInfo) != -1)           /* dest. file exists */
        errorCode = ERR_FILE_ALREADY_EXISTS;
    else                                       /* dest. file may not exist. */
    {
        if (errno != ENOENT)            /* we want the dest to not exist... */
            errorCode = fileSysErrors();
        else
        {
            if (stat(ascizOldName, &statInfo) == -1)    /* orig must exist. */
                errorCode = fileSysErrors();
            else
            {
                if (rename(ascizOldName, ascizNewName) == -1)
                {
                    if (errno != EXDEV)       /* not a rename across disks? */
                        errorCode = fileSysErrors();
                    else
                    {
                        if (S_ISDIR(statInfo.st_mode))
                            errorCode = ERR_RENAME_ACROSS_DISKS;
                        else
                            _vbpSS_filecopy(ascizOldName, ascizNewName);
                    } /* else */
                } /* if */
            } /* else */
        } /* else */
    } /* else */

    __memFree(ascizOldName);
    __memFree(ascizNewName);
    __runtimeError(errorCode);          /* returns normally if ERR_NO_ERROR */
} /* _vbpSS_name */


static inline int doRead(int fileDes, char *buf, int size)
/*
 * Wraps call to read(). If an error is returned, and errno is
 *  EINTR, we just try again. Why the C library doesn't do this
 *  for you is beyond me, especially when they recommend you
 *  do this in their own manual...
 *
 *     params : fileDes == file handle to read from.
 *              buf == buffer to fill with data.
 *              size == maximum bytes to read.
 *    returns : return value from read().
 */
{
    int retVal;

    do
    {
        retVal = read(fileDes, buf, size);
    } while ((retVal == -1) && (errno == EINTR));

    return(retVal);
} /* doRead */


static inline int doWrite(int fileDes, char *buf, int size)
/*
 * Wraps call to write(). If an error is returned, and errno is
 *  EINTR, we just try again. Why the C library doesn't do this
 *  for you is beyond me, especially when they recommend you
 *  do this in their own manual...
 *
 * Furthermore, this function will keep writing until we either
 *  encounter an error (other than EINTR), or (size) bytes have been
 *  written.
 *
 *     params : fileDes == file handle to write to.
 *              buf == buffer to get write data from.
 *              size == maximum bytes to write.
 *    returns : returns either (-1) or (size). (errno) set if (-1) returned.
 */
{
    int retVal;
    int bytesWritten = 0;

    do
    {
        do
        {
            retVal = write(fileDes, buf, size);
        } while ((retVal == -1) && (errno == EINTR));
        bytesWritten += retVal;
    } while ((retVal != -1) && (bytesWritten < size));

    return(retVal);
} /* doWrite */


void _vbpSS_filecopy(PBasicString src, PBasicString dest)
/*
 * Copy a file from filename (src) to (dest). If (dest) exists,
 *  and the user has write access to it, it is overwritten. If it doesn't
 *  exist, FileCopy tries to create it. If an error occurs while copying
 *  the file, an error is thrown, and what has already been copied is
 *  left behind.
 *
 *      params : src  == filename to copy from.
 *               dest == filename to copy to.
 *     returns : void.
 */
{
    int inFile;
    int outFile;
    char buffer[512];
    struct stat statInfo;
    int errorCode = ERR_NO_ERROR;
    int br = 0;           /* total bytes read.    */
    int rc;               /* generic return code. */

    inFile = open(__basicStringToAsciz(src), O_RDONLY);
    if (inFile == -1)
        __runtimeError(fileSysErrors());

    outFile = open(__basicStringToAsciz(dest), O_WRONLY | O_CREAT | O_TRUNC);
    if (outFile == -1)
    {
        close(inFile);
        __runtimeError(fileSysErrors());
    } /* if */

    fstat(inFile, &statInfo);

    do
    {
        rc = doRead(inFile, buffer, sizeof (buffer));
        if (rc == -1)
            errorCode = fileSysErrors();
        else
        {
            br += rc;
            if (doWrite(outFile, buffer, rc) == -1)
                errorCode = fileSysErrors();
        } /* else if */
    } while ((br < statInfo.st_size) && (errorCode == ERR_NO_ERROR));

    fchmod(outFile, statInfo.st_mode);      /* don't care if this fails... */
    close(inFile);
    close(outFile);

    __runtimeError(errorCode);          /* returns normally if ERR_NO_ERROR */
} /* _vbpSS_filecopy */

/* end of FileSystemFunctions.c ... */

