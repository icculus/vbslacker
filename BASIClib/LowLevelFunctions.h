/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_LOWLEVELFUNCTIONS_H_
#define _INCLUDE_LOWLEVELFUNCTIONS_H_

#include "StdBasic.h"

int vbil_peek(STATEPARAMS, long addr);
void vbpli_poke(STATEPARAMS, long addr, int newVal);
int vbil_inp(STATEPARAMS, long ioport);
void vbpli_out(STATEPARAMS, long ioport, int byte);
PBasicString vbSi_ioctl_DC_(STATEPARAMS, int devFileNum);
void vbpiS_ioctl(STATEPARAMS, int devFileNum, PBasicString ctlStr);
long vblA_varptr(STATEPARAMS, void *myVar);
PBasicString vbSA_varptr_DC_(STATEPARAMS, void *myVar);
long vblA_varseg(STATEPARAMS, void *myVar);

#endif

/* end of LowLevelFunctions.h ... */


