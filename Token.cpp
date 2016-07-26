//
//  Token.cpp
//  LexicalAnalyzer
//
//  Created by Kyle Liu on 2/15/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include "Token.h"

Token::Token()
{
    lexeme = "";
    datatype = "";
    prevTokPtr = NULL;
    nextTokPtr = NULL;
    
}

Token::Token(string name, string type, Token* prev = NULL, Token* next = NULL)
{
    lexeme = name;
    datatype = type;
    prevTokPtr = prev;
    nextTokPtr = next;
}

void Token::setLexeme(string incoming)
{
    lexeme = incoming;
}

string Token::getLexeme()
{
    return lexeme;
}

void Token::setDataType(string incoming)
{
    datatype = incoming;
}

string Token::getDataType()
{
    return datatype;
}

void Token::setPrevTokPtr(Token* incomingTokPtr)
{
    prevTokPtr = incomingTokPtr;
}

Token* Token::getPrevTokPtr()
{
    return prevTokPtr;
}

void Token::setNextTokPtr(Token* incomingTokPtr)
{
    nextTokPtr = incomingTokPtr;
}

Token* Token::getNextTokPtr()
{
    return nextTokPtr;
}