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

/* !!! Make __setResumeStack and __setResumeInstructs assembly? */

/*
 *  __setResumeStack fills in the current base pointer and stack
 *   to the current STATEPARAMS...
 */
#define __setResumeStack __getBasePointer(&__stBP); \
                         __getStackPointer(&__stSP);

/*
 *  __setResumeInstructs fills in the current instruction pointer and the
 *   "next" instruction pointer to the current STATEPARAMS...
 */
#define __setResumeInstructs(ptr1, ptr2)  __stIP = ptr1; \
                                          __stNextIP = ptr2;

#define PUSHNULLSTATEARGS   "pushl   $0\n\t" \
                            "pushl   $0\n\t" \
                            "pushl   $0\n\t" \
                            "pushl   $0\n\t"
#define STATEARGSSIZESTR    "16"

#endif
#endif

/* end of RegState.h ... */


