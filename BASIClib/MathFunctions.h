/*
 * BASIC support library: math functions header.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_MATHFUNCTIONS_H_
#define _INCLUDE_MATHFUNCTIONS_H_

#include "RegState.h"

/* !!! nowhere better for these, currently... */
#define __min(x, y) ((x) < (y) ? (x) : (y))
#define __max(x, y) ((x) > (y) ? (x) : (y))
#define __round(w, x, y, z, num) vbld_int(w, x, y, z, num)


double vbdd_abs(STATEPARAMS, double x);
double vbdd_sqr(STATEPARAMS, double x);
double vbdd_atn(STATEPARAMS, double x);
double vbdd_sin(STATEPARAMS, double x);
double vbdd_cos(STATEPARAMS, double x);
double vbdd_tan(STATEPARAMS, double x);
double vbddd_exp(STATEPARAMS, double x, double y);
double vbdd_log(STATEPARAMS, double x);
long vbld_fix(STATEPARAMS, double x);
long vbld_int(STATEPARAMS, double x);
int vbid_sgn(STATEPARAMS, double x);

#endif

/* end of MathFunctions.h ... */


