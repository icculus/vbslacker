/*
 * Some platform-specific stuff that all vbSlacker code should include.
 *  This file is automagically included by StdBasic.h and BasicLib.h ...
 *
 * You need to define one (and only one) of the following before including
 *  this file (if you don't, the compile generates a fatal error...) :
 *
 *    LINUXELF_I386  for intel-based ELF-binary linux machines.
 *    WIN32_I386     for intel-based Win32 machines (Win95/98/NT).
 *    (...and perhaps more to come...)
 *
 * And you'll end up with these defines when this file is done processing...
 *
 *    FUNCNAME_PREPEND ... equals "_", or "", or whatever, depending on
 *                         what the platform's compiler adds to the beginning
 *                         of function identifiers. You need to know this for
 *                         inline assembly calls.
 *
 *    ARCHITECTURE     ... equals "i386" (...and perhaps more to come...),
 *                         depending on what the platform's processor type is.
 *
 *    STACK_DIRECTION  ... equals (-1) if the stack on a given platform grows
 *                         downward, or (1) if stack grows upward.
 *
 *    EOL_STRING       ... equals "\n" or "\r\n" (...and perhaps more to
 *                         come...) depending on what characters signify
 *                         an end-of-line on a given platform.
 *
 *    PATHCHAR         ... equals '/' or '\\' (...and perhaps more to
 *                         come...) depending on what character signifies
 *                         a path separator on a given platform.
 *
 *    CURRENTDIRSTR    ... equals "." (...and perhaps more to
 *                         come...) depending on what characters signify
 *                         the current directory on a given platform.
 *
 *    __boolean        ..  intrinsic type for BASIC's BOOLEAN. (8-bit bool)
 *    __byte           ... intrinsic type for BASIC's BYTE.    (8-bit uint)
 *    __integer        ... intrinsic type for BASIC's INTEGER. (16-bit int)
 *    __long           ... intrinsic type for BASIC's LONG.    (32-bit int)
 *    __single         ... intrinsic type for BASIC's SINGLE.  (16-bit float)
 *    __double         ... intrinsic type for BASIC's DOUBLE.  (32-bit float)
 *
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_PLATFORM_H_
#define _INCLUDE_PLATFORM_H_


/* !!! Check for more than one defined platform... */

/* !!! Move elsewhere? */
#define true  -1
#define false 0


/* Verify that a supported platform has been defined... */

    #if defined LINUXELF_I386
        #ifndef LINUX
            #define LINUX
        #endif
        #define I386
        #define ARCHITECTURE      "i386"
        #define FUNCNAME_PREPEND  ""
        #define STACK_DIRECTION   -1
        #define EOL_STRING        "\n"
        #define PATHCHAR          '/'
        #define CURRENTDIRSTR     "."
        typedef short __boolean;
        typedef unsigned char __byte;
        typedef short  __integer;
        typedef long   __long;
        typedef float  __single;
        typedef double __double;

    #elif defined LINUXAOUT_I386
        #ifndef LINUX
            #define LINUX
        #endif
        #define I386
        #define ARCHITECTURE      "i386"
        #define FUNCNAME_PREPEND  "_"
        #define STACK_DIRECTION   -1
        #define EOL_STRING        "\n"
        #define PATHCHAR          '/'
        #define CURRENTDIRSTR     "."
        typedef short __boolean;
        typedef unsigned char __byte;
        typedef short  __integer;
        typedef long   __long;
        typedef float  __single;
        typedef double __double;

    #elif defined WIN32_I386
        #ifndef WIN32
            #define WIN32
        #endif
        #define I386
        #define ARCHITECTURE      "i386"
        #define FUNCNAME_PREPEND  "_"
        #define STACK_DIRECTION   -1
        #define EOL_STRING        "\r\n"
        #define PATHCHAR          '\\'
        #define CURRENTDIRSTR     "."
        typedef short __boolean;
        typedef unsigned char __byte;
        typedef short  __integer;
        typedef long   __long;
        typedef float  __single;
        typedef double __double;

    #else
        #error No valid platform has been defined.
        #error Please use one of the following with the commandline/Makefile:
        #error  -DLINUXELF_I386
        #error  -DWIN32_I386

    #endif /* defined platform */

#endif /* _INCLUDE_PLATFORM_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of Platform.h ... */

