/*
 * Some lowlevel stuff that all vbSlacker code should include.
 * ALL __asm__ STATEMENTS SHOULD BE ISOLATED TO THIS FILE, UNDER THE
 *  CORRECT ARCHITECTURE. This is for abstraction purposes.
 *
 *  This file is automagically included by StdBasic.h and BasicLib.h ...
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_ASSEMBLER_H_
#define _INCLUDE_ASSEMBLER_H_

#if (defined I386)

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
 * Unconditional jump to a specified address, for better or for worse.
 */
#define __jump(addr) __asm__ __volatile__ ("jmpl %0\n\t" \
                                            : /* no output */ \
                                            : "=q" (addr) )

/*
 * This is used by __triggerOnEvent()...adjust the stack, and unconditionally
 *  jump to an address...
 */
#define __setStackAndJump(base, stack, addr) __asm__ __volatile__       \
                                             (                          \
                                                "movl %0, %%ebp \n\t"   \
                                                "movl %1, %%esp \n\t"   \
                                                "jmpl %2        \n\t"   \
                                                    : /* no output */   \
                                                    : "=q" (base)       \
                                                    : "=q" (stack)      \
                                                    : "=q" (addr)       \
                                             ) /* __setStackAndJump */


/*
 * Reset compiler assumptions about optimizations. By telling gcc we fucked
 *  with memory, it believes we've "clobbered" all the registers, and will
 *  reload anything stored in them.
 */
#define __resetAssumptions __asm__ __volatile__ ("\n\t" \
                                                 : /* no output */ \
                                                 : /* no input */  \
                                                 : "memory" )


#define PUSHNULLSTATEARGS   "pushl   $0\n\t" \
                            "pushl   $0\n\t" \
                            "pushl   $0\n\t" \
                            "pushl   $0\n\t"
#define STATEARGSSIZESTR    "16"

#else

#error No ASM written for this platform, yet...

#endif /* ARCHITECTURE == i386 */


/* higher-level abstract stuff... */

/* VBSLACKER STATE */

/*
 * DO NOT REORDER THE FOLLOWING STRUCTURES! Assembly code expects this format.
 *
 *  Adding fields to the bottom should be okay, since the ASM code
 *   references fields by their offsets to the structure's base pointer.
 */

#define STATEPARAMS void *__stIP, void *__stNextIP, void *__stBP, void *__stSP
#define STATEARGS __stIP, __stNextIP, __stBP, __stSP
#define NULLSTATEARGS NULL, NULL, NULL, NULL


/*
 *  __setStateStack fills in the current base pointer and stack
 *   to the current STATEPARAMS...
 */

#define __setStateStack  __getBasePointer(&__stBP); \
                         __getStackPointer(&__stSP)

/*
 *  __setStateInstructs fills in the current instruction pointer and the
 *   "next" instruction pointer to the current STATEPARAMS...
 */
#define __setStateInstructs(ptr1, ptr2)  __stIP = ptr1; \
                                         __stNextIP = ptr2

#endif /* defined _INCLUDE_ASSEMBLER_H_ */
#endif /* defined _INCLUDE_STDBASIC_H_ */

/* end of Assembler.h ... */


