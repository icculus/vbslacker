/*
 * Test filesystem functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include "BasicLib.h"


void test_filecopy(void)
/*
 * Test FileCopy() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    PBasicString copyHere = __createString("testFileCopy", false);

#ifdef LINUX
    PBasicString fileName = __createString("/etc/inittab", false);
#elif (defined WIN32)
    PBasicString fileName = __createString("C:\\msdos.sys", false);
#else
    #error Please add a definitely-existing file for your system here!
#endif


    __ONERRORINIT;
    __setOnErrorHandler(fileCopyChoked);

    printf("Testing FileCopy...");

    _vbpSS_filecopy(fileName, copyHere);


    printf("done.\n");
    __exitCleanupOnError;
    return;

__insertLineLabel(fileCopyChoked);

    printf(" - Threw error. (#%ld)\n", _vbl_err());
    __exitCleanupOnError;
} /* test_filecopy */


void testFileSystemFunctions(void)
/*
 * This code tests all the filesystem functions in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("\n[TESTING FILESYSTEM FUNCTIONS...]\n");

    test_filecopy();
} /* testFileSystemFunctions */


#ifdef STANDALONE

int main(int argc, char **argv, char **envp)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv, envp);
    testFileSystemFunctions();
    return(0);
} /* main */

#endif

/* end of TestFileSystemFunctions.c ... */

