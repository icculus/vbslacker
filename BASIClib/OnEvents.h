/*
 * Header for ON EVENT GOTO support in BASIClib.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ONEVENTS_H_
#define _INCLUDE_ONEVENTS_H_

typedef enum
{
    ONERROR,
    ONTIMER,
    ONCOM,
    TOTAL
} OnEventTypeEnum;

typedef OnEventTypeEnum *POnEventTypeEnum;

/* DO NOT REORDER THIS STRUCTURE! Assembly code expects this format. */
typedef struct
{
    void *handlerAddr;
    void *stackStart;
    void *stackEnd;
} OnEventHandler;

typedef OnEventHandler *POnEventHandler;

POnEventHandler __getOnEventHandler(OnEventTypeEnum evType);
void __initOnEvents();
void __registerOnEventHandler(void *handlerAddr, void *stackSize,
                              void *stackEnd, OnEventTypeEnum evType);

void __deregisterOnEventHandler(void *handlerAddr, OnEventTypeEnum evType);

void __callOnEventHandler(POnEventHandler pHandler);

#endif

/* end of OnEvents.h ... */


