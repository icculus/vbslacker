/*
 * This is the inline assembly macros for GNU C++ compilation on
 *  Intel 386-compatible chipsets...
 *
 * Please see asm_skeleton.h for instructions on modifying this
 *  file or defining support for a new platform.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *   This file written by Ryan C. Gordon.
 */

#ifndef _INCLUDING_LOCAL_ASM_CODE_

#error Please include this file from Assembler.h ONLY!

#else


    /* God, inline assembly in GNU C is SCARY lookin', ain't it? */


#ifndef _INCLUDE_ASM_I386_GNU_H_
#define _INCLUDE_ASM_I386_GNU_H_

        /*
         *  Get the stack pointer, and store it in (retVal).
         */
    #define __getStackPointer(retVal)             __asm__ __volatile__      \
                                                  (                         \
                                                    "movl %%esp, %0\n\t"    \
                                                        : "=q" (retVal)     \
                                                  )

        /*
         *  Get the base pointer, and store it in (retVal).
         */
    #define __getBasePointer(retVal)              __asm__ __volatile__      \
                                                  (                         \
                                                    "movl %%ebp, %0\n\t"    \
                                                        : "=q" (retVal)     \
                                                  )


        /*
         * Unconditional jump to a specified address, (addr), for
         *  better or for worse.
         */
    #define __jump(addr)                          __asm__ __volatile__      \
                                                  (                         \
                                                    "jmpl *%0\n\t"          \
                                                        :                   \
                                                        : "q" (addr)        \
                                                  )

        /*
         * This is used by __triggerOnError()...adjust the
         *  stack and base pointers to (stack) and (base), and
         *  unconditionally jump to (addr)...
         */
    #define __setStackAndJump(base, stack, addr)  __asm__ __volatile__      \
                                                  (                         \
                                                    "movl %0, %%ebp \n\t"   \
                                                    "movl %1, %%esp \n\t"   \
                                                    "jmpl *%2       \n\t"   \
                                                        :                   \
                                                        : "q" (base),       \
                                                          "q" (stack),      \
                                                          "q" (addr)        \
                                                  )

        /*
         * Insert the linelabel (label) at the current point in the
         *  code. Since (outside of GNU C) there's no way to get the
         *  address of a C line label, but we need it to __jump() to
         *  specified addresses, we need to implement this in assembly,
         *  where line label addresses are commonplace.
         */
    #define __insertLineLabel(label)              __asm__ __volatile__      \
                                                  (                         \
                                                    #label ":\n\t"          \
                                                  )
        /*
         * Get the address in memory of a line label specified by
         *  __insertLineLabel, and store it in (retVal).
         */
    #define __getLabelAddr(label, retVal)         __asm__ __volatile__      \
                                                  (                         \
                                                    "movl " #label ",%0\n\t"\
                                                        : "=q" (retVal)     \
                                                  )

#endif /* defined _INCLUDE_I386_GNU_H_ */
#endif /* defined _INCLUDING_LOCAL_ASM_CODE__ */

/* end of Asm_I386_GNU.h ... */


