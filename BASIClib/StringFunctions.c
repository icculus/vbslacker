/*
 * BASIClib string-related user functions. For the more low-level
 *  BasicString manipulation routines, such as __createString(),
 *  refer to BasicString.c ...
 *
 * Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include <string.h>
#include <memory.h>
#include <ctype.h>
#include "StringFunctions.h"


PBasicString vbSSi_right_DC_(STATEPARAMS, PBasicString pStr, int count)
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
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else if ((unsigned int) count > pStr->length)
        __assignString(STATEARGS, &retVal, pStr);
    else
    {
        retVal = __allocString(STATEARGS, count, false);
        memcpy(retVal->data, pStr->data + (pStr->length - count), count);
    } /* else */

    return(retVal);
} /* vbSSi_right_DC_ */


PBasicString vbSSi_left_DC_(STATEPARAMS, PBasicString pStr, int count)
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
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else if ((unsigned int) count > pStr->length)
        __assignString(STATEARGS, &retVal, pStr);
    else
    {
        retVal = __allocString(STATEARGS, count, false);
        memcpy(retVal->data, pStr->data, count);
    } /* else */

    return(retVal);
} /* vbSSi_left_DC_ */


PBasicString vbSS_rtrim_DC_(STATEPARAMS, PBasicString pStr)
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
        retVal = __allocString(STATEARGS, i + 1, false);
        memcpy(retVal->data, pStr->data, i + 1);
    } /* if */
    else        /* no trim? Just copy it to retVal. */
        __assignString(STATEARGS, &retVal, pStr);

    return(retVal);
} /* vbSS_rtrim_DC_ */


PBasicString vbSS_ltrim_DC_(STATEPARAMS, PBasicString pStr)
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
        retVal = __allocString(STATEARGS, newSize, false);
        memcpy(retVal->data, pStr->data + i, newSize);
    } /* if */
    else        /* no trim? Just copy it to retVal. */
        __assignString(STATEARGS, &retVal, pStr);

    return(retVal);
} /* vbSS_ltrim_DC_ */


PBasicString vbSS_lcase_DC_(STATEPARAMS, PBasicString pStr)
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

    __assignString(STATEARGS, &retVal, pStr);

    max = (retVal->data + retVal->length) - 1;

    for (i = retVal->data; i < max; i++)
        *i = tolower(*i);

    return(retVal);
} /* vbSS_lcase_DC_ */



PBasicString vbSS_ucase_DC_(STATEPARAMS, PBasicString pStr)
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

    __assignString(STATEARGS, &retVal, pStr);

    max = (retVal->data + retVal->length) - 1;

    for (i = retVal->data; i < max; i++)
        *i = toupper(*i);

    return(retVal);
} /* vbSS_ucase_DC_ */


int vbiS_len(STATEPARAMS, PBasicString pStr)
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
} /* vbiS_len */


PBasicString vbSi_space_DC_(STATEPARAMS, int length)
/*
 * Create a BasicString consisting of (length) (' ') chars.
 *
 *   params : length == number of spaces to put into string.
 *  returns : newly allocated string.
 */
{
    PBasicString retVal;

    if (length < 0)
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        retVal = __allocString(STATEARGS, length, false);
        memset(retVal->data, ' ', length);
    } /* else */

    return(retVal);
} /* vbSi_space_DC_ */


int vbiiSS_instr(STATEPARAMS, int start, PBasicString str1, PBasicString str2)
/*
 * Search a string for the first occurrence of a substring.
 *
 *    params : start == char position (option base 1) to start search.
 *             str1 == String to search for (str2).
 *             str2 == String to find in (str1).
 *   returns : index of first char of (str2) in (str1), (0) if not found.
 */
{
    int retVal = 0;
    char *ascizStr1;
    char *ascizStr2;
    char *rc;

    if (start <= 0)
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        if (start <= str1->length)
        {
            start--;   /* make option base 0. */
            ascizStr1 = __basicStringToAsciz(STATEARGS, str1);
            ascizStr2 = __basicStringToAsciz(STATEARGS, str2);
            rc = strstr(ascizStr1 + start, ascizStr2);

            if (rc != NULL)
                retVal = ((rc - ascizStr1) + 1);

            __memFree(STATEARGS, ascizStr1);
            __memFree(STATEARGS, ascizStr2);
        } /* if */
    } /* else */

    return(retVal);
} /* vbiiSS_instr */


int vbiSS_instr(STATEPARAMS, PBasicString str1, PBasicString str2)
/*
 * Search a string for the first occurrence of a substring, starting
 *  the search at position #1 of (str1).
 *
 *    params : str1 == String to search for (str2).
 *             str2 == String to find in (str1).
 *   returns : index of first char of (str2) in (str1), (0) if not found.
 */
{
    return(vbiiSS_instr(STATEARGS, 1, str1, str2));
} /* vbiSS_instr */



PBasicString vbSii_string_DC_(STATEPARAMS, int rep, int ch)
{
    PBasicString retVal = __allocString(STATEARGS, rep, false);
    memset(retVal->data, ch, rep);
    return(retVal);
} /* vbSii_string_DC_ */


PBasicString vbSiS_string_DC_(STATEPARAMS, int rep, PBasicString strCh)
{
    PBasicString retVal = NULL;

    if (strCh->length == 0)
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else
        retVal = vbSii_string_DC_(STATEARGS, rep, (int) strCh->data[0]);

    return(retVal);
} /* vbSiS_string_DC_ */


/* !!! still need: */
/* mid$ */
/* mid$ = */
/* LSET */
/* rset */

/* end of StringFunctions.c ... */


