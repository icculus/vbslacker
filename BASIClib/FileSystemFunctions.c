/*
 * File system interfacing functions. This deals with directories,
 *  collections of files, and file hierarchy. Specific file i/o can
 *  be found in the FileIOFunctions module.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *     This file initially written by Ryan C. Gordon.
 */

    /* fnmatch() seems to need this... */
#ifndef _GNU_SOURCE
#   define _GNU_SOURCE 1
#endif

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


    /*
     * Paths need conversion to and from Windows compatible formats.
     *  Please write your platform-specific functions, and fill them in here.
     *  Note that the WinToLocal function may be passed arguments that
     *  are already in local form...so you'll have to decide if the
     *  conversion needs to be done in the first place...
     *
     * Return values should be newly allocated, and collectable.
     */
#if (defined UNIX)
#   define __convertPathWinToLocal(pathname)  __convertPathWinToUnix(pathname)
#   define __convertPathLocalToWin(pathname)  __convertPathUnixToWin(pathname)
#elif ((defined WIN32) || (defined OS2))
#   define __convertPathWinToLocal(pathname)  __assignString(NULL, pathname)
#   define __convertPathLocalToWin(pathname)  __assignString(NULL, pathname)
#else
#   error No file system compatibility routines for this system!
#endif

    /* Stuff still todo... */
#warning Filename case check!
#warning Check if files already open!


/* These are according to MSDN... */
#define vbNormal    0
#define vbReadOnly  1
#define vbHidden    2
#define vbSystem    4
#define vbVolume    8
#define vbDirectory 16


    /*
     * This structure keeps tracks of thread-specific data. The DIR$()
     *  functions need to keep state between calls, so an array of
     *  pointers to these structures (one element per thread) is kept in
     *  (threadDirInfo), defined below.
     */
typedef struct _THREADDIRINFO
{
    __integer attributes;       /* file attributes for DIR$() matching. */
    DIR *dir;                   /* retval from opendir(), for DIR$()... */
    char *pattern;              /* wildcard pattern to match in DIR$(). */
} ThreadDirInfo;



static ThreadDirInfo **threadDirInfo = NULL;  /* Thread state. */
static ThreadLock fileSystemLock;             /* Threadproofs threadDirInfo. */
static __boolean ignoreFilenameCase;          /* from __getInitFlags(). */
static __boolean windowsFileSystem;           /* from __getInitFlags(). */


static void cleanupDir(ThreadDirInfo *tdi)
/*
 * This function just cleans up thread state after DIR$() is done with
 *  an old query; this may be because a new query is requested, or an
 *  old one has run out of new directory entries. A thread termination
 *  also calls this. It gives us a place to close the (DIR *) stream, and
 *  remove references so the garbage collector can kick in.
 *
 *      params : tdi == ThreadDirInfo object for this thread.
 *     returns : void.
 */
{
    if (tdi->dir != NULL)
    {
        closedir(tdi->dir);
        tdi->dir = NULL;
    } /* if */

    tdi->pattern = NULL;
    tdi->attributes = vbNormal;
} /* cleanupDir */


void __initFileSystemFunctions(void)
/*
 * Initialize this module. Creates a ThreadLock, and sets up a few
 *  static variables, so we don't have to continually call __getInitFlags()...
 *
 *     params : void.
 *    returns : void.
 */
{
    __createThreadLock(&fileSystemLock);
    ignoreFilenameCase = ((__getInitFlags() & INITFLAG_FILENAMES_IGNORECASE) ?
                                true : false);
    windowsFileSystem  = ((__getInitFlags() & INITFLAG_WINDOWS_FILESYSTEM) ?
                                true : false);
} /* __initFileSystemFunctions */


void __deinitFileSystemFunctions(void)
/*
 * Deinitialize this module; destroy the ThreadLock.
 *
 *     params : void.
 *    returns : void.
 */
{
    __destroyThreadLock(&fileSystemLock);
} /* __deinitFileSystemFunctions */


