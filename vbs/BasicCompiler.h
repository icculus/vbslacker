/*
 * Declarations for the BasicCompiler class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSCOMPILER_H_
#define _INCLUDE_BASICSCOMPILER_H_

// File type of a Basic module
#define BASIC_FILE_VBP      0x00
#define BASIC_FILE_FRM      0x01
#define BASIC_FILE_BAS      0x02
#define BASIC_FILE_CLS      0x03
#define BASIC_FILE_UNKNOWN  0xFF

typedef BASIC_FILE_TYPE     short

class BasicCompiler
{
    public:
        void BasicCompiler(char *strFileName, BasicContext *pBasicContext = NULL);
        void ~BasicCompiler();

    private:
        void Compile(char *strFileName, BasicContext *pBasicContext);
        BASIC_FILE_TYPE GetFileType(char *strFileName);

        BasicContext *m_pBasicContext;
};

#endif

/* end of BasicCompiler.h */