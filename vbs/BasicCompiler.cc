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
    this->m_lLineNumber = 0;

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
                                // Not a valid extension
            return BASIC_FILE_UNKNOWN;
        }
    }
                                // If we didn't find an extension
    return BASIC_FILE_UNKNOWN;
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
            break;
        default:                // File was not valid
            this->m_pBasicContext->GetErrors()->
                AddError(BASIC_ERROR_UNRECOGNIZED_FILE);
            return;             // Bail out, don't process the file
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
                                // Read next statement from file
            this->ProcessStatement();
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
            {
                bWhiteSpace = TRUE;
                break;
            }
            p2++;
        }

        if(bWhiteSpace)
            p--;
        else
            *(++p) = '\x0';
    } while(bWhiteSpace);
}

void BasicCompiler::ProcessStatement()
/*
 * Reads and process a valid basic statement.  This differs from just a line
 *  of text, in the fact that statements can be spread over multiple lines
 *  using the '_' character.
 *
 *     params : none
 *    returns : none.  Raises an error in BasicError if there's problems.
 */
{
    char *p;
    BOOLEAN bDone;
    BOOLEAN bOk;                // Did we process line okay?
    long lFirstLine;            // First line of this statement

    p = this->m_strBasicStatement;
    *p = '\x0';                 // Make string 0 length to start out.
    bDone = FALSE;
    bOk = FALSE;
    lFirstLine = this->m_lLineNumber + 1;

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
            bDone = TRUE;
            bOk = FALSE;
        }
        else                    // It's all good
        {
                                // Keep track of current line number
            this->m_lLineNumber++;
                                // Cleanup the string we've got so far
            this->CleanupString(p);

            p += strlen(p);     // Go to last character in string

                                // If last character is not a "continue" then
                                //  we're done.  Otherwise, we'll read the next
                                //  line and append it to our current line.
                                // We'll keep the '_' character in there so
                                //  that the BasicStatement object that we
                                //  pass our string to will know that it's a
                                //  new line.
            if(*(p-1) != '_')
            {
                bDone = TRUE;
                bOk = TRUE;
            }
        }
    }

    if(bOk)                     // If statement was process successfully.
    {
                                // Put statement through compiler, passing
                                //  the line number of the start of the
                                //  statement.
        this->m_pBasicContext->GetStatements()->
            ProcessStatement(this->m_strBasicStatement, lFirstLine);

        /***TEMPORARY***/
        printf("%s\n", this->m_strBasicStatement);
    }
}
/* end of BasicCompiler.cpp */