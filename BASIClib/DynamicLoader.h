/*
 * Header for the BASIClib Dynamic Loader API.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *   This file written by Ryan C. Gordon.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_DYNAMICLOADER_H_
#define _INCLUDE_DYNAMICLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void *__DynamicBinaryHandle;
#define __dyn_NULL_HANDLE  NULL

__DynamicBinaryHandle __dynLoadBinary(char *binaryName);
void *__dynFindSymbol(__DynamicBinaryHandle handle, char *symbolName);
void __dynFreeBinary(__DynamicBinaryHandle handle);
char *__dynPlatformBinaryName(char *baseName);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_DYNAMICLOADER_H_ */
#endif /* _INCLUDE_STDBASIC_H_ */

/* end of DynamicLoader.h ... */

