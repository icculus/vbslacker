/*
 * Implementation of the BasicCompiler class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicCompiler.h"
#include "ContextObject.h"
#include "BasicContext.h"
#include <string.h>

#define MAX_LINE_SIZE   2048    // A line of basic code can't be more than 2K
#define FILE_TYPE_COUNT 4       // Number of file types we're looking for

                                // All these character codes are considered
                                //  to be whitespace
#define TAB                     0x09
#define CR                      0x0D
#define LF                      0x0A
#define SPACE                   0x20
#define WHITESPACE_COUNT        4

BasicCompiler::BasicCompiler(char *strFileName, BasicContext *pBasicContext)
/*
 * Constructor
 *
 *     params : strFileName     == Full path and name of file.
 *              pBasicContext   == Context of compile process.
 *    returns : none
 */
{
    if(pBasicContext == NULL)
    {
        this->m_pBasicContext = new BasicContext;
        this->m_bCreatedBasicContext = TRUE;
    }
    else
    {
        this->m_pBasicContext = pBasicContext;
        this->m_bCreatedBasicContext = FALSE;
    }
                                // Allocate space for source code line
    this->m_strBasicStatement = new char[MAX_LINE_SIZE];

    this->Compile(strFileName);
}

BasicCompiler::BasicCompiler(char *strFileName, BASIC_EVENT_HANDLER pEventHandler)
/*
 * Constructor 2 - This constructor calls the first constructor, and sets
 *  the event handler member.
 *
 *     params : strFileName     == Full path and name of file.
 *              pEventHandler   == Pointer to event handler
 *    returns : none
 */
{
    this->m_pBasicContext = new BasicContext(pEventHandler);
    BasicCompiler(strFileName, this->m_pBasicContext);
                                // The first constructor will set this to FALSE
                                //  because it thinks it didn't create it.  But
                                //  since we created it here, we'll have to
                                //  make sure it's deleted at deconstruction.
    this->m_bCreatedBasicContext = TRUE;
}

BasicCompiler::~BasicCompiler()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
                                // Delete context if we created it in the
                                //  constructor.
    if(this->m_bCreatedBasicContext)
        delete this->m_pBasicContext;
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
    char *FileType[] = {"VBP","FRM","BAS","CLS"};
    char **pFile;               // Pointer to file extension in FileType array
    short i;                    // Generic counter
                                // Type to return
    BASIC_FILE_TYPE ReturnValue;

                                    
                                // Set starting index to end of string
    p = strFileName + strlen(strFileName);

    while(p != strFileName)     // Only loop through until we hit start
    {
        p--;
        if(*p == '.')           // If we've hit the beginning of the file ext.
        {
            p++;                // Increment just after '.'

            pFile = FileType;   // Set to beginning of array lookup list

                                // Loop through array
            for(i = 0; i < FILE_TYPE_COUNT; i++)
            {
                                // If extension is equal to one in array
                if(strcasecmp(*pFile, p) == 0)
                    /***Set ReturnValue instead (better coding)***/
                    return (BASIC_FILE_TYPE)i;

                pFile++;        // Increment to next array element
            }
        }
    }

    if(p == strFileName)        // If we didn't find an extension
        ReturnValue = BASIC_FILE_UNKNOWN;

    return ReturnValue;
}

