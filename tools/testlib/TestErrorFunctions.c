/*
 * Test error functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"


void test___runtimeError(STATEPARAMS)
/*
 * Test __runtimeError() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("Testing __runtimeError()...\n");

    __setResumeStack;
    __registerOnEventHandler(STATEARGS, &&errError, ONERROR);

    __basicErrno = ERR_NO_ERROR;
    __runtimeError(STATEARGS, ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");
    __deregisterOnEventHandlers(STATEARGS);
    return;

errError:              /* error handler... */ 
    if (__basicErrno != ERR_TOO_MANY_FILES)
    {
        printf("  - Threw error (%d), should have thrown (%d)!\n",
                             __basicErrno, ERR_TOO_MANY_FILES);

    } /* if */

    __deregisterOnEventHandlers(STATEARGS);
} /* test___runtimeError */


void test_err(STATEPARAMS)
/*
 * Test vbi_err() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int rc;

    printf("Testing ERR()...\n");

    __setResumeStack;
    __registerOnEventHandler(STATEARGS, &&errError, ONERROR);

    __basicErrno = ERR_NO_ERROR;
    __runtimeError(STATEARGS, ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");
    __deregisterOnEventHandlers(STATEARGS);
    return;

errError:              /* error handler... */ 
    rc = vbi_err(STATEARGS);

    if (rc != ERR_TOO_MANY_FILES)
    {
        printf("  - ERR() returned (%d), should have returned (%d)!\n",
                        rc, ERR_TOO_MANY_FILES);
    } /* if */

    __deregisterOnEventHandlers(STATEARGS);
} /* test_func_err */



void test_error(STATEPARAMS)
/*
 * Test ERROR statement functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    int rc;

    printf("Testing ERROR()...\n");

    __setResumeStack;
    __registerOnEventHandler(STATEARGS, &&errErrorHandler, ONERROR);

    vbpi_error(STATEARGS, ERR_LABEL_NOT_DEFINED);
    printf("  - Didn't call error handler.\n");
    return;

errErrorHandler:

    if (__basicErrno != ERR_LABEL_NOT_DEFINED)
    {
        printf("  - returned (%d), should have returned (%d)!\n",
                        rc, ERR_LABEL_NOT_DEFINED);
    } /* if */

    __deregisterOnEventHandlers(STATEARGS);
} /* test_proc_err */



void testErrorFunctions(STATEPARAMS)
/*
 * This code tests all the error functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING ERROR FUNCTIONS...]\n");

    test___runtimeError(STATEARGS);
    test_err(STATEARGS);
    test_error(STATEARGS);
} /* testErrorFunctions */


#ifdef STANDALONE

int main(void)
{
    __initBasicLib(NULLSTATEARGS, INITFLAG_NO_FLAG);
    testErrorFunctions(NULLSTATEARGS);
    __deinitBasicLib(NULLSTATEARGS);
    return(0);
} /* main */

#endif

/* end of TestErrorFunctions.c ... */
