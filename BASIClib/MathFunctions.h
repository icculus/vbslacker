/*
 * BASIC support library: math functions header.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_MATHFUNCTIONS_H_
#define _INCLUDE_MATHFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

__double _vbdd_abs(__double x);
__double _vbdd_sqr(__double x);
__double _vbdd_atn(__double x);
__double _vbdd_sin(__double x);
__double _vbdd_cos(__double x);
__double _vbdd_tan(__double x);
__double _vbdd_exp(__double x);
__double _vbdd_log(__double x);
__long _vbld_fix(__double x);
__long _vbld_int(__double x);
__integer _vbid_sgn(__double x);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_MATHFUNCTIONS_H_ */

/* end of MathFunctions.h ... */

