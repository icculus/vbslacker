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
void vbpS_chdir(PBasicString newDir);
PBasicString vbSS_curdir_DC_(PBasicString drive);
PBasicString vbS_curdir_DC_(void);
PBasicString vbSS_environ_DC_(PBasicString envVarName);
PBasicString vbSi_environ_DC_(int n);
void vbpS_environ(PBasicString newEnvrStr);
int vbii_fre(int arg);
int vbiS_fre(PBasicString strExp);
PBasicString vbS_command_DC_(void);

#ifdef __cplusplus
}
#endif

#endif  /* _INCLUDE_ENVRFUNCTIONS_H_ */

/* end of EnvrFunctions.h ... */

