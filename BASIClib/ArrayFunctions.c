/*
 * Some BASIC API calls for manipulation of BASIC arrays.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "ArrayFunctions.h"

#warning write these!
static void __arrayEraseStatic(__PBasicArray array)
/*
 * Handles the ERASE command for dynamic arrays.
 *  Contents or array are set to zero (or blank, whatever).
 *
 *      params : array == Dynamic BASIC array to erase.
 *     returns : void.
 */
{
} /* __arrayEraseStatic */


static void __arrayEraseDynamic(__PBasicArray array)
/*
 * Handles the ERASE command for dynamic arrays.
 *  Array and contents are freed.
 *
 *      params : array == Dynamic BASIC array to erase.
 *     returns : void.
 */
{
} /* __arrayEraseDynamic */


void _vbpAn_erase(__PBasicArray array)
/*
 * BASIC's ERASE command: If specified array is static, set all
 *  elements in array to zero (or equivalent). If specified array
 *  is dynamic, free the array and all its contents.
 *
 *      params : array == array to erase.
 *     returns : void.
 */
{
    if (array->isStatic)
        __arrayEraseStatic(array);
    else
        __arrayEraseDynamic(array);
} /* vbpAn_erase */


static __PBasicArray __getArrayDimension(__PBasicArray array, __long dimension)
/*
 * This function will return the vector that represents the
 *  (dimension) dimension of (array).
 *
 *    params : array == BASIC array to find dimension of.
 *             dimension == which dimension to find.
 *   returns : see above.
 */
{
    __long currentDim;

    if (dimension < 1)
        __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
    else
    {
        for (currentDim = 1; currentDim != dimension; currentDim++)
        {
            if (array->multiDimensional == true)
                array = (__PBasicArray) array->data;
            else
                __runtimeError(ERR_ILLEGAL_FUNCTION_CALL); /* correct? !!! */
        } /* for */
    } /* else */

    return(array);
} /* __getArrayDimension */


__long _vblAl_lbound(__PBasicArray array, __long dimension)
{
    return(__getArrayDimension(array, dimension)->lBound);
} /* _vbiAl_lbound */


__long _vblAl_ubound(__PBasicArray array, __long dimension)
{
    return(__getArrayDimension(array, dimension)->uBound);
} /* _vbiAl_ubound */


/* dim */
/* redim */

/* end of ArrayFunctions.c ... */


