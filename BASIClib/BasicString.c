/*
 * Internal routines for handling BASIC strings...
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include "BasicString.h"

#define __min(x, y) ((x) < (y) ? (x) : (y))

PBasicString __allocString(int length, __boolean isFixed)
/*
 * Build a string, but don't initialize the data.
 *
 *   params : length  == size of new string.
 *            isFixed == allow dynamic string resizing?
 *  returns : newly boxcar-allocated string.
 */
{
    PBasicString retVal;

    retVal = __memAlloc(sizeof (BasicString));
    retVal->data = (length > 0) ? __memAlloc(length) : NULL;
    retVal->length = length;
    retVal->fixedLength = isFixed;

    return(retVal);
} /* __allocString */


PBasicString __createString(char *asciz, __boolean fixedLength)
/*
 * Create a BASIC string from a ASCII zero-terminated "C" string.
 *
 *    params : asciz == ASCII null-terminated string.
 *             fixedLength == should length be unmodifiable?
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
 * Constant BASIC strings are inherently fixed-length.
 *
 *      params : asciz == ASCII null-terminated "C" string to put in BASIC str.
 *     returns : newly created BASIC string. NOT IN A BOXCAR!
 */
{
    PBasicString retVal = __memAlloc(sizeof (BasicString));

    retVal->data = asciz;
    retVal->length = strlen(asciz);
    retVal->fixedLength = true;

    return(retVal);
} /* __constString */


#if 0
/* !!! This is not needed, thanks to the boxcar system. I think. */
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
#endif


PBasicString __assignString(PBasicString to, PBasicString from)
/*
 * Assign a BASIC string value to a BASIC string variable.
 *
 *  Doing this is in BASIC:
 *
 *   str1 = str2
 *
 *  is equivalent to:
 *
 *   str1 = __assignString(str1, str2);
 *
 *    params : to   == Previous value should be forgotten, if it
 *                      exists, and boxcar collection will reclaim it
 *                      later. Fixed length flag is copied from this
 *                      string, if it is not NULL. If NULL, it is
 *                      nonfixed length.
 *             from == String to "assign".
 *   returns : copy of (pStrToAssign) newly allocated in a boxcar.
 */
{
    int copyCount;
    __boolean isFixedLength;
    PBasicString retVal;

        /*
         * if (to) isn't NULL, use its fixedLength setting...
         */
    isFixedLength = ((to == NULL) ? false : to->fixedLength);

        /*
         * if new string is fixedlength, that will be the buffer size,
         *  otherwise, use size of (from) string...
         */
    copyCount = ((isFixedLength == true) ? to->length : from->length);

        /*
         * Build the new string...
         */
    retVal = __allocString(copyCount, isFixedLength);
    memcpy(retVal->data, from->data, __min(copyCount, from->length));

    return(retVal);
} /* __assignString */


PBasicString __catString(PBasicString str1, PBasicString str2)
/*
 * Concatenate two BASIC strings.
 *
 *  Doing this is in BASIC:
 *
 *   a$ = b$ + c$
 *
 *  is equivalent to:
 *
 *   a_DC_ = __assignString(a_DC_, __catString(b_DC_, c_DC_));
 *
 *    params : str1 == BASIC string to concatenate to.
 *             str2 == BASIC string to add to (str1).
 *   returns : newly allocated String with concatenation in boxcar.
 *             (str1) and (str2) are NOT altered.
 */
{
    PBasicString retVal = NULL;
    int newLength;

    if ((str1 == NULL) || (str2 == NULL))
        __runtimeError(ERR_INTERNAL_ERROR);
    else
    {
        newLength = str1->length + str2->length;
        retVal = __allocString(newLength, false);
        retVal->length = newLength;
        memcpy(retVal->data, str1->data, str1->length);
        memcpy(retVal->data + str1->length, str2->data, str2->length);
    } /* else */

    return(retVal);
} /* __catString */


char *__basicStringToAsciz(PBasicString pStr)
/*
 * Copy the data of a BasicString to a C-style ASCIZ (ASCII-Zero
 *  terminated) string. 
 *
 *     params : pStr == BasicString to convert.
 *    returns : newly allocated C string in boxcar.
 */
{
    char *retVal = __memAlloc(pStr->length + 1);
    memcpy(retVal, pStr->data, pStr->length);
    retVal[pStr->length] = '\0';
    return(retVal);
} /* __basicStringToAsciz */


#warning keep __freeString()?
void __freeString(PBasicString pStr)
{
    __memFree(pStr->data);
    __memFree(pStr);
}

/* end of BasicString.c ... */


