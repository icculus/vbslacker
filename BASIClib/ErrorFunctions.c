/*
 * BASIC runtime error subsystem.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
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
int basicErrno = ERR_NO_ERROR;
int __basicErrno = ERR_NO_ERROR;

static char *errStrings[MAX_ERRS];

void __initErrorFunctions(void)
/*
 * Initialize the table of errorStrings.
 *
 *  !!! write this later. Internationalization should be considered, eh?
 */
{
    memset(errStrings, '\0', sizeof (errStrings));
} /* __initErrorFunctions */


void __defaultRuntimeErrorHandler(void)
{
    char *errStr;

    errStr = ((basicErrno > MAX_ERRS) ? UNKNOWN_ERR : errStrings[basicErrno]);
    if (errStr == NULL)
        errStr = UNKNOWN_ERR;

    printf("\n\n***Unhandled runtime error***\n");
    printf("  \"%s\" (#%d)\n\n", errStr, basicErrno);
    exit(basicErrno);
} /* __defaultRuntimeErrorHandler */


void __runtimeError(int errorNum)
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

    basicErrno = (int) newErr;
} /* proc_err */


/* end of ErrorFunctions.c ... */


