/*
 * BASIClib's file system abstraction layer...
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICFILESYSTEM_H_
#define _INCLUDE_BASICFILESYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* fnmatch() seems to need this... */
#ifndef _GNU_SOURCE
#   define _GNU_SOURCE 1
#endif

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
#   define __convertPathWinToLocal(pathname)  __copyAscizString(pathname)
#   define __convertPathLocalToWin(pathname)  __copyAscizString(pathname)
#else
#   error No file system compatibility routines for this system!
#endif


    /* The functions. */

__byte   *__convertPathWinToUnix(__byte *pathName);
__byte   *__convertPathUnixToWin(__byte *pathName);
__byte    __getCurrentDriveLetter(void);
__long    __vbFileSystemErrors(void);

#ifdef __VBUNIXDIRDEFINED__
    __boolean __getCasedDirEntry(DIR *dir, __byte *path, __byte *origFile);
      void    __parsePathForInsensitiveMatches(__byte *dirToParse);
      void    __parseDir(__byte *dirToParse, DIR **dirInfo,
                         __byte **fileName, __byte **path);
#endif


#ifdef __cplusplus
}
#endif

#endif  /* _INCLUDE_BASICFILESYSTEM_H_ */
#endif  /* _INCLUDE_STDBASIC_H_   */

/* end of BasicFileSystem.h ... */


