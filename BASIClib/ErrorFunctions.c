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

/* erdev */
/* erdev$ */


/* end of ErrorFunctions.c ... */

