/*
 *  Declarations for the BasicStatements class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSTATEMENTS_H_
#define _INCLUDE_BASICSTATEMENTS_H_

#include "BasicStatement.h"

class BasicStatements
{
    public:
        BasicStatements(void *pBasicContext);
        ~BasicStatements();

        void ProcessStatement(char *strStatement, long lLineNumber);
    private:
        void *m_pBasicContext;
        BasicStatement **m_pBasicStatements;
};

#endif

/* end of BasicStatements.h */