/*
 * APIs that deal with the environment, or, the interface between the
 *  operating system and the application.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "EnvrFunctions.h"

extern char **environ;

static ThreadLock envrLock;
static PBasicString commandLine;
static __boolean unixFileSystem;


void __initEnvrFunctions(STATEPARAMS, int argc, char **argv)
/*
 * Initializations;
 *   - Create a ThreadLock for thread-proofing getenv() calls...
 *   - Check if the "unix file system compatible" init flag was set.
 *   - get the command line, and convert it to a BASIC string for COMMAND$().
 *
 *      params : argc, argv == same as params to main()...
 *     returns : void.
 */
{
    int i;
    int size = 0;
    char *cmdLine;

    __createThreadLock(STATEARGS, &envrLock);

    unixFileSystem = ((__getInitFlags(STATEARGS) & INITFLAG_UNIX_FILE_SYSTEM) ?
                          true : false);


    /* !!! This is basically inefficient... */

    for (i = 1; i < argc; i++)        /* get size of command line string... */
        size += strlen(argv[i]) + 1;

    cmdLine = __memAlloc(STATEARGS, size);
    memset(cmdLine, '\0', size);

    for (i = 1; i < argc; i++)   /* Make argv[] one space-separated string. */
    {
        strcat(cmdLine, argv[i]);
        if (i < argc - 1)
            cmdLine[strlen(cmdLine)] = ' ';
    } /* for */

                                 /* Convert to a BASIC string. */
    commandLine = __createString(STATEARGS, cmdLine, false);
    __memFree(STATEARGS, cmdLine);
} /* __initEnvrFunctions */


void __deinitEnvrFunctions(STATEPARAMS)
/*
 * Deinitialization; Destroy the ThreadLock. The rest will take care of
 *  itself.
 *
 *      params : void.
 *     returns : void.
 */
{
    __destroyThreadLock(STATEARGS, &envrLock);
} /* __initEnvrFunctions */


void vbpS_chdir(STATEPARAMS, PBasicString newDir)
/*
 * Change the current working directory. All relative paths used in
 *  file i/o will work from the new working directory from now on.
 *
 *    params : newDir == new directory to make current.
 *   returns : void. Throws a few errors, though.
 */
{
    char *str = __basicStringToAsciz(STATEARGS, newDir);
    int rc;

    rc = chdir(str);
    __memFree(STATEARGS, str);

    if (rc == -1)
    {
        switch (errno)
        {
            case ENOENT:
            case ENOTDIR:
                rc = ERR_PATH_NOT_FOUND;
                break;

            case ELOOP:
            case EIO:
                rc = ERR_PATH_FILE_ACCESS_ERROR;
                break;

            case EACCES:
                rc = ERR_PERMISSION_DENIED;
                break;

            case EFAULT:
            case ENAMETOOLONG:
            case ENOMEM:
            default:
                rc = ERR_INTERNAL_ERROR;
                break;
        } /* switch */

        __runtimeError(STATEARGS, rc);
    } /* if */
} /* vbpS_chdir */


PBasicString vbSS_curdir_DC_(STATEPARAMS, PBasicString drive)
/*
 * Return current working directory by drive letter. Under Unix-like
 *  Operating systems, the only valid drive letter is "C"...You can set
 *  the behavior of this by setting the "unix file system compatible"
 *  option at compile time. Either way on unix, you get a runtime error
 *  if you specify a drive other than C or blank, but you'll either get
 *  "/home/gordonr" or "C:\home\gordonr", depending on how you set the
 *  compatibility. 
 *
 *    params : drive == drive letter. Only first letter is read. ("") means
 *                      current drive.
 *   returns : see above.
 */
{
    
} /* vbSS_curdir */


PBasicString vbS_curdir_DC_(STATEPARAMS)
/*
 * Same as above vbSS_curdir_DC_(), but always gets current drives's
 *  working directory. Equivalent to vbSS_curdir_DC_(STATEARGS, "");
 *
 *     params : void.
 *    returns : see above.
 */
{
    PBasicString blankString = __createString(STATEARGS, "", false);
    return(vbSS_curdir_DC_(STATEARGS, blankString));
} /* vbS_curdir_DC_ */


PBasicString vbSS_environ_DC_(STATEPARAMS, PBasicString envVarName)
/*
 * Get the value of the environment variable named by (envVarName).
 *
 *    params : envVarName == Environment variable name to lookup.
 *   returns : value of environment variable (envVarName). Returns ("") if
 *              such a variable does not exist.
 */
{
    char *str = __basicStringToAsciz(STATEARGS, envVarName);
    char *rc;
    PBasicString retVal;

        /* other threads may overwrite this, so lock it... */
    __obtainThreadLock(STATEARGS, &envrLock);
    rc = getenv(str);
    retVal = __createString(STATEARGS, (rc == NULL) ? "" : rc, false);
    __releaseThreadLock(STATEARGS, &envrLock);

    __memFree(STATEARGS, str);
    return(retVal);
} /* vbSS_environ_DC_ */


PBasicString vbSi_environ_DC_(STATEPARAMS, int n)
/*
 * Get the (n)th string from the list of environment variables, whatever
 *  it may be.
 *
 *     params : n == the index of the variable you wish to retrieve.
 *    returns : BASIC String in form "VARNAME=VARVALUE". The case of
 *               returned values is undefined. Returns ("") if there isn't
 *               an (n)th environment variable.
 */
{
    PBasicString retVal;
    int i;

    for (i = 0; (i < n) && (environ[i] != NULL); i++)
        /* do nothing. */;

    retVal = __createString(STATEARGS,
                            (environ[i] == NULL) ? "" : environ[i],
                            false);
    return(retVal);
} /* vbSi_environ_DC_ */


void vbpS_environ(STATEPARAMS, PBasicString newEnvrStr)
{
#warning vbpS_environ() is a stub!
} /* vbpS_environ */


int vbii_fre(STATEPARAMS, int arg)
/*
 * Returns a byte count of available memory remaining.
 *
 *  if (arg) == -1, then FRE() returns the size of the largest (nonstring)
 *   array that can be allocated.
 *  if (arg) == -2, then FRE() returns the unused stack space.
 *  For any other value, FRE() will return the available string space.
 *
 *  Ugly. Just ugly.
 *
 *    params : arg == see above.
 *   returns : see above.
 */
{
    int retVal = 0;

    if (arg == -1)       /* return available memory. */
        /* !!! */ ;
    else if (arg == -2)  /* return available stack space. */
        /* !!! */ ;
    else                 /* return string space. */
        retVal = vbiS_fre(STATEARGS, NULL);

    return(retVal);
} /* vbii_fre */


int vbiS_fre(STATEPARAMS, PBasicString strExp)
/*
 * Compact memory (garbage collect), and return the available string space,
 *  in bytes.
 *
 *     params : strExp == any BASIC string. Ignored.
 *    returns : see above.
 */
{
    __memForceFullBoxcarRelease(STATEARGS);
    return(65767);  /* !!! */
} /* vbiS_fre */


PBasicString vbS_command_DC_(STATEPARAMS)
/*
 * Return the command line of this application.
 *
 *   params : void.
 *  returns : BASIC string containing the command line.
 */
{
    PBasicString retVal = NULL;

    __assignString(STATEARGS, &retVal, commandLine);
    return(retVal);
} /* vbS_command_DC_ */

/* end of EnvrFunctions.c ... */
