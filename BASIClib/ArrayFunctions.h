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

void _vbpAn_erase(__PBasicArray array);
__long _vblAl_lbound(__PBasicArray array, __long dimension);
__long _vblAl_ubound(__PBasicArray array, __long dimension);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_ARRAYFUNCTIONS_H_ */

/* end of ArrayFunctions.h ... */
