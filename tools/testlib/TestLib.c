/*
 * Mainline for BASIClib testing routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "TestLib.h"


void initTestLib(int argc, char **argv)
{
    void *base;
    __getBasePointer(base);

    setbuf(stdout, NULL);
    printf("\n[TESTLIB FOR VBSLACKER STARTING UP...]\n");
    printf("Making call to __initBasicLib()...\n"); 

    __initBasicLib(base, INITFLAG_DISABLE_CONSOLE, argc, argv);
} /* initTestLib */


void deinitTestLib(void)
{
    printf("\n[TESTLIB FOR VBSLACKER SHUTTING DOWN...]\n");
    printf("Making call to __deinitBasicLib()...\n"); 

        /* !!! This will be done automatically, eventually... */
    __deinitBasicLib();

    printf("\n");
} /* deinitTestLib */


void testEverything(void)
{
    testOnError();
    testErrorFunctions();
    testThreads();
    testStringFunctions();
    testConversionFunctions();
    testFileIOFunctions();
    testMathFunctions();
} /* testEverything */


int main(int argc, char **argv)
{
    initTestLib(argc, argv);
    testEverything();
    deinitTestLib();
    return(0);
} /* main */

/* end of TestLib.c ... */

