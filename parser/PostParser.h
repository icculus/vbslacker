/* PostParser - Header file for PostParser class
 *
 * Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 * 
 * The ANTLR grammer will pass along parsed VB statements to this class for
 * furthur processing.
 */

#ifndef __POSTPARSER_H
#define __POSTPARSER_H

#include "antlr/Token.hpp"
#include "antlr/ParserException.hpp"

typedef enum BasicScope {SCOPE_PUBLIC, SCOPE_PRIVATE, SCOPE_DIM, SCOPE_FRIEND};
typedef enum Boolean {FALSE = 0, TRUE = 1};

class PostParser
{
public:
    PostParser();
    ~PostParser();

	void ReportError(ParserException *pException);
	
    void VarDecl(BasicScope Scope, RefToken VarName, RefToken DataType,
                 Boolean WithEvents, Boolean AsNew);
};

#endif