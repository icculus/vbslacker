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

double vbdd_abs(double x);
double vbdd_sqr(double x);
double vbdd_atn(double x);
double vbdd_sin(double x);
double vbdd_cos(double x);
double vbdd_tan(double x);
double vbdd_exp(double x);
double vbdd_log(double x);
long vbld_fix(double x);
long vbld_int(double x);
int vbid_sgn(double x);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_MATHFUNCTIONS_H_ */

/* end of MathFunctions.h ... */

