/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_LOWLEVEL_H_
#define _INCLUDE_LOWLEVEL_H_

#include "RegState.h"

char peek(STATEPARAMS, long addr);
void poke(STATEPARAMS, long addr, char newVal);
char inp(STATEPARAMS, long ioport);
void out(STATEPARAMS, long ioport, char byte);

#endif

/* end of LowLevel.h ... */


