/*
 * Header for ON EVENT GOTO support in BASIClib.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ONEVENTS_H_
#define _INCLUDE_ONEVENTS_H_

#include "Threads.h"
#include "RegState.h"

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



/* God, inline assembly is SCARY lookin' in gcc, isn't it? */

/*
 *  Get the stack pointer, and store it in *retVal.
 */
#define __getStackPointer(retVal) __asm__ __volatile__ ("movl %%esp, %0\n\t" \
                                                        : "=q" (*retVal) )

/*
 *  Get the base pointer, and store it in *retVal.
 */
#define __getBasePointer(retVal) __asm__ __volatile__ ("movl %%ebp, %0\n\t" \
                                                        : "=q" (*retVal) )
/*
 * Reset compiler assumptions about optimizations. By telling gcc we fucked
 *  with memory, it believes we've "clobbered" all the registers, and will
 *  reload anything stored in them.
 */
#define __resetAssumptions __asm__ __volatile__ ("\n\t" \
                                                 : /* no output */ \
                                                 : /* no input */  \
                                                 : "memory" );


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
void __deregisterAllOnEventHandlers(STATEPARAMS);
void __deregisterOnEventHandlers(STATEPARAMS);
void __registerOnEventHandler(STATEPARAMS, void *handlerAddr,
                              OnEventTypeEnum evType);
void __triggerOnEvent(STATEPARAMS, POnEventHandler pHandler,
                      OnEventTypeEnum evType);

#endif

/* end of OnEvents.h ... */

