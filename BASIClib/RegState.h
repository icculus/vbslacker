/*
 * Some lowlevel stuff that all vbSlacker code should include.
 *
 *  This file is automagically included by StdBasic.h and BasicLib.h ...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_REGSTATE_H_
#define _INCLUDE_REGSTATE_H_

/*
 * DO NOT REORDER THE FOLLOWING STRUCTURES! Assembly code expects this format.
 *
 *  Adding fields to the bottom should be okay, since the ASM code
 *   references fields by their offsets to the structure's base pointer.
 */

#define STATEPARAMS void *__stIP, void *__stNextIP, void *__stBP, void *__stSP
#define STATEARGS __stIP, __stNextIP, __stBP, __stSP
#define NULLSTATEARGS NULL, NULL, NULL, NULL


#if (ARCHITECTURE == "i386")

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




#define PUSHNULLSTATEARGS   "pushl   $0\n\t" \
                            "pushl   $0\n\t" \
                            "pushl   $0\n\t" \
                            "pushl   $0\n\t"
#define STATEARGSSIZESTR    "16"

#else

#error No RegState ASM written for this platform, yet...

#endif /* ARCHITECTURE == i386 */


/*
 *  __setResumeStack fills in the current base pointer and stack
 *   to the current STATEPARAMS...
 */
#define __setStateStack  __getBasePointer(&__stBP); \
                         __getStackPointer(&__stSP);

/*
 *  __setResumeInstructs fills in the current instruction pointer and the
 *   "next" instruction pointer to the current STATEPARAMS...
 */
#define __setStateInstructs(ptr1, ptr2)  __stIP = ptr1; \
                                         __stNextIP = ptr2;

#endif /* defined _INCLUDE_REGSTATE_H_ */
#endif /* defined _INCLUDE_STDBASIC_H_ */

/* end of RegState.h ... */


