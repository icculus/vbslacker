/*
 * Test filesystem functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include "BasicLib.h"

extern long errors;
extern long warnings;


static PBasicString dirConst = NULL;


void test_rmdir(void)
/*
 * Test RMDIR() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    struct stat statBuf;

    __ONERRORINIT;
    __setOnErrorHandler(rmdirChoked);

    printf("Testing RmDir...\n");

    _vbpS_rmdir(dirConst);

    if (stat(__basicStringToAsciz(dirConst), &statBuf) == 0)
    {
        printf(" - Directory was not removed.\n");
        errors++;
    } /* if */
    else
    {
        if (errno != ENOENT)
        {
            printf(" - Dir is now unstat()able, but for wrong reason.\n");
            errors++;
        } /* if */
    } /* else */

    __exitCleanupOnError;
    return;

__insertLineLabel(rmdirChoked);

    printf(" - Threw error. (#%ld)\n", _vbl_err());
    warnings++;
    __exitCleanupOnError;
} /* test_rmdir */


void test_mkdir(void)
/*
 * Test MKDIR() functionality.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    struct stat statBuf;

    __ONERRORINIT;
    __setOnErrorHandler(mkdirChoked);

    printf("Testing MkDir...\n");

    _vbpS_mkdir(dirConst);

    if (stat(__basicStringToAsciz(dirConst), &statBuf) != 0)
    {
        printf(" - New directory is unstat()able.\n");
        errors++;
    } /* if */
    else
    {
        if (!S_ISDIR(statBuf.st_mode))
        {
            printf(" - Created file is NOT a directory!\n");
            errors++;
        } /* if */
    } /* else */

    __exitCleanupOnError;
    return;

__insertLineLabel(mkdirChoked);

    printf(" - Threw error. (#%ld)\n", _vbl_err());
    warnings++;
    __exitCleanupOnError;
} /* test_mkdir */


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
    __setOnErrorHandler(killChoked);

    printf("Testing Kill...\n");

    _vbpS_kill(fileName);

    if (stat(__basicStringToAsciz(fileName), &statBuf) == 0)
    {
        printf(" - File was not killed.\n");
        errors++;
    } /* if */
    else
    {
        if (errno != ENOENT)
        {
            printf(" - File is now unstat()able, but for wrong reason.\n");
            errors++;
        } /* if */
    } /* else */

    __exitCleanupOnError;
    return;

__insertLineLabel(killChoked);

    printf("\n - Threw error. (#%ld)\n", _vbl_err());
    warnings++;
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

    printf("Testing Name...\n");

    _vbpSS_name(copyThis, copyHere);

    if (stat(__basicStringToAsciz(copyHere), &statBuf) != 0)
    {
        printf(" - renaming is unstat()able.\n");
        errors++;
    } /* if */

    __exitCleanupOnError;
    return;

__insertLineLabel(nameChoked);

    printf(" - Threw error. (#%ld)\n", _vbl_err());
    warnings++;
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

    printf("Testing FileCopy...\n");

    _vbpSS_filecopy(fileName, copyHere);

    if (stat(__basicStringToAsciz(copyHere), &statBuf) != 0)
    {
        printf(" - new copy is unstat()able.\n");
        errors++;
    } /* if */

    __exitCleanupOnError;
    return;

__insertLineLabel(fileCopyChoked);

    printf(" - Threw error. (#%ld)\n", _vbl_err());
    warnings++;
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

    dirConst = __constString("newdir");

    test_filecopy();
    test_name();
    test_kill();
    test_mkdir();
    test_rmdir();
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

