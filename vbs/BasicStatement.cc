/*
 * Implementation of the BasicStatement class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicStatement.h"
#include "BasicContext.h"

BasicStatement::BasicStatement(void *pBasicContext)
/*
 * Constructor
 *
 *     params : pBasicContext   ==  Pointer to a BasicContext object.
 *    returns : none
 */
{
    this->m_pBasicContext = pBasicContext;
}

BasicStatement::~BasicStatement()
/*
 * Destructor
 *
 *     params : none
 *    returns : none
 */
{
}

BOOLEAN BasicStatement::ProcessStatement(char *strStatement)
/*
 * Attempt to process and compile source code statement
 *
 *     params : strStatement    ==  Source code statement to process
 *    returns : TRUE if handled, FALSE if not handled.
 */
{
    //CleanupString();
    return this->IsTokenMatch();
}

BOOLEAN BasicStatement::IsTokenMatch()
/*
 * Returns TRUE if statement matches the token for the BasicStatement
 *  instance.
 *
 *     params : none
 *    returns : See description above
 */
{
    /***Temporary until we make it really do something***/
    return TRUE;
}

/* end of BasicStatement.cpp */