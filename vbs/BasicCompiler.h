/*
 * Declarations for the BasicCompiler class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICCOMPILER_H_
#define _INCLUDE_BASICCOMPILER_H_

#include "BasicContext.h"
#include "boolean.h"
#include <stdio.h>

// File type of a Basic module
#define BASIC_FILE_VBP      0x00
#define BASIC_FILE_FRM      0x01
#define BASIC_FILE_BAS      0x02
#define BASIC_FILE_CLS      0x03
#define BASIC_FILE_UNKNOWN  0xFF

typedef short BASIC_FILE_TYPE;

class BasicCompiler
{
    public:
        BasicCompiler(char *strFileName, BasicContext *pBasicContext = NULL);
        BasicCompiler(char *strFileName, BASIC_EVENT_HANDLER pEventHandler);
        ~BasicCompiler();

    private:
        void Compile(char *strFileName);
        BASIC_FILE_TYPE GetFileType(char *strFileName);
        void ProcessStatement();
        void CleanupString(char *strStatement);

                                // Line of basic code read from input file
        char *m_strBasicStatement;
        BasicContext *m_pBasicContext;
        BOOLEAN m_bCreatedBasicContext;
        FILE *m_pStream;        // Input file stream for basic module

        long m_lLineNumber;     // Current line number we're on
};

#endif

/* end of BasicCompiler.h */