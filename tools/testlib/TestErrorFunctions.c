/*
 * Test error functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"

extern long errors;
extern long warning;


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

    __runtimeError(ERR_TOO_MANY_FILES);
    printf("  - Didn't call error handler.\n");
    errors++;

    __exitCleanupOnError;
    return;

__insertLineLabel(errError);              /* error handler... */
    rc = _vbl_err();

    if (rc != ERR_TOO_MANY_FILES)
    {
        printf("  - ERR() returned (%d), should have returned (%d)!\n",
                        rc, ERR_TOO_MANY_FILES);
        errors++;
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

    _vbpl_error(ERR_DISK_FULL);
    printf("  - Didn't call error handler.\n");
    errors++;
    __exitCleanupOnError;
    return;

__insertLineLabel(errorErrorHandler);

    if (_vbl_err() != ERR_DISK_FULL)
    {
        printf("  - returned (%d), should have returned (%d)!\n",
                        rc, ERR_DISK_FULL);
        errors++;
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

    test_err();
    test_error();
} /* testErrorFunctions */


#ifdef STANDALONE

long errors = 0;
long warnings = 0;

int main(int argc, char **argv, char **envp)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv, envp);
    testErrorFunctions();
    return(0);
} /* main */

#endif

/* end of TestErrorFunctions.c ... */
