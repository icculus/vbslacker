/* Header file for PostParser class
 * Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 * 
 * Description
 *    The ANTLR grammer will pass along parsed VB statements to this class
 *    for furthur processing.
 */
/*
12345678901234567890123456789012345678901234567890123456789012345678901234567890
*/

#ifndef __POSTPARSER_H
#define __POSTPARSER_H

#include "antlr/Token.hpp"

typedef enum BasicScope {SCOPE_PUBLIC, SCOPE_PRIVATE, SCOPE_DIM, SCOPE_FRIEND};
typedef enum Boolean {FALSE = 0, TRUE = 1};

class PostParser
{
   public:
      PostParser();
      ~PostParser();

      void VarDecl(BasicScope Scope, RefToken VarName, RefToken DataType,
         Boolean WithEvents, Boolean AsNew);
};

#endif