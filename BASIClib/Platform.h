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
 *                         depending on what the platform's processor type
 *                         is.
 *
 *    STACK_DIRECTION  ... equals (-1) if the stack on a given platform grows
 *                         downward, or (1) if stack grows upward.
 *
 *    EOL_STRING       ... equals "\n" or "\r\n" (...and perhaps more to
 *                         come...) depending on what characters signify
 *                         an end-of-line on a given platform.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_PLATFORM_H_
#define _INCLUDE_PLATFORM_H_


/* !!! Check for more than one defined platform... */


/* Verify that a supported platform has been defined... */

#if defined LINUXELF_I386
#    define ARCHITECTURE      "i386"
#    define FUNCNAME_PREPEND  ""
#    define STACK_DIRECTION   -1
#    define EOL_STRING        "\n"
#elif defined WIN32_I386
#    define ARCHITECTURE      "i386"
#    define FUNCNAME_PREPEND  "_"
#    define STACK_DIRECTION   -1
#    define EOL_STRING        "\r\n"
#else
#    error No valid platform has been defined.
#    error Please use one of the following with the commandline/Makefile:
#    error  -DLINUXELF_I386
#    error  -DWIN32_I386
#endif /* defined platform */

#endif /* _INCLUDE_PLATFORM_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of Platform.h ... */

