/*
 * BASIC runtime error subsystem.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include "ErrorFunctions.h"
#include "OnEvents.h"
#include "Threads.h"

/*
 * We keep an extra copy of the BASIC error number around. Both are set
 *  equally in case of an error, but otherwise, "__basicErrno" is
 *  ignored. This allows internal BASIClib routines to reset the errno to
 *  NO_ERROR, call something, and test for error, all without altering
 *  the errno that is accessable by outside programs.
 */
int basicErrno = (RuntimeErrEnum) NO_ERROR;
int __basicErrno = (RuntimeErrEnum) NO_ERROR;


void __defaultRuntimeErrorHandler(void)
{
    printf("\n\nUnhandled runtime error: %d\n\n", (int) basicErrno);
    exit(basicErrno);
} /* __defaultRuntimeErrorHandler */


void __runtimeError(RuntimeErrEnum errorNum)
{
    POnEventHandler pHandler;

    basicErrno = __basicErrno = errorNum;

    pHandler = __getOnEventHandler(ONERROR);
    if (pHandler == NULL)
        __defaultRuntimeErrorHandler();
    else
        __triggerOnEvent(ONERROR);
} /* __runtimeError */


double func_err(void)
{
    return((double) basicErrno);
} /* func_err */


void proc_err(double newErr)
{
    /* !!! check this later. Fuck. Double vs. int vs. enum. */

    basicErrno = (RuntimeErrEnum) newErr;
} /* proc_err */


/* end of ErrorFunctions.c ... */


