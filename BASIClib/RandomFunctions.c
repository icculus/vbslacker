/*
 * Random number functions.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include "RandomFunctions.h"

/* !!!
 * in the final prime-time version, we need to thread-proof this,
 *  so threads don't overwrite the seed while another one is doing
 *  so as well. Here, it's not worth the effort.
 */

/* !!! This is probably gonna suck on a bigendian system... */

__single _vbf_timer(void);   /* need this for _vbp_randomize() .. */

/* !!! These need to go. */

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long long QWORD;
typedef unsigned char BYTE;

typedef union
{
    QWORD qw;
    DWORD dw[2];
    WORD  w[4];
    BYTE  b[8];
    __double d;
    __single s;
} ByteBlock;


    /* These are the (hopefully) constants in b$EightSpaces... */
#define ENTROPY1 0x43FD
#define ENTROPY2 0x9EC3
#define ENTROPY3 0xC3
#define ENTROPY4 (__single) 0x1000000

    /* The psuedorandom seed. */
    /* Is there one of these for each thread? */
static DWORD randomSeed = 0x50000;


 /* !!! get rid of this. */
static void printBytes(char *name, char *addr, int size)
{
    int i;

    printf("bytes of %s == ", name);

    for (i = 0; i < size; i++)
        printf("[%d]  ", (int) addr[i]);

    printf("\n");
}



void _vbpd_randomize(__double seedAlterer)
/*
 * Alter the psuedorandom number generator's seed. Note that the
 *  seed is merely ALTERED, and not overwritten. While a consistent
 *  series of calls to RND() and RANDOMIZE with consistent arguments
 *  will always produce the same series of psuedorandom numbers, the
 *  series may not be reproduced midprogram with the RANDOMIZE function.
 *
 * Randomize does NOT restart the seed, EVER.  You can restart the seed
 *  (although not to the beginning seed, at least not easily) with RND(x),
 *  where (x < 0). If you REALLY need a reproducible series of numbers,
 *  Call RND(-1) before your series of RANDOMIZE and RND() calls. Then
 *  repeat the process to get the same series. Then consider why you are
 *  using random numbers in the first place, if you must do this.
 *
 *    params : seedAlterer == double param to alter seed.
 *   returns : void.
 */
{
        /*
         * (middleSeed) points to the 2nd byte in the random seed variable.
         *  Writing a 16-bit word here leaves one byte on each side
         *  of the random seed unmolested. This is why RANDOMIZE only
         *  alters, and doesn't overwrite the seed.
         */
    WORD *middleSeed = (WORD *) ( ((BYTE *) &randomSeed) + 1 );
    ByteBlock bb;

    bb.d = seedAlterer;
    *middleSeed = bb.w[2] ^ bb.w[3];  /* write to center of seed. */
} /* _vbpd_randomize */


void _vbp_randomize(void)
/*
 * Alter the psuedorandom number generator's seed.
 *  This is equivalent to the BASIC command "RANDOMIZE TIMER".
 *
 *    params : void.
 *   returns : void.
 */
{
    _vbpd_randomize((__double) _vbf_timer());
} /* _vbp_randomize */


static __single getCurrentGeneration(void)
/*
 * Get the current generation in the sequence of psuedorandom numbers.
 *  The seed is unchanged by this function.
 *
 *    params : void.
 *   returns : next generation in sequence.
 */
{
    ByteBlock bb;
    bb.d = randomSeed;

printf("seedUnion.single == (%f). dword == (%ld).\n", bb.s, randomSeed);
printBytes("seedUnion.single", (char *) &bb.s, sizeof (__single));
printBytes("randomSeed", (char *) &randomSeed, sizeof (DWORD));

    return(bb.s / ENTROPY4);
} /* getCurrentGeneration */


static __single getNextGeneration(void)
/*
 * Get the next generation in the sequence of psuedorandom numbers.
 *  This updates the seed and then figures the current number based
 *  on the update.
 *
 *    params : void.
 *   returns : next generation in sequence.
 */
{
    ByteBlock bb;
    ByteBlock tmp;
    WORD lowWord;
    WORD highWord;

    bb.dw[0] = randomSeed;

    tmp.dw[0] = ((DWORD) bb.w[0] * (DWORD) ENTROPY1);
    lowWord = tmp.w[0];
    highWord = tmp.w[1];

    tmp.dw[0] = ((DWORD) bb.w[1] * (DWORD) ENTROPY1);
    highWord += tmp.w[0];

    tmp.w[0] = highWord;
    highWord += (BYTE) (tmp.b[0] + (BYTE) ENTROPY3);
    if ( ( (DWORD) lowWord + ENTROPY2 ) > 0xFFFF)
        highWord++;

    tmp.dw[0] = (DWORD) bb.w[0] * (DWORD) ENTROPY1;
    lowWord += ENTROPY2;
    tmp.w[0] = highWord + tmp.w[0];
    highWord = (WORD) tmp.b[0];

    bb.w[0] = lowWord;
    bb.w[1] = highWord;
    randomSeed = bb.dw[0];

    return(getCurrentGeneration());
} /* getNextGeneration */


static __single completelyReseed(__single arg)
/*
 * This is the called when rnd(x) is called where (x < 0). The random
 *  number seed is completely overwritten with a new number based on
 *  (arg). Therefore, psuedorandom sequences generated after this
 *  function are guaranteed to have identical sequences for to every
 *  other call where (arg) is identical. That is, -5.0 will always then
 *  seed to the exact same sequence as the next time this function is
 *  called with -5.0, for example.
 *
 *     params : arg == argument to base new seed upon.
 *    returns : first number in new psuedorandom number sequence.
 */
{
    union
    {
        DWORD dword;
        WORD  words[2];
    } seedUnion;

    union
    {
        WORD word;
        BYTE bytes[2];
    } wordUnion1, wordUnion2;

    union
    {
        __single single;
        WORD words[2];
    } argUnion;

    seedUnion.dword = randomSeed;

    argUnion.single = arg;
    wordUnion1.word = argUnion.words[1];
    wordUnion2.word = seedUnion.words[0];

        /* Add with carry's a bitch in C... */
    if ( (WORD) ((WORD) wordUnion2.bytes[1] + (WORD) wordUnion1.bytes[0]) > (WORD) 255 )
    {
        if ( (WORD) ((WORD) (wordUnion2.bytes[1]) + 1) > (WORD) 255 )
            wordUnion1.bytes[1] += 1;
        wordUnion2.bytes[0] += 1;
    } /* if */

    wordUnion2.bytes[1] += wordUnion1.bytes[0];
    wordUnion1.bytes[1] = 0;

    seedUnion.words[0] = wordUnion2.word;
    seedUnion.words[1] = wordUnion1.word;
    randomSeed = seedUnion.dword;  /* completely overwrite the seed. */

printf("randomSeed == (%ld).\n", randomSeed);

    return(getNextGeneration());
} /* completelyReseed */


__single _vbff_rnd(__single arg)
{
    __single retVal = 0.0;

    if (arg > 0.0)
        retVal = getNextGeneration();
    else if (arg == 0.0)
        retVal = getCurrentGeneration();
    else
        retVal = completelyReseed(arg);

    return(retVal);
} /* _vbff_rnd */


__single _vbf_rnd(void)
{
    return(_vbff_rnd(1.0));
} /* _vbf_rnd */


/* end of RandomFunctions.c ... */

