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

__integer _vbil_peek(__long addr);
void _vbpli_poke(__long addr, __integer newVal);
__integer _vbil_inp(__long ioport);
void _vbpli_out(__long ioport, __integer byte);
PBasicString _vbSi_ioctl_DC_(__integer devFileNum);
void _vbpiS_ioctl(__integer devFileNum, PBasicString ctlStr);
__long _vblA_varptr(void *myVar);
PBasicString _vbSA_varptr_DC_(void *myVar);
__long _vblA_varseg(void *myVar);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_LOWLEVELFUNCTIONS_H_ */

/* end of LowLevelFunctions.h ... */

