/*
 * Some lowlevel stuff that all vbSlacker code should include.
 *
 *  This file is automagically included by StdBasic.h and BasicLib.h ...
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

#if defined LINUX_I386
#    define ARCHITECTURE_I386
#elif defined WIN32_I386
#    define ARCHITECTURE_I386
#else
#    error No valid platform has been defined.
#    error Please use one of the following with the commandline/Makefile:
#    error  -DLINUX_I386
#    error  -DWIN32_I386
#endif /* defined platform */

#ifdef ARCHITECTURE_I386
#    define STACK_DIRECTION -1
#endif


#endif /* _INCLUDE_PLATFORM_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of Platform.h ... */

