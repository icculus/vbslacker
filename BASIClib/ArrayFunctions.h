/*
 * Array manipulating API header file.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ARRAYFUNCTIONS_H_
#define _INCLUDE_ARRAYFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

void vbpAn_erase(__PBasicArray array);
int vbiAi_lbound(__PBasicArray array, int dimension);
int vbiAi_ubound(__PBasicArray array, int dimension);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_ARRAYFUNCTIONS_H_ */

/* end of ArrayFunctions.h ... */
