/*
 * Header for Process-control BASIClib API.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_PROCESSFUNCTIONS_H_
#define _INCLUDE_PROCESSFUNCTIONS_H_

#include "StdBasic.h"

void vbpS_shell(PBasicString shCmd);
void vbp_end(void);
void vbp_sleep(void);
void vbpl_sleep(long napTime);

#define vbp_system vbp_end
#define vbp_stop   vbp_end

#endif /* _INCLUDE_PROCESSFUNCTIONS_H_ */

/* end of ProcessFunctions.h ... */

