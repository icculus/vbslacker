/*
 *  Declarations for the BasicStatement class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICSTATEMENT_H_
#define _INCLUDE_BASICSTATEMENT_H_

#include "boolean.h"

class BasicStatement
{
    public:
                                // Derived classes should override this
                                //  constructor.
                                // It also must call the base class
                                //  constructor FIRST.
        BasicStatement(void *pBasicContext);
        ~BasicStatement();

                                // Derived classes should override this function.
                                //  The new function must return TRUE if the
                                //  statement is handled by that function, EVEN
                                //  if the statement still generates a compiler
                                //  error.
                                // You must call the base class version of
                                //  this function FIRST.
        virtual BOOLEAN ProcessStatement(char *strStatement);
    private:
        virtual BOOLEAN IsTokenMatch();

        void *m_pBasicContext;
};

#endif

/* end of BasicStatement.h */