/*
 * This is the inline assembly macros for Microsoft Visual C++
 *  compilation on Intel 386-compatible chipsets...
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


#ifndef _INCLUDE_ASM_I386_MSVC_H_
#define _INCLUDE_ASM_I386_MSVC_H_

        /*
         *  Get the stack pointer, and store it in (retVal).
         */
    #define __getStackPointer(retVal)             __asm                     \
                                                  {                         \
                                                    __asm mov retVal, esp   \
                                                  }

        /*
         *  Get the base pointer, and store it in (retVal).
         */
    #define __getBasePointer(retVal)              __asm                     \
                                                  {                         \
                                                    __asm mov retVal, ebp   \
                                                  }


        /*
         * Unconditional jump to a specified address, (addr), for
         *  better or for worse.
         */
    #define __jump(addr)                          __asm                     \
                                                  {                         \
                                                    __asm jmp addr          \
                                                  }

        /*
         * Unconditional jump to a specified line label, (label), for
         *  better or for worse. This label must be specified with
         *  __insertLineLabel, or results are really undefined.
         */
    #define __jumpLabel(label)                    __asm                     \
                                                  {                         \
                                                    __asm jmp label         \
                                                  }


        /*
         * This is used by __triggerOnError()...adjust the
         *  stack and base pointers to (stack) and (base), and
         *  unconditionally jump to (addr)...
         */
    #define __setStackAndJump(base, stack, addr)  __asm                     \
                                                  {                         \
                                                    __asm mov ebp, base     \
                                                    __asm mov esp, stack    \
                                                    __asm jmp addr          \
                                                  }

        /*
         * Insert the linelabel (label) at the current point in the
         *  code. Since (outside of GNU C) there's no way to get the
         *  address of a C line label, but we need it to __jump() to
         *  specified addresses, we need to implement this in assembly,
         *  where line label addresses are commonplace.
         */
    #define __insertLineLabel(label)              __asm                     \
                                                  {                         \
                                                    __asm label:            \
                                                  }

        /*
         * Get the address in memory of a line label specified by
         *  __insertLineLabel, and store it in (retVal).
         */
    #define __getLabelAddr(label, retVal)         __asm                     \
                                                  {                         \
                                                    __asm mov retVal, label \
                                                  }

#endif /* defined _INCLUDE_I386_MSVC_H_ */
#endif /* defined _INCLUDING_LOCAL_ASM_CODE__ */

/* end of Asm_I386_MSVC.h ... */


