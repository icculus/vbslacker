/*
 * Everything that didn't fit somewhere else...
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "MiscFunctions.h"

void vbp_clear(void)
/*
 * There's no way in hell we're supporting this. Microsoft docs
 *  says that if it's used anywhere but the mainline, it throws
 *  an ILLEGAL FUNCTION CALL, so we'll just throw that anyhow.
 *  Later VBs don't even HAVE this call. Sheesh.
 *
 *     params : void.
 *    returns : Always throws error.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* vbp_clear */


void vbpNNl_clear(__long stackSize)
/*
 * Refer to vbp_clear()...
 *
 *     params : void.
 *    returns : Always throws error.
 */
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* vbNNl_clear */

/* end of MiscFunctions.c ... */


