/*
 * Test conversion functions.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdio.h>
#include <string.h>
#include "OnEvents.h"
#include "ErrorFunctions.h"


/* !!! Comment functions. */

#define TESTVAR_VALUE1 0xABCD
#define TESTVAR_VALUE2 0x1234

#define STR1 "The Quick brown fox"
#define STR2 "jumped over the lazy dog."
void test_getBasePointer(void)
/*
 * This function is very i386 specific. Your mileage may vary.
 *
 *  !!! more comments.
 */
{
    char var1[] = STR1;
    char var2[] = STR2;
    
    char *loc1;
    char *loc2;

#ifdef DEBUG
    char *i;
#endif

    printf("Testing __getBasePointer()...");

    __getBasePointer(&_base_ptr_);
    loc1 = (char *) (((int) _base_ptr_) + 8);
    loc2 = (char *) (((int) _base_ptr_) + strlen(STR2) + 8);

#ifdef DEBUG
    printf("var1 == [%s]...should be [%s]\n", loc1, var1);
    printf("var2 == [%s]...should be [%s]\n", loc2, var2);

    printf("\n");

    for (i = loc1 - 20; i < loc1 + 20; i++)
        printf("%2X ", (int) (*i));

    printf("\n");

#endif

    if ( (strcmp(loc1, STR1) != 0) || (strcmp(loc2, STR2) != 0) )
        printf("failed.");
    else
        printf("successful.");
} /* test_getBasePointer */


void testOnErrorHandling(int x)
{
    int testVar1 = TESTVAR_VALUE1;
    int testVar2 = TESTVAR_VALUE2;
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
        __getBasePointer(&_base_ptr_);   /* !!! lose this! */
        printf("Base pointer inside error handler is (%p)\n", _base_ptr_);

        printf("\n  Base pointer is damaged.\n");
        printf("  testVar1 is 0x%X, should be 0x%X...\n",
                                 testVar1, TESTVAR_VALUE1);
        printf("  testVar2 is 0x%X, should be 0x%X...\n",
                                 testVar2, TESTVAR_VALUE2);
        printf("  ...Stopping tests.\n");
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
        case 1:
            printf("Successful.\n");
            break;
        case 2:
            printf("Handler missed. Failed.\n");
            break;
        case 0:
        default:
            printf("Confused! Failed.\n");
            break;
    } /* switch */

    __deregisterOnEventHandler(&x + sizeof (x), ONERROR);
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
    printf("  (Note these might crash the program really horribly...)\n\n");

    test_getBasePointer();
    testOnErrorHandling(0);
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


