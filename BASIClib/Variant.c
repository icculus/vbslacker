/*
 * Internal Variant datatype conversions. The Parser/compiler will have
 *  to insert calls to these before passing variant variables to
 *  functions expecting non-variant intrinsics...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "Variant.h"

int __variantToInt(STATEPARAMS, PVariant var)
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
            __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToInt */



long __variantToLong(STATEPARAMS, PVariant var)
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
            __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToLong */



float __variantToFloat(STATEPARAMS, PVariant var)
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
            __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToFloat */



double __variantToDouble(STATEPARAMS, PVariant var)
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
            __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToDouble */



PBasicString __variantToString(STATEPARAMS, PVariant pVar, boolean byRef)
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
            __assignString(STATEARGS, &retVal, pVar->data._string);
    } /* if */
    else
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */

    return(retVal);
} /* __variantToString */

/* end of Variant.c ... */

