/*
 * Declares needed for GOSUB and RETURN support in BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_GOSUB_H_
#define _INCLUDE_GOSUB_H_

typedef struct
{
    void **addrs;
    int count;
} __GosubState;

typedef __GosubState *__PGosubState;

void __prepareGosub(STATEPARAMS, void *ret, PGosubState state);
void *__prepareReturn(STATEPARAMS, void *addr, PGosubState state);
/* __jump() is a macro defined in RegState.h ... */


#define __GOSUBSUPPORT __GosubState __gosub = {NULL, 0}

#define __doGosub(addr, ret) __prepareGosub(STATEARGS, ret, &__gosub); \
                             __jump(addr)

#define __doReturn(addr) __jump(__prepareReturn(STATEARGS, addr, &__gosub))

#endif /* defined _INCLUDE_GOSUB_H_ */
#endif /* defined _INCLUDE_STDBASIC_H_ */

/* end of Gosub.h ... */
