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

boolean test___FileOpen(void)
{
    PBasicString pFile;

    pFile = __createString("./test.txt", false); 

    printf("Testing VBOpen functions...\n");

    printf("   Opening 'test.txt' for writing as binary (overwrite if exist)\n");
    VBopen_NoAccess_NoLock_NoRecLen(pFile, Output, 1);

    return true;
}

boolean test___FileClose(void)
{
    printf("Testing VBClose functions...\n");

    printf("   Closing file handle 1...\n");
    VBclose_Params(1, 1);

    return true;
}

void testFileIOFunctions(void)
/*
 * Tests all file i/o routines
 */
{
    if(!test___FileOpen())             /* Nothing else will work if it fails */
        return;

    test___FileClose();
}


#ifdef STANDALONE

#include "Initialize.h"

int main(void)
{
    __initBasicLib();
    testFileIOFunctions();
    __deinitBasicLib();
    return(0);
} /* main */

#endif

/*** End TestFileIOFunctions.c ***/