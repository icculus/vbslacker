/*
 * BASIClib string-related user functions. For the more low-level
 *  BasicString manipulation routines, such as __createString(),
 *  refer to BasicString.c ...
 *
 * Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
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

    for (i = pStr->length - 1; (i >= 0) && (retVal == NULL); i--)
    {
        if (pStr->data[i] != ' ')    /* End of trimmable space? */
        {
            retVal = __allocString(i, false);
            memcpy(retVal->data, pStr->data, i);
        } /* if */
    } /* for */

!!!

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
} /* ltrim_DC_ */


PBasicString lcase_DC_(PBasicString pStr)
/*
 * Returns a lowercased copy of (pStr).
 *
 *    params : pStr == string to lcase.
 *   returns : newly allocated string, per above details.
 */
{
} /* lcase_DC_ */


PBasicString ucase_DC_(PBasicString pStr)
/*
 * Returns a uppercased copy of (pStr).
 *
 *    params : pStr == string to ucase.
 *   returns : newly allocated string, per above details.
 */
{
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

/* end of StringFunctions.c ... */


