/*
 * BASIC support library: math functions header.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_MATHFUNCTIONS_H_
#define _INCLUDE_MATHFUNCTIONS_H_

/* !!! nowhere better for these, currently... */
#define __min(x, y) ((x) < (y) ? (x) : (y))
#define __max(x, y) ((x) > (y) ? (x) : (y))

int __round(STATEPARAMS, double x);
double func_abs(STATEPARAMS, double x);
double func_sqr(STATEPARAMS, double x);

#endif

/* end of MathFunctions.h ... */


