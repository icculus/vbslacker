/*
 * BASIClib initialization declarations.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_INITIALIZE_H_
#define _INCLUDE_INITIALIZE_H_

#include "RegState.h"

#define INITFLAG_NO_FLAGS              0
#define INITFLAG_NOT_INITIALIZED       0
#define INITFLAG_DISABLE_RESUME        1
#define INITFLAG_DISABLE_ONEVENTS      2
#define INITFLAG_DISABLE_RANGE_CHECKS  4
#define INITFLAG_DISABLE_NUM_OVERFLOW  8
#define INITFLAG_DISABLE_VARIANTS      16
#define INITFLAG_DISABLE_CONSOLE       32


    /* function prototypes ... */
void __initBasicLib(STATEPARAMS, long flags);
void __deinitBasicLib(void);
long __getInitFlags(STATEPARAMS);

#endif

/* end of Initialize.h ... */


