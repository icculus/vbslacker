/*
 * BASIC support library: math functions header.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_MATHFUNCTIONS_H_
#define _INCLUDE_MATHFUNCTIONS_H_

#include "StdBasic.h"

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

#endif /* _INCLUDE_MATHFUNCTIONS_H_ */

/* end of MathFunctions.h ... */

