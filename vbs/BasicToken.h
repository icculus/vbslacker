/*
 * Declarations for the BasicToken class.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICTOKEN_H_
#define _INCLUDE_BASICTOKEN_H_

enum BASIC_TOKEN_TYPE {
    BASIC_TOKEN_STRING              =   0x00,
    BASIC_TOKEN_EXPRESSION          =   0x01,
    BASIC_TOKEN_GENERIC             =   0x02
};

class BasicToken
{
    public:
        BasicToken(BASIC_TOKEN_TYPE TokenType, long lLineNumber);
        ~BasicToken();
    private:
                                // Type of token this is...duh.
        BASIC_TOKEN_TYPE m_TokenType;
        long m_lLineNumber;     // Line token was on in basic code.

};

#endif
/* end of BasicToken.h */