/*
 * Declarations for the AssignmentStatement class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_ASSIGNSTATEMENT_H_
#define _INCLUDE_ASSIGNSTATEMENT_H_

#include "BasicStatement.h"
#include "BasicContext.h"

class AssignStatement : public BasicStatement
{
    public:
        AssignStatement(void *pBasicContext);
        ~AssignStatement();
    private:
};

#endif

/* end of BasicStatement.h */