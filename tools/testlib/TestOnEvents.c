/*
 * Test conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "TestLib.h"
#include "OnEvents.h"
#include "ErrorFunctions.h"
#include "Boolean.h"

/* !!! Comment functions. */

#define TESTVAR_VALUE1 0xABCD
#define TESTVAR_VALUE2 0x1234

#define RECURSION_COUNT 3

#define FAILED "  - Failed.\n"
#define FAILED_WITH_WARNING "  - Failed. Later tests will fail, too.\n"

int recursive = 0;

boolean binaryDump(char *fileName, void *data, int size)
/*
 * Dump a chunk of memory to a binary file.
 *
 *   params : fileName == file to try to write to. It will destroy
 *                         any previous contents.
 *            data     == memory to write.
 *            size     == number of bytes to write, starting with (data).
 *  returns : boolean true if data written, boolean false otherwise.
 */
{
    FILE *binStream = fopen(fileName, "wb");
    boolean retVal = false;

    if (binStream == NULL)
        printf("  - COULDN'T OPEN [%s] FOR BINARY OUTPUT!\n", fileName);
    else
    {
        if (fwrite(data, size, 1, binStream) != 1)
            printf("  - COULDN'T WRITE DATA DUMP TO DISK!\n");
        else
        {
            printf("  - Data dump is in [%s]...\n", fileName);
            retVal = true;
        } /* else */
        fclose(binStream);
    } /* else */

    return(retVal);
} /* binaryDump */


void test__getBasePointer(void)
/*
 * This function is pretty i386 specific. Your mileage may vary.
 *
 *  !!! more comments.
 */
{
    int var1 = TESTVAR_VALUE1;

    printf("Testing __getBasePointer()...\n");

    __getBasePointer(&_base_ptr_);

    if (*((int *) (((int) _base_ptr_) - 4)) != TESTVAR_VALUE1)
    {
        printf(FAILED_WITH_WARNING);
        printf("  - dword [ebp] should have been (0x%X), but is (0x%X).\n",
                TESTVAR_VALUE1, *((int *) (((int) _base_ptr_) - 4)));
        if (binaryDump("debugBP.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
    } /* if */

    var1 = 0;  /* prevent compiler warnings... */
} /* test__getBasePointer */


void test__getStackPointer_recurse(void)
/*
 * This function calls itself RECURSION_COUNT times, to pile
 *  some data on the stack.
 */
{
    recursive++;
    if (recursive == RECURSION_COUNT)
        __getStackPointer(&_stack_ptr_);
    else
        test__getStackPointer_recurse();
} /* test__getStackPointer_recurse */


void test__getStackPointer()
/*
 * The theory being that a void function with void arguments should
 *  add 8 bytes to the stack; four for the return address and four for the
 *  saved base pointer. So we grab the current stack pointer, and call
 *  a recursive function that'll grab the stack pointer before returning.
 *  By doing the math between the two pointers, we can see if we're
 *  grabbing it correctly.
 */
{
    void *expected;
    void *sp;

    printf("Testing __getStackPointer()...\n");

    __getStackPointer(&sp);

    expected = (sp - (RECURSION_COUNT * 8));

    recursive = 0;
    test__getStackPointer_recurse();

    if (_stack_ptr_ != expected)
    {
        printf(FAILED_WITH_WARNING);
        printf("  - expected (%p), but got (%p)\n", expected, _stack_ptr_);
    } /* if */
} /* test__getStackPointer */


void testOnErrorGotoHandling(int x)
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
    char testVar3[] = "Look for this string in the data dump!";
    int landed = 0;

    printf("Testing ON ERROR GOTO handling...");

    __getStackPointer(&_stack_ptr_);
    __getBasePointer(&_base_ptr_);
    __registerOnEventHandler(&&errHandler, &x + sizeof (x),
                             _stack_ptr_, &x - sizeof (void *),
                             _base_ptr_, ONERROR);
    __runtimeError(INTERNAL_ERROR);

    goto missedHandler;

errHandler:
    __markOnEventHandlerAddr;
    if ((testVar1 != TESTVAR_VALUE1) || (testVar2 != TESTVAR_VALUE2))
    {
        __getBasePointer(&_base_ptr_); 

        printf("  - Base pointer is damaged.\n");
        printf("  - EPB inside error handler is (%p)\n", _base_ptr_);
        printf("  - testVar1 is (0x%X), should be (0x%X)...\n",
                                 testVar1, TESTVAR_VALUE1);
        printf("  - testVar2 is (0x%X), should be (0x%X)...\n",
                                 testVar2, TESTVAR_VALUE2);
        if (binaryDump("debugErrGoto.bin", _base_ptr_ - 100, 200) == true)
            printf("  - (That's our supposed [ebp-100] to [ebp+100]...)\n");
        printf("  - Stack will be FUBAR. Must terminate testing.\n");
        exit(1);
    } /* if */

    landed = 1;
    goto endTest;

missedHandler:
    landed = 2;
    goto endTest;

endTest:
    switch (landed)
    {
        case 1:   /* good. */
            break;
        case 2:
            printf("  - Handler missed. %s", FAILED);
            break;
        default:
            printf("  - Confused! %s", FAILED);
            break;
    } /* switch */

    testVar3[0] = '\0';  /* stops compiler whining. */

    __deregisterOnEventHandler(&x + sizeof (x), ONERROR);
} /* testOnErrorGotoHandling */


void testOnErrorHandling(void)
{
    testOnErrorGotoHandling(0);
} /* testOnErrorHandling */


void testOnEvents(void)
/*
 * Test ON EVENT functionality in BASIClib.
 *
 *    params : void.
 *   returns : void.
 */
{
    printf("Testing ON EVENT handling...\n");
    test__getBasePointer();
    test__getStackPointer();
    testOnErrorHandling();
} /* testOnEvents */


#ifdef STANDALONE

void main(void)
{
    __initBasicLib();
    testOnEvents();
    __deinitBasicLib();
} /* main */

#endif

/* end of TestConversionFunctions.c ... */


