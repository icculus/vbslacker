/*
 * Implementation of the BasicStatements class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */


#include "BasicStatements.h"
#include "AssignStatement.h"
#include "BasicContext.h"

#define NUMBER_OF_BASIC_STATEMENTS          1

                                // Macro that's only valid in BasicStatements'
                                //  constructor.  It's been created to make
                                //  adding BasicStatement objects a bit easier.
#define REGISTER_STATEMENT(ClassObject) *p++ = new ClassObject(this->m_pBasicContext)

BasicStatements::BasicStatements(void *pBasicContext)
/*
 * Constructor
 *
 *     params : pBasicContext   ==  Pointer to a BasicContext object.
 *    returns : none
 */
{
    BasicStatement **p;         // Pointer to next index in array

                                // Save the context
    this->m_pBasicContext = pBasicContext;

                                // Allocate array to fit all derived
                                //  BasicStatement objects.
    this->m_pBasicStatements = 
        new BasicStatement*[NUMBER_OF_BASIC_STATEMENTS];
                                // Set start index
    p = this->m_pBasicStatements;


    /*
     * The order of these REGISTER_STATEMENTs is very important.  It determines
     *  the order that they will be processed.  Some statements may have tokens
     *  which fit multiple conditions.  For example, if the IfStatement appears
     *  after the FunctionCallStatement then a valid 'if' statement would be
     *  considered a function call.
     */
                                // Create instances of each type of
                                //  BasicStatement class.
    REGISTER_STATEMENT(AssignStatement);
}

BasicStatements::~BasicStatements()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
    short i;                    // Generic loop counter
    BasicStatement **p;         // Pointer to current BasicStatement object

                                // Loop through our BasicStatement array
                                //  and delete all the ones we created.
    p = this->m_pBasicStatements;
    for(i = 0; i < NUMBER_OF_BASIC_STATEMENTS; i++)
    {
        delete (*p);
    }

                                // Now remove the array.
    delete this->m_pBasicStatements;
}

void BasicStatements::ProcessStatement(char *strStatement)
/*
 * Attempt to process statement through one of the BasicStatement objects
 *  in the collection.
 *
 *     params : strStatement    ==  Basic source code statement to process.
 *    returns : none
 */
{
    short i;                    // Generic loop counter
    BasicStatement **p;         // Pointer to current BasicStatement object
    BOOLEAN IsProcessed;        // Did statement get processed?

    IsProcessed = FALSE;

                                // Loop through our BasicStatement array
    p = this->m_pBasicStatements;
    for(i = 0; i < NUMBER_OF_BASIC_STATEMENTS; i++)
    {
                                // If statement was processed by current
                                //  BasicStatement object, then we'll exit
                                //  the loop right away.
        if((*p)->ProcessStatement(strStatement))
        {
            IsProcessed = TRUE;
            break;
        }
    }
                                // If statement was not processed then we'll
                                //  throw a 'syntax error' message.
    if(!IsProcessed)            
    {
        ((BasicContext *)this->m_pBasicContext)
            ->GetErrors()->AddError(BASIC_ERROR_SYNTAX);
    }
}

/* end of BasicStatements.cpp */