/*
 * BASIC support library: conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <ctype.h>
#include "BasicStrings.h"
#include "BasicErrors.h"
#include "Variant.h"
#include "Boolean.h"


double asc(PBasicString pBasicStr)
/*
 * Get the ASCII value of the first character of (str)...
 *
 *    params : str == string where ASCII value is desired.
 *   returns : ASCII value of first char in string.
 */
{
    double retVal = 0;

    if (pBasicStr->length == 0)    /* blank string? */
        __runtimeError(ILLEGAL_FUNCTION_CALL);
    else
        retVal = (double) pBasicStr->data[0];

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

    if (!isascii((int) asciiValue))
        __runtimeError(ILLEGAL_FUNCTION_CALL);
    else
    {
        retVal = __allocString(1, false);
        retVal.data[0] = (char) asciiValue;
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
    PBasicString retVal;
    char buffer[20];        /* !!! Problematic to hard code a "20"? */
    double i;
    int index = 1;
    int number;

    buffer[0] = ((numeric < 0) ? '-' : ' ');   /* negative or positive? */

    for (i = 1.0; (numeric % i) != numeric; i *= 10.0); /* get dec. places */

    for (i /= 10.0; i >= 1; i /= 10)
    {
        number = ((int) numeric) / i;
        buffer[index] = '0' + number;
        index++;
        numeric -= number;
    } /* for */

    buffer[index] = '\0';
    return(__createString(buffer, false));
} /* str_DC_ */



int BASICINT_valEndOfNumberString(PBasicString pBasicStr)
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
    char ch;                        /* for individual chars in string.    */

    for (i = 0; (i < max) && (!foundEnd); i++)
    {
        ch = str[i];
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
} /* BASICINT_valEndOfNumberString */



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
    double retVal = 0.0;
    int strEndIndex = BASICINT_valEndOfNumberString(pBasicStr);
    int decimalPlace = 1;
    char ch;
    int i;

    for (i = strEndIndex; i >= 0; i--)
    {
        ch = pBasicStr->data[i];
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

    rounded = ( ((x % 1.0) >= 0.5) ? (int) x + 1 : (int) x );
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

    rounded = ( ((x % 1.0) >= 0.5) ? (int) x + 1 : (int) x );
    sprintf(buffer, "%o", rounded);

    return(__createString(buffer, false));
} /* hex_DC_ */




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
        case VariantType.INTEGER:
            retVal = var->data._integer;
            break;
        case VariantType.LONG:
            retVal = (int) var->data._long;
            break;
        case VariantType.SINGLE:
            retVal = (int) var->data._single;
            break;
        case VariantType.DOUBLE:
            retVal = (int) var->data._double;
            break;
        default:
            __runtimeError(ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __VariantToInt */



long __VariantToLong(PVariant var)
{
    long retVal = 0;

    switch (var->type)
    {
        case VarientType.INTEGER:
            retVal = (long) var->data._integer;
            break;
        case VarientType.LONG:
            retVal = var->data._long;
            break;
        case VarientType.SINGLE:
            retVal = (long) var->data._single;
            break;
        case VarientType.DOUBLE:
            retVal = (long) var->data._double;
            break;
        default:
            __runtimeError(ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __VariantToLong */



float __VariantToFloat(PVariant var)
{
    float retVal = 0;

    switch (var->type)
    {
        case VarientType.INTEGER:
            retVal = (float) var->data._integer;
            break;
        case VarientType.LONG:
            retVal = (float) var->data._long;
            break;
        case VarientType.SINGLE:
            retVal = var->data._single;
            break;
        case VarientType.DOUBLE:
            retVal = (float) var->data._double;
            break;
        default:
            __runtimeError(ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __VariantToFloat */



double __VariantToDouble(PVariant var)
{
    double retVal = 0;

    switch (var->type)
    {
        case VarientType.INTEGER:
            retVal = (double) var->data._integer;
            break;
        case VarientType.LONG:
            retVal = (double) var->data._long;
            break;
        case VarientType.SINGLE:
            retVal = (double) var->data._single;
            break;
        case VarientType.DOUBLE:
            retVal = var->data._double;
            break;
        default:
            __runtimeError(ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
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

    if (pVar->type == VarientType.STRING)
    {
        if (byRef)
            retVal = pVar->data._string;
        else
            retVal = __CloneString(pVar->data._string);
    } // if
    else
        __runtimeError(ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */

    return(retVal);
} /* __VariantToString */


/* end of ConversionFunctions.c ... */


