/*
 * Some BASIC API calls for manipulation of BASIC arrays.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

static void __arrayEraseStatic(STATEPARAMS, PBasicArray array)
/*
 * Handles the ERASE command for dynamic arrays.
 *  Contents or array are set to zero (or blank, whatever).
 *
 *      params : array == Dynamic BASIC array to erase.
 *     returns : void.
 */
{
} /* __arrayEraseStatic */


static void __arrayEraseDynamic(STATEPARAMS, PBasicArray array)
/*
 * Handles the ERASE command for dynamic arrays.
 *  Array and contents are freed.
 *
 *      params : array == Dynamic BASIC array to erase.
 *     returns : void.
 */
{
} /* __arrayEraseDynamic */


void vbpAn_erase(STATEPARAMS, PBasicArray array)
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
        __arrayEraseStatic(STATEARGS, array);
    else
        __arrayEraseDynamic(STATEARGS, array);
} /* vbpAn_erase */


static PBasicArray __getArrayDimension(STATEPARAMS, 
                                       PBasicArray array, 
                                       int dimension)
/*
 * This function will return the vector that represents the
 *  (dimension) dimension of (array).
 *
 *    params : array == BASIC array to find dimension of.
 *             dimension == which dimension to find.
 *   returns : see above.
 */
{
    int currentDim;

    if (dimension < 1)
        __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL);
    else    
    {
        for (currentDim = 1; currentDim != dimension; currentDim++)
        {
            if (array->isMultidimensional == true)
                array = (PBasicArray) array->data;
            else
                __runtimeError(STATEARGS, ERR_ILLEGAL_FUNCTION_CALL); /* correct? !!! */
        } /* for */
    } /* else */
    
    return(array);
} /* __getArrayDimension */


int vbiAi_lbound(STATEPARAMS, PBasicArray array, int dimension)
{
    return(__getArrayDimension(STATEARGS, array, dimension)->lBound);
} /* vbiAi_lbound */


int vbiAi_ubound(STATEPARAMS, PBasicArray array, int dimension)
{
    return(__getArrayDimension(STATEARGS, array, dimension)->uBound);
} /* vbiAi_ubound */


/* dim */
/* redim */

/* end of ArrayFunctions.c ... */


