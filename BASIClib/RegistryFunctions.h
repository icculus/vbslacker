/*
 * APIs that emulate the Windows registry.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_REGISTRYFUNCTIONS_H_
#define _INCLUDE_REGISTRYFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

void _vbpSS_deletesetting(PBasicString appName, PBasicString section);
void _vbpSSS_deletesetting(PBasicString appName,
                           PBasicString section,
                           PBasicString key);
void _vbpSSSV_savesetting(PBasicString appName, PBasicString section,
                          PBasicString key, PVariant setting);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_REGISTRYFUNCTIONS_H_ */

/* end of RegistryFunctions.h ... */

