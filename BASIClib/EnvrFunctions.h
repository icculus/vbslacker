/*
 * Header file for Environment-interfacing APIs in BASIClib.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ENVRFUNCTIONS_H_
#define _INCLUDE_ENVRFUNCTIONS_H_

#include "StdBasic.h"

void __initEnvrFunctions(STATEPARAMS, int argc, char **argv);
void __deinitEnvrFunctions(STATEPARAMS);
void vbpS_chdir(STATEPARAMS, PBasicString newDir);
PBasicString vbSS_curdir_DC_(STATEPARAMS, PBasicString drive);
PBasicString vbS_curdir_DC_(STATEPARAMS);
PBasicString vbSS_environ_DC_(STATEPARAMS, PBasicString envVarName);
PBasicString vbSi_environ_DC_(STATEPARAMS, int n);
void vbpS_environ(STATEPARAMS, PBasicString newEnvrStr);
int vbii_fre(STATEPARAMS, int arg);
int vbiS_fre(STATEPARAMS, PBasicString strExp);
PBasicString vbS_command_DC_(STATEPARAMS);

#endif

/* end of EnvrFunctions.h ... */

