/*
 * BASIC support library: math functions.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <math.h>
#include "MathFunctions.h"


__double _vbdd_abs(__double x)
/*
 * Returns the absolute value of a numeric.
 *
 *   params : x == number we want the absolute value of.
 *  returns : absolute value.
 */
{
    return((x < 0) ? -x : x);
} /* _vbdd_abs */


__double _vbdd_sqr(__double x)
/*
 * Returns the square root of a numeric.
 *
 *   params : x == number we want the square root of.
 *  returns : absolute value.
 */
{
    if (x < 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);

    return(sqrt(x));
} /* _vbdd_sqr */


__double _vbdd_atn(__double x)
{
    return(atan(x));
} /* _vbdd_atn */


__double _vbdd_sin(__double x)
{
    return(sin(x));
} /* _vbdd_sin */


__double _vbdd_cos(__double x)
{
    return(cos(x));
} /* _vbdd_cos */


__double _vbdd_tan(__double x)
{
    return(tan(x));
} /* _vbdd_tan */


__double _vbdd_exp(__double x)
#warning uh?
{
    return(pow(2.718282, 2.0));
} /* _vbddd_exp */


__double _vbdd_log(__double x)
{
    if (x <= 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    return(log(x));
} /* _vbdd_log */


__long _vbld_fix(__double x)
{
    return((__long) x);
} /* _vbld_fix */


__long _vbld_int(__double x)
/*
 * Return (next largest integer <= (x)).
 *
 *    params : x == floating point number to round.
 *   returns : x rounded to the next lowest long.
 */
{
    __long retVal;

    if (x < 0.0)
    {
        if ( (x - ((__long) x)) == 0.0 )   /* already whole number? */
            retVal = (__long) x;
        else
            retVal = (__long) (x - 1.0);
    } /* if */
    else
        retVal = (__long) x;

    return(retVal);
} /* _vbld_int */


__integer _vbid_sgn(__double x)
{
    __integer retVal;

    if (x == 0.0)
        retVal = 0;
    else if (x < 0.0)
        retVal = -1;
    else
        retVal = 1;

    return(retVal);
} /* _vbid_sgn */

/* end of MathFunctions.c ... */


