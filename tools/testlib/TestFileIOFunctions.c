/*
 * Test file i/o functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include "Initialize.h"
#include "Boolean.h"
#include "BasicString.h"
#include "FileIOFunctions.h"

boolean test___FileOpen(STATEPARAMS)
{
    PBasicString pFile;

    pFile = __createString(STATEARGS, "./test.txt", false); 

    printf("Testing VBOpen functions...\n");

        /* Opening 'test.txt' for writing as binary (overwrite if exist) */
    VBopen_NoAccess_NoLock_NoRecLen(STATEARGS, pFile, Output, 1);

/*    __freeString(STATEARGS, pFile); */
    return true;
}

boolean test___FileClose(STATEPARAMS)
{
    printf("Testing VBClose functions...\n");

    VBclose_Params(STATEARGS, 1, 1);

    return true;
}

void testFileIOFunctions(STATEPARAMS)
/*
 * Tests all file i/o routines
 */
{
    printf("\n[TESTING FILE I/O FUNCTIONS...]\n");

    if(!test___FileOpen(STATEARGS))     /* Nothing else will work if it fails */
        return;

    test___FileClose(STATEARGS);
}


#ifdef STANDALONE

#include "Initialize.h"

int main(void)
{
    __initBasicLib(NULLSTATEARGS, INITFLAG_NO_FLAGS);
    testFileIOFunctions(NULLSTATEARGS);
    __deinitBasicLib(NULLSTATEARGS);
    return(0);
} /* main */

#endif

/*** End TestFileIOFunctions.c ***/
