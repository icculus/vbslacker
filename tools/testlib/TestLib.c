/*
 * Mainline for BASIClib testing routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "TestLib.h"


void initTestLib(int argc, char **argv, char **envp)
{
    setbuf(stdout, NULL);
    printf("\n[TESTLIB FOR VBSLACKER STARTING UP...]\n");
    printf("Making call to __initBasicLib()...");

    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv, envp);
    printf("done.\n");
} /* initTestLib */


void deinitTestLib(void)
{
    printf("\n[TESTLIB FOR VBSLACKER SHUTTING DOWN...]\n");
    printf("Making call to __deinitBasicLib()...");

    /*
     * This is normally done automatically, but is safe to do
     *  explicitly...so long as you don't call any BASIClib
     *  functions after doing so...
     */

    __deinitBasicLib();

    printf("done.\n\n");
} /* deinitTestLib */


void testEverything(void)
{
    testOnError();
    testErrorFunctions();
    testGarbageCollector();
    testThreads();
    testStringFunctions();
    testConversionFunctions();
/*    testFileIOFunctions();*/
    testMathFunctions();
} /* testEverything */


int main(int argc, char **argv, char **envp)
{
    initTestLib(argc, argv, envp);
    testEverything();
    deinitTestLib();
    return(0);
} /* main */

/* end of TestLib.c ... */

