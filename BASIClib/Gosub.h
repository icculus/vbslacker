/*
 * Declares needed for GOSUB and RETURN support in BASIClib.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_GOSUB_H_
#define _INCLUDE_GOSUB_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void **addrs;
    __integer count;
} __GosubState;

typedef __GosubState *__PGosubState;

void __prepareGosub(void *ret, __PGosubState state);
void *__prepareReturn(__PGosubState state);

/* __jump() and __jumpLabel() are macros defined in Assembler.h ... */

#define __GOSUBVARS __GosubState __gosub = {NULL, 0}

#define __doGosub(label, ret)  __prepareGosub(ret, &__gosub); \
                               __jumpLabel(addr)

#define __doReturn()           __jump(__prepareReturn(&__gosub));

#define __doReturnLabel(label) __prepareReturn(&__gosub); \
                               __jumpLabel(label);

#ifdef __cplusplus
}
#endif

#endif /* defined _INCLUDE_GOSUB_H_ */
#endif /* defined _INCLUDE_STDBASIC_H_ */

/* end of Gosub.h ... */
