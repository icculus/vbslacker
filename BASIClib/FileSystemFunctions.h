/*
 * Header file for BASIClib's file system interface API.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_FILESYSTEMFUNCTIONS_H_
#define _INCLUDE_FILESYSTEMFUNCTIONS_H_

#include "StdBasic.h"

__boolean __fileExists(char *fullPathName);
void vbpS_kill(PBasicString fileName);
void vbpS_mkdir(PBasicString dirStr);
void vbpS_rmdir(PBasicString dirStr);
void vbpSS_name(PBasicString oldName, PBasicString newName);
void vbp_files(void);
void vbpSS_filecopy(PBasicString src, PBasicString dest);

#endif /* _INCLUDE_FILESYSTEMFUNCTIONS_H_ */

/* end of FileSystemFunctions.h ... */

