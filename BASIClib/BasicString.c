/*
 * Internal routines for handling BASIC strings...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include "BasicString.h"

#define __min(x, y) ((x) < (y) ? (x) : (y))

PBasicString __allocString(STATEPARAMS, int length, __boolean fixedLength)
/*
 * Build a string, but don't initialize the data.
 *
 *   params : length == size of new string.
 *            fixedLength == allow dynamic string resizing?
 *  returns : newly allocated string.
 */
{
    PBasicString retVal;

    retVal = __memAlloc(STATEARGS, sizeof (BasicString));
    retVal->data = ((length > 0) ? __memAlloc(STATEARGS, length) : NULL);
    retVal->length = length;
    retVal->fixedLength = fixedLength;

    return(retVal);
} /* __allocString */


PBasicString __createString(STATEPARAMS, char *asciz, __boolean fixedLength)
/*
 * Create a BASIC string from a ASCII zero-terminated "C" string.
 *
 *    params : asciz == ASCII null-terminated string.
 *             fixedLength == should length be unmodifiable?
 *   returns : Newly allocated BASIC string.
 */
{
    int length = strlen(asciz);
    PBasicString retVal = __allocString(STATEARGS, length, fixedLength);

    memcpy(retVal->data, asciz, length);
    return(retVal);
} /* __createString */


PBasicString __constString(STATEPARAMS, char *asciz)
/*
 * This builds a new BASIC string, but rather than copy the string data
 *  over, it just copies the pointer. You have to be careful never to
 *  modify these strings, or even try to free them. This is useful for
 *  constant string values in the converted BASIC code, so we don't have
 *  to have two copies of every string literal floating about.
 *
 * Constant BASIC strings are inherently fixed-length.
 *
 *      params : asciz == ASCII null-terminated "C" string to put in BASIC str.
 *     returns : newly created BASIC string.
 */
{
    PBasicString retVal = __allocString(STATEARGS, 0, true);

    retVal->data = asciz;
    retVal->length = strlen(asciz);
    return(retVal);
} /* __constString */


void __freeString(STATEPARAMS, PBasicString pBasicStr)
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
            __memFree(STATEARGS, pBasicStr->data);

        __memFree(STATEARGS, pBasicStr);
    } /* if */
} /* __freeString */


void __assignString(STATEPARAMS,
                    PBasicString *ppBasicStr,
                    PBasicString pStrToAssign)
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
        pStr = *ppBasicStr = __allocString(STATEARGS, pStrToAssign->length,
                                           false);

    if (pStr->fixedLength == true)
        copyCount = __min(pStr->length, pStrToAssign->length);
    else
    {
        copyCount = pStrToAssign->length;
        pStr->data = __memRealloc(STATEARGS, pStr->data, copyCount);
        pStr->length = copyCount;
    } /* else */

    memcpy(pStr->data, pStrToAssign->data, copyCount);
} /* __assignString */


void __catString(STATEPARAMS, PBasicString *ppBasicStr, PBasicString pStrToCat)
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
        *ppBasicStr = __createString(STATEARGS, pStrToCat->data, false);
    else
    {
            /*
             * In theory, this function should never be called with
             *  a fixed length BASIC string. Throw error if it is.
             */
        if (pStr->fixedLength == true)
            __runtimeError(STATEARGS, ERR_INTERNAL_ERROR);
        else
        {
            newLength = pStr->length + pStrToCat->length;            
            pStr->data = __memRealloc(STATEARGS, pStr->data, newLength);
            memcpy(pStr->data + pStr->length, pStrToCat->data,
                   pStrToCat->length);
            pStr->length = newLength;
        } /* else */
    } /* else */
} /* __catString */


char *__basicStringToAsciz(STATEPARAMS, PBasicString pStr)
/*
 * Copy the data of a BasicString to a C-style ASCIZ (ASCIi-Zero
 *  terminated) string. Please call __memFree() on the returned 
 *  value when done with it.
 *
 *     params : pStr == BasicString to convert.
 *    returns : newly allocated C string.
 */
{
    char *retVal = __memAlloc(STATEARGS, pStr->length + 1);
    memcpy(retVal, pStr->data, pStr->length);
    retVal[pStr->length] = '\0';
    return(retVal);
} /* __basicStringToAsciz */

/* end of BasicString.c ... */


