/*
 * Mainline for BASIClib testing routines.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include "Initialize.h"
#include "TestLib.h"

int main(void)
{
    setbuf(stdout, NULL);
    printf("Making call to __initBasicLib()...\n"); 
    __initBasicLib();
    testOnEvents();
    testConversions();

        /* !!! This will be done automatically, eventually... */
    printf("Making call to __deinitBasicLib()...\n"); 
    __deinitBasicLib();
    return(0);
} /* main */

/* end of TestLib.c ... */

