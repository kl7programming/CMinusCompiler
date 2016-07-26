//
//  SymbolTable.cpp
//  SemanticAnalyzer
//
//  Created by Kyle Liu on 3/17/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include "SymbolTable.h"

ScopeAssignmentProtocol::ScopeAssignmentProtocol()
{
    lastAssigned = 0;
    scopeNumber numberNode;
    numberNode.number = lastAssigned;
    numberNode.isAccessible = true;
    assignmentList.insert(assignmentList.end(), numberNode);
}

int ScopeAssignmentProtocol::getNextAvailableScope()
{
    scopeNumber numberNode;
    numberNode.number = getLastAssignedNumber()+1;
    lastAssigned++;
    numberNode.isAccessible = true;
    assignmentList.insert(assignmentList.end(), numberNode);
    return assignmentList.back().number;
}

void ScopeAssignmentProtocol::scopeEnded()
{
    for(list<scopeNumber>::reverse_iterator i = assignmentList.rbegin(); i != assignmentList.rend(); i++)
    {
        if(i->isAccessible)
        {
            i->isAccessible = false;
            break;
        }
    }
}

int ScopeAssignmentProtocol::getCurrentScope()
{
    for (list<scopeNumber>::reverse_iterator i = assignmentList.rbegin(); i != assignmentList.rend(); i++)
    {
        if(i->isAccessible)
        {
            return i->number;
        }
    }
    
    return 0;
}

int ScopeAssignmentProtocol::getLastAssignedNumber()
{
    return lastAssigned;
}

Symbol::Symbol(string name, string type, int scope, int category, int pCount, string paramSig)
{
    setName(name);
    setType(type);
    setScope(scope);
    setCategory(category);
    setParameterCount(pCount);
    setParameterSignature(paramSig);
}

void Symbol::setCategory(int category)
{
    this->category = category;
}

void Symbol::setName(string name)
{
    this->name = name;
}

void Symbol::setScope(int scope)
{
    this->scope = scope;
}

void Symbol::setType(string type)
{
    this->type = type;
}

void Symbol::setParameterCount(int count)
{
    this->parameterCount = count;
}

void Symbol::setParameterSignature(string paramSig)
{
    this->paramSignature = paramSig;
}

int Symbol::getCategory()
{
    return category;
}

string Symbol::getName()
{
    return name;
}

int Symbol::getScope()
{
    return scope;
}

string Symbol::getType()
{
    return type;
}

int Symbol::getParameterCount()
{
    return parameterCount;
}

string Symbol::getParameterSignature()
{
    return paramSignature;
}

SymbolTable::SymbolTable()
{
    
}

SymbolTable::SymbolTable(int defaultScope)
{
    this->defaultScope = defaultScope;
}

void SymbolTable::insert(string name, string type, int scope, int category, int pCount, string paramSig)
{
    Symbol newSym(name, type, scope, category, pCount, paramSig);
    SymbolList.insert(SymbolList.end(), newSym);
}

bool SymbolTable::search(string name)
{
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        if(sym->getName() == name && sym->getScope() == defaultScope)
        {
            return true;
        }
    }
    
    return false;
}

bool SymbolTable::search(string name, int category)
{
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        if(sym->getName() == name && sym->getCategory() == category)
        {
            return true;
        }
    }
    
    return false;
}

bool SymbolTable::search(string name, Symbol * holder)
{
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        if(sym->getName() == name && sym->getScope() == defaultScope)
        {
            *holder = *sym;

            return true;
        }
    }
    
    return false;
    
}

bool SymbolTable::search(string name, int category, Symbol * holder)
{
    
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        if((sym->getName() == name) && (sym->getCategory() == category))
        {
            *holder = *sym;
            return true;
        }
    }
    
    return false;
    
}

bool SymbolTable::search(string name, string paramSignature)
{
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        if(sym->getName() == name && sym->getParameterSignature() == paramSignature)
        {
            return true;
        }
    }
    
    return false;

}

bool SymbolTable::search(string name, string paramSignature, Symbol *holder)
{
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        if(sym->getName() == name && sym->getParameterSignature() == paramSignature)
        {
            *holder = *sym;
            return true;
        }
    }
    
    return false;
}

int SymbolTable::countSymbol(string name, int category)
{
    int count = 0;
    
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        if(sym->getName() == name && sym->getCategory() == category)
        {
            count++;
        }
    }
    
    return count;
}

void SymbolTable::printTable()
{
    for(list<Symbol>::iterator sym = SymbolList.begin(); sym != SymbolList.end(); sym++)
    {
        cout<<sym->getType()<<" \t "<<sym->getName()<<" \t "<<sym->getScope();
        if(sym->getCategory() == Variable)
        {
            cout<<"\tVariable"<<endl;
        }
        else
        {
            cout<<"\tFunction"<<endl;
        }
    }
    
}

int SymbolTable::getDefaultScope()
{
    return defaultScope;
}