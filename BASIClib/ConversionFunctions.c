/*
 * BASIC support library: conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <ctype.h>
#include "ConversionFunctions.h"
#include "InternalMemManager.h"
#include "MathFunctions.h"
#include "BasicString.h"
#include "ErrorFunctions.h"
#include "Variant.h"
#include "Boolean.h"


int asc(PBasicString pBasicStr)
/*
 * Get the ASCII value of the first character of (str)...
 *
 *    params : str == string where ASCII value is desired.
 *   returns : ASCII value of first char in string.
 */
{
    unsigned int retVal = 0;

    if (pBasicStr->length == 0)    /* blank string? */
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
        retVal = (unsigned int) pBasicStr->data[0];

    return(retVal);
} /* asc */



PBasicString chr_DC_(double asciiValue)
/*
 * Returns a new basic string of one character length based on the ascii
 *  value passed.
 *
 *   params : asciiValue == ascii code to convert to a string.
 *  returns : one char string of ascii value.
 */
{
    PBasicString retVal = NULL;
    int intAscii = (int) asciiValue;

    if ((intAscii < 0) || (intAscii > 255))
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        retVal = __allocString(1, false);
        retVal->data[0] = (char) asciiValue;
    } /* else */

    return(retVal);
} /* chr_DC_ */



PBasicString str_DC_(double numeric)
/*
 * Convert a numeric to a BASIC string. If (numeric) is positive, the
 *  string contains a leading blank (' ') character. If negative, that
 *  blank character will be a '-' char. Decimal points and places are only
 *  placed in the string if needed. For example, str$(1.0) will return " 1".
 *
 *    params : numeric == number to convert to a BASIC string.
 *   returns : newly allocated BASIC string.
 */
{
#ifdef BROKEN
    PBasicString retVal;
    int allocated = 10;
    int decimalPlaces = 0;
    char *buffer = __memAlloc(allocated);
    double i;
    int index = 1;
    int number;

    if (numeric < 0.0)   /* negative? */
    {
        buffer[0] = '-';
        numeric = -numeric;   /* make number positive. */
    } /* if */
    else
        buffer[0] = ' ';

    for (i = 1.0; i < numeric; i *= 10.0, decimalPlaces--);

    for (i /= 10.0; numeric != 0.0; i /= 10.0)
    {
        if (i == 0.1)  /* decimal places? */
        {
            buffer[index] = '.';      /* add decimal point to string. */
            index++;
        } /* if */

        number = (int) (numeric / i);
        numeric -= (number * i);
        buffer[index] = ('0' + number);
        index++;
        decimalPlaces++;

        if (index + 1 >= allocated)
        {
            allocated += 10;
            buffer = __memRealloc(buffer, allocated);
        } /* if */            
    } /* for */

    buffer[index] = '\0';     /* null-terminate string for conversion. */
    retVal = __createString(buffer, false);
    __memFree(buffer);
    return(retVal);

#else

#warning "str_DC_() is still a kludge!"

/* !!! fix this. */

    char buffer[100];
    sprintf(buffer, "%f", numeric);
    return(__createString(buffer, false));
#endif

} /* str_DC_ */



int __valEndOfNumberString(PBasicString pBasicStr)
/*
 * Used by val() to find index in string where number ends.
 *
 *   params : pBasicStr == BASIC string to convert.
 *  returns : index in string where number to convert ends.
 */
{
    boolean pastFirst = false;      /* so extra '-' chars don't sneak in. */
    boolean foundDecimal = false;   /* so extra '.' chars don't sneak in. */
    boolean foundEnd = false;       /* loop control variable.             */
    int max = pBasicStr->length;    /* loop control variable.             */
    int i;                          /* loop control variable.             */
    char *str = pBasicStr->data;    /* the actual string data.            */
    int ch;                         /* for individual chars in string.    */

    for (i = 0; (i < max) && (!foundEnd); i++)
    {
        ch = (int) str[i];
        if (!isdigit(ch))
        {
            switch (ch)
            {
                case ' ':             /* whitespace that val() ignores. */
                case '\t':
                case '\n':
                    break;

                case '-':             /* negative number? */
                    if (pastFirst)
                        foundEnd = true;
                    else
                        pastFirst = true;
                    break;

                case '.':             /* decimal place? */
                    if (foundDecimal)
                        foundEnd = true;
                    else
                        foundDecimal = true;
                    pastFirst = true;
                    break;

                default:              /* ...everything else. */
                    pastFirst = foundEnd = true;
                    break;
            } /* switch */
        } /* if */
        else
            pastFirst = true;
    } /* for */

    return(i - 1);   /* minus one since for-loop will bump us past end... */
} /* __valEndOfNumberString */



