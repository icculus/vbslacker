/*
 * Header for Process-control BASIClib API.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_PROCESSFUNCTIONS_H_
#define _INCLUDE_PROCESSFUNCTIONS_H_

#include "StdBasic.h"

void vbpS_shell(STATEPARAMS, PBasicString shCmd);
void vbp_system(STATEPARAMS);
void vbp_end(STATEPARAMS);

#endif

/* end of ProcessFunctions.h ... */

