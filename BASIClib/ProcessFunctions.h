/*
 * Header for Process-control BASIClib API.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_PROCESSFUNCTIONS_H_
#define _INCLUDE_PROCESSFUNCTIONS_H_

#include "StdBasic.h"

void vbpS_shell(STATEPARAMS, PBasicString shCmd);
void vbp_end(STATEPARAMS);
void vbp_sleep(STATEPARAMS, long napTime);
void vbpl_sleep(STATEPARAMS, long napTime);

#define vbp_system(stateargs) vbp_end(stateargs)
#define vbp_stop(stateargs) vbp_end(stateargs)

#endif

/* end of ProcessFunctions.h ... */

