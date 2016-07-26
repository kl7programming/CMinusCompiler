//
//  parser.hpp
//  Parser
//
//  Created by Kyle Liu on 2/20/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include <stdio.h>
#include <stack>
#include <string>

#include "Token.h"
#include "TokenList.h"
#include "SymbolTable.h"
#include "codegenerator.h"

using std::stack;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::to_string;


class Parser
{
public:
    Parser();
    Parser(Token* tokenListStartPtr);
    void execute();
    void populateStack();                   //push all the tokens onto the stack
    void checkStackMatch();
    void reject(int rejcode);             //reject with exit codes to determine where the reject originate
    void printStack();                      //Debug Use
    void pushToDStack(string text);
    void buildSymbolTable();                  //build the symbol table
    void printSymbolTable();                  //for debug use, prints the entire symbol table
    list<SymbolTable> getSymbolTable();         //returns the symbol table
    void semanticReject(int rejcode);
    void returnCheck();
    
    /*
     Productions:
    */
    void Program();
    void DeclarationList();
    void DeclarationLoop();
    void TypeSpecifier();
    void Declaration();
    void oneDeclaration();
    void DeclarationType();
    void Params();
    void Parameter();
    void ParamLoop();
    void Param();
    void ParamType();
    void CompoundStatement();
    void LocalDeclarations();
    void VariableDeclaration();
    void StatementList();
    void Statement();
    void SelectionStatement();
    void ExpressionStatement();
    void ElseStatement();
    void IterationStatement();
    void ReturnStatement();
    void ReturnVar();
    void Expression();
    void VarType();
    void VarFollow();
    void IDFollow();
    void ExpressionFollow();
    void Var();
    void Relation();
    void RelOp();
    void AdditiveExpression();
    void AdditiveExpressionLoop();
    void AddOp();
    void Term();
    void TermLoop();
    void MulOp();
    void Factor();
    void Call();
    void Args();
    void ArgsList();
    void ArgListLoop();
    
    /*Code Generation*/
    void handleReturn(Token** start);
    void handleMath(Token** start);
    void handleFuncCall(Token** start);
    void handleArray(Token** start);
    bool isOperator(string test);
    bool isCompOperator(string test);
    bool precedence(string op1, string op2);
    
    
private:
    Token* tokenListStart;
    stack<Token> codeStack;
    stack<string> derivedStack;
    list<SymbolTable> symTabList;
    bool debugMode;      //set to true to display debug messages
    bool enableSemRej; //turns semantic check on/off
    CodeGenerator gen;
    
    
};

#endif /* parser_h */
