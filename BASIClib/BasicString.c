/*
 * Internal routines for handling BASIC strings...
 *
 * BASIC strings differ from C strings in a few respects. C strings are
 *  signified with a char pointer, and are, literally, a string of
 *  characters. BASIC strings are wrapped in a structure containing extra
 *  information, and are not terminated by a NULL char (chr$(0)), like
 *  their C counterparts. This is consistent with Microsoft Basics.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *    Written by Ryan C. Gordon.
 */

#include <string.h>
#include "BasicString.h"

#ifndef __min
#    define __min(x, y) ((x) < (y) ? (x) : (y))
#endif


PBasicString __allocString(__long length, __boolean isFixed)
/*
 * Build a string, but don't initialize the data. The returned data
 *  is dynamically allocated, and may be explicitly freed with the
 *  __freeString() function, but if left alone, will eventually be
 *  released by the garbage collector.
 *
 *   params : length  == size of new string.
 *            isFixed == allow dynamic string resizing?
 *  returns : newly allocated string.
 */
{
    PBasicString retVal;

    retVal = __memAlloc(sizeof (BasicString));
    retVal->data = (length > 0) ? __memAlloc(length) : NULL;
    retVal->length = length;
    retVal->fixedLength = isFixed;

    return(retVal);
} /* __allocString */


PBasicString __createString(__byte *asciz, __boolean fixedLength)
/*
 * Create a BASIC string from a ASCII zero-terminated "C" string.
 *
 *    params : asciz == ASCII null-terminated string.
 *             fixedLength == should length be unmodifiable?
 *   returns : Newly allocated BASIC string.
 */
{
    __long length = (__long) strlen(asciz);
    PBasicString retVal = __allocString(length, fixedLength);

    memcpy(retVal->data, asciz, length);
    return(retVal);
} /* __createString */


PBasicString __constString(__byte *asciz)
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
    PBasicString retVal = __memAlloc(sizeof (BasicString));

    retVal->data = asciz;
    retVal->length = (__long) strlen(asciz);
    retVal->fixedLength = true;

    return(retVal);
} /* __constString */


void __freeString(PBasicString pBasicStr)
/*
 * Free a previously allocated BASIC String. This is unnecessary, since
 *  the garbage collector will handle this automatically, but for
 *  efficiency's sake, this option is left available.
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


PBasicString __assignString(PBasicString to, PBasicString from)
/*
 * Assign a BASIC string value to a BASIC string variable.
 *
 *  Doing this is in BASIC:
 *
 *   str1$ = str2$
 *
 *  is equivalent to:
 *
 *   str1 = __assignString(str1, str2);
 *
 *    params : to   == Previous value should be forgotten, if it
 *                      exists, and garbage collection will reclaim it
 *                      later. Fixed length flag is copied from this
 *                      string, if it is not NULL. If NULL, it is
 *                      nonfixed length.
 *             from == String to "assign" from.
 *   returns : copy of (from), newly allocated.
 */
{
    __long copyCount;
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
 *   returns : newly allocated String with concatenation.
 *             (str1) and (str2) are NOT altered.
 */
{
    PBasicString retVal = NULL;
    __long newLength;

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


__byte *__basicStringToAsciz(PBasicString pStr)
/*
 * Copy the data of a BasicString to a C-style ASCIZ (ASCII-Zero
 *  terminated) string. 
 *
 *     params : pStr == BasicString to convert.
 *    returns : newly allocated C string. Garbage collectable.
 */
{
    char *retVal = __memAlloc(pStr->length + 1);
    memcpy(retVal, pStr->data, pStr->length);
    retVal[pStr->length] = '\0';
    return(retVal);
} /* __basicStringToAsciz */


__byte *__copyAscizString(__byte *copyThis)
/*
 * Allocates a new, garbage collectable object, and copies (copyThis) into it.
 *  The newly allocated object must never be used to store ANY pointers.
 *
 *      params : copyThis == ASCIZ string to copy.
 *     returns : newly-allocated copy of (copyThis).
 */
{
    __byte *retVal = __memAllocNoPtrs(strlen(copyThis) + 1);
    strcpy(retVal, copyThis);
    return(retVal);
} /* __copyAscizString */

/* end of BasicString.c ... */


