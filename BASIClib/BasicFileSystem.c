/*
 * Abstracted file system layer. This is used by the FileSystemFunctions
 *  and FileIOFunctions modules, among others.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *    Written by Ryan C. Gordon.
 */

#include <sys/stat.h>
#define __VBUNIXDIRDEFINED__
#include "StdBasic.h"

__byte __getCurrentDriveLetter(void)
/*
 * Retrieve the process (or system-wide, whatever applies) current
 *  drive letter. For systems that don't support drive letters, such
 *  as Unix and MacOS, this just returns ('C').
 *
 *       params : void.
 *      returns : Current working drive letter.
 */
{

#   if (defined __NODRIVELETTERS)
        return('C');
#   elif (defined WIN32)
#       error WIN32 needs to get drive letters!
#   elif (defined OS2)
#       error OS/2 needs to get drive letters!
#   endif

} /* __getCurrentDriveLetter */


__long __vbFileSystemErrors(void)
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
} /* __vbFileSysErrors */


static __boolean casedSearch(DIR *dir, __byte *file)
/*
 * This is an internal convenience function; this is ONLY called from
 *  __getCasedDirEntry().
 *
 * This function determines if there are any case-insensitive matches
 *  to (file) in the directory represented by (dir).
 *
 * If one is found, the string pointed to by (file) is overwritten with
 *  the match's case.
 *
 *     params : dir  == DIR pointer to get dir entries from.
 *              file == filename to insensitively search for.
 *    returns : boolean true if match found, boolean false otherwise.
 */
{
    struct dirent dirEntry;
    struct dirent *pDir;
    int rc;

    do
    {
        pDir = &dirEntry;
        rc = readdir_r(dirInfo, pDir, &pDir);
        if (rc == 0)
        {
            if (strcasecmp(file, pDir->d_name) == 0)    /* match. */
            {
                strcpy(file, pDir->d_name);
                return(true);
            } /* if */
        } /* if */
    } while (rc == 0);

    return(false);
} /* casedSearch */


__boolean __getCasedDirEntry(DIR *dir, __byte *path, __byte *origFile)
/*
 * This finds a directory entry, in a case-insensitive manner. This is
 *  done whether the "Ignore filename case" option is enabled or not.
 *
 * (origFile), in (path), is stat()ed, and if it exists, it's returned.
 *  Otherwise, we check to see if the filename exists in a different case.
 *  If so, the first to match a case-insensitive search is returned.
 *
 * Note that access rights beyond what's needed to stat() and read directory
 *  entries are not checked. This is just goes by existance. Furthermore, this
 *  means that the user may not have rights to the case-insensitive file
 *  chosen, but may have rights to another file that would have matched.
 *  The solution here is to either not use case-insensitivity, or specify
 *  the correct filename case in the first place.
 *
 *       params : dir      == DIR pointer. This is the directory where we
 *                             want to search for matching entries.
 *                path     == directory (w/o trailing pathchar) where
 *                             file resides.
 *                origFile == filename to search for.
 *      returns : boolean true if match was found. boolean false otherwise.
 *                 The string pointed to by (origFile) is overwritten with
 *                 the chosen entry if a match is found.
 */
{
    __boolean retVal = true;
    struct stat statBuf;
    char completeName[strlen(path) + strlen(origFile) + 2];

    sprintf(completeName, "%s%c%s", path, __PATHCHAR, origFile);

    if (stat(completeName, &statBuf) == -1)  /* original case not exist? */
        retVal = casedSearch(dir, origFile);

    return(retVal);
} /* __getCasedDirEntry */


__byte *__convertPathWinToUnix(__byte *pathName)
/*
 * Convert "C:\path\path\filename.txt" to "/path/path/filename.txt"
 *
 * The drive is removed if it is "C:" or "c:". If the drive letter is
 *  anything else, ERR_PATH_NOT_FOUND !!! is thrown. The drive is assumed
 *  to be "C:" if it is not specified.
 *
 * The return value is allocated from scratch, and is collectable.
 *
 *     params : pathName == path to convert.
 *    returns : see above.
 */
{
    __integer length = strlen(pathName);
    __byte *retVal = __memAllocNoPtrs(length + 1);
    __integer i;

    if ((length >= 2) && (pathName[1] == ':'))   /* drive letter? */
    {
        if (tolower(pathName[0]) != 'c')
            __runtimeError(ERR_PATH_NOT_FOUND);  /* !!! check this. */
        else
            pathName += 2;                       /* bump pathName past it. */
    } /* if */

    for (i = 0; *pathName != '\0'; pathName++, i++)
        retVal[i] = ((*pathName == '\\') ? '/' : *pathName);

    retVal[i] = '\0';   /* terminate the new string. */
    return(retVal);
} /* __convertPathWinToUnix */


