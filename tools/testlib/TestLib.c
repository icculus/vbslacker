/*
 * Mainline for BASIClib testing routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TestLib.h"

#define stricmp strcasecmp


long warnings = 0;
long errors = 0;


static int skipCollector = 0;


void initTestLib(int argc, char **argv)
{
    int i;

    for (i = 0; i < argc; i++)
    {
        if (stricmp(argv[i], "--skipgc") == 0)
            skipCollector = 1;
    } /* for */

    setbuf(stdout, NULL);
    printf("\n[TESTLIB FOR VBSLACKER STARTING UP...]\n");
    printf("Making call to __initBasicLib()...");

    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv);
    printf("done.\n");
} /* initTestLib */


void deinitTestLib(void)
{
    printf("\n[TESTLIB FOR VBSLACKER SHUTTING DOWN...]\n");
    printf("Making call to __deinitBasicLib()...\n");

    /*
     * This is normally done automatically, but is safe to do
     *  explicitly...so long as you don't call any BASIClib
     *  functions after doing so...
     */

    __deinitBasicLib();

    printf("\n"
           "TestLib execution has completed.\n"
           "\n"
           "    warnings : %ld\n"
           "    errors   : %ld\n"
           "\n",
           warnings, errors);
} /* deinitTestLib */


void testEverything(void)
{
    testOnError();
    testErrorFunctions();
    testGarbageCollector(skipCollector);
    testThreads();
    testStringFunctions();
    testConversionFunctions();
    testMathFunctions();
    testFileSystemFunctions();
    testRandomFunctions();
} /* testEverything */


int main(int argc, char **argv)
{
    initTestLib(argc, argv);
    testEverything();
    deinitTestLib();
    exit(0);
    return(0);
} /* main */

/* end of TestLib.c ... */

