/*
 * BASIClib string-related user functions. For the more low-level
 *  BasicString manipulation routines, such as __createString(),
 *  refer to BasicString.c ...
 *
 * Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include <ctype.h>
#include "BasicString.h"
#include "ErrorFunctions.h"

/* !!!
mid$
mid$ =
*/


PBasicString right_DC_(PBasicString pStr, int count)
/*
 * BASIC RIGHT$() function. Returns rightmost (count) chars in
 *  (pStr) in a new string.
 *
 *    params : pStr  == BasicString to get characters from.
 *             count == number of bytes to get.
 *   returns : pointer to newly allocated string, per above details.
 */
{
    PBasicString retVal = NULL;

    if (count < 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else if (count > pStr->length)
        __assignString(&retVal, pStr);
    else
    {
        retVal = __allocString(count, false);
        memcpy(retVal->data, pStr->data + (pStr->length - count), count);
    } /* else */

    return(retVal);
} /* right_DC_ */


PBasicString left_DC_(PBasicString pStr, int count)
/*
 * BASIC LEFT$() function. Returns leftmost (count) chars in
 *  (pStr) in a new string.
 *
 *    params : pStr  == BasicString to get characters from.
 *             count == number of bytes to get.
 *   returns : newly allocated string, per above details.
 */
{
    PBasicString retVal = NULL;

    if (count < 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else if (count > pStr->length)
        __assignString(&retVal, pStr);
    else
    {
        retVal = __allocString(count, false);
        memcpy(retVal->data, pStr->data, count);
    } /* else */

    return(retVal);
} /* left_DC_ */


PBasicString rtrim_DC_(PBasicString pStr)
/*
 * BASIC RTRIM$() function. Returns a new string with a copy of (pStr),
 *  minus any (' ') chars from the righthand side.
 *  
 *    params : pStr == String to "trim"
 *   returns : newly allocated string with (pStr)'s right side trimmed.
 */
{
    PBasicString retVal = NULL;
    int i;

        /* loop to find end of trimmable chars... */
    for (i = pStr->length - 1; (i >= 0) && (pStr->data[i] == ' '); i--);

    if (i >= 0)    /* chars to trim? */
    {
        retVal = __allocString(i + 1, false);
        memcpy(retVal->data, pStr->data, i + 1);
    } /* if */
    else        /* no trim? Just copy it to retVal. */
        __assignString(&retVal, pStr);

    return(retVal);
} /* rtrim_DC_ */


PBasicString ltrim_DC_(PBasicString pStr)
/*
 * BASIC LTRIM$() function. Returns a new string with a copy of (pStr),
 *  minus any (' ') chars from the lefthand side.
 *  
 *    params : pStr == String to "trim"
 *   returns : newly allocated string with (pStr)'s left side trimmed.
 */
{
    PBasicString retVal = NULL;
    int i;
    int newSize;
    int max = pStr->length;

        /* loop to find end of trimmable chars... */
    for (i = 0; (i <= max) && (pStr->data[i] == ' '); i++);

    if (i < max)    /* chars to trim? */
    {
        newSize = max - i;
        retVal = __allocString(newSize, false);
        memcpy(retVal->data, pStr->data + i, newSize);
    } /* if */
    else        /* no trim? Just copy it to retVal. */
        __assignString(&retVal, pStr);

    return(retVal);
} /* ltrim_DC_ */


PBasicString lcase_DC_(PBasicString pStr)
/*
 * Returns a lowercased copy of (pStr).
 *
 *    params : pStr == string to lcase.
 *   returns : newly allocated string, per above details.
 */
{
    PBasicString retVal = NULL;
    char *i;
    char *max;

    __assignString(&retVal, pStr);

    max = (retVal->data + retVal->length) - 1;

    for (i = retVal->data; i < max; i++)
        *i = tolower(*i);

    return(retVal);
} /* lcase_DC_ */



PBasicString ucase_DC_(PBasicString pStr)
/*
 * Returns a uppercased copy of (pStr).
 *
 *    params : pStr == string to ucase.
 *   returns : newly allocated string, per above details.
 */
{
    PBasicString retVal = NULL;
    char *i;
    char *max;

    __assignString(&retVal, pStr);

    max = (retVal->data + retVal->length) - 1;

    for (i = retVal->data; i < max; i++)
        *i = toupper(*i);

    return(retVal);
} /* ucase_DC_ */


int len(PBasicString pStr)
/*
 * Return the number of characters contained by (pStr). Remember that
 *  the data in the BasicString structure is NOT null-terminated, and
 *  null characters INSIDE the string are acceptable and are counted when
 *  determining string length.
 *
 *   params : pStr == BasicString to determine the length of.
 *  returns : length in bytes of string.
 */
{
    return(pStr->length);
} /* len */


PBasicString space_DC_(int length)
/*
 * Create a BasicString consisting of (length) (' ') chars.
 *
 *   params : length == number of spaces to put into string.
 *  returns : newly allocated string.
 */
{
    PBasicString retVal;

    if (length < 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        retVal = __allocString(length, false);
        memset(retVal->data, ' ', length);
    } /* else */

    return(retVal);
} /* space_DC_ */

/* end of StringFunctions.c ... */


