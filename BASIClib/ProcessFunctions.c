/*
 * Process-related functions for BASIClib.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include "ProcessFunctions.h"


void vbpS_shell(STATEPARAMS, PBasicString shCmd)
{
    char *cmd = __basicStringToAsciz(STATEARGS, shCmd);

    __shellOutNotification(STATEARGS);
    system(cmd);
    __shellRetNotification(STATEARGS);
    __memFree(STATEARGS, cmd);
} /* vbpS_shell */


void vbp_system(STATEPARAMS)
{
    exit(0);
} /* vbp_system */


void vbp_end(STATEPARAMS)
{
    exit(0);
} /* vbp_end */

/* sleep */


/* end of ProcessFunctions.c ... */

