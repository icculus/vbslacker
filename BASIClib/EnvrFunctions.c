/*
 * APIs that deal with the environment, or, the interface between the
 *  operating system and the application.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "EnvrFunctions.h"

extern char **environ;

static ThreadLock envrLock;
static PBasicString commandLine;


void __initEnvrFunctions(int argc, char **argv)
/*
 * Initializations;
 *   - Create a ThreadLock for thread-proofing getenv() calls...
 *   - get the command line, and convert it to a BASIC string for COMMAND$().
 *
 *      params : argc, argv == same as params to main()...
 *     returns : void.
 */
{
    __integer i;
    __integer size = 0;
    __byte *cmdLine;

    __createThreadLock(&envrLock);

    /* !!! This is basically inefficient... */

    for (i = 1; i < argc; i++)        /* get size of command line string... */
        size += (__integer) (strlen(argv[i]) + 1);

    cmdLine = __memAllocNoPtrs(size);
    cmdLine[0] = '\0';

    for (i = 1; i < argc; i++)   /* Make argv[] one space-separated string. */
    {
        strcat(cmdLine, argv[i]);
        if (i < argc - 1)
            cmdLine[strlen(cmdLine)] = ' ';
    } /* for */

                                 /* Convert to a BASIC string. */
    commandLine = __createString(cmdLine, false);
    __memFree(cmdLine);
} /* __initEnvrFunctions */


void __deinitEnvrFunctions(void)
/*
 * Deinitialization; Destroy the ThreadLock. The rest will take care of
 *  itself.
 *
 *      params : void.
 *     returns : void.
 */
{
    __destroyThreadLock(&envrLock);
} /* __initEnvrFunctions */


PBasicString _vbSS_environ_DC_(PBasicString envVarName)
/*
 * Get the value of the environment variable named by (envVarName).
 *
 *    params : envVarName == Environment variable name to lookup.
 *   returns : value of environment variable (envVarName). Returns ("") if
 *              such a variable does not exist.
 */
{
    __byte *str = __basicStringToAsciz(envVarName);
    __byte *rc;
    PBasicString retVal;

        /* other threads may overwrite this, so lock it... */
    __obtainThreadLock(&envrLock);
    rc = getenv(str);
    retVal = __createString((rc == NULL) ? "" : (char *) rc, false);
    __releaseThreadLock(&envrLock);

    __memFree(str);
    return(retVal);
} /* vbSS_environ_DC_ */


PBasicString _vbSf_environ_DC_(__single n)
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
    __integer i;
    __integer rounded = (__integer) (n + 0.5);

    if (rounded <= 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);

    for (i = 0; (i < rounded) && (environ[i] != NULL); i++)
        /* do nothing. */;

    retVal = __createString(((environ[i] == NULL) ? "" : environ[i]), false);
    return(retVal);
} /* _vbSf_environ_DC_ */


void _vbpS_environ(PBasicString newEnvrStr)
/*
 * Change or add environment variables for the process; any child processes
 *  spawned will reflect these changes, as well as future calls to
 *  _vbSi_environ_DC_()...
 *
 *   params : newEnvStr == BASIC string in the format of
 *                         "EnvVarName = EnvVarValue" or
 *                         "EnvVarName EnvVarValue".
 *  returns : void.
 */
{
#if 0
    void *separator;
    __byte *envrName;
    __byte *envrVal;
    __byte *buffer;

    separator = memchr(newEnvrStr->data, ' ', newEnvrStr->length);
    if (separator == NULL)
    {
        separator = memchr(newEnvrStr->data, '=', newEnvrStr->length);
        if (separator == NULL)
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    } /* if */

    envrName = __memAlloc(separator - (void *) newEnvrStr->data);
    memcpy(envrName, newEnvrStr->data, separator - newEnvrStr->data);
    /* !!! not done! */
#endif
} /* _vbpS_environ */


__integer _vbiS_fre(PBasicString strExp)
/*
 * Compact memory (garbage collect), and return the available string space,
 *  in bytes.
 *
 *     params : strExp == any BASIC string. Ignored.
 *    returns : see above.
 */
{
    __memDoFullCollect();
    return((__integer) 65767);  /* !!! */
} /* _vbiS_fre */


PBasicString _vbS_command_DC_(void)
/*
 * Return the command line of this application.
 *
 *   params : void.
 *  returns : BASIC string containing the command line.
 */
{
    PBasicString retVal = NULL;

    retVal = __assignString(retVal, commandLine);
    return(retVal);
} /* _vbS_command_DC_ */

/* end of EnvrFunctions.c ... */
