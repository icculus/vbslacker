/*
 * APIs that deal with the environment, or, the interface between the
 *  operating system and the application.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "EnvrFunctions.h"

#warning Most of EnvrFunctions.c are stubs...


void vbpS_chdir(STATEPARAMS, PBasicString newDir)
{
    /* path not found. */
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
} /* vbSS_environ_DC_ */


PBasicString vbSi_environ_DC_(STATEPARAMS, int n)
/*
 * Get the (n)th string from the list of environment variables, whatever
 *  it may be.
 *
 *     params : n == the index of the variable you wish to retrieve.
 *    returns : (?!) !!!
 */
{
} /* vbSS_environ_DC_ */


void vbpS_environ(STATEPARAMS, PBasicString newEnvrStr)
{
} /* vbpS_environ */


vbii_fre(STATEPARAMS, int arg)
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


vbiS_fre(STATEPARAMS, PBasicString strExp)
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

