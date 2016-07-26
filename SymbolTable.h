//
//  SymbolTable.h
//  SemanticAnalyzer
//
//  Created by Kyle Liu on 3/17/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#ifndef SymbolTable_h
#define SymbolTable_h

#include <stdio.h>
#include <string>
#include <stack>
#include <list>
#include <iostream>

using std::string;
using std::list;
using std::cout;
using std::endl;

enum categories {Function, Variable, Array};

struct scopeNumber
{
    int number;
    bool isAccessible;
};

/* This class is responsible for the scope number assignments */
class ScopeAssignmentProtocol
{
public:
    ScopeAssignmentProtocol();
    int getNextAvailableScope();
    void scopeEnded();
    int getCurrentScope();
    
private:
    int getLastAssignedNumber();
    list<scopeNumber> assignmentList;
    int lastAssigned;
};

class Symbol
{
public:
    Symbol(string name, string type, int scope, int category, int pCount, string paramSig);
    void setCategory(int category);
    void setName(string name);
    void setScope(int scope);
    void setType(string type);
    void setParameterCount(int count);
    void setParameterSignature(string paramSig);
    int getCategory();
    string getName();
    int getScope();
    int getParameterCount();
    string getType();
    string getParameterSignature();
    
private:
    int category;
    string name;
    int scope;
    string type;
    int parameterCount;
    string paramSignature;
};

class SymbolTable
{
public:
    SymbolTable();
    SymbolTable(int defaultScope = 0);
    void insert(string name, string type, int scope, int category, int pCount = 0, string paramSig = "");
    bool search(string name);                               //returns true if symbol is found in global
    bool search(string name, int category);  
    bool search(string name, Symbol * holder);
    bool search(string name, int category, Symbol * holder);
    bool search(string name, string paramSignature);            //searches for a ID with that specific parameter signature
    bool search(string name, string paramSignature, Symbol * holder);
    int countSymbol(string name, int category);
    void printTable();
    int getDefaultScope();

private:
    list<Symbol> SymbolList;
    int defaultScope;
};

#endif /* SymbolTable_h */
