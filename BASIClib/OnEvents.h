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

typedef enum
{
    ONERROR,
    ONTIMER,
    ONCOM,
    TOTAL
} OnEventTypeEnum;

typedef OnEventTypeEnum *POnEventTypeEnum;

/*
 * DO NOT REORDER THE FOLLOWING STRUCTURES! Assembly code expects this format.
 *
 *  Adding fields to the bottom should be okay, since the ASM code
 *   references fields by their offsets to the structure's base pointer.
 */

typedef struct
{
    void *handlerAddr;
    void *basePtr;
    void *stackPtr;
    OnEventTypeEnum evType;
} OnEventHandler;

typedef OnEventHandler *POnEventHandler;

/* function prototypes... */

POnEventHandler __getOnEventHandler(STATEPARAMS, OnEventTypeEnum evType);
unsigned long __getOnEventsRecursionCount(STATEPARAMS);
void __initOnEvents(STATEPARAMS);
void __deinitOnEvents(STATEPARAMS);
void __initThreadOnEvents(STATEPARAMS, int tidx);
void __deinitThreadOnEvents(STATEPARAMS, int tidx);
void __resumeNext(STATEPARAMS);
void __resumeZero(STATEPARAMS);
void __triggerOnEventByType(STATEPARAMS, OnEventTypeEnum evType);
void __deregisterOnEventHandlers(STATEPARAMS);
void __registerOnEventHandler(STATEPARAMS, void *handlerAddr,
                              OnEventTypeEnum evType);
void __triggerOnEvent(STATEPARAMS, POnEventHandler pHandler,
                      OnEventTypeEnum evType);

#endif /* _INCLUDE_ONEVENTS_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of OnEvents.h ... */

