/*
 * Some lowlevel stuff that all vbSlacker code should include.
 *
 * ALL ASSEMBLY LANGUAGE STATEMENTS SHOULD BE ISOLATED TO THIS FILE,
 *  UNDER THE CORRECT ARCHITECTURE. This is for abstraction purposes.
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

    /*
     * God, inline asm is SCARY lookin' in gcc, isn't it?
     */
#if (defined __GNUC__)

    #if (defined I386)

            /*
             *  Get the stack pointer, and store it in *retVal.
             */
        #define __getStackPointer(retVal) __asm__ __volatile__              \
                                                  (                         \
                                                    "movl %%esp, %0\n\t"    \
                                                        : "=q" (*retVal)    \
                                                  )

            /*
             *  Get the base pointer, and store it in *retVal.
             */
        #define __getBasePointer(retVal) __asm__ __volatile__               \
                                                  (                         \
                                                    "movl %%ebp, %0\n\t"    \
                                                        : "=q" (*retVal)    \
                                                  )


            /*
             * Unconditional jump to a specified address, for
             *  better or for worse.
             */
        #define __jump(addr) __asm__ __volatile__ (                         \
                                                    "jmpl *%0\n\t"          \
                                                        :                   \
                                                        : "q" (addr)        \
                                                  )

            /*
             * This is used by __triggerOnError()...adjust the
             *  stack, and unconditionally jump to an address...
             */
        #define __setStackAndJump(base, stack, addr) __asm__ __volatile__   \
                                                  (                         \
                                                    "movl %0, %%ebp \n\t"   \
                                                    "movl %1, %%esp \n\t"   \
                                                    "jmpl *%2       \n\t"   \
                                                        :                   \
                                                        : "q" (base),       \
                                                          "q" (stack),      \
                                                          "q" (addr)        \
                                                  )


    #else
        #error No ASM written for this platform, yet...
    #endif /* defined (architecture) */

#else
    #error No idea how to use inline assembly on your compiler.
#endif /* defined (compiler system) */


#endif /* defined _INCLUDE_ASSEMBLER_H_ */
#endif /* defined _INCLUDE_STDBASIC_H_ */

/* end of Assembler.h ... */