double val(PBasicString pBasicStr)
/*
 * Convert a BASIC string into a numeric. This function stops conversion
 *  at the end of the string, or when it runs into a character it can't
 *  recognize as numeric. Spaces (' '), tabs ('\t') and linefeeds ('\n') are
 *  ignored. So "1615 198th Street" would return (1615198).
 *
 *    params : pBasicStr == pointer to BASIC string to convert. ("") accepted.
 *   returns : see above.
 */
{

#warning "val() still isn't right!"

    double retVal = 0.0;
    int strEndIndex = __valEndOfNumberString(pBasicStr);
    int decimalPlace = 1;
    int ch;
    int i;

    for (i = strEndIndex; i >= 0; i--)
    {
        ch = (int) pBasicStr->data[i];
        if (isdigit(ch))
        {
            retVal += ((ch - '0') * decimalPlace);
            decimalPlace *= 10;
        } /* if */
        else
        {
            switch (ch)
            {
                case ' ':   /* ignore these. */
                case '\t':
                case '\n':
                    break;

                case '-':
                    retVal = -retVal;
                    break;

                case '.':
                    retVal /= (double) decimalPlace;
                    decimalPlace = 1;
                    break;
            } /* switch */
        } /* else */
    } /* for */

    return(retVal);
} /* val */



PBasicString hex_DC_(double x)
/*
 * Convert a numeric to a string in Hexadecimal format. Numeric is rounded
 *  as necessary.
 *
 *   params : x == numeric to convert.
 *  returns : newly allocated BASIC string with hex version of (x).
 */
{
    int rounded;

    /* !!! this is a kludge. Really write this... */

    char buffer[20];

    rounded = __round(x);
    sprintf(buffer, "%X", rounded);

    return(__createString(buffer, false));
} /* hex_DC_ */



PBasicString oct_DC_(double x)
/*
 * Convert a numeric to a string in Octal format. Numeric is rounded
 *  as necessary.
 *
 *   params : x == numeric to convert.
 *  returns : newly allocated BASIC string with octal version of (x).
 */
{
    int rounded;

    /* !!! this is a kludge. Really write this... */

    char buffer[20];

    rounded = __round(x);
    sprintf(buffer, "%o", rounded);

    return(__createString(buffer, false));
} /* oct_DC_ */




    /*
     * Internal Variant datatype conversions. The Parser/compiler will have
     *  to insert calls to these before passing variant variables to
     *  functions expecting non-variant intrinsics...
     */

int __VariantToInt(PVariant var)
{
    int retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = var->data._integer;
            break;
        case LONG:
            retVal = (int) var->data._long;
            break;
        case SINGLE:
            retVal = (int) var->data._single;
            break;
        case DOUBLE:
            retVal = (int) var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __VariantToInt */



long __VariantToLong(PVariant var)
{
    long retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = (long) var->data._integer;
            break;
        case LONG:
            retVal = var->data._long;
            break;
        case SINGLE:
            retVal = (long) var->data._single;
            break;
        case DOUBLE:
            retVal = (long) var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __VariantToLong */



float __VariantToFloat(PVariant var)
{
    float retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = (float) var->data._integer;
            break;
        case LONG:
            retVal = (float) var->data._long;
            break;
        case SINGLE:
            retVal = var->data._single;
            break;
        case DOUBLE:
            retVal = (float) var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __VariantToFloat */



double __VariantToDouble(PVariant var)
{
    double retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = (double) var->data._integer;
            break;
        case LONG:
            retVal = (double) var->data._long;
            break;
        case SINGLE:
            retVal = (double) var->data._single;
            break;
        case DOUBLE:
            retVal = var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __VariantToDouble */



PBasicString __VariantToString(PVariant pVar, boolean byRef)
/*
 * The parser/compiler will be responsible for free()ing the return value
 *  from this call after the program is done with it...
 *
 *    params : pVar == Variant to convert.
 *             byRef == (true) to return the PBasicString stored in the
 *                      variant structure, (false) to make a new copy of it.
 */
{
    PBasicString retVal = NULL;

    /* !!! Should we auto-convert numbers? How does BASIC do this? */

    if (pVar->type == STRING)
    {
        if (byRef)
            retVal = pVar->data._string;
        else
            __assignString(&retVal, pVar->data._string);
    } // if
    else
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */

    return(retVal);
} /* __VariantToString */


/* end of ConversionFunctions.c ... */


