/*
 * File system interfacing functions. This deals with directories,
 *  collections of files, and file hierarchy. Specific file i/o can
 *  be found in the BasicFileStream and FileIOFunctions modules.
 *
 *     Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
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


#if defined LINUX
#define PATHCHAR '/'
#define CURRENTDIR "."
#elif defined WIN32
#define PATHCHAR '\\'
#define CURRENTDIR "."
#endif


boolean __fileExists(STATEPARAMS, char *fullPathName)
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
    char path[strlen(fullPathName) + 1];
    char *fileName;
    boolean getOut = false;
    boolean retVal = false;

    strcpy(path, fullPathName);
    fileName = strrchr(path, PATHCHAR);

    if (fileName == NULL)   /* no path? */
        fileName = path;
    else
    {
        fileName = '\0';   /* split string into path and filename... */
        fileName++;        /* get past path char... */
    } /* else */

    dp = opendir((fileName == path) ? CURRENTDIR : path);
    if (dp == NULL)
        __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
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


int __fileSystemErrors(STATEPARAMS)
/*
 * Convert C's (errno) into one of the common errors in the BASIClib
 *  file system API.
 *
 *    params : void.
 *   returns : One of the ERR_* constants defined in "ErrorFunctions.h" ...
 */
{

#warning __fileSystemErrors() is not accurate.

    int errorType;

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


void vbpS_kill(STATEPARAMS, PBasicString fileName)
{
    char *ascizFileName = __basicStringToAsciz(STATEARGS, fileName);
    int errorType;

    if (remove(ascizFileName) == -1)   /* -1 == error. */
    {
        errorType = __fileSystemErrors(STATEARGS);
        __runtimeError(STATEARGS, errorType);
    } /* if */

    __memFree(STATEARGS, ascizFileName);
} /* vbpS_kill */


void vbpS_mkdir(STATEPARAMS, PBasicString dirStr)
{
    char *ascizDirName = __basicStringToAsciz(STATEARGS, dirStr);
    int errorType;

    if (mkdir(ascizDirName, S_IRWXU) == -1)
    {
        errorType = __fileSystemErrors(STATEARGS);
        __runtimeError(STATEARGS, errorType);
    } /* if */

    __memFree(STATEARGS, ascizDirName);
} /* vbpS_mkdir */


void vbpS_rmdir(STATEPARAMS, PBasicString dirStr)
{
    char *ascizDirName = __basicStringToAsciz(STATEARGS, dirStr);
    int errorType;

    if (rmdir(ascizDirName) == -1)
    {
        errorType = __fileSystemErrors(STATEARGS);
        __runtimeError(STATEARGS, errorType);
    } /* if */

    __memFree(STATEARGS, ascizDirName);
} /* vbpS_rmdir */


void vbpSS_name(STATEPARAMS, PBasicString oldName, PBasicString newName)
{
    char *ascizOldName = __basicStringToAsciz(STATEARGS, oldName);
    char *ascizNewName = __basicStringToAsciz(STATEARGS, newName);
    int errorType;

    if (rename(ascizOldName, ascizNewName) == -1)
    {
        errorType = __fileSystemErrors(STATEARGS);
        __runtimeError(STATEARGS, errorType);
    } /* if */

    __memFree(STATEARGS, ascizOldName);
    __memFree(STATEARGS, ascizNewName);
} /* vbpSS_name */


void vbp_files(STATEPARAMS)
{
    DIR *dp;
    struct dirent *ep;
    PBasicString pStr;

    dp = opendir ("./");
    if (dp == NULL)
        __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
    else
    {
        /* !!! spacing won't be right. */
        while ((ep = readdir(dp)))
        {
            pStr = __createString(STATEARGS, ep->d_name, true);

/*
!!! need console abstraction...just use vbpS_print?
            vbpS_print(STATEARGS, pStr);
            vbpS_print(STATEARGS, pSpaces);
*/

            __freeString(STATEARGS, pStr);
        } /* while */
        closedir(dp);
    } /* else */
} /* vbp_files */


/* end of FileSystemFunctions.c ... */

