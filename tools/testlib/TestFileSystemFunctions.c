/*
 * Test filesystem functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include "BasicLib.h"


void test_kill(void)
/*
 * Test KILL() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    PBasicString fileName = __createString("testName", false);
    struct stat statBuf;


    __ONERRORINIT;
    __setOnErrorHandler(nameChoked);

    printf("Testing Kill...");

    _vbpS_kill(fileName);

    if (stat(__basicStringToAsciz(fileName), &statBuf) == 0)
        printf("\n - File was not killed.\n");
    else
    {
        if (errno == ENOENT)
            printf("done.\n");
        else
            printf("\n - File is now unstat()able, but for wrong reason.\n");
    } /* else */

    __exitCleanupOnError;
    return;

__insertLineLabel(nameChoked);

    printf("\n - Threw error. (#%ld)\n", _vbl_err());
    __exitCleanupOnError;
} /* test_kill */


void test_name(void)
/*
 * Test NAME() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    PBasicString copyThis = __createString("testFileCopy", false);
    PBasicString copyHere = __createString("testName", false);
    struct stat statBuf;


    __ONERRORINIT;
    __setOnErrorHandler(nameChoked);

    printf("Testing Name...");

    _vbpSS_name(copyThis, copyHere);

    if (stat(__basicStringToAsciz(copyHere), &statBuf) != 0)
        printf("\n - renaming is unstat()able.\n");
    else
        printf("done.\n");

    __exitCleanupOnError;
    return;

__insertLineLabel(nameChoked);

    printf("\n - Threw error. (#%ld)\n", _vbl_err());
    __exitCleanupOnError;
} /* test_name */


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
    struct stat statBuf;

#if (defined LINUX)
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

    if (stat(__basicStringToAsciz(copyHere), &statBuf) != 0)
        printf("\n - new copy is unstat()able.\n");
    else
        printf("done.\n");

    __exitCleanupOnError;
    return;

__insertLineLabel(fileCopyChoked);

    printf("\n - Threw error. (#%ld)\n", _vbl_err());
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
    test_name();
    test_kill();
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

