/*
 * Header file for Environment-interfacing APIs in BASIClib.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ENVRFUNCTIONS_H_
#define _INCLUDE_ENVRFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

void __initEnvrFunctions(int argc, char **argv);
void __deinitEnvrFunctions(void);
PBasicString _vbSS_environ_DC_(PBasicString envVarName);
PBasicString _vbSf_environ_DC_(__single n);
void _vbpS_environ(PBasicString newEnvrStr);
__integer _vbiS_fre(PBasicString strExp);
PBasicString _vbS_command_DC_(void);

#ifdef __cplusplus
}
#endif

#endif  /* _INCLUDE_ENVRFUNCTIONS_H_ */

/* end of EnvrFunctions.h ... */

