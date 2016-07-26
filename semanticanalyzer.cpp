//
//  semanticanalyzer.cpp
//  SemanticAnalyzer
//
//  Created by Kyle Liu on 3/27/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include "semanticanalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(Token* startPtr, list<SymbolTable> table)
{
    tokenListStart = startPtr;
    symTabList = table;
}

void SemanticAnalyzer::execute()
{
    mainCheck();
    
    //cout<<"ACCEPT"<<endl;
}

void SemanticAnalyzer::reject(int code)
{
    cout<<"REJECT"<<endl;
    exit(code);
}

void SemanticAnalyzer::mainCheck()
{
    int mainCount = 0;
    
    for(list<SymbolTable>::iterator i = symTabList.begin(); i != symTabList.end(); i++)
    {
        if(i->getDefaultScope() == 0)
        {
            mainCount = i->countSymbol("main", Function);
        }
    }
    
    if(mainCount == 0)
    {
        //cout<<"No main found"<<endl;
        reject(3);
    }
    
    if(mainCount > 1)
    {
        //cout<<"Multiple main found"<<endl;
        reject(3);
    }
}