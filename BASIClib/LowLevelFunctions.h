/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_LOWLEVELFUNCTIONS_H_
#define _INCLUDE_LOWLEVELFUNCTIONS_H_

#include "StdBasic.h"

int vbil_peek(long addr);
void vbpli_poke(long addr, int newVal);
int vbil_inp(long ioport);
void vbpli_out(long ioport, int byte);
PBasicString vbSi_ioctl_DC_(int devFileNum);
void vbpiS_ioctl(int devFileNum, PBasicString ctlStr);
long vblA_varptr(void *myVar);
PBasicString vbSA_varptr_DC_(void *myVar);
long vblA_varseg(void *myVar);

#endif /* _INCLUDE_LOWLEVELFUNCTIONS_H_ */

/* end of LowLevelFunctions.h ... */

