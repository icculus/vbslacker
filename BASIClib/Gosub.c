/*
 * Declares needed for GOSUB and RETURN support in BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "Gosub.h"

void __prepareGosub(STATEPARAMS, void *ret, PGosubState state)
/*
 * This is called to prepare a vbSlacker application to perform a GOSUB.
 *  GOSUBs should only be executed via the __doGosub macro (found in
 *  "Gosub.h"), in a procedure that uses the __GOSUBSUPPORT macro in its
 *  initialization section.
 *
 *      params : addr == address that code will jump to for GOSUB.
 *               ret  == Address of next line of BASIC code.
 *               state == ptr to __gosub structure, created by __GOSUBSUPPORT.
 *     returns : void.
 */
{
    state->count++;

    /* !!! memory leak! */
    state->addrs = __memRealloc(STATEARGS, state->addrs,
                                sizeof (void *) * state->count);

    state->addrs[state->count - 1] = ret;
} /* __prepareGosub */


void *__prepareReturn(STATEPARAMS, void *addr, PGosubState state)
/*
 * This is called to prepare a vbSlacker application to perform a RETURN.
 *  RETURNs should only be executed via the __doReturn macro (found in
 *  "Gosub.h"), in a procedure that uses the __GOSUBSUPPORT macro in its
 *  initialization section.
 *
 *      params : addr == address that code will jump to for RETURN. NULL
 *                       returns to the address supplied by the GOSUB at the
 *                       top of the stack for its "ret" parameter.
 *               state == ptr to __gosub structure, created by __GOSUBSUPPORT.
 *     returns : void.
 */
{
    void *retVal = NULL;

    if (state->count <= 0)
        __runtimeError(STATEARGS, ERR_RETURN_WITHOUT_GOSUB);
    else
    {
        state->count--;
        retVal = ((addr == NULL) ? state->addrs[count] : addr);
    } /* else */

    return(retVal);
} /* __prepareReturn */

/* end of Gosub.c ... */