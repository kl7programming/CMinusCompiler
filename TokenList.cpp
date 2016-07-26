//
//  TokenList.cpp
//  LexicalAnalyzer
//
//  Created by Kyle Liu on 2/15/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include "TokenList.h"

TokenList::TokenList()
{
    length = 0;
    startTok = NULL;
    endTok = NULL;
    prevTok = NULL;
}

TokenList::~TokenList()
{
    /*Token* curTok = getStartToken();
    
    while(curTok != 0)
    {
        Token* nextPtr = curTok->getNextTokPtr();
        delete curTok;
        curTok = nextPtr;
    }
    
    endTok = 0;*/
}

void TokenList::insertToken(string name, string type)
{
    if(length == 0)
    {
        Token* newToken = new Token(name, type, NULL, NULL);
        endTok = newToken;
        startTok = newToken;
    }
    else
    {
        Token* newToken = new Token(name, type, endTok, NULL);
        endTok->setNextTokPtr(newToken);
        endTok = newToken;
    }
    
    length++;
    
}

Token* TokenList::getStartToken()
{
    return startTok;
}

bool TokenList::isEmpty()
{
    if(startTok == NULL && endTok == NULL)
    {
        return true;
    }
    
    return false;
}