__byte *__convertPathUnixToWin(__byte *pathName)
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
    __byte *retVal = __memAllocNoPtrs(strlen(pathName) + 3);
    __integer i;

    strcpy(retVal, "C:");

    for (i = 2; *pathName != '\0'; pathName++, i++)
        retVal[i] = ((*pathName == '/') ? '\\' : *pathName);

    retVal[i] = '\0';
    return(retVal);
} /* __convertPathUnixToWin */


static DIR *openDirRelativeOrAbsolute(__byte **dirToParse)
/*
 * This function evaluates a path passed to it, and decides if it's
 *  an ABSOLUTE or RELATIVE path. If it's absolute, this function
 *  calls opendir() on the root directory (or the root directory on
 *  the current working drive, if applicable), otherwise, opendir()
 *  is called on the current working directory (of the current working
 *  drive, if applicable). The results from the opendir() call is returned.
 *
 *     params : dirToParse == directory to check for absolutivity/relativity.
 *    returns : opendir() on working or root directory (of working drive,
 *               if applicable). (*dirToParse) is also set to point to
 *               the first path segment, past the __PATHCHAR, or the drive
 *               letter or whatnot.
 */
{

#ifdef __NODRIVELETTERS

    char rootDir[2] = {__PATHCHAR, '\0'};
    DIR *retVal;

    if ((*dirToParse)[0] != __PATHCHAR)  /* doesn't start with a path char? */
        retVal = __CURRENTDIRSTR;
    else
    {
        (*dirToParse)++;
        retVal = opendir(rootDir);
    } /* else */

    return(retVal);

#else

    int pathIndex = 0;
    char dirToOpen[4] = {'?', ':', __PATHCHAR, '\0'};
    DIR *retVal;

    if ( ((*dirToParse)[0] == '\0') || ((*dirToParse)[1] != ':') )
        dirToOpen[0] = __getCurrentDriveLetter();
    else
    {
        pathIndex = 2;                      /* point past drive letter.   */
        dirToOpen[0] = (*dirToParse)[0];    /* fill in root drive letter. */
    } /* else */

    if ((*dirToParse)[pathIndex] == __PATHCHAR)         /* absolute path? */
    {
        rootDir[2] = __CURRENTDIRCHAR;
        pathIndex++;
    } /* if */

    (*dirToParse) += pathIndex;

    return(opendir(dirToOpen));

#endif

} /* openDirRelativeOrAbsolute */


