/*
 * Declarations for BASIC runtime error subsystem.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ERRORFUNCTIONS_H_
#define _INCLUDE_ERRORFUNCTIONS_H_

#include "StdBasic.h"

int vbi_err(STATEPARAMS);
void vbpi_error(STATEPARAMS, int newErr);
int vbi_erl(STATEPARAMS);
int vbi_erdev(STATEPARAMS);
PBasicString vbS_erdev_DC_(STATEPARAMS);

#endif

/* end of ErrorFunctions.h ... */


