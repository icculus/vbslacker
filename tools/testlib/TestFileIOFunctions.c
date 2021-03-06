/*
 * Test file i/o functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 *    Written by Gregory S. Read.
 */

#include <stdio.h>
#include "BasicLib.h"


__boolean test___FileOpen(void)
{
    PBasicString pFile;

    pFile = __createString("./test.txt", false); 

    printf("Testing VBOpen functions...\n");

        /* Opening 'test.txt' for writing as binary (overwrite if exist) */
    VBopen_NoAccess_NoLock_NoRecLen(pFile, Output, 1);

/*    __freeString(pFile); */
    return true;
}

__boolean test___FileClose(void)
{
    printf("Testing VBClose functions...\n");

    VBclose_Params(1, 1);

    return true;
}

void testFileIOFunctions(void)
/*
 * Tests all file i/o routines
 */
{
    printf("\n[TESTING FILE I/O FUNCTIONS...]\n");

    if(!test___FileOpen())     /* Nothing else will work if it fails */
        return;

    test___FileClose();
}


#ifdef STANDALONE

long errors = 0;
long warnings = 0;

int main(int argc, char **argv)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv);
    testFileIOFunctions();
    return(0);
} /* main */

#endif

/* end of TestFileIOFunctions.c ... */

