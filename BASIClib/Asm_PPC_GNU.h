/*
 * This is the inline assembly macros for GNU C/C++ compilation on
 *  PowerPC chipsets...
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *   This file written by Ryan C. Gordon.
 */

#ifndef _INCLUDING_LOCAL_ASM_CODE_

#error Please include this file from Assembler.h ONLY!

#else

#ifndef _INCLUDE_ASM_PPC_GNU_H_
#define _INCLUDE_ASM_PPC_GNU_H_

#error not filled in, yet.

        /*
         * Get the stack pointer, and store it in (retVal).
         */
    #define __getStackPointer(retVal)            /* YOUR DEFINITION HERE... */

        /*
         * Get the base pointer, and store it in (retVal).
         */
    #define __getBasePointer(retVal)             /* YOUR DEFINITION HERE... */

        /*
         * Unconditional jump to a specified address, (addr), for
         *  better or for worse.
         */
    #define __jump(addr)                         /* YOUR DEFINITION HERE... */

        /*
         * Unconditional jump to a specified line label, (label), for
         *  better or for worse. This label must be specified with
         *  __insertLineLabel, or results are really undefined.
         */
    #define __jumpLabel(label)                   /* YOUR DEFINITION HERE... */

        /*
         * This is used by __triggerOnError()...adjust the
         *  stack and base pointers to (stack) and (base), and
         *  unconditionally jump to (addr)...
         */
    #define __setStackAndJump(base, stack, addr) /* YOUR DEFINITION HERE... */

        /*
         * Insert the linelabel (label) at the current point in the
         *  code. Since (outside of GNU C) there's no way to get the
         *  address of a C line label, but we need it to __jump() to
         *  specified addresses, we need to implement this in assembly,
         *  where line label addresses are commonplace.
         */
    #define __insertLineLabel(label)             /* YOUR DEFINITION HERE... */

        /*
         * Get the address in memory of a line label specified by
         *  __insertLineLabel, and store it in (retVal).
         */
    #define __getLabelAddr(label, retVal)        /* YOUR DEFINITION HERE... */


#endif /* defined _INCLUDE_ASM_PPC_GNU_H_ */
#endif /* defined _INCLUDING_LOCAL_ASM_CODE__ */

/* end of Asm_PPC_GNU.h ... */