void BasicCompiler::Compile(char *strFileName)
/*
 * Compiles the module specified by 'strFileName'.
 *
 *     params : strFileName     == Full path and name of file.
 *    returns : none
 */
{
    BASIC_FILE_TYPE FileType;   // Type of file passed to compiler
    CONTEXT_TYPE    Context;    // Context constant associated with file type

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
            this->m_pBasicContext->GetErrors()->
                AddError(BASIC_ERROR_UNRECOGNIZED_FILE);
    };

    if((this->m_pStream = fopen(strFileName, "rt")) == NULL)
        this->m_pBasicContext->GetErrors()->AddError(BASIC_ERROR_FILE_OPEN);
    else
    {
                                // Set context to the passed file
        this->m_pBasicContext->EnterContext(Context, strFileName);

                                // Loop until end of file, or a request
                                //  for compile termination has occured.
        while(!feof(this->m_pStream) && 
        !this->m_pBasicContext->GetTerminateFlag())
        {
                                // Read a line in from the file
            if(this->GetStatement())
            {
                /***TEMPORARY***/
                printf("%s\n", this->m_strBasicStatement);
                                // Put statement through compiler.
                this->m_pBasicContext->GetStatements()->
                    ProcessStatement(this->m_strBasicStatement);
            }
        }
                                // Close file since we're done with it.
        fclose(this->m_pStream);
    }
}

void BasicCompiler::CleanupString(char *strStatement)
/*
 * Just cleans up excess whitespace before and after the statement.
 *
 *     params : strStatement    ==  Statement to process.  Note that this does
 *                                  not need to be a complete/valid statement.
 *    returns : none
 */
{
                                // Lookup table for whitespace characters
    char WhiteSpace[WHITESPACE_COUNT] = {TAB, CR, LF, SPACE};    
    short i;                    // Generic counters
    char *p, *p2;               // Pointer into strStatement
    BOOLEAN bWhiteSpace;        // TRUE if on a whitespace character

    p = strStatement;
    do
    {
        p2 = WhiteSpace;
        bWhiteSpace = FALSE;
        for(i = 0; i < WHITESPACE_COUNT; i++)
        {
                                // If TRUE, it's just whitespace
            if(*p == *p2)
            {
                bWhiteSpace = TRUE;
                break;
            }
            p2++;
        }

        if(bWhiteSpace)
            p++;
    } while(bWhiteSpace);
    strcpy(strStatement, p);    // Cut off the beginning whitespace (if any)

                                // Go to the end of the statement
    p = strStatement + strlen(strStatement) - 1;
    do
    {
        p2 = WhiteSpace;
        bWhiteSpace = FALSE;
        for(i = 0; i < WHITESPACE_COUNT; i++)
        {
                                // If TRUE, it's just whitespace
            if(*p == *p2)
                bWhiteSpace = TRUE;
        }

        if(bWhiteSpace)
            p--;
        else
            *(p++) = '\x0';
    } while(bWhiteSpace);
}

BOOLEAN BasicCompiler::GetStatement()
/*
 * Reads in a valid line of BASIC source code.  In a nutshell, it's just like
 *  an 'fgets' statement except in accounts for lines ending with '_', which
 *  means to continue processing the next line as if it were the same
 *  statement.
 *
 *     params : none
 *    returns : TRUE if successful, otherwise FALSE on error.
 */
{
    char *p;
    BOOLEAN bDone;
    BOOLEAN bReturnValue;

    p = this->m_strBasicStatement;
    *p = '\x0';                 // Make string 0 length to start out.
    bDone = FALSE;
    bReturnValue = TRUE

    while(!bDone)
    {
                                // Read a line...if it errors, throw an error
                                //  into the BasicErrors collection.
        if(fgets(p, MAX_LINE_SIZE - strlen(this->m_strBasicStatement), 
        this->m_pStream) == NULL && !feof(this->m_pStream))
        {
            this->m_pBasicContext->GetErrors()->AddError(BASIC_ERROR_FILE_READ);
                                // Flag to stop parsing since we got a fatal
                                //  error.
            this->m_pBasicContext->SetTerminateFlag();
                                // Return that we failed
            bReturnValue = FALSE;
            bDone = TRUE;
        }
        else                    // It's all good
        {
                                // Cleanup the string we've got so far
            this->CleanupString(p);

            p += strlen(p) - 1; // Go to last character in string

                                // If last character is not a "continue" then
                                //  we're done.  Otherwise, we'll read the next
                                //  line and append it to our current line.
            if(*p != '_')       
                bDone = TRUE;
        }
    }

    return bReturnValue;
}
/* end of BasicCompiler.cpp */