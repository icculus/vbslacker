/*
 *  Declarations for the BasicStatements class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSTATEMENT_H_
#define _INCLUDE_BASICSTATEMENT_H_

#include "BasicStatement.h"
#include "BasicContext.h"


class BasicStatements
{
    public:
        void BasicStatements(BasicContext *pContext);
        void ~BasicStatements();

        void ProcessStatement(char *strStatement);
    private:
        BasicContext *m_pContext;
        BasicStatement *m_pBasicStatements[];
};

#endif

/* end of BasicStatement.h */