/* PostParser.cpp - Implementation of the PostParser class
 *
 * Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include "PostParser.h"
#include <stdio.h>
#include <string.h>

// Constructor
PostParser::PostParser()
{
}

// Destructor
PostParser::~PostParser()
{
}

void PostParser::ReportError(ParserException *pException)
/* Description
 *      Called by parser grammar to report any exceptions that are raised
 *      as a result of parsing.
 * Parameters
 *      pException  -   Pointer to a ParserException object
 * Returns
 *      none
 */
{
    // We'll just report it generically for now.
    printf("<<<%s>>>", pException->toString().data());
}

void PostParser::VarDecl(BasicScope Scope, RefToken VarName, RefToken DataType,
                         Boolean WithEvents, Boolean AsNew)
/* Description
 *      Declares a variable
 * Parameters
 *      Scope       -   Context scope of the variable
 *      VarName     -   Pointer to a token that represents the variable name.
 *      DataType    -   Pointer to a token that represents the data type.
 *      WithEvents  -   TRUE if variables was declared using "WithEvents"
 *      AsNew       -   TRUE if variable was also instantiated.
 * Returns
 *      none
 */
{
    char strDataType[64];
    if(DataType == NULL)
        strcpy(strDataType, "Variant");
    else
        strcpy(strDataType, DataType->getText().data());
      
    printf("Declared %s as type %s\n", VarName->getText().data(), strDataType);

    if(WithEvents)
        printf("Declared WithEvents\n");
    if(AsNew)
        printf("Object created\n");
}