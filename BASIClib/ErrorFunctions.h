/*
 * Declarations for BASIC runtime error subsystem.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ERRORFUNCTIONS_H_
#define _INCLUDE_ERRORFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

__long _vbl_err(void);
void _vbpl_error(__long newErr);
__integer _vbi_erl(void);
__integer _vbi_erdev(void);
PBasicString _vbS_erdev_DC_(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_ERRORFUNCTIONS_H_ */

/* end of ErrorFunctions.h ... */


