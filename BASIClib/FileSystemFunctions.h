/*
 * Header file for BASIClib's file system interface API.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_FILESYSTEMFUNCTIONS_H_
#define _INCLUDE_FILESYSTEMFUNCTIONS_H_

#include "BasicString.h"
#include "RegState.h"
#include "Boolean.h"

boolean __fileExists(STATEPARAMS, char *fileName);
void vbpS_kill(STATEPARAMS, PBasicString fileName);
void vbpS_mkdir(STATEPARAMS, PBasicString dirStr);
void vbpS_rmdir(STATEPARAMS, PBasicString dirStr);
void vbpSS_name(STATEPARAMS, PBasicString oldName, PBasicString newName);
void vbp_files(STATEPARAMS);

#endif

/* end of FileSystemFunctions.h ... */

