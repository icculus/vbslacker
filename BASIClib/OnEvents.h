/*
 * Header for ON EVENT GOTO support in BASIClib.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_ONEVENTS_H_
#define _INCLUDE_ONEVENTS_H_

#include "StdBasic.h"

#define ONEVTYPE_ONERROR    1

typedef struct
{
    void *thisInstruction;
    void *nextInstruction;
} __OnEventsState;

typedef __OnEventsState *__POnEventsState;


/* function prototypes... */

void __initOnEvents(STATEPARAMS);
void __deinitOnEvents(STATEPARAMS);
void __initThreadOnEvents(STATEPARAMS, int tidx);
void __deinitThreadOnEvents(STATEPARAMS, int tidx);

void __doResume(STATEPARAMS);
void __triggerOnEvent(STATEPARAMS, __integer evType);
void __deregisterOnEventHandlers(STATEPARAMS);
void __registerOnEventHandler(STATEPARAMS, void *base, void *stack,
                              void *handlerAddr, __integer evType);


#define __ONEVENTSSUPPORT   __OnEventsState __onEvents = {NULL, NULL}

#define __resumeNext        __doResume(STATEARGS); \
                            __jump(__onEvents.nextInstruction)

#define __resumeZero        __doResume(STATEARGS); \
                            __jump(__onEvents.thisInstruction)

#define __resumeLabel(addr) __doResume(STATEARGS); \
                            __jump(addr)

#endif /* _INCLUDE_ONEVENTS_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of OnEvents.h ... */

