/*
 * Header for redirected (to stdout) console access.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_REDIRECTEDCONSOLE_H_
#define _INCLUDE_REDIRECTEDCONSOLE_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

__boolean __initRedirectedConsole(void);
void    __forceRedirectedConsole(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_REDIRECTEDCONSOLE_H_ */

/* end of RedirectedConsole.h ... */

