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

#ifdef __cplusplus
extern "C" {
#endif

#define INITFLAG_NO_FLAGS              0
#define INITFLAG_NOT_INITIALIZED       0
#define INITFLAG_DISABLE_RANGE_CHECKS  1
#define INITFLAG_DISABLE_NUM_OVERFLOW  2
#define INITFLAG_DISABLE_VARIANTS      4
#define INITFLAG_DISABLE_CONSOLE       8
#define INITFLAG_WINDOWS_FILESYSTEM    16
#define INITFLAG_ENABLE_GUIFRONTEND    32
#define INITFLAG_FILENAMES_IGNORECASE  64

    /* function prototypes ... */
char *__getUserAppDir(void);
void __initBasicLib(__long flags, int argc, char **argv, char **envp);
void __deinitBasicLib(void);
__long __getInitFlags();
void __shellOutNotification();
void __shellRetNotification();

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_INITIALIZE_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of Initialize.h ... */

