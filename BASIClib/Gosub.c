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
 *  "Gosub.h"), in a procedure that uses the __GOSUBSUPPORT macro in its
 *  initialization section.
 *
 *      params : ret  == Address of next line of BASIC code.
 *               state == ptr to __gosub structure, created by __GOSUBSUPPORT.
 *     returns : void.
 */
{
    state->count++;
    state->addrs = __memRealloc(state->addrs, sizeof (void *) * state->count);
    state->addrs[state->count - 1] = ret;
} /* __prepareGosub */


void *__prepareReturn(void *addr, __PGosubState state)
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
        __runtimeError(ERR_RETURN_WITHOUT_GOSUB);
    else
    {
        state->count--;
        retVal = ((addr == NULL) ? state->addrs[state->count] : addr);
    } /* else */

    return(retVal);
} /* __prepareReturn */

/* end of Gosub.c ... */
