/*
 * Header for Process-control BASIClib API.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_PROCESSFUNCTIONS_H_
#define _INCLUDE_PROCESSFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

void _vbpS_shell(PBasicString shCmd);
void _vbp_end(void);
void _vbp_sleep(void);
void _vbpl_sleep(__long napTime);

#define _vbp_system _vbp_end
#define _vbp_stop   _vbp_end

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_PROCESSFUNCTIONS_H_ */

/* end of ProcessFunctions.h ... */

