/* This will exist until we turn this parse into a shared library */

//#include <iostream>
#include <strstream.h>
#include <stdio.h>
#include "BasicLexer.hpp"
#include "BasicParser.hpp"
#include "PostParser.h"

// Maximum length of a single VB line
#define MAX_LINE_LENGTH     1024

int main()
{
    BasicLexer Lexer(std::cin);
    BasicParser Parser(Lexer);
    PostParser MyPostParser;
 
    // Start parsing the buffer
    Parser.start(&MyPostParser);

    return 0;
}
