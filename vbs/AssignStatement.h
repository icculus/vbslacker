/*
 * Declarations for the AssignmentStatement class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSTATEMENT_H_
#define _INCLUDE_BASICSTATEMENT_H_

#include "BasicContext.h"

class AssignmentStatement : public BasicStatement
{
    public:
        void AssignStatement(BasicContext *pContext);
    private:
        short Compile()=0;
};

#endif

/* end of BasicStatement.h */