/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

#include <errno.h>
#include <sys/io.h>
#include "ErrorFunctions.h"
#include "Boolean.h"

static boolean portAccess = false;


char peek(STATEPARAMS, long addr)
{
    return(*((char *) addr));
} /* peek */


void poke(STATEPARAMS, long addr, char newVal)
{
    *((char *) addr) = newVal;
} /* poke */


void getPortAccess(STATEPARAMS)
{
    int rc;

    if (portAccess == false)
    {
        rc = iopl(3);

        if (rc == -1)
        {
            __runtimeError(STATEARGS, (errno == EPERM) ?
                            ERR_PERMISSION_DENIED : ERR_INTERNAL_ERROR);
        } /* if */
        else
            portAccess = true;
    } /* if */
} /* getPortAccess */


char inp(STATEPARAMS, long ioport)
{
    getPortAccess(STATEARGS);
    return(inb(ioport));
} /* inp */


void out(STATEPARAMS, long ioport, char byte)
{
    getPortAccess(STATEARGS);
    outb(byte, ioport);
} /* outp */

/* call absolute */
/* bload */
/* bsave */
/* ioctl */
/* ioctl$ */
/* varptr */
/* varptr$ */
/* varseg */
/* wait */

/* end of LowLevel.c ... */


