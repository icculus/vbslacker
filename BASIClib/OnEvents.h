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

/* 
 * DO NOT REORDER THE FOLLOWING STRUCTURE! Assembly code expects this format.
 *
 *  Adding fields to the bottom should be okay, since the ASM code
 *   references fields by their offsets to the structure's base pointer.
 */
typedef struct
{
    void *handlerAddr;
    void *stackStart;
    void *stackEnd;
    void *origReturnAddr;
} OnEventHandler;

typedef OnEventHandler *POnEventHandler;


/* !!! ...hope I'm doing this right...
 * God, inline assembly is SCARY lookin' in gcc, isn't it?
 *
 *  Get the stack pointer, and store it in *retVal.
 */
#define __getStackPointer(retVal) __asm__ __volatile__ ("movl %%esp, %0\n\t" \
                                                        : "=q" (*retVal) )

POnEventHandler __getOnEventHandler(OnEventTypeEnum evType);
void __initOnEvents();
void __registerOnEventHandler(void *handlerAddr, void *stackSize,
                              void *stackEnd, void *origReturnAddr,
                              OnEventTypeEnum evType);

void __deregisterOnEventHandler(void *handlerAddr, OnEventTypeEnum evType);

void __callOnEventHandler(POnEventHandler pHandler);

#endif

/* end of OnEvents.h ... */