void __initThreadFileSystemFunctions(__integer tidx)
/*
 * This is called whenever a new thread is spun, to notify this
 *  module that details need to be updated.
 *
 * We make sure the threadDirInfo array is large enough to handle
 *  all existing threads, and plug in a newly allocated ThreadDirInfo
 *  structure for this new thread.
 *
 *      params : tidx == thread index of newly-spun thread.
 *     returns : void.
 */
{
    __long alloc = (__getHighestThreadIndex + 1) * sizeof (ThreadDirInfo *);

    __obtainThreadLock(&fileSystemLock);
    threadDirInfo = __memRealloc(threadDirInfo, alloc);
    threadDirInfo[tidx] = __memAlloc(sizeof (ThreadDirInfo));
    threadDirInfo[tidx]->attributes = vbNormal;
    threadDirInfo[tidx]->dir = NULL;
    threadDirInfo[tidx]->pattern = NULL;
    __releaseThreadLock(&fileSystemLock);
} /* __initThreadFileSystemFunctions */


void __deinitThreadFileSystemFunctions(__integer tidx)
/*
 * Cleanup thread state, and set the ThreadDirInfo element for
 *  the dying thread to NULL to make it eligible for garbage collection.
 *
 *     params : tidx == thread index of dying thread.
 *    returns : void.
 */
{
    __obtainThreadLock(&fileSystemLock);
    cleanupDir(threadDirInfo[tidx]);
    threadDirInfo[tidx] = NULL;
    __releaseThreadLock(&fileSystemLock);
} /* __deinitThreadFileSystemFunctions */


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
            return(ERR_DEVICE_IO_ERROR);
    } /* switch */

    return(ERR_INTERNAL_ERROR);    /* uh? */
} /* fileSysErrors */


#ifdef UNIX
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
#endif /* defined UNIX */


static void parseDir(__byte *dirToParse, DIR **dirInfo,
                     __byte **fileName, __byte **path)
/*
 * This is an internal convenience function. Take a path (possibly
 *  containing wildcards in the filename), and check its validity
 *  by trying to open the directory (via opendir()).
 *
 * The string pointed to by (dirToParse) is split on the final
 *  path character. That char is replaced by a NULL char, and (*fileName)
 *  is set to the first char of the filename (the character after the
 *  final path char. This may be a NULL char itself.) If there are no
 *  path characters, the entire string is assumed to be a filename in the
 *  current directory, and (*fileName) points to (dirToParse).
 *
 * (*path) is either set to (dirToParse), or it points to  __CURRENTDIRSTR
 *  (".", for example.) As it might point to a constant, (*path) should be
 *  considered read only.
 *
 * (*dirInfo) will hold the retval from opendir(). This may be NULL, if
 *   there was an error, i.e. the path doesn't exist, etc...It would be
 *   wise to check this value before any others.
 *
 * Confused, yet? This comment is longer than the code!  :)
 *
 *      params : dirToParse == string of path/filename to parse.
 *               *dirInfo   == returns retval from opendir().
 *               *fileName  == returns pointer to filename part of (dirToParse).
 *               *path      == returns pointer to path part of (dirToParse).
 *     returns : void.
 */
{
#warning parseDir() does NOT check directory name case!

    __byte *ascizFileName = __convertPathWinToLocal(dirToParse);

    *fileName = strrchr(ascizFileName, __PATHCHAR);
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
         *  one directory.)   !!! I think?
         */
    *dirInfo = opendir(*path);
} /* parseDir */


