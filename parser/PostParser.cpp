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

void PostParser::VarDecl(BasicScope Scope, RefToken VarName, RefToken DataType,
Boolean WithEvents, Boolean AsNew)
{
   char strDataType[64];
   if(DataType == NULL)
      strcpy(strDataType, "Variant");
   else
      strcpy(strDataType, DataType->getText().data());
      
   printf("Declared %s as type %s\n", VarName->getText().data(), strDataType);
   if(WithEvents) printf("Declared WithEvents\n");
   if(AsNew) printf("Object created\n");
}