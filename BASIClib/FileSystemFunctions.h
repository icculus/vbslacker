/*
 * Header file for BASIClib's file system interface API.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _INCLUDE_FILESYSTEMFUNCTIONS_H_
#define _INCLUDE_FILESYSTEMFUNCTIONS_H_

#include "StdBasic.h"

#ifdef __cplusplus
extern "C" {
#endif

void __initFileSystemFunctions(void);
void __deinitFileSystemFunctions(void);
void __initThreadFileSystemFunctions(__integer tidx);

void _vbpS_kill(PBasicString fileName);
void _vbpS_mkdir(PBasicString dirStr);
void _vbpS_rmdir(PBasicString dirStr);
void _vbpSS_name(PBasicString oldName, PBasicString newName);
void _vbpSS_filecopy(PBasicString src, PBasicString dest);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_FILESYSTEMFUNCTIONS_H_ */

/* end of FileSystemFunctions.h ... */

