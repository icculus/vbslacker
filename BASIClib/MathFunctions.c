/*
 * BASIC support library: math functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <math.h>
#include "MathFunctions.h"


int round(double x)
/*
 * Round (x) to the nearest whole number.
 *
 *    params : x == floating point number to round.
 *   returns : x rounded to nearest int.
 */
{
    return((int) (x + 0.5));
} /* __round */


double func_abs(double x)
/*
 * Returns the absolute value of a numeric.
 *
 *   params : x == number we want the absolute value of.
 *  returns : absolute value.
 */
{
    return((x < 0) ? -x : x);
} /* func_abs */


double func_sqr(double x)
/*
 * Returns the square root of a numeric.
 *
 *   params : x == number we want the square root of.
 *  returns : absolute value.
 */
{
    return(sqrt(x));
} /* func_sqr */


/* end of MathFunctions.c ... */


