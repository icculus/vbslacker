/*
 * Implementation of the BasicCompiler class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicCompiler.h"
#include "ContextObject.h"
#include "BasicContext.h"

#define MAX_LINE_SIZE   2048    // A line of basic code can't be more than 2K
#define FILE_TYPE_COUNT 4       // Number of file types we're looking for

void BasicCompiler::BasicCompiler(char *strFileName, BasicContext *pBasicContext)
/*
 * Constructor
 *
 *     params : strFileName     == Full path and name of file.
 *              pBasicContext   == Context of compile process.
 *    returns : none
 */
{
    if(pBasicContext == NULL)
        m_pBasicContext = new BasicContext;
        pBasicContext = m_pBasicContext;
    else
        m_pBasicContext = NULL;

    this->Compile(strFileName, pBasicContext);
}

void BasicCompiler::~BasicCompiler()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
                                // Delete context if we created it in the
                                //  constructor.
    if(m_pBasicContext != NULL)
        delete m_pBasicContext;
}

BASIC_FILE_TYPE BasicCompiler::GetFileType(char *strFileName)
/*
 * Figures out the filetype of the passed filepath based on the extension.
 *
 *     params : strFileName     == Full name and path of file
 *    returns : Type of file, returns BASIC_FILE_UNKNOWN if unknown.
 */
{
    char *p;                    // Pointer to character index

                                // Each index in the list cooresponds with
                                //  the appropriate file type constant.
    char *FileType[] = {"VBP","FRM,"BAS","CLS"};
    char **pFile;               // Pointer to file extension in FileType array
    short i;                    // Generic counter
                                    
                                // Set starting index to end of string
    p = strFileName + strlen(strFileName);

    while(p != strFileName)     // Only loop through until we hit start
    {
        p--;
        if(*p = '.')            // If we've hit the beginning of the file ext.
        {
            p++;                // Increment just after '.'

            pFile = FileType;   // Set to beginning of array lookup list

                                // Loop through array
            for(i = 0; i < FILE_TYPE_COUNT; i++)
            {
                                // If extension is equal to one in array
                if(strcmpi(*pFile, p) == 0)
                    ReturnValue = i;

                pFile++;        // Increment to next array element
            }
        }
    }

    if(p == strFileName)        // If we didn't find an extension
        ReturnValue = BASIC_FILE_UNKNOWN;

    return ReturnValue;
}

void BasicCompiler::Compile(char *strFileName, BasicContext *pBasicContext)
/*
 * Compiles the module specified by 'strFileName'.
 *
 *     params : strFileName     == Full path and name of file.
 *              pBasicContext   == Context of compile process.  This is set to
 *                                 NULL if compile starts at specified module.
 *    returns : none
 */
{
    BASIC_FILE_TYPE FileType;   // Type of file passed to compiler
    CONTEXT_TYPE    Context;    // Context constant associated with file type
    FILE            *pStream;   // Input file stream for basic module
                                // Line of basic code read from input file
    char            strBasicStatement[MAX_LINE_SIZE];
                                // Basic statement that matches code line.
    BasicStatement  *pBasicStatement;

                                // Create BasicContext object if they didn't
                                //  pass one.  This means we're starting a
                                //  new compile.
    if(pBasicContext == NULL)
        pBasicContext = new BasicContext();

                                // Get type of file we're compiling
    FileType = this->GetFileType(strFileName);

                                // Find context type associate with file type
    switch(FileType)
    {
        case BASIC_FILE_VBP:
            Context = CONTEXT_PROJECT;
            break;
        case BASIC_FILE_FRM:
            Context = CONTEXT_MODULE_FRM;
            break;
        case BASIC_FILE_BAS:
            Context = CONTEXT_MODULE_BAS;
            break;
        case BASIC_FILE_CLS:
            Context = CONTEXT_MODULE_CLS;
        default:
            pBasicContext->BasicErrors->AddError(BASIC_ERROR_UNRECOGNIZED_FILE);
    };

    if((fStream = fopen(strFileName, "rt")) == NULL)
        pBasicContext->BasicErrors->AddError(BASIC_ERROR_FILE_OPEN);
    else
    {
                                // Set context to the passed file
        pBasicContext->EnterContext(Context, strFileName);

        while(!feof(pStream))
        {
                                // Read a line in from the file
            if(fgets(strStatement, MAX_LINE_SIZE, pStream) != NULL)
            {
                                // Put statement through compiler.
                pBasicContext->m_pBasicStatements->
                    ProcessStatement(strStatement);
            }
                                // If line wasn't read, either EOF was hit or
                                //  we've got an error.
            else if(!feof(pStream))
                pBasicContext->BasicErrors->AddError(BASIC_ERROR_FILE_READ);
        }
    }
}

/* end of BasicCompiler.cpp */