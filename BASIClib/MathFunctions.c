/*
 * BASIC support library: math functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <math.h>
#include "MathFunctions.h"

double vbdd_abs(STATEPARAMS, double x)
/*
 * Returns the absolute value of a numeric.
 *
 *   params : x == number we want the absolute value of.
 *  returns : absolute value.
 */
{
    return((x < 0) ? -x : x);
} /* vbdd_abs */


double vbdd_sqr(STATEPARAMS, double x)
/*
 * Returns the square root of a numeric.
 *
 *   params : x == number we want the square root of.
 *  returns : absolute value.
 */
{
    return(sqrt(x));
} /* vbdd_sqr */


double vbdd_atn(STATEPARAMS, double x)
{
    return(atan(x));
} /* vbdd_atn */


double vbdd_sin(STATEPARAMS, double x)
{
    return(sin(x));
} /* vbdd_sin */


double vbdd_cos(STATEPARAMS, double x)
{
    return(cos(x));
} /* vbdd_cos */


double vbdd_tan(STATEPARAMS, double x)
{
    return(tan(x));
} /* vbdd_tan */


double vbddd_exp(STATEPARAMS, double x, double y)
{
    return(pow(x, y));
} /* vbddd_exp */


double vbdd_log(STATEPARAMS, double x)
{
    return(log(x));
} /* vbdd_log */


long vbld_fix(STATEPARAMS, double x)
{
    return((long) x);
} /* vbld_fix */


long vbld_int(STATEPARAMS, double x)
/*
 * Return (next largest integer <= (x)).
 *
 *    params : x == floating point number to round.
 *   returns : x rounded to the next lowest long.
 */
{
    long retVal;

    if (x < 0.0)
    {
        if ( (x - ((long) x)) == 0.0 )   /* already whole number? */
            retVal = (long) x;
        else
            retVal = (long) (x - 1.0);
    } /* if */
    else
        retVal = (long) x;

    return(retVal);
} /* vbld_int */


int vbid_sgn(STATEPARAMS, double x)
{
    int retVal;

    if (x == 0.0)
        retVal = 0;
    else if (x < 0.0)
        retVal = -1;
    else
        retVal = 1;

    return(retVal);
} /* vbid_sgn */

/* !!! still need: */
/* rnd */
/* randomize */

/* end of MathFunctions.c ... */


