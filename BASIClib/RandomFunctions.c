/*
 * Random number functions.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "RandomFunctions.h"

/* !!!
 * in the final prime-time version, we need to thread-proof this,
 *  so threads don't overwrite the seed while another one is doing
 *  so as well. Here, it's not worth the effort.
 */

__single _vbf_timer(void);   /* need this for _vbp_randomize() .. */

#ifdef BIGENDIAN
#   define WORD0     0x0000FFFF
#   define WORD1     0xFFFF0000
#   define DWORD1    0x00000000FFFFFFFF
#   define LOWBYTE   0xFF00
#   define SHIFTHIGH >>
#else
#   define WORD0     0xFFFF0000
#   define WORD1     0x0000FFFF
#   define DWORD1    0xFFFFFFFF00000000
#   define LOWBYTE   0x00FF
#   define SHIFTHIGH <<
#endif


    /* These are the (hopefully) constants in b$EightSpaces... */
#define ENTROPY1 0x43FD
#define ENTROPY2 0x9EC3
#define ENTROPY3 0x195
#define ENTROPY4 (__single) 0x1000000

    /* The psuedorandom seed. */
static DWORD randomSeed = 0x50000;

void _vbpf_randomize(__double seedAlterer)
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
    DWORD trimmedBits;
    WORD result;
    union  /* preserve bit pattern. */
    {
        __double d;
        QWORD q;
    } alterUnion;

    alterUnion.d = seedAlterer;
    trimmedBits = (alterUnion.q & DWORD1);
    result = (trimmedBits & WORD0);
    result ^= (trimmedBits & WORD1);
    *middleSeed = result;              /* write to center of seed. */
} /* _vbpf_randomize */


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
    union  /* save bit pattern. */
    {
        DWORD dword;
        __single single;
    } seedUnion;

    seedUnion.dword = randomSeed;
    return(seedUnion.single / ENTROPY4);
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
    DWORD tmp;
    WORD lowWord;
    WORD highWord;

    tmp = (randomSeed & WORD0) * ENTROPY1;
    lowWord = (tmp & WORD0);
    highWord = (tmp & WORD1);

    tmp = (randomSeed & WORD1) * ENTROPY1;
    highWord += (tmp & WORD0);

    tmp = (randomSeed & WORD0) * ENTROPY1;
    lowWord += ENTROPY2;

    highWord += (tmp & WORD0);

    __addWithCarry(highWord.h.l, ENTROPY3);
    highWord &= LOWBYTE;  /* clear high byte. */

    randomSeed = (lowWord | (highWord SHIFTHIGH (sizeof (WORD) * 8)));

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

    seedUnion.dword = randomSeed;

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
    return(_vbf_rnd(1.0));
} /* _vbf_rnd */


/* end of RandomFunctions.c ... */

