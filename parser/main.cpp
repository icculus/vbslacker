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
   char strInput[MAX_LINE_LENGTH];
   istrstream *pInputStream;
   BasicLexer *pLexer;
   BasicParser *pParser;
   PostParser MyPostParser;

   /* Until I find a better way to do this, I'm creating a new lexer and
    * parser for each line of VB code.  Right now, I can't get the parser
    * to parse multiple lines without getting all weird if there are
    * syntax errors.  So I'm doing it this way until I think of a better
    * option.  This better be temporary. :-)
    */
   while(fgets(strInput, MAX_LINE_LENGTH, stdin) != NULL)
   {
      // Create new objects for parsing
      pInputStream = new istrstream(strInput);
      pLexer = new BasicLexer(*pInputStream);
      pParser = new BasicParser(*pLexer);

      // Start parsing the buffer
      pParser->start(&MyPostParser);

      // Delete the objects since we're done with them.
      delete pInputStream;
      delete pLexer;
      delete pParser;
   }

   return 0;
}
