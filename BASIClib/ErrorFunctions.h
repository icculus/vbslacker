/*
 * Declarations for BASIC runtime error subsystem.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ERRORFUNCTIONS_H_
#define _INCLUDE_ERRORFUNCTIONS_H_

#include "StdBasic.h"

int vbi_err(void);
void vbpi_error(int newErr);
int vbi_erl(void);
int vbi_erdev(void);
PBasicString vbS_erdev_DC_(void);

#endif

/* end of ErrorFunctions.h ... */


