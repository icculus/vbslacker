/*
 * BASIC support library: math functions header.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_MATHFUNCTIONS_H_
#define _INCLUDE_MATHFUNCTIONS_H_

#include "StdBasic.h"

double vbdd_abs(STATEPARAMS, double x);
double vbdd_sqr(STATEPARAMS, double x);
double vbdd_atn(STATEPARAMS, double x);
double vbdd_sin(STATEPARAMS, double x);
double vbdd_cos(STATEPARAMS, double x);
double vbdd_tan(STATEPARAMS, double x);
double vbdd_exp(STATEPARAMS, double x);
double vbdd_log(STATEPARAMS, double x);
long vbld_fix(STATEPARAMS, double x);
long vbld_int(STATEPARAMS, double x);
int vbid_sgn(STATEPARAMS, double x);

#endif /* _INCLUDE_MATHFUNCTIONS_H_ */

/* end of MathFunctions.h ... */

