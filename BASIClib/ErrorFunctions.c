/*
 * BASIC runtime error API.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "ErrorFunctions.h"

extern int basicErrno;

double vbd_err(STATEPARAMS)
{
    return((double) basicErrno);
} /* func_err */


void vbpi_error(STATEPARAMS, int errorNum)
{
    __runtimeError(STATEARGS, errorNum);
} /* vbpd_error */


int vbi_erl(STATEPARAMS)
/*
 * In QBasic, returns line number of latest error. According to
 *  Microsoft's documentation, line labels are not returned, and
 *  if no line number is available, (0) is returned. This function
 *  was basically retarded to begin with, but for the sake of
 *  having it, we provide a stub that always returns zero.
 */
{
    return(0);
} /* vbi_erl */


int vbi_erdev(STATEPARAMS)
/*
 * Very DOS and hardware-specific remnant of old BASICs. Suppose to
 *  return the last device error number reported from DOS. We don't do this.
 *
 *     params : void.
 *    returns : always (0).
 */
{
    return(0);
} /* vbi_erdev */


PBasicString vbS_erdev_DC_(STATEPARAMS)
/*
 * Very DOS and hardware-specific remnant of old BASICs. Suppose to
 *  return the name of the last device to generate a critical error
 *  number reported from DOS. We don't do this.
 *
 *     params : void.
 *    returns : always a empty BASIC string ("").
 */
{
    return(__createString(STATEARGS, "", false));
} /* vbS_erdev_DC_ */

/* end of ErrorFunctions.c ... */

