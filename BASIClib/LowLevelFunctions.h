/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_LOWLEVELFUNCTIONS_H_
#define _INCLUDE_LOWLEVELFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

int _vbil_peek(long addr);
void _vbpli_poke(long addr, int newVal);
int _vbil_inp(long ioport);
void _vbpli_out(long ioport, int byte);
PBasicString _vbSi_ioctl_DC_(int devFileNum);
void _vbpiS_ioctl(int devFileNum, PBasicString ctlStr);
long _vblA_varptr(void *myVar);
PBasicString _vbSA_varptr_DC_(void *myVar);
long _vblA_varseg(void *myVar);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_LOWLEVELFUNCTIONS_H_ */

/* end of LowLevelFunctions.h ... */

