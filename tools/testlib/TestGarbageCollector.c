/*
 * Test BASIClib's garbage collector.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <stdlib.h>
#include "BasicLib.h"

void testGarbageCollector(void)
/*
 * This code tests the BASIClib garbage collector.
 *
 *    params : void.
 *   returns : void.
 */
{
    __ONERRORVARS;
    __long totalBlocks = 150000;
    __long blockSize = 5004;
    unsigned long allocated;
    char *ptr;
    int tmp;

    __ONERRORINIT;

    printf("\n[TESTING GARBAGE COLLECTOR...]\n");

    __setOnErrorHandler(__memAllocBailed);

    printf("Testing brute force allocation: %ld blocks of %ld bytes...",
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

    printf("done.\n");
    printf(" - Please compile with WANT_MALLOC_NOT_MEMALLOC for comparison.\n");
    __exitCleanupOnError;
    return;

__insertLineLabel(__memAllocBailed);

    tmp = _vbl_err();
    if (tmp == ERR_OUT_OF_MEMORY)
    {
        printf("\n"
               " - Ran out of memory! Something's wrong with the collector!\n");
    } /* if */
    else
        printf(" - Some strange runtime error was thrown. (#%d)\n", tmp);

    printf(" - %ld blocks of %ld bytes (%ld bytes total) were allocated.\n",
                allocated, blockSize, allocated * blockSize);
    printf(" - Aborting tests. Sorry about the swapping...\n\n");
    _vbp_end();
} /* testGarbageCollector */



#ifdef STANDALONE

int main(int argc, char **argv, char **envp)
{
    void *base;

    __getBasePointer(base);
    __initBasicLib(INITFLAG_DISABLE_CONSOLE, argc, argv, envp);
    testGarbageCollection();
    return(0);
} /* main */

#endif


/* end of TestConversionFunctions.c ... */


