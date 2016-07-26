//
//  codegenerator.cpp
//  CodeGenerator
//
//  Created by Kyle Liu on 4/19/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include "codegenerator.h"


Quad::Quad(int lineNumber, string op, string arg1, string arg2, string result)
{
    setNumber(lineNumber);
    setOp(op);
    setArg1(arg1);
    setArg2(arg2);
    setResult(result);
}

string Quad::toString()
{
    stringstream ss;
    
    ss<<right<<setw(NUMW)<<lineNumber;
    ss<<right<<setw(OPW)<<op;
    ss<<right<<setw(ARGW)<<arg1;
    ss<<right<<setw(ARGW)<<arg2;
    ss<<right<<setw(RESW)<<result;
    ss<<endl;
    
    return ss.str();
}

void Quad::print()
{
    cout<<right<<setw(NUMW)<<lineNumber;
    cout<<right<<setw(OPW)<<op;
    cout<<right<<setw(ARGW)<<arg1;
    cout<<right<<setw(ARGW)<<arg2;
    cout<<right<<setw(RESW)<<result;
    cout<<endl;
}

CodeGenerator::CodeGenerator()
{
    curLineNum = 0;
    tempCounter = 1;
}

void CodeGenerator::insert(string newOp, string newArg1, string newArg2, string newRes)
{
    Quad newQuad(curLineNum, newOp, newArg1, newArg2, newRes);
    quadList.push_back(newQuad);
    curLineNum++;
}

void CodeGenerator::modify(int lineNumber, string newOp, string newArg1, string newArg2, string newRes)
{
    for(list<Quad>::iterator i = quadList.begin(); i != quadList.end(); i++)
    {
        if(i->getNumber() == lineNumber)
        {
            if(newOp != "")
            {
                i->setOp(newOp);
            }
            if(newArg1 != "")
            {
                i->setArg1(newArg1);
            }
            if(newArg2 != "")
            {
                i->setArg2(newArg2);
            }
            if(newRes != "")
            {
                i->setResult(newRes);
            }
        }
    }
}

void CodeGenerator::printList()
{
    for(list<Quad>::iterator i = quadList.begin(); i != quadList.end(); i++)
    {
        i->print();
    }
}
