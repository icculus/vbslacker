/*
 * BASIClib initialization declarations.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_INITIALIZE_H_
#define _INCLUDE_INITIALIZE_H_

#define INITFLAG_NO_FLAGS              0
#define INITFLAG_NOT_INITIALIZED       0
#define INITFLAG_DISABLE_RANGE_CHECKS  1
#define INITFLAG_DISABLE_NUM_OVERFLOW  2
#define INITFLAG_DISABLE_VARIANTS      4
#define INITFLAG_DISABLE_CONSOLE       8
#define INITFLAG_UNIX_FILE_SYSTEM      16

    /* function prototypes ... */
void __initBasicLib(long flags, int argc, char **argv);
void __deinitBasicLib(void);
long __getInitFlags();
void __shellOutNotification();
void __shellRetNotification();

#endif /* _INCLUDE_INITIALIZE_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of Initialize.h ... */

