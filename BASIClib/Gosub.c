/*
 * Functions needed for GOSUB and RETURN support in BASIClib.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "Gosub.h"

void __prepareGosub(void *ret, __PGosubState state)
/*
 * This is called to prepare a vbSlacker application to perform a GOSUB.
 *  GOSUBs should only be executed via the __doGosub macro (found in
 *  "Gosub.h"), in a procedure that uses the __GOSUBVARS macro in its
 *  initialization section.
 *
 *      params : ret  == Address of next line of BASIC code.
 *               state == ptr to __gosub structure, created by __GOSUBVARS.
 *     returns : void.
 */
{
    state->count++;
    state->addrs = __memRealloc(state->addrs, sizeof (void *) * state->count);
    state->addrs[state->count - 1] = ret;
} /* __prepareGosub */


void *__prepareReturn(__PGosubState state)
/*
 * This is called to prepare a vbSlacker application to perform a RETURN.
 *  RETURNs should only be executed via the __doReturn or __doReturnLabel
 *  macros (found in "Gosub.h"), in a procedure that uses the __GOSUBVARS
 *  macro in its initialization section.
 *
 *      params : state == ptr to __gosub structure, created by __GOSUBVARS.
 *     returns : Address that RETURN 0 statements should jump to.
 */
{
    void *retVal = NULL;

    if (state->count <= 0)
        __runtimeError(ERR_RETURN_WITHOUT_GOSUB);
    else
    {
        state->count--;
        retVal = (state->addrs[state->count]);
    } /* else */

    return(retVal);
} /* __prepareReturn */

/* end of Gosub.c ... */
