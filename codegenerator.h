//
//  codegenerator.hpp
//  CodeGenerator
//
//  Created by Kyle Liu on 4/19/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#ifndef codegenerator_h
#define codegenerator_h
#define NUMW 3
#define OPW 10
#define ARGW 15
#define RESW 10


#include <stdio.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <list>

#include "Token.h"

using std::stringstream;
using std::string;
using std::cout;
using std::endl;
using std::setw;
using std::right;
using std::list;

class Quad
{
public:
    Quad(int lineNumber, string op = "", string arg1 = "", string arg2 = "", string result = "");
    inline void setNumber(int num){ this->lineNumber = num;};
    inline void setOp(string newOp){ this->op = newOp;};
    inline void setArg1(string newArg){ this->arg1 = newArg;};
    inline void setArg2(string newArg){ this->arg2 = newArg;};
    inline void setResult(string newResult){ this->result = newResult;};
    inline int getNumber(){return lineNumber;};
    inline string getOp(){return op;};
    inline string getArg1(){return arg1;};
    inline string getArg2(){return arg2;};
    inline string getResult(){return result;};
    string toString();
    void print();
    
    
    
private:
    int lineNumber;
    string op;
    string arg1;
    string arg2;
    string result;
};

class CodeGenerator
{
public:
    CodeGenerator();
    void insert(string newOp, string newArg1, string newArg2, string newRes);
    void printList();
    inline int getTempCounter(){return tempCounter++;}
    inline int getCurLineNumber(){return curLineNum;}
    void modify(int lineNumber, string newOp, string newArg1, string newArg2, string newRes);
    
    
private:
    list<Quad> quadList;
    int curLineNum;
    int tempCounter;
};

#endif /* codegenerator_h */
