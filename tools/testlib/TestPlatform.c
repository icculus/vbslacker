        #define __I386
        #define __ARCHITECTURE      "i386"
        #define __FUNCNAME_PREPEND  ""
        #define __STACK_DIRECTION   -1
        #define __EOL_STRING        "\n"
        #define __PATHCHAR          '/'
        #define __CURRENTDIRSTR     "."
        #define __CURRENTDIRCHAR     '.'
        #define __NODRIVELETTERS
        typedef short _VB_boolean;
        typedef unsigned char _VB_byte;
        typedef short  __integer;
        typedef long   __long;
        typedef float  __single;
        typedef double __double;

void test_data_types(void)
{
    printf("Testing data types...\n");

    if (sizeof (__byte) != 1)
    {
        printf(" - __byte is (%d) bytes, should be 1.\n", sizeof (__byte));
        errors++;
    } // if

    if (sizeof (__integer) != 2)
    {
        printf(" - __integer is (%d) bytes, should be 2.\n", sizeof (__byte));
        errors++;
    } // if

}


