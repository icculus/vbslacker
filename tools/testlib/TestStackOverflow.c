#include <unistd.h>
#include "BasicLib.h"

void stackThis(void)
{
    stackThis();
} /* stackThis */

int main(int argc, char **argv)
{
    __initBasicLib(INITFLAG_NO_FLAGS, argc, argv);
    __printAsciz("About to cause this BASIClib application "
                 "to have a STACK OVERFLOW.\n");

    sleep(3);

    __printAsciz("Here goes...\n");

    sleep(3);

    stackThis();
    return(0);
} /* main */

/* end of TestSegFault.c ... */

