/*
 * Low level BASIC API calls.
 *
 *  Copyright (c) Ryan C. Gordon and Gregory S. Read.
 */

char peek(long addr)
{
    return(*((char *) addr));
} /* peek */


void poke(long addr, char newVal)
{
    *((char *) addr) = newVal;
} /* poke */

/* end of LowLevel.c ... */


