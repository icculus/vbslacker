/*
 * Internal routines for handling BASIC strings...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include "InternalMemManager.h"
#include "BasicString.h"
#include "Boolean.h"



PBasicString __allocString(int length, boolean fixedLength)
/*
 * Build a string, but don't initialize the data.
 *
 *   params : length == size of new string.
 *            fixedLength == allow dynamic string resizing?
 *  returns : newly allocated string.
 */
{
    PBasicString retVal;

    retVal = __memAlloc(sizeof (BasicString));
    retVal->data = __memAlloc(length);
    retVal->length = length;
    retVal->fixedLength = fixedLength;

    return(retVal);
} /* __allocString */



void __freeString(PBasicString pBasicStr)
/*
 * Free a previously allocated BASIC String.
 *
 *   params : pBasicStr == BASIC String to free.
 *  returns : void.
 */
{
    if (pBasicStr != NULL)
    {
        if (pBasicStr->data != NULL)
            __memFree(pBasicStr->data);

        __memFree(pBasicStr);
    } /* if */
} /* __freeString */



PBasicString __cloneString(PBasicString pBasicStr)
/*
 * Copy a BASIC string to a newly allocated structure.
 *
 *   params : pBasicStr == BASIC string to copy.
 *  returns : Copy of (pBasicStr) in newly allocated structure.
 */
{
    PBasicString retVal;

    retVal = __allocString(pBasicStr->length, pBasicStr->fixedLength);
    memcpy(retVal->data, pBasicStr->data, pBasicStr->length);

    return(retVal);
} /* __cloneString */



PBasicString __createString(char *asciz, boolean fixedLength)
/*
 * Create a BASIC string from a ASCII zero-terminated "C" string.
 *
 *    params : asciz == ASCII null-terminated string.
 *             fixedLength == should length be modifiable?
 *   returns : Newly allocated BASIC string.
 */
{
    int length = strlen(asciz);
    PBasicString retVal = __allocString(length, fixedLength);

    memcpy(retVal->data, asciz, length);
    return(retVal);
} /* __createString */

/* end of BasicString.c ... */