void __parsePathForInsensitiveMatches(__byte *dirToParse)
/*
 * This is the ugliest code in all of BASIClib.
 *
 * Each element in the path specified by (dirToParse) is checked for
 *  a case-sensitive match. Failing that, the first case-insensitive
 *  match is chosen, and the string pointed to by (dirToParse) is updated
 *  to reflect this.
 *
 * The resulting string will be the same, but pieces of it may change case.
 *  Identical matches are selected first on each path element, and failing
 *  that, the first matching entry is selected. If no elements match,
 *  the parsing is abandoned, and a NULL is returned for (dirInfo). Note this
 *  whole process adds lots of overhead, and is done by the OS on Windows and
 *  OS/2 anyhow...but if you REALLY need this on Unix and friends, it's here.
 *  Without case-ignoring, we can just call opendir on the whole thing, but
 *  with it, we need to open each subdirectory in the path, and do a lot of
 *  comparing within each directory to find matches. While this probably isn't
 *  usually noticable overhead, there is the potential for it to be on
 *  VERY large directories, and/or if you call file-system related APIs in a
 *  loop. Also, you may end up in an unexpected directory, or hit a file
 *  match before the desired directory, in which case you've got a bogus
 *  path. Caveat emptor.
 *
 *      params : dirToParse == string of path to check.
 *     returns : void. (dirToParse) is updated.
 */
{
    struct stat statBuf;
    __byte *pathPtr = dirToParse;
    DIR *tmpDir = openDirRelativeOrAbsolute(&pathPtr);
    __byte *endEntry = strchr(pathPtr, __PATHCHAR);
    __byte tmpByte1;
    __byte tmpByte2;

    do
    {
        if (endEntry == NULL)
            endEntry = pathPtr + strlen(pathPtr);    /* point to NULL char. */

        tmpByte1 = *endEntry;
        *endEntry = '\0';

        tmpByte2 = *pathPtr;
        *pathPtr = '\0';

        __getCasedDirEntry(tmpDir, dirToParse, pathPtr + 1);
        closedir(tmpDir);
        *pathPtr = tmpByte2;    /* now last matched entry is part of dir. */
        tmpDir = opendir(dirToParse);    /* attempt to open it. */

        *endEntry = tmpByte1;   /* now whole orig path is whole again. */

            /*
             * If the newly matched element cannot be opened as a
             *  directory, we bail. This either means we've hit the
             *  file element, so our work is done, or the path is
             *  invalid (or at least, is invalid after our matching...)
             */
        if (tmpDir == NULL)
            return;     

        pathPtr = endEntry;
        if (*endEntry != '\0')                      /* end of string? */
            endEntry = strchr(endEntry + 1);
    } while (*endEntry != '\0');

    closedir(tmpDir);
} /* __parsePathForInsensitiveMatches */


void __parseDir(__byte *dirToParse, DIR **dirInfo,
                __byte **fileName, __byte **path)
/*
 * Take a path (possibly containing wildcards in the filename part, and
 *  possibly not containing a filename at all), and check its validity by
 *  trying to open the directory (via opendir()).
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
 *  considered read only, non-reallocatable, and non-collectable.
 *
 * (path), (fileName), and/or (dirInfo) may be NULL arguments, in which case
 *  the NULL vars will not be filled in with return values.
 *
 * (*dirInfo) will hold the retval from opendir(). This may be NULL, if
 *   there was an error, i.e. the path doesn't exist, etc...It would be
 *   wise to check this value before any others.
 *
 * (dirToParse) may point to a directory entry. This entry will be stored
 *  in the fileName, even though it is not a file, per se.

 * If (dirToParse) ends with a pathchar, that final pathchar will be set to
 *  '\0', and (*fileName) will point to it. This is okay.
 *
 * Wildcards are not checked here. Not all callers to this function want
 *  to support wildcards (like RMDIR), and in BASIC, no APIs support
 *  wildcards on directories (such as "C:\mydir\*\core")...files in the
 *  destination directory will be checked for wildcards (in a
 *  case-insensitive way, if the option is enabled) by the APIs themselves.
 *
 *      params : dirToParse == string of path/filename to parse.
 *               *dirInfo   == returns retval from opendir().
 *               *fileName  == returns pointer to filename part of (dirToParse).
 *               *path      == returns pointer to path part of (dirToParse).
 *     returns : void.
 */
{
    __byte *retValPath;
    __byte *retValFileName;

    retValPath = strrchr(dirToParse, __PATHCHAR);
    if (retValPath == NULL)
    {
        retValPath = __CURRENTDIRSTR;
        retValFileName = dirToParse;
    } /* if */
    else
    {
        *retValPath = '\0';    /* split path and filename. */
        retValFileName = retValPath + 1;
        retValPath = dirToParse;
    } /* else */

    if (dirInfo != NULL)
        *dirInfo = opendir(retValPath);

    if (path != NULL)
        *path = retValPath;

    if (fileName != NULL)
        *fileName = retValFileName;
} /* __parseDir */


void __registerFileHandle(__byte *filename, __integer handle)
{
} /* __registerFileHandle */


void __deregisterFileHandle(__byte *filename, __integer handle)
{
} /* __deregisterFileHandle */


__integer __searchForFileHandle(__integer handle)
{
} /* __searchForFileHandle */


__integer __searchForFileHandleByName(__byte *filename)
{
} /* __searchForFileHandleByName */


/* end of BasicFileSystem.c ... */


