/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) Ryan C. Gordon and Gregory S. Read.
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


int vbil_peek(long addr)
{
    return(*((char *) addr));
} /* vbil_peek */


void vbpli_poke(long addr, int newVal)
{
    *((char *) addr) = (char) newVal;
} /* vbpli_poke */


static void __getPortAccess(void)
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
} /* __getPortAccess */


int vbil_inp(long ioport)
{
    __getPortAccess();
    return((int) inb(ioport));
} /* vbil_inp */


void vbpli_out(long ioport, int byte)
{
    __getPortAccess();
    outb((char) byte, ioport);
} /* vbpli_out */


PBasicString vbSi_ioctl_DC_(int devFileNum)
{
    return(__createString("", false));
} /* vbSi_ioctl_DC_ */


void vbpiS_ioctl(int devFileNum, PBasicString ctlStr) {}


long vblA_varptr(void *myVar)
{
    return((long) myVar);
} /* vblA_varptr */


PBasicString vbSA_varptr_DC_(void *myVar)
{
    PBasicString retVal = __allocString(sizeof (void *), false);

    memcpy(retVal->data, &myVar, sizeof (void *));
    return(retVal);
} /* vbSA_varptr_DC_ */


long vblA_varseg(void *myVar)
{
    return(0);   /* no segments in 32-bit architecture. */
} /* vblA_varseg */


/* !!! still need: */
/* call absolute */
/* bload */
/* bsave */
/* wait */

/* end of LowLevelFunctions.c ... */


