/*
 * Everything that didn't fit somewhere else...
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "MiscFunctions.h"

void _vbp_clear(void)
/*
 * There's no way in hell we're supporting this. Microsoft docs
 *  say that if it's used anywhere but the mainline, it throws
 *  an ILLEGAL FUNCTION CALL, so we'll just throw that anyhow.
 *  Later VBs don't even HAVE this call. Sheesh.
 *
 *     params : void.
 *    returns : Always throws error.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* _vbp_clear */


void _vbpNNl_clear(__attribute__ ((unused)) __long stackSize)
/*
 * Refer to vbp_clear()...
 *
 *     params : void.
 *    returns : Always throws error.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* _vbNNl_clear */

/* end of MiscFunctions.c ... */


