/*
 * File system interfacing functions. This deals with directories,
 *  collections of files, and file hierarchy. Specific file i/o can
 *  be found in the BasicFileStream and FileIOFunctions modules.
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
#include "FileSystemFunctions.h"


__boolean __fileExists(__byte *fullPathName)
/*
 * Check to see if (fileName) is an existing file in the file system.
 *  This doesn't do more than verify the existance; permissions, etc...
 *  are NOT checked.
 *
 *    params : fullPathName == name of file to look up.
 *   returns : boolean true if file exists, boolean false otherwise.
 */
{
    DIR *dp;
    struct dirent *ep;
    __byte path[strlen(fullPathName) + 1];
    __byte *fileName;
    __boolean getOut = false;
    __boolean retVal = false;

    strcpy(path, fullPathName);
    fileName = strrchr(path, PATHCHAR);

    if (fileName == NULL)   /* no path? */
        fileName = path;
    else
    {
        fileName = '\0';   /* split string into path and filename... */
        fileName++;        /* get past path char... */
    } /* else */

    dp = opendir((fileName == path) ? CURRENTDIRSTR : (char *) path);
    if (dp == NULL)
        __runtimeError(ERR_INTERNAL_ERROR);
    else
    {
        for (ep = readdir(dp);
            (getOut == false) && (ep != NULL);
             ep = readdir(dp))
        {
            if (strcmp(fileName, ep->d_name) == 0)
                getOut = retVal = true;
        } /* for */
        closedir(dp);
    } /* else */

    return(retVal);
} /* __fileExists */


static __byte *__convertPathWinToUnix(__byte *pathName)
/*
 * Convert "C:\path\path\filename.txt" to "/path/path/filename.txt"
 *
 *  The drive is removed if it exists, regardless of the letter.
 *  The return value is allocated from scratch.
 *
 *     params : pathName == path to convert.
 *    returns : see above.
 */
{
    __integer length = strlen(pathName);
    __byte *retVal = __memAlloc(length + 1);
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
 *  A "C:" is appended. The return value is allocated from scratch.
 *
 *     params : pathName == path to convert.
 *    returns : see above.
 */
{
    __integer length = strlen(pathName);
    __byte *retVal = __memAlloc(length + 3);
    __integer i;

    strcpy(retVal, "C:");

    for (i = 2; *pathName != '\0'; pathName++, i++)
        retVal[i] = ((*pathName == '/') ? '\\' : *pathName);

    retVal[i] = '\0';
    return(retVal);
} /* __convertPathUnixToWin */



static __long __fileSystemErrors(void)
/*
 * Convert C's (errno) into one of the common errors in the BASIClib
 *  file system API.
 *
 *    params : void.
 *   returns : One of the ERR_* constants defined in "ErrorFunctions.h" ...
 */
{

#warning __fileSystemErrors() is not accurate.

    __long errorType;

    switch (errno)
    {
        case EXDEV:
            errorType = ERR_RENAME_ACROSS_DISKS;
            break;
        case EACCES:
        case EBUSY:
        case EPERM:
        case EISDIR:
        case EROFS:
            errorType = ERR_PERMISSION_DENIED;
            break;
        case ENOENT:
        case ENOTDIR:
            errorType = ERR_FILE_NOT_FOUND;
            break;
        case ENOMEM:
            errorType = ERR_OUT_OF_MEMORY;
            break;
        default:
            errorType = ERR_INTERNAL_ERROR;
            break;
    } /* switch */

   return(errorType);
} /* __fileSystemErrors */


void _vbpS_kill(PBasicString fileName)
{
    __byte *ascizFileName = __basicStringToAsciz(fileName);
    __long errorType;

    if (remove(ascizFileName) == -1)   /* -1 == error. */
    {
        errorType = __fileSystemErrors();
        __runtimeError(errorType);
    } /* if */

    __memFree(ascizFileName);
} /* _vbpS_kill */


void _vbpS_mkdir(PBasicString dirStr)
{
    __byte *ascizDirName = __basicStringToAsciz(dirStr);
    __long errorType;

    if (mkdir(ascizDirName, S_IRWXU) == -1)
    {
        errorType = __fileSystemErrors();
        __runtimeError(errorType);
    } /* if */

    __memFree(ascizDirName);
} /* _vbpS_mkdir */


void _vbpS_rmdir(PBasicString dirStr)
{
    __byte *ascizDirName = __basicStringToAsciz(dirStr);
    __long errorType;

    if (rmdir(ascizDirName) == -1)
    {
        errorType = __fileSystemErrors();
        __runtimeError(errorType);
    } /* if */

    __memFree(ascizDirName);
} /* _vbpS_rmdir */


void _vbpSS_name(PBasicString oldName, PBasicString newName)
{
    __byte *ascizOldName = __basicStringToAsciz(oldName);
    __byte *ascizNewName = __basicStringToAsciz(newName);
    __long errorType;

    if (rename(ascizOldName, ascizNewName) == -1)
    {
        errorType = __fileSystemErrors();
        __runtimeError(errorType);
    } /* if */

    __memFree(ascizOldName);
    __memFree(ascizNewName);
} /* _vbpSS_name */


void _vbp_files(void)
{
#warning Scrap FILES statement?
/* !!! SCRAP THIS?! */
    DIR *dp;
    struct dirent *ep;

    dp = opendir ("./");
    if (dp == NULL)
        __runtimeError(ERR_INTERNAL_ERROR);
    else
    {
        /* !!! spacing won't be right. */
        while ((ep = readdir(dp)))
        {
            __printAsciz(ep->d_name);
            __printNewLine();
        } /* while */
        closedir(dp);
    } /* else */
} /* _vbp_files */


void _vbpSS_filecopy(PBasicString src, PBasicString dest)
{
} /* _vbpSS_filecopy */

/* end of FileSystemFunctions.c ... */

