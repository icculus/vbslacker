#include <unistd.h>
#include "BasicLib.h"

int main(int argc, char **argv, char **envp)
{
    __initBasicLib(INITFLAG_NO_FLAGS, argc, argv, envp);
    __printAsciz("About to crash this BASIClib application with a SIGSEGV.\n");

    sleep(3);

    __printAsciz("Here goes...\n");

    sleep(3);

    *((char *) 75) = '\0';
    return(0);
} /* main */

/* end of TestSegFault.c ... */

