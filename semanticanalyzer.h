//
//  semanticanalyzer.hpp
//  SemanticAnalyzer
//
//  Created by Kyle Liu on 3/27/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#ifndef semanticanalyzer_h
#define semanticanalyzer_h


#include <stdio.h>
#include <iostream>

#include "Token.h"
#include "SymbolTable.h"
#include "TokenList.h"

using std::endl;
using std::cout;

class SemanticAnalyzer
{
public:
    SemanticAnalyzer(Token* startPtr, list<SymbolTable> table);
    void execute();
    
private:
    void mainCheck();
    void reject(int code);
    void returnCheck();
    string operationCheck(Token* expStartPtr);
    Token* tokenListStart;
    list<SymbolTable> symTabList;
    
};


#endif /* semanticanalyzer_h */
