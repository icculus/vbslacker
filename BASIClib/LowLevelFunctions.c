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


__integer _vbil_peek(__long addr)
{
    return(*((__byte *) addr));
} /* _vbil_peek */


void _vbpli_poke(__long addr, __integer newVal)
{
    *((__byte *) addr) = (__byte) newVal;
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


__integer _vbil_inp(__long ioport)
{
    __requestPortAccess();
    return((__integer) inb(ioport));
} /* _vbil_inp */


void _vbpli_out(__long ioport, __integer outByte)
{
    __requestPortAccess();
    outb((char) outByte, ioport);
} /* _vbpli_out */


PBasicString _vbSi_ioctl_DC_(__integer devFileNum)
{
    return(__createString("", false));
} /* _vbSi_ioctl_DC_ */


void _vbpiS_ioctl(__integer devFileNum, PBasicString ctlStr) {}


__long _vblA_varptr(void *myVar)
{
    return((long) myVar);
} /* _vblA_varptr */


PBasicString _vbSA_varptr_DC_(void *myVar)
{
    PBasicString retVal = __allocString(sizeof (void *), false);

    memcpy(retVal->data, &myVar, sizeof (void *));
    return(retVal);
} /* _vbSA_varptr_DC_ */


__long _vblA_varseg(void *myVar)
{
    return(0);   /* no segments in 32-bit architecture. */
} /* _vblA_varseg */


/* !!! still need: */
/* call absolute */
/* bload */
/* bsave */
/* wait */

/* end of LowLevelFunctions.c ... */


