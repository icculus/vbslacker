/*
 * Header for (n)curses console access.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_REDIRECTEDCONSOLE_H_
#define _INCLUDE_REDIRECTEDCONSOLE_H_

#include "StdBasic.h"

__boolean __initRedirectedConsole(STATEPARAMS);
void    __forceRedirectedConsole(STATEPARAMS);

#endif

/* end of RedirectedConsole.h ... */

