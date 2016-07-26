//
//  TokenList.h
//  LexicalAnalyzer
//
//  Created by Kyle Liu on 2/15/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#ifndef TokenList_hpp
#define TokenList_hpp

#include "Token.h"

#include <stdio.h>
#include <string>
#include <iostream>

using std::string;
using std::cout;

class TokenList
{
public:
    TokenList();
    ~TokenList();
    void insertToken(string, string);
    bool isEmpty();
    Token* getStartToken();
    
private:
    int length;
    Token* startTok;
    Token* prevTok;
    Token* endTok;
    
    
    
};

#endif /* TokenList_h */
