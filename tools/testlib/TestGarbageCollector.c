/*
 * Test BASIClib's garbage collector.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"

extern long errors;
extern long warnings;

void testGarbageCollector(int skipCollector)
/*
 * This code tests the BASIClib garbage collector.
 *
 *    params : skipCollector == non-zero, skip this lengthy test.
 *   returns : void.
 */
{
    __ONERRORVARS;


    __long blockSize = 5004;

        /*
         * !!! This should be dynamically selected, based on the
         * !!!  amount of virtual RAM in the box, so that there
         * !!!  would HAVE to be an allocation failure if this many
         * !!!  blocks of (blockSize) length were malloc()ed without
         * !!!  garbage collection.
         */
    __long totalBlocks = 150000;
    unsigned long allocated;
    char *ptr;
    int tmp;

    __ONERRORINIT;

    printf("\n[TESTING GARBAGE COLLECTOR. THIS MAY TAKE A LONG TIME...]\n");


    if (skipCollector)
    {
        printf(" - Test skipped by user request.\n");
        warnings++;
    } /* if */
    else
    {
        __setOnErrorHandler(__memAllocBailed);

        printf("Testing brute force allocation: %ld blocks of %ld bytes...\n",
                totalBlocks, blockSize);

        for (allocated = 0; allocated < totalBlocks; allocated++)
        {
#ifdef WANT_MALLOC_NOT_MEMALLOC
            ptr = malloc(blockSize);
            if (ptr == NULL)
                __runtimeError(ERR_OUT_OF_MEMORY);
#else
            ptr = __memAlloc(blockSize);
#endif
            for (tmp = 0; tmp < blockSize; tmp++)
                ptr[tmp] = '\0';    /* touch all allocated bytes... */
        } /* for */
    } /* else */

    __exitCleanupOnError;
    return;

__insertLineLabel(__memAllocBailed);

    tmp = _vbl_err();
    if (tmp == ERR_OUT_OF_MEMORY)
    {
        printf("\n"
               " - Ran out of memory! Something's wrong with the collector!\n");
        errors++;
    } /* if */
    else
    {
        printf(" - Some strange runtime error was thrown. (#%d)\n", tmp);
        errors++;
    } /* else */

    printf(" - %ld blocks of %ld bytes (%ld bytes total) were allocated.\n",
                allocated, blockSize, allocated * blockSize);
    printf(" - Aborting tests. Sorry about the swapping...\n\n");
    _vbp_end();
} /* testGarbageCollector */



#ifdef STANDALONE

long errors = 0;
long warnings = 0;

int main(int argc, char **argv, char **envp)
{
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv, envp);
    testGarbageCollector(0);
    return(0);
} /* main */

#endif


/* end of TestConversionFunctions.c ... */