static __long killWildcards(DIR *dirInfo, __byte *path, __byte *fileName)
/*
 * Called exclusively from _vbpS_kill().
 *
 * Terminates a collection of files that match the pattern in (fileName).
 *  Directories are unharmed. This functions is particularly dangerous if
 *  the "ignore filename case" runtime flag is set...
 *
 *     params : dirInfo  == structure of directory where files reside.
 *              path     == pathname of directory where files reside.
 *              fileName == wildcard pattern to match.
 *    returns : error code to throw. ERR_NO_ERROR if cool.
 */
{
    __long retVal = ERR_NO_ERROR;
    struct dirent dirEntry;
    struct dirent *pDir;
    int rc;
    int flags = 0;
    char fullName[strlen(path) + NAME_MAX + 2];

    if (ignoreFilenameCase)
        flags |= FNM_CASEFOLD;

    do
    {
        pDir = &dirEntry;
        rc = readdir_r(dirInfo, pDir, &pDir);
        if (rc == 0)
        {
            if (pDir->d_type != DT_DIR)
            {
                if (fnmatch(fileName, pDir->d_name, flags) == 0)
                {
                    sprintf(fullName, "%s%c%s", path, __PATHCHAR, pDir->d_name);
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

    sprintf(fullName, "%s%c%s", path, __PATHCHAR, fileName);

#warning killSingle() does NOT check filename case.

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


void _vbpS_kill(PBasicString fileSpec)
/*
 * Remove a file or files from the file system. Wildcards may be
 *  specified to remove multiple files. Use with care; there is no
 *  "undelete".
 *
 *    params : fileName == Name of file to remove (wildcards accepted).
 *   returns : void.
 */
{
    __byte *path = NULL;
    __byte *fileName;
    __long errorCode;
    DIR *dirInfo;

    parseDir(__basicStringToAsciz(fileSpec), &dirInfo, &path, &fileName);
    if (dirInfo != NULL)
        errorCode = ((errno == ENOENT) ? ERR_PATH_NOT_FOUND : fileSysErrors());
    else
    {
        if ((strchr(fileName, '*') != NULL) || (strchr(fileName, '?') != NULL))
            errorCode = killWildcards(dirInfo, path, fileName);
        else
            errorCode = killSingle(path, fileName);
    } /* else */
    closedir(dirInfo);

    __runtimeError(errorCode);         /* continues normally if ERR_NO_ERROR */
} /* _vbpS_kill */


static __boolean checkReadOnly(struct dirent *pDir)
{
#warning write checkReadOnly()!
    return(false);
} /* checkReadOnly */


static __boolean checkDirAttrs(struct dirent *pDir, ThreadDirInfo *tdi)
/*
 * This is used by DIR$() to determine if a specified file fits the
 *  specified attributes and wildcard pattern.
 *
 *     params : pDir == ptr to structure containing file under scrutiny.
 *              tdi  == thread state containing this DIR$()'s query params.
 *    returns : boolean (true) if file qualifies, boolean (false) otherwise.
 */
{
    int flags = 0;
    __boolean retVal = true;

    if (ignoreFilenameCase)
        flags |= FNM_CASEFOLD;

        /* check pattern match, then attributes... */
    if (fnmatch(tdi->pattern, pDir->d_name, flags) == 0)
    {
        if (tdi->attributes & vbDirectory)
        {
            if (pDir->d_type != DT_DIR)
                retVal = false;
        } /* if */

        if (tdi->attributes & vbReadOnly)
        {
            if (checkReadOnly(pDir) == false)
                retVal = false;
        } /* if */
    } /* if */

    return(retVal);
} /* checkDirAttrs */


static inline PBasicString dirOutput(__byte *fileName)
/*
 * This function takes a filename, and, if required, converts it
 *  to a Windows-like format. This is called exclusively from
 *  _vbS_dir(void);
 *
 *    params : fileName == filename to parse.
 *   returns : newly allocated (collectable) BASIC string of either
 *              (fname)'s data, or (fname) converted to a Windows-like string.
 */
{
    __byte *retVal;

    if (windowsFileSystem)
        retVal = __convertPathLocalToWin(fileName);
    else
        retVal = __createString(fileName, false)

    return(retVal);
} /* dirOutput */


PBasicString _vbS_dir(void)
/*
 * Ooh, a recursive function!
 *
 * This function returns the next entry in a directory, under the
 *  parameters previously specified by _vbSS_dir() or _vbSSi_dir().
 *
 * The other DIR$() functions merely set up the ThreadDirInfo structure.
 *  Then, they actually call this function to return the first value.
 *
 * This function gets the next directory entry (via readdir_r()),
 *  and calls checkDirAttrs() to see if the entry fits the pattern and
 *  attributes specified. If the file does not match, we call ourself again
 *  (this is the recursive part), and get the next entry, until we either hit
 *  the end of the list (return ("")), or find a fitting entry.
 *
 * Dir$() functions do not throw errors. They just return ("") if they can't
 *  help you. !!! is this right?
 *
 *       params : void. Must call another DIR() function prior to this
 *                 one for it to be useful, though.
 *      returns : string of next filename in specified pattern. Returns
 *                 an empty string ("") if there are no more matches, or
 *                 if the pattern was bogus, or whatnot.
 */
{
    __integer tidx = __getCurrentThreadIndex;
    ThreadDirInfo *tdi;
    struct dirent dirEntry;
    struct dirent *pDir;
    PBasicString retVal = NULL;

    __obtainThreadLock(&fileSystemLock);
    tdi = threadDirInfo[tidx];
    __releaseThreadLock(&fileSystemLock);

    if (tdi->dir != NULL)
    {
        pDir = &dirEntry;
        if (readdir_r(tdi->dir, pDir, &pDir) != 0)
        {
            cleanupDir(tdi);
            retVal = __createString("", false);
        } /* if */
        else
        {
            if (checkDirAttrs(pDir, tdi) == true)
                retVal = dirOutput(pDir->d_name);
            else
                retVal = _vbS_dir();
        } /* else */
    } /* if */

    return(retVal);
} /* _vbS_dir */


PBasicString _vbSSi_dir(PBasicString pathname, __integer attributes)
/*
 * Start a new directory query, specifying both a wildcard pattern, and
 *  specific file attributes.
 *
 *    params : pathname   == wildcard pattern to match.
 *             attributes == numeric attributes files must have.
 *   returns : BASIC String of first matching file. ("") if none.
 */
{
    DIR *dirInfo;
    ThreadDirInfo *tdi;
    __byte *path;
    __byte *filename;

    __obtainThreadLock(&fileSystemLock);
    tdi = threadDirInfo[__getCurrentThreadIndex];
    __releaseThreadLock(&fileSystemLock);

    cleanupDir(tdi);

    parseDir(__basicStringToAsciz(pathname), &dirInfo, &path, &filename);

    tdi->attributes = attributes;
    tdi->dir = dirInfo;
    tdi->pattern = ((*filename == '\0') ? (__byte *) "*" : filename);

    return(_vbS_dir());
} /* _vbSSi_dir */


PBasicString _vbSS_dir(PBasicString pathname)
/*
 * Start a new directory query, specifying only a wildcard pattern.
 *
 *    params : pathname   == wildcard pattern to match.
 *   returns : BASIC String of first matching file. ("") if none.
 */
{
    return(_vbSSi_dir(pathname, vbNormal));
} /* _vbSS_dir */


void _vbpS_mkdir(PBasicString dirStr)
/*
 * Create a new directory.
 *
 *    params : dirStr == path to create.
 *   returns : void.
 */
{
    __byte *newDir = __convertPathWinToLocal(__basicStringToAsciz(dirStr));
    __long errorCode = ERR_NO_ERROR;

    if (mkdir(newDir, S_IRWXU) == -1)
        errorCode = fileSysErrors();

    __memFree(newDir);
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
    __byte *newDir = __convertPathWinToLocal(__basicStringToAsciz(dirStr));
    __long errorCode = ERR_NO_ERROR;

    if (rmdir(newDir) == -1)
        errorCode = ((errno == ENOENT) ? ERR_PATH_NOT_FOUND : fileSysErrors());

    __memFree(newDir);
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

#warning _vbpSS_name() needs to check for filename case!

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
                            _vbpSS_filecopy(oldName, newName);
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
    char buffer = __memAllocNoPtrs(512);
    struct stat statInfo;
    int errorCode = ERR_NO_ERROR;
    int br = 0;           /* total bytes read.    */
    int rc;               /* generic return code. */


#warning filecopy needs to check filename case!

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
    __memFree(buffer);

    __runtimeError(errorCode);          /* returns normally if ERR_NO_ERROR */
} /* _vbpSS_filecopy */


void _vbpS_chdir(PBasicString newDir)
/*
 * Change the current working directory. All relative paths used in
 *  file i/o will work from the new working directory from now on.
 *
 *    params : newDir == new directory to make current.
 *   returns : void. Throws a few errors, though.
 */
{
    __byte *str = __convertFilenameWinToLocal(__basicStringToAsciz(newDir));

#warning _vbpS_chdir() needs to check filename case!

    int rc = chdir(str);
    int errorCode = ((rc == -1) ? fileSystemErrors() : ERR_NO_ERROR);

    __memFree(str);
    __runtimeError(errorCode);          /* returns normally if ERR_NO_ERROR */
} /* _vbpS_chdir */


static __byte *doGetcwd(void)
/*
 * This is called exclusively from _vbSS_curdir_DC_(). Since getcwd()
 *  is a little retarded (and a little less portable) unless you let it
 *  return a malloc()ed object (which is unacceptable to our garbage
 *  collector. Therefore this function handles the retrieval of the
 *  current working directory (CWD) in a garbage-collection-friendly way.
 *
 *     params : void.
 *    returns : A string of the current working directory (CWD) in
 *               a garbage collectable object.
 */
{
    __long bufSize = 256;
    __byte *buf;
    __boolean getOut = false;
    __byte retVal;

    do
    {
        buf = __memAlloc(bufSize);
        if (getcwd(buf, bufSize) != NULL)
            getOut = true;
        else
        {
            bufSize *= 2;
            __memFree(buf);
        } /* if */
    } while (getOut == false);

    if (windowsFileSystem)
    {
        retVal = __convertFilenameLocalToWin(buf);
        __memFree(buf);
    } /* if */
    else
        retVal = buf;

    return(retVal);
} /* doGetcwd */



PBasicString _vbSS_curdir_DC_(PBasicString drive)
/*
 * Return current working directory by drive letter. Under Unix-like
 *  Operating systems, the only valid drive letter is "C"...You can set
 *  the behavior of this by setting the "windows filesystem compatible"
 *  option at compile time. Either way on unix, you get a runtime error
 *  if you specify a drive other than C or blank, but you'll either get
 *  "/home/gordonr" or "C:\home\gordonr", depending on how you set the
 *  compatibility. 
 *
 *    params : drive == drive letter. Only first letter is read. ("") means
 *                      current drive.
 *   returns : see above.
 */
{
    PBasicString retVal = NULL;

    #ifdef __NODRIVELETTERS

        if ((drive->length > 0) || (toupper(drive->data[0]) != 'C'))
            __runtimeError(ERR_PATH_FILE_ACCESS_ERROR);  /* !!! check this! */
        else
            retVal = __createString(doGetcwd(), false);

    #else

        #warning Drive-letter OSes do not implement _vbSS_curdir_DC_() right!
        retVal = __createString(doGetcwd(), false);

    #endif

    return(retVal);
} /* _vbSS_curdir */


PBasicString _vbS_curdir_DC_(void)
/*
 * Same as above vbSS_curdir_DC_(), but always gets current drives's
 *  working directory.
 *
 *     params : void.
 *    returns : see above.
 */
{
    return(_vbSS_curdir_DC_(__createString("", false)));
} /* _vbS_curdir_DC_ */

/* end of FileSystemFunctions.c ... */

