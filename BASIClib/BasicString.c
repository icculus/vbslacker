/*
 * Internal routines for handling BASIC strings...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include "BasicString.h"
#include "ErrorFunctions.h"
#include "InternalMemManager.h"
#include "MathFunctions.h"
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
    retVal->data = ((length > 0) ? __memAlloc(length) : NULL);
    retVal->length = length;
    retVal->fixedLength = fixedLength;

    return(retVal);
} /* __allocString */


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


PBasicString __constString(char *asciz)
/*
 * This builds a new BASIC string, but rather than copy the string data
 *  over, it just copies the pointer. You have to be careful never to
 *  modify these strings, or even try to free them. This is useful for
 *  constant string values in the converted BASIC code, so we don't have
 *  to have two copies of every string literal floating about.
 *
 * Constant BASIC strings are inherently fixed length.
 *
 *      params : asciz == ASCII null-terminated "C" string to put in BASIC str.
 *     returns : newly created BASIC string.
 */
{
    PBasicString retVal = __allocString(0, true);

    retVal->data = asciz;
    retVal->length = strlen(asciz);
    return(retVal);
} /* __constString */


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


void __assignString(PBasicString *ppBasicStr, PBasicString pStrToAssign)
/*
 * Assign a BASIC string value to a BASIC string variable.
 *
 *  Doing this is in BASIC:
 *
 *   str1 = str2
 *
 *  is equivalent to:
 *
 *   __assignString(&str1, str2);
 *
 *
 *
 *    params : ppBasicStr == String to do assigning to. Previous values are
 *                            __memRealloc()d, if they exist. Passing a NULL
 *                            here will result in the creation of a new, 
 *                            non-fixed length BASIC string.
 *             pStrToAssign == String to assign to (ppBasicStr).
 *   returns : void. *ppBasicStr may hold new data. 
 */
{
    int copyCount;
    PBasicString pStr = *ppBasicStr;

    if (pStr == NULL)
        *ppBasicStr = __createString(pStrToAssign->data, false);
    else
    {
        if (pStr->fixedLength == true)
            copyCount = __min(pStr->length, pStrToAssign->length);
        else
        {
            copyCount = pStrToAssign->length;
            pStr->data = __memRealloc(pStr->data, copyCount);
            pStr->length = copyCount;
        } /* else */

        memcpy(pStr->data, pStrToAssign, copyCount);
    } /* else */
} /* __assignString */


void __catString(PBasicString *ppBasicStr, PBasicString pStrToCat)
/*
 * Concatenate two BASIC strings.
 *
 *    params : ppBasicStr == BASIC string to concatenate to. If NULL, a
 *                            new non-fixed length string is created. 
 *             pStrToCat  == BASIC string to add to (ppBasicStr).
 *   returns : void, but *ppBasicStr may hold new data. 
 */
{
    int newLength;
    PBasicString pStr = *ppBasicStr;

    if (pStr == NULL)
        *ppBasicStr = __createString(pStrToCat->data, false);
    else
    {
            /*
             * In theory, this function should never be called with
             *  a fixed length BASIC string. Throw error if it is.
             */
        if (pStr->fixedLength == true)
            __runtimeError(ERR_INTERNAL_ERROR);
        else
        {
            newLength = pStr->length + pStrToCat->length;            
            pStr->data = __memRealloc(pStr->data, newLength);
            memcpy(pStr->data + pStr->length, pStrToCat->data,
                   pStrToCat->length);
            pStr->length = newLength;
        } /* else */
    } /* else */
} /* __catString */


/* end of BasicString.c ... */


