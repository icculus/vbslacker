/*
 * BASIC support library: math functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicError.h"


double abs(double x)
/*
 * Returns the absolute value of a numeric.
 *
 *   params : x == number we want the absolute value of.
 *  returns : absolute value.
 */
{
    return((x < 0) ? -x : x);
} /* abs */


double sqr(double x)
/*
 * Returns the square root of a numeric.
 *
 *   params : x == number we want the square root of.
 *  returns : absolute value.
 */
{
    return(sqrt(x));
} /* sqrt */




/* end of MathFunctions.c ... */


