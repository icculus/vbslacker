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


#if defined LINUX
/* linux stuff... */
#elif defined WIN32

#else
    #error No platform defined.
    #error  Please use one of the following on the commandline/Makefile:
    #error  -DLINUX
    #error  -DWIN32
#endif


#endif /* _INCLUDE_PLATFORM_H_ */

/* end of Platform.h ... */

