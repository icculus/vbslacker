/*
 * Registry-emulating API for BASIClib.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "RegistryFunctions.h"

void _vbpSS_deletesetting(PBasicString appName, PBasicString section)
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* _vbpSS_deletesetting */


void _vbpSSS_deletesetting(PBasicString appName,
                           PBasicString section,
                           PBasicString key)
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* _vbpSSS_deletesetting */


void _vbpSSSV_savesetting(PBasicString appName, PBasicString section,
                          PBasicString key, PVariant setting)
{
    __runtimeError(ERR_ILLEGAL_FUNCTION_CALL);
} /* _vbpSSSV_savesetting */

/* end of RegistryFunctions.c ... */
