/*
 * BASIC runtime error API.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "ErrorFunctions.h"

extern int basicErrno;

int vbi_err(void)
/*
 * Find out last thrown error number.
 *
 *    params : void.
 *   returns : last thrown BASIC error.
 */
{
    return(basicErrno);
} /* vbi_err */


void vbpi_error(int errorNum)
/*
 * Throw a runtime error. (errorNum) must be between 1 and 255, per
 *  Qbasic specifications, so the error thrown will change to
 *  ERR_ILLEGAL_FUNCTION_CALL if not in that range.
 *
 * !!! has this changed with later BASICs?
 *
 *     params : errorNum == int between 1 and 255 of error number to throw.
 *    returns : never returns; code jumps to error handler or something.
 */
{
    if ((errorNum < 1) || (errorNum > 255))
        errorNum = ERR_ILLEGAL_FUNCTION_CALL;
    __runtimeError(errorNum);
} /* vbpi_error */


int vbi_erl(void)
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


int vbi_erdev(void)
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


PBasicString vbS_erdev_DC_(void)
/*
 * Very DOS and hardware-specific remnant of old BASICs. Suppose to
 *  return the name of the last device to generate a critical error
 *  number reported from DOS. We don't do this.
 *
 *     params : void.
 *    returns : always a empty BASIC string ("").
 */
{
    return(__createString("", false));
} /* vbS_erdev_DC_ */

/* end of ErrorFunctions.c ... */

