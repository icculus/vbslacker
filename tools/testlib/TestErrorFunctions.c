/*
 * Test error functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"


void test___runtimeError(void)
/*
 * Test __runtimeError() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    printf("Testing __runtimeError()...\n");

    __ONERRORINIT;
    __setOnErrorHandler(rtErrError);

    __basicErrno = ERR_NO_ERROR;
    __runtimeError(ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");

    __exitCleanupOnError;
    return;

__insertLineLabel(rtErrError);              /* error handler... */
    if (__basicErrno != ERR_TOO_MANY_FILES)
    {
        printf("  - Threw error (%d), should have thrown (%d)!\n",
                             __basicErrno, ERR_TOO_MANY_FILES);

    } /* if */

    __exitCleanupOnError;
    return;
} /* test___runtimeError */


void test_err(void)
/*
 * Test vbi_err() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    int rc;

    printf("Testing ERR()...\n");

    __ONERRORINIT;
    __setOnErrorHandler(errError);

    __basicErrno = ERR_NO_ERROR;
    __runtimeError(ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");

    __exitCleanupOnError;
    return;

__insertLineLabel(errError);              /* error handler... */
    rc = vbi_err();

    if (rc != ERR_TOO_MANY_FILES)
    {
        printf("  - ERR() returned (%d), should have returned (%d)!\n",
                        rc, ERR_TOO_MANY_FILES);
    } /* if */

    __exitCleanupOnError;
    return;
} /* test_func_err */



void test_error(void)
/*
 * Test ERROR statement functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    int rc;

    printf("Testing ERROR()...\n");

    __ONERRORINIT;

    __setOnErrorHandler(errorErrorHandler);

    vbpi_error(ERR_LABEL_NOT_DEFINED);
    printf("  - Didn't call error handler.\n");
    __exitCleanupOnError;
    return;

__insertLineLabel(errorErrorHandler);

    if (__basicErrno != ERR_LABEL_NOT_DEFINED)
    {
        printf("  - returned (%d), should have returned (%d)!\n",
                        rc, ERR_LABEL_NOT_DEFINED);
    } /* if */

    __exitCleanupOnError;
    return;
} /* test_proc_err */



void testErrorFunctions(void)
/*
 * This code tests all the error functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING ERROR FUNCTIONS...]\n");

    test___runtimeError();
    test_err();
    test_error();
} /* testErrorFunctions */


#ifdef STANDALONE

int main(int argc, char **argv)
{
    void *base;

    __getBasePointer(base);
    __initBasicLib(base, INITFLAG_DISABLE_CONSOLE, argc, argv);
    testErrorFunctions();
    __deinitBasicLib();
    return(0);
} /* main */

#endif

/* end of TestErrorFunctions.c ... */
