/*
 * This is a skeleton template for adding inline assembly for a
 *  new compiler and platform. Just follow the directions below.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *   This file written by Ryan C. Gordon.
 */

/*
 * 1. Name your new file as such: Asm_[ARCH]_[SYS].h where [ARCH] is
 *     the architecture you are compiling for, like "I386", "ALPHA", "MIPS",
 *     etc...and [SYS] is the development system used for compilations, like
 *     "GNU", or "MSVC".
 * 2. Change all occurances of _INCLUDE_ASM_ARCH_SYS_H_ to your unique
 *     constant that matches your filename.
 * 3. Fill in the macros below with the correct details for your platform.
 * 4. Change the "End of Asm_ARCH_SYS.h ..." comment at the bottom to
 *     reflect your filename.
 * 5. Anything refering to the "base" pointer might be more familiar to
 *     you as a "frame" pointer or "frame address."
 * 6. Make sure the comments at the top above the Copyright notice reflect
 *     your code. Note anything significant.
 * 7. LEAVE THE COPYRIGHT NOTICE ALONE. We own the code. Sorry: we're asses.
 * 8. DO, however, change the "written by" line to credit you.
 * 9. If you make changes, mail them back to vbslacker@hotmail.com for
 *     incorporation in a later version of the program. We'll credit you
 *     for your work.
 */

#ifndef _INCLUDING_LOCAL_ASM_CODE_

#error Please include this file from Assembler.h ONLY!

#else

#ifndef _INCLUDE_ASM_ARCH_SYS_H_
#define _INCLUDE_ASM_ARCH_SYS_H_

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


#endif /* defined _INCLUDE_ASM_ARCH_SYS_H_ */
#endif /* defined _INCLUDING_LOCAL_ASM_CODE__ */

/* end of Asm_ARCH_SYS.h ... */


