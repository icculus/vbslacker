/*
 * APIs that deal with the environment, or, the interface between the
 *  operating system and the application.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "EnvrFunctions.h"


extern char **environ;

ThreadLock envrLock;


void __initEnvrFunctions(STATEPARAMS)
{
    __createThreadLock(STATEARGS, &envrLock);
} /* __initEnvrFunctions */


void __deinitEnvrFunctions(STATEPARAMS)
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
        /* !!! */ ;

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
    return(0);  /* !!! */
} /* vbiS_fre */


/* end of EnvrFunctions.c ... */

