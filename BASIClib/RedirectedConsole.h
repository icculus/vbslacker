/*
 * Header for redirected (to stdout) console access.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_REDIRECTEDCONSOLE_H_
#define _INCLUDE_REDIRECTEDCONSOLE_H_

#include "StdBasic.h"

__boolean __initRedirectedConsole(void);
void    __forceRedirectedConsole(void);

#endif /* _INCLUDE_REDIRECTEDCONSOLE_H_ */

/* end of RedirectedConsole.h ... */

