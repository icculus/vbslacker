/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "LowLevelFunctions.h"

#include <errno.h>

#ifdef LINUX
#include <sys/io.h>


#elif defined WIN32
#warning WIN32 portio is nonexistant, right now!
#define iopl(x) -1
#define inb(x) 0
#define outb(x, y)
#endif


static __boolean portAccess = false;


int _vbil_peek(long addr)
{
    return(*((char *) addr));
} /* _vbil_peek */


void _vbpli_poke(long addr, int newVal)
{
    *((char *) addr) = (char) newVal;
} /* _vbpli_poke */


static void __requestPortAccess(void)
{
    int rc;

    if (portAccess == false)
    {
        rc = iopl(3);

        if (rc == -1)
        {
            __runtimeError((errno == EPERM) ?
                            ERR_PERMISSION_DENIED : ERR_INTERNAL_ERROR);
        } /* if */
        else
            portAccess = true;
    } /* if */
} /* __requestPortAccess */


int _vbil_inp(long ioport)
{
    __requestPortAccess();
    return((int) inb(ioport));
} /* _vbil_inp */


void _vbpli_out(long ioport, int byte)
{
    __requestPortAccess();
    outb((char) byte, ioport);
} /* _vbpli_out */


PBasicString _vbSi_ioctl_DC_(int devFileNum)
{
    return(__createString("", false));
} /* _vbSi_ioctl_DC_ */


void _vbpiS_ioctl(int devFileNum, PBasicString ctlStr) {}


long _vblA_varptr(void *myVar)
{
    return((long) myVar);
} /* _vblA_varptr */


PBasicString _vbSA_varptr_DC_(void *myVar)
{
    PBasicString retVal = __allocString(sizeof (void *), false);

    memcpy(retVal->data, &myVar, sizeof (void *));
    return(retVal);
} /* _vbSA_varptr_DC_ */


long _vblA_varseg(void *myVar)
{
    return(0);   /* no segments in 32-bit architecture. */
} /* _vblA_varseg */


/* !!! still need: */
/* call absolute */
/* bload */
/* bsave */
/* wait */

/* end of LowLevelFunctions.c ... */


