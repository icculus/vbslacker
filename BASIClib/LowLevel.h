/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_LOWLEVEL_H_
#define _INCLUDE_LOWLEVEL_H_

char peek(long addr);
void poke(long addr, char newVal);

#endif

/* end of LowLevel.h ... */


