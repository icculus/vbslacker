/*
 * BASIClib string-related user functions. For the more low-level
 *  BasicString manipulation routines, such as __createString(),
 *  refer to BasicString.c ...
 *
 * Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include <memory.h>
#include <ctype.h>
#include "StringFunctions.h"


PBasicString _vbSSl_right_DC_(PBasicString pStr, __long count)
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
    else if ((unsigned int) count > pStr->length)
        retVal = __assignString(retVal, pStr);
    else
    {
        retVal = __allocString(count, false);
        memcpy(retVal->data, pStr->data + (pStr->length - count), count);
    } /* else */

    return(retVal);
} /* _vbSSi_right_DC_ */


PBasicString _vbSSl_left_DC_(PBasicString pStr, __long count)
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
    else if ((unsigned int) count > pStr->length)
        retVal = __assignString(retVal, pStr);
    else
    {
        retVal = __allocString(count, false);
        memcpy(retVal->data, pStr->data, count);
    } /* else */

    return(retVal);
} /* _vbSSi_left_DC_ */


PBasicString _vbSS_rtrim_DC_(PBasicString pStr)
/*
 * BASIC RTRIM$() function. Returns a new string with a copy of (pStr),
 *  minus any (' ') chars from the righthand side.
 *  
 *    params : pStr == String to "trim"
 *   returns : newly allocated string with (pStr)'s right side trimmed.
 */
{
    PBasicString retVal = NULL;
    __long i;

        /* loop to find end of trimmable chars... */
    for (i = pStr->length - 1; (i >= 0) && (pStr->data[i] == ' '); i--);

    if (i >= 0)    /* chars to trim? */
    {
        retVal = __allocString(i + 1, false);
        memcpy(retVal->data, pStr->data, i + 1);
    } /* if */
    else        /* no trim? Just copy it to retVal. */
        retVal = __assignString(retVal, pStr);

    return(retVal);
} /* _vbSS_rtrim_DC_ */


PBasicString _vbSS_ltrim_DC_(PBasicString pStr)
/*
 * BASIC LTRIM$() function. Returns a new string with a copy of (pStr),
 *  minus any (' ') chars from the lefthand side.
 *  
 *    params : pStr == String to "trim"
 *   returns : newly allocated string with (pStr)'s left side trimmed.
 */
{
    PBasicString retVal = NULL;
    __long i;
    __long newSize;
    __long max = pStr->length;

        /* loop to find end of trimmable chars... */
    for (i = 0; (i <= max) && (pStr->data[i] == ' '); i++);

    if (i < max)    /* chars to trim? */
    {
        newSize = max - i;
        retVal = __allocString(newSize, false);
        memcpy(retVal->data, pStr->data + i, newSize);
    } /* if */
    else        /* no trim? Just copy it to retVal. */
        retVal = __assignString(retVal, pStr);

    return(retVal);
} /* _vbSS_ltrim_DC_ */


PBasicString _vbSS_lcase_DC_(PBasicString pStr)
/*
 * Returns a lowercased copy of (pStr).
 *
 *    params : pStr == string to lcase.
 *   returns : newly allocated string, per above details.
 */
{
    PBasicString retVal = NULL;
    __byte *i;
    __byte *max;

    retVal = __assignString(retVal, pStr);

    max = (retVal->data + retVal->length) - 1;

    for (i = retVal->data; i < max; i++)
        *i = tolower(*i);

    return(retVal);
} /* _vbSS_lcase_DC_ */



PBasicString _vbSS_ucase_DC_(PBasicString pStr)
/*
 * Returns a uppercased copy of (pStr).
 *
 *    params : pStr == string to ucase.
 *   returns : newly allocated string, per above details.
 */
{
    PBasicString retVal = NULL;
    __byte *i;
    __byte *max;

    retVal = __assignString(retVal, pStr);

    max = (retVal->data + retVal->length) - 1;

    for (i = retVal->data; i < max; i++)
        *i = toupper(*i);

    return(retVal);
} /* _vbSS_ucase_DC_ */


__long _vblS_len(PBasicString pStr)
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
} /* _vblS_len */


PBasicString _vbSl_space_DC_(__long length)
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
} /* _vbSi_space_DC_ */


__long _vbllSS_instr(__long start, PBasicString str1, PBasicString str2)
/*
 * Search a string for the first occurrence of a substring.
 *
 *    params : start == char position (option base 1) to start search.
 *             str1 == String to search for (str2).
 *             str2 == String to find in (str1).
 *   returns : index of first char of (str2) in (str1), (0) if not found.
 */
{
    __long retVal = 0;
    __byte *ascizStr1;
    __byte *ascizStr2;
    __byte *rc;

    if (start <= 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        if (start <= str1->length)
        {
#warning look at instr()!
    /* memstr? No need to convert? !!! */
            start--;   /* make option base 0. */
            ascizStr1 = __basicStringToAsciz(str1);
            ascizStr2 = __basicStringToAsciz(str2);
            rc = strstr(ascizStr1 + start, ascizStr2);

            if (rc != NULL)
                retVal = ((rc - ascizStr1) + 1);

            __memFree(ascizStr1);
            __memFree(ascizStr2);
        } /* if */
    } /* else */

    return(retVal);
} /* _vbllSS_instr */


__long _vblSS_instr(PBasicString str1, PBasicString str2)
/*
 * Search a string for the first occurrence of a substring, starting
 *  the search at position #1 of (str1).
 *
 *    params : str1 == String to search for (str2).
 *             str2 == String to find in (str1).
 *   returns : index of first char of (str2) in (str1), (0) if not found.
 */
{
    return(_vbllSS_instr(1, str1, str2));
} /* _vblSS_instr */



PBasicString _vbSli_string_DC_(__long rep, __integer ch)
{
    PBasicString retVal = __allocString(rep, false);
    memset(retVal->data, ch, rep);
    return(retVal);
} /* _vbSli_string_DC_ */


PBasicString _vbSlS_string_DC_(__long rep, PBasicString strCh)
{
    PBasicString retVal = NULL;

    if (strCh->length == 0)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
        retVal = _vbSli_string_DC_(rep, (__integer) strCh->data[0]);

    return(retVal);
} /* _vbSlS_string_DC_ */


/* !!! still need: */
/* mid$ */
/* mid$ = */
/* LSET */
/* rset */

/* end of StringFunctions.c ... */


