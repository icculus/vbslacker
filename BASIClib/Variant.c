/*
 * Internal Variant datatype conversions. The Parser/compiler will have
 *  to insert calls to these before passing variant variables to
 *  functions expecting non-variant intrinsics...
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>   /* need NULL definition... */
#include "Variant.h"


void __freeVariant(PVariant var)
/*
 * Free the resources occupied by a variant. The variant (and contained
 *  data) is inaccessible after this call.
 *
 *     params : var == variant to free.
 *    returns : void.
 */
{
/*    if (var->type == STRING)
 *        __freeString(var->data._string);
 */
    __memFree(var);
} /* __freeVariant */


__integer __variantToInt(PVariant var)
{
    __integer retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = var->data._integer;
            break;
        case LONG:
            retVal = (__integer) var->data._long;
            break;
        case SINGLE:
            retVal = (__integer) var->data._single;
            break;
        case DOUBLE:
            retVal = (__integer) var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToInt */



__long __variantToLong(PVariant var)
{
    __long retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = (__long) var->data._integer;
            break;
        case LONG:
            retVal = var->data._long;
            break;
        case SINGLE:
            retVal = (__long) var->data._single;
            break;
        case DOUBLE:
            retVal = (__long) var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToLong */



#warning variantToFloat and not Single?

__single __variantToFloat(PVariant var)
{
    __single retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = (__single) var->data._integer;
            break;
        case LONG:
            retVal = (__single) var->data._long;
            break;
        case SINGLE:
            retVal = var->data._single;
            break;
        case DOUBLE:
            retVal = (__single) var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToFloat */



__double __variantToDouble(PVariant var)
{
    __double retVal = 0;

    switch (var->type)
    {
        case INTEGER:
            retVal = (__double) var->data._integer;
            break;
        case LONG:
            retVal = (__double) var->data._long;
            break;
        case SINGLE:
            retVal = (__double) var->data._single;
            break;
        case DOUBLE:
            retVal = var->data._double;
            break;
        default:
            __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */
            break;
    } /* switch */

    return(retVal);
} /* __variantToDouble */



PBasicString __variantToString(PVariant pVar, __boolean byRef)
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
            retVal = __assignString(retVal, pVar->data._string);
    } /* if */
    else
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* !!! Is that the right error code? */

    return(retVal);
} /* __variantToString */

/* end of Variant.c ... */

