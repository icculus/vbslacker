/*
 * Array manipulating API header file.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ARRAYFUNCTIONS_H_
#define _INCLUDE_ARRAYFUNCTIONS_H_

#include "StdBasic.h"

void vbpAn_erase(STATEPARAMS, PBasicArray array);
int vbiAi_lbound(STATEPARAMS, PBasicArray array, int dimension);
int vbiAi_ubound(STATEPARAMS, PBasicArray array, int dimension);

#endif /* _INCLUDE_ARRAYFUNCTIONS_H_ */

/* end of ArrayFunctions.h ... */