//
//  Token.h
//  LexicalAnalyzer
//
//  Created by Kyle Liu on 2/15/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#ifndef Token_hpp
#define Token_hpp

#include <stdio.h>
#include <string>

using std::string;


class Token
{
public:
    Token();
    Token(string, string, Token*, Token*);
    void setLexeme(string);
    string getLexeme();
    void setDataType(string);
    string getDataType();
    void setPrevTokPtr(Token*);
    Token* getPrevTokPtr();
    void setNextTokPtr(Token*);
    Token* getNextTokPtr();
    
private:
    string lexeme;
    string datatype;
    Token* prevTokPtr;
    Token* nextTokPtr;
};

#endif /* Token_h */
