/*
 *  Main module - Execution starts here.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicCompiler.h"
#include <stdio.h>

void MyEventHandler(BASIC_EVENT Event, void *pData)
{
    BasicError *pBasicError;

    switch(Event)
    {
        case BASIC_EVENT_ERROR:
            pBasicError = (BasicError *)pData;
            printf("%s\n", pBasicError->GetMessage());
            break;
    }
}

int main(int argc, char *argv[])
{
    BasicCompiler *pMyCompiler;

    if(argc > 1)
       pMyCompiler = new BasicCompiler(argv[1], &MyEventHandler);
    else
       printf("No argument specified.\n");
       
    return 0;
}

/* end of main.cpp */