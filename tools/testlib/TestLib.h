/*
 * Prototypes from various BASIClib testing subsystems.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_TESTLIB_H_
#define _INCLUDE_TESTLIB_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

void testOnError(void);
void testErrorFunctions(void);
void testThreads(void);
void testStringFunctions(void);
void testConversionFunctions(void);
void testFileIOFunctions(void);
void testMathFunctions(void);

#ifdef __cplusplus
}
#endif

#endif

/* end of TestLib.h ... */

