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
    void *basePtr;
} OnEventHandler;

typedef OnEventHandler *POnEventHandler;


extern void *_stack_ptr_;
extern void *_base_ptr_;


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
 * Insert this right AFTER a line label that serves as the start of an
 *  ON EVENT handler.
 *
 * The i386 version guarantees that optimizations aren't saving any
 *  important data in the registers. By telling gcc we fucked with memory, it
 *  believes we've "clobbered" all the registers, and will reload anything
 *  stored in them.
 */
#define __markOnEventHandlerAddr __asm__ __volatile__ ("nop\n\t" : \
                                                        : : "memory" );

POnEventHandler __getOnEventHandler(OnEventTypeEnum evType);
void __initThreadOnEvents(int tidx);
void __deinitThreadOnEvents(int tidx);
void __registerOnEventHandler(void *handlerAddr, void *stackSize,
                              void *stackEnd, void *origReturnAddr,
                              void *basePtr, OnEventTypeEnum evType);
void __deregisterOnEventHandler(void *handlerAddr, OnEventTypeEnum evType);
void __triggerOnEvent(OnEventTypeEnum evType);


#endif

/* end of OnEvents.h ... */

