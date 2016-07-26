//
//  parser.cpp
//  Parser
//
//  Created by Kyle Liu on 2/20/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include "parser.h"

Parser::Parser()
{
    cerr<<"No Token List Received. \nExiting...";
    exit(1);
}
Parser::Parser(Token* tokenListStartPtr)
{
    tokenListStart = tokenListStartPtr;
    populateStack();
    debugMode = false;
    enableSemRej = false;
}

void Parser::execute()
{
    Program();
    buildSymbolTable();
    gen.printList();
}

void Parser::populateStack()
{
    codeStack.push(Token("$", "$", NULL, NULL));
    derivedStack.push("$");
    
    Token* curPtr = tokenListStart;

    while(curPtr->getNextTokPtr() != NULL)
    {
        //cout<<curPtr->getLexeme()<<" ";
        curPtr = curPtr->getNextTokPtr();
    } // get to the end of the link list
    
    //cout<<curPtr->getLexeme()<<" ";
    //cout<<"\n\n";
    
    while(curPtr->getPrevTokPtr() != NULL)
    {
        //cout<<curPtr->getLexeme()<<" ";
        if(curPtr->getDataType() == "ID")
        {
            Token newToken("ID", curPtr->getDataType(), NULL, NULL);
            codeStack.push(newToken);
        }
        else if(curPtr->getDataType() == "NUM")
        {
            Token newToken("NUM", curPtr->getDataType(), NULL, NULL);
            codeStack.push(newToken);
        }
        else if(curPtr->getDataType() == "FLOAT")
        {
            Token newToken("NUM", curPtr->getDataType(), NULL, NULL);
            codeStack.push(newToken);
        }
        else
        {
            Token newToken(curPtr->getLexeme(), curPtr->getDataType(), NULL, NULL);
            codeStack.push(newToken);
        }
        
        curPtr = curPtr->getPrevTokPtr();

    }
    
    Token newToken(curPtr->getLexeme(), curPtr->getDataType(), NULL, NULL);
    codeStack.push(newToken);
    
    //cout<<curPtr->getLexeme();
    
    //cout<<"\n";
}

void Parser::checkStackMatch()
{
    //cout<<"Checking |"<<codeStack.top().getLexeme()<<"| with |"<<derivedStack.top()<<"|"<<endl;
    if(codeStack.top().getLexeme().compare(derivedStack.top()) == 0)
    {
        codeStack.pop();
        derivedStack.pop();
        //cout<<"POPPED"<<endl;
        return;
    }
    
    reject(100);
}

void Parser::pushToDStack(string text)
{
    derivedStack.push(text);
    checkStackMatch();
}

void Parser::reject(int rejcode)
{
    if(codeStack.top().getLexeme() == "$" && derivedStack.top() == "$")
    {
        return;
    }
    cout<<"REJECT"<<endl;
    //printStack();
    exit(rejcode);
}

void Parser::printStack()
{
    cout<<"CODESTACK: "<<endl;
    while(!codeStack.empty())
    {
        cout<<codeStack.top().getLexeme()<<"|";
        codeStack.pop();
    }
    
    cout<<"\n\nDERIVEDSTACK: "<<endl;
    while(!derivedStack.empty())
    {
        cout<<derivedStack.top()<<"|";
        derivedStack.pop();
    }
}

void Parser::buildSymbolTable()
{
    Token* curPtr = tokenListStart;
    ScopeAssignmentProtocol sap;
    stack<SymbolTable> tableStack;
    tableStack.push(SymbolTable(sap.getCurrentScope()));
    bool skipLeftBrace = false;
    bool returnFound = false;
    bool returnNeeded = false;
    bool isVoid = false;
    bool allowEmptyArray = false;
    Token* currentFunctionPtr = NULL;
    string returnedType = "";
    
    /*For Code Gen*/
    int backpatch = 0;
    int breakpatch = 0;
    bool inWhile = false;
    int whileBraceCount = 0;
    int whileBRholder = 0;
    bool inIf = false;
    int elseLine = 0;
    int ifBraceCount;
    int endElseLine = 0;
    bool inElse = false;
    int elseBraceCount = 0;
    int ifBreakModifyLine = 0;
    
    while(true)
    {
        if(curPtr->getLexeme() == "{" && inWhile)
        {
            whileBraceCount++;
        }
        
        if(curPtr->getLexeme() == "}" && inWhile)
        {
            whileBraceCount--;
        }

        if(curPtr->getLexeme() == "{" && inIf)
        {
               ifBraceCount++;
        }
        
        if(curPtr->getLexeme() == "}" && inIf)
        {
            ifBraceCount--;
        }
        
        if(whileBraceCount == -1)
        {
            inWhile = false;
            whileBraceCount = 0;
            
            gen.modify(whileBRholder, "", "", "", to_string(static_cast<unsigned long long>(gen.getCurLineNumber())));
            gen.insert("br", "", "", to_string(static_cast<unsigned long long>(backpatch)));
            
        }
        
        if(ifBraceCount == -1)
        {
            inIf = false;
            ifBraceCount = 0;
            endElseLine = gen.getCurLineNumber();
            
            if(curPtr->getNextTokPtr()->getLexeme() == "else")
            {
                //ifBreakModifyLine = gen.getCurLineNumber();
                gen.insert("br", "", "", "");
            }
            
            gen.modify(ifBreakModifyLine, "", "", "", to_string(static_cast<unsigned long long>(gen.getCurLineNumber()+1)));
            
            gen.modify(elseLine, "", "", "", to_string(static_cast<unsigned long long>(gen.getCurLineNumber()+1)));
        }
        
        if(curPtr->getLexeme() == "{" && inElse)
        {
            elseBraceCount++;
        }
        
        if(curPtr->getLexeme() == "}" && inElse)
        {
            elseBraceCount--;
        }
        
        if(elseBraceCount == -1)
        {
            inElse = false;
            elseBraceCount = 0;
            gen.modify(elseLine, "", "", "", to_string(static_cast<unsigned long long>(gen.getCurLineNumber()+1)));
            gen.modify(endElseLine, "", "", "", to_string(static_cast<unsigned long long>(gen.getCurLineNumber()+1)));
            
        }
        
        if(curPtr->getLexeme() == "else")
        {
            inElse = true;
            elseBraceCount = -1;
            elseLine = gen.getCurLineNumber();
        }

        
        if(curPtr->getLexeme() == "{" && !skipLeftBrace)
        {
            gen.insert("block", "", "", "");
            sap.getNextAvailableScope();
            tableStack.push(SymbolTable(sap.getCurrentScope()));
        }
        
        if(curPtr->getLexeme() == "{" && skipLeftBrace)
        {
            allowEmptyArray = false;
            skipLeftBrace = false;
        }
        
        if(curPtr->getLexeme() == "}")
        {
            stack<SymbolTable> tempStack;
            tempStack.push(tableStack.top());
            tableStack.pop();
            bool endFunc = false;
            if(tableStack.top().getDefaultScope() == 0)
            {
                while(!tempStack.empty())
                {
                    tableStack.push(tempStack.top());
                    tempStack.pop();
                }
                
                if(currentFunctionPtr->getPrevTokPtr()->getLexeme() == "void")
                {
                    isVoid = true;
                    returnNeeded = false;
                }
                else
                {
                    returnNeeded = true;
                }
                
                Token * tempPtr = currentFunctionPtr;
                string expectedReturnType = currentFunctionPtr->getPrevTokPtr()->getLexeme();
                string currentFuncName = currentFunctionPtr->getLexeme();
               // cout<<"Function "<<currentFuncName<<" expects "<<expectedReturnType<<endl;
                while(tempPtr->getLexeme() != "{"){tempPtr = tempPtr->getNextTokPtr();}
                int braceEncountered = 0;
                while(braceEncountered != -1)
                {
                    tempPtr = tempPtr->getNextTokPtr();
                    if(tempPtr->getLexeme() == "return")
                    {
                        returnFound = true;
                        
                        handleReturn(&tempPtr);
                        
                        if(isVoid && tempPtr->getNextTokPtr()->getLexeme() != ";")
                        {
                            if(debugMode)
                                cout<<"Void function cannot return value"<<endl;
                            if(enableSemRej){semanticReject(3);}
                        }
                        
                        if(!isVoid)
                        {
                            if(tempPtr->getNextTokPtr()->getDataType() == "FLOAT")
                            {
                                returnedType = "float";
                                
                            }
                            if(tempPtr->getNextTokPtr()->getDataType() == "NUM")
                            {
                                returnedType = "int";
                      
                            }
                            if(tempPtr->getNextTokPtr()->getDataType() == "ID")
                            {
                                Symbol tempSymPtr("", "", 0, 0, 0,"");
                                if(!tableStack.top().search(tempPtr->getNextTokPtr()->getLexeme(), Variable, &tempSymPtr))
                                {
                                    tempStack.push(tableStack.top());
                                    tableStack.pop();
                                    tableStack.top().search(tempPtr->getNextTokPtr()->getLexeme(),&tempSymPtr);
                                    tableStack.push(tempStack.top());
                                    tempStack.pop();
                                }

                                returnedType = tempSymPtr.getType();
                            }
                            
                            if(currentFunctionPtr != NULL)
                            {
                                if(returnedType != expectedReturnType)
                                {
                                    if(debugMode)
                                    cout<<"Cannot return "<<returnedType<<" for "<<expectedReturnType<<" function "<<currentFuncName<<endl;
                                    if(enableSemRej){semanticReject(3);}
                                }
                            }
                        }
                    }
                    
                    if(tempPtr->getLexeme() == "{"){braceEncountered++;}
                    if(tempPtr->getLexeme() == "}"){braceEncountered--;}
                }
                
                if(braceEncountered == -1 && !returnFound && returnNeeded)
                {
                    if(debugMode)
                    cout<<"Return statement required for function "<<currentFunctionPtr->getLexeme()<<endl;
                    if(enableSemRej){semanticReject(3);}
                }
                
                gen.insert("end", "func", currentFuncName, "");
                endFunc = true;
                returnNeeded = false;
                returnFound = false;
                isVoid = false;
                returnedType = "";
                currentFunctionPtr = NULL;
            }
            else
            {
                while(!tempStack.empty())
                {
                    tableStack.push(tempStack.top());
                    tempStack.pop();
                }
            }
            
            if(!endFunc){gen.insert("end", "block", "", "");}
            sap.scopeEnded();
            symTabList.insert(symTabList.end(), tableStack.top());
            tableStack.pop();
        }
        
        if(curPtr->getDataType() == "ID")
        {
            /*checks if the previous token was int or float and the next token is not a '(', if it matches then it's a varibale declaration*/
            if((curPtr->getPrevTokPtr()->getLexeme() == "int" || curPtr->getPrevTokPtr()->getLexeme() == "float") && curPtr->getNextTokPtr()->getLexeme() != "(")
            {
                if(!tableStack.top().search(curPtr->getLexeme(), Variable) && !tableStack.top().search(curPtr->getLexeme(), Array))
                {
                    if(curPtr->getNextTokPtr()->getLexeme() != "[")
                    {
                        gen.insert("alloc", "4", "", curPtr->getLexeme());
                        tableStack.top().insert(curPtr->getLexeme(), curPtr->getPrevTokPtr()->getLexeme(), sap.getCurrentScope(), Variable, 0, "");
                        //cout<<"Inserted Varible "<<curPtr->getLexeme()<<" of type "<<curPtr->getPrevTokPtr()->getLexeme()<<endl;
                    }
                    else
                    {
                        if(curPtr->getNextTokPtr()->getNextTokPtr()->getDataType() == "NUM")
                        {
                        int arraySize = stoi(curPtr->getNextTokPtr()->getNextTokPtr()->getLexeme());
                        gen.insert("alloc", to_string(static_cast<unsigned long long>(arraySize*4)), "", curPtr->getLexeme());
                        }
                        tableStack.top().insert(curPtr->getLexeme(), curPtr->getPrevTokPtr()->getLexeme(), sap.getCurrentScope(), Array, 0, "");
                    }
                    
                }
                else
                {
                    if(debugMode)
                    cout<<curPtr->getLexeme()<<" already exist"<<endl;
                    if(enableSemRej){semanticReject(3);}
                }
            }
            
            /*checks if the previous token was int, float or void and the next token is a '(' if it matches then it's a function declaration*/
            if((curPtr->getPrevTokPtr()->getLexeme() == "int" || curPtr->getPrevTokPtr()->getLexeme() == "float" || curPtr->getPrevTokPtr()->getLexeme() == "void") && curPtr->getNextTokPtr()->getLexeme() == "(")
            {
                allowEmptyArray = true;
                currentFunctionPtr = curPtr;
                Token* tempPtr = curPtr;
                int paramCount = 0;
                string signature = "";
                
                while(tempPtr->getLexeme() != ")")
                {
                    if(tempPtr->getLexeme() == "void" && tempPtr->getNextTokPtr()->getDataType() == "ID")
                    {
                        if(debugMode)
                        cout<<"Invalid datatype in function "<<curPtr->getLexeme()<<" parameter"<<endl;
                        if(enableSemRej){semanticReject(3);}
                    }
                    
                    if(tempPtr->getLexeme() == "int" || tempPtr->getLexeme() == "float")
                    {
                        paramCount++;
                        if(tempPtr->getLexeme() == "int")
                        {
                            signature += "int ";
                        }
                        if(tempPtr->getLexeme() == "float")
                        {
                            signature += "float ";
                        }
                    }
                    tempPtr = tempPtr->getNextTokPtr();
                }
                
                //cout<<paramCount<<" parameters for function "<<curPtr->getLexeme()<<"() with signature "<<signature<<endl;
                
                tableStack.top().insert(curPtr->getLexeme(), curPtr->getPrevTokPtr()->getLexeme(), sap.getCurrentScope(), Function, paramCount, signature);
                //cout<<"Inserted Function"<<curPtr->getLexeme()<<" of type "<<curPtr->getPrevTokPtr()->getLexeme()<<endl;
                sap.getNextAvailableScope();
                tableStack.push(SymbolTable(sap.getCurrentScope()));
                gen.insert("func", curPtr->getLexeme(), curPtr->getPrevTokPtr()->getLexeme(), to_string(static_cast<unsigned long long>(paramCount)));
                if(paramCount > 0)
                {
                    gen.insert("param", "", "", "");
                }
                skipLeftBrace = true;
            }
            
            /* checks if the the token is a variable call, ensures that variable exist on the symbol table and is accessible*/
            if((curPtr->getPrevTokPtr()->getLexeme() != "int" || curPtr->getPrevTokPtr()->getLexeme() != "float") && curPtr->getNextTokPtr()->getLexeme() != "(")
            {
                stack<SymbolTable> tempStack;
                bool notFoundFlag = true;
                int searchType = Variable;
                
                if(curPtr->getNextTokPtr()->getLexeme() == "[")
                {
                    searchType = Array;
                }
                
                while(!tableStack.empty())
                {
                    
                    if(tableStack.top().search(curPtr->getLexeme(), searchType))
                    {
                        notFoundFlag = false;
                        break;
                    }
                    
                    tempStack.push(tableStack.top());
                    tableStack.pop();
                }
                
                while(!tempStack.empty())
                {
                    tableStack.push(tempStack.top());
                    tempStack.pop();
                }
                
                if(notFoundFlag)
                {
                    if(debugMode)
                    cout<<curPtr->getLexeme()<<" not found in scope."<<endl;
                    if(enableSemRej){semanticReject(3);}
                    //exit(5);
                }
                
            }
            
            /*checks to see if token is a function call, ensures that the function is already declared*/
            if((curPtr->getPrevTokPtr()->getLexeme() != "int" && curPtr->getPrevTokPtr()->getLexeme() != "float" && curPtr->getPrevTokPtr()->getLexeme() != "void") && curPtr->getNextTokPtr()->getLexeme() == "(")
            {
                stack<SymbolTable> tempStack;
                bool notFoundFlag = true;
                
                while(!tableStack.empty())
                {
                    if(tableStack.top().search(curPtr->getLexeme(), Function))
                    {
                        notFoundFlag = false;
                        break;
                    }
                    
                    tempStack.push(tableStack.top());
                    tableStack.pop();
                }
                
                while(!tempStack.empty())
                {
                    tableStack.push(tempStack.top());
                    tempStack.pop();
                }
                
                if(notFoundFlag)
                {
                    if(debugMode)
                    cout<<"function "<<curPtr->getLexeme()<<"() does not exist."<<endl;
                    //exit(5);
                }
                
                string argumentSig = "";
                Token * tempTokPtr = curPtr->getNextTokPtr();
                bool typeCaptured = false;
                
                while(tempTokPtr->getLexeme() != ")")
                {
                    if(tempTokPtr->getDataType() == "NUM")  //NUM = int when tokenizing
                    {
                        argumentSig += "int ";
                        typeCaptured = true;
                    }
                    
                    if(tempTokPtr->getDataType() == "FLOAT")    //FLOAT = float when tokenizing
                    {
                        argumentSig += "float ";
                        typeCaptured = true;
                    }
                    
                    if(tempTokPtr->getDataType() == "ID")
                    {
                        Symbol tempSym("", "", 0, 0, 0,"");
                        
                        while(!tableStack.empty())
                        {
                            if(tableStack.top().search(tempTokPtr->getLexeme(), Variable, &tempSym) || tableStack.top().search(tempTokPtr->getLexeme(), Array, &tempSym))
                            {
                                break;
                            }
                            
                            tempStack.push(tableStack.top());
                            tableStack.pop();
                        }
                        
                        while(!tempStack.empty())
                        {
                            tableStack.push(tempStack.top());
                            tempStack.pop();
                        }
                        
                        if(tempSym.getType() == "int")
                        {
                            argumentSig += "int ";
                        }
                        
                        if(tempSym.getType() == "float")
                        {
                            argumentSig += "float ";
                        }
                        typeCaptured = true;
                    }
                    
                    tempTokPtr = tempTokPtr->getNextTokPtr();
                    while(tempTokPtr->getLexeme() != "," && tempTokPtr->getLexeme() != ")" && typeCaptured==true)
                    {
                        if(debugMode)
                        cout<<tempTokPtr->getLexeme()<< " is not , or )"<<endl;
                        tempTokPtr = tempTokPtr->getNextTokPtr();
                    }
                    typeCaptured = false;
                }
                
                bool funcFoundFlag = false;
                
                while(!tableStack.empty())
                {
                    if(tableStack.top().search(curPtr->getLexeme(), argumentSig))
                    {
                        funcFoundFlag = true;
                      
                    }
                    
                    tempStack.push(tableStack.top());
                    tableStack.pop();
                }
                
                while(!tempStack.empty())
                {
                    tableStack.push(tempStack.top());
                    tempStack.pop();
                }
                
                if(!funcFoundFlag)
                {
                    if(debugMode)
                    cout<<"No matching function "<<curPtr->getLexeme()<<" with argument "<<argumentSig<<endl;
                    if(enableSemRej){semanticReject(3);}
                }
                
                handleMath(&curPtr);
            }
        }
        
        //Checks for Operator Symbols
        if(curPtr->getLexeme() == "+" || curPtr->getLexeme() == "-" || curPtr->getLexeme() == "*" || curPtr->getLexeme() == "/" || curPtr->getLexeme() == "<" || curPtr->getLexeme() == "<=" || curPtr->getLexeme() == ">" || curPtr->getLexeme() == ">=" || curPtr->getLexeme() == "==" || curPtr->getLexeme() == "!=" || curPtr->getLexeme() == "=")
        {
            if(curPtr->getLexeme() == "=")
            {
                bool arrayAssignment = false;
                int arraySize;
                string arrayName = "";
                
                if(curPtr->getPrevTokPtr()->getLexeme() == "]")
                {
                    arrayAssignment = true;
                    if(curPtr->getPrevTokPtr()->getPrevTokPtr()->getDataType() == "NUM")
                    {
                        arraySize = stoi(curPtr->getPrevTokPtr()->getPrevTokPtr()->getLexeme())*4;
                    }
                    arrayName = curPtr->getPrevTokPtr()->getPrevTokPtr()->getPrevTokPtr()->getPrevTokPtr()->getLexeme();
                }
                
                Token* expStart = curPtr->getNextTokPtr();
                
                handleMath(&expStart);
                
                if(!arrayAssignment)
                {
                    gen.insert("assign", curPtr->getNextTokPtr()->getLexeme(), "", curPtr->getPrevTokPtr()->getLexeme());
                }
                else
                {
                    string array = arrayName+","+to_string(static_cast<unsigned long long>(arraySize));
                    gen.insert("assign", curPtr->getNextTokPtr()->getLexeme(), "", array);
                }
            }
            
            Token * leftToken = curPtr->getPrevTokPtr(); //Ptr to LHS
            Token * rightToken = curPtr->getNextTokPtr();   //Ptr to RHS
            string leftType = "";       //Type of LHS
            string rightType = "";      //Type of RHS
            
            //Checks if LHS is plain number
            if(leftToken->getDataType() == "NUM")
            {
                leftType = "int";
            }
            //Check if LHS is plain float
            if(leftToken->getDataType() == "FLOAT")
            {
                leftType = "float";
            }
            
            //Check if LHS is a function, if it is then it fetches the ID for the function
            if(leftToken->getLexeme() == ")")
            {
                Token* shadow = leftToken->getPrevTokPtr();      //Traverses to find ID of the function
                int parenCount = 0;
                
                while(parenCount != -1 && shadow != NULL)
                {
                    if(shadow->getLexeme() == ")")
                    {
                        parenCount++;
                    }
                    if(shadow->getLexeme() == "(")
                    {
                        parenCount--;
                    }
                    
                    shadow = shadow->getPrevTokPtr();
                }
                
                    leftToken = shadow;
            }
            
            //Checks if LHS is an array element
            if(leftToken->getLexeme() == "]")
            {
                Token* shadow = leftToken;
                //cout<<"lefttok is "<<leftToken->getLexeme()<<endl;
                while(shadow->getLexeme() != "[")
                {
                    shadow = shadow->getPrevTokPtr();
                   // cout<<"shadow is "<<shadow->getLexeme()<<endl;
                }
                
                    leftToken = shadow->getPrevTokPtr();
                //cout<<"leftok is "<<leftToken->getLexeme()<<endl;
            }
            
            //Checks the data type of the ID on the LHS
            if(leftToken->getDataType() == "ID")
            {
                Symbol tempSym("", "", 0, 0, 0,"");
                stack<SymbolTable> tempStack;
                int symbolCategory = 0;
                
                if(leftToken->getNextTokPtr()->getLexeme() == "(")
                {
                    symbolCategory = Function;
                }
                else if(leftToken->getNextTokPtr()->getLexeme() == "[")
                {
                    symbolCategory = Array;
                }
                else
                {
                    symbolCategory = Variable;
                }
                
                while(!tableStack.empty())
                {
                    if(tableStack.top().search(leftToken->getLexeme(), symbolCategory, &tempSym))
                    {
                        break;
                    }
                    tempStack.push(tableStack.top());
                    tableStack.pop();
                }
                
                while(!tempStack.empty())
                {
                    tableStack.push(tempStack.top());
                    tempStack.pop();
                }
                
                //cout<<"LHS "<<tempSym.getName()<<" found with type "<<tempSym.getType()<<endl;
                leftType = tempSym.getType();
            }
            
            if(rightToken->getLexeme() == "(")
            {
                Token* shadow = rightToken;
                while(shadow->getDataType() != "NUM" || shadow->getDataType() != "FLOAT" || shadow->getDataType() != "ID")
                {
                    shadow = shadow->getNextTokPtr();
                }
                
                rightToken = shadow;
            }
            
            //Checks if RHS is plain number
            if(rightToken->getDataType() == "NUM")
            {
                rightType = "int";
            }
            //Check if RHS is plain float
            if(rightToken->getDataType() == "FLOAT")
            {
                rightType = "float";
            }
            
            if(rightToken->getDataType() == "ID")
            {
                Symbol tempSym("", "", 0, 0, 0,"");
                stack<SymbolTable> tempStack;
                int symbolCategory = 0;
                
                if(rightToken->getNextTokPtr()->getLexeme() == "(")
                {
                    symbolCategory = Function;
                }
                else if(rightToken->getNextTokPtr()->getLexeme() == "[")
                {
                    symbolCategory = Array;
                }
                else
                {
                    symbolCategory = Variable;
                }
                
                while(!tableStack.empty())
                {
                    if(tableStack.top().search(rightToken->getLexeme(), symbolCategory, &tempSym))
                    {
                        break;
                    }
                    tempStack.push(tableStack.top());
                    tableStack.pop();
                }
                
                while(!tempStack.empty())
                {
                    tableStack.push(tempStack.top());
                    tempStack.pop();
                }
                
                //cout<<"RHS "<<tempSym.getName()<<" found with type "<<tempSym.getType()<<endl;
                rightType = tempSym.getType();
            }
            
            if(leftType != rightType)
            {
                if(debugMode)
                {
                    cout<<"Type mismatch for operator "<<curPtr->getLexeme()<<endl;
                    cout<<"cannot perform operation on "<<leftType<<" and "<<rightType<<endl;
                }
                if(enableSemRej){semanticReject(3);}
            }
        }
       
        if(curPtr->getLexeme() == "[" && !allowEmptyArray)
        {
            Token * shadow = curPtr;
            bool isInt = false;
            string lexeme;

            while(shadow->getLexeme() != "]")
            {
                if(shadow->getDataType() == "NUM")
                {
                    isInt = true;
                    break;
                }
                
                
                if(shadow->getDataType() == "ID")
                {
                    Symbol tempSym("", "", 0, 0, 0,"");
                    stack<SymbolTable> tempStack;
                    
                    while(!tableStack.empty())
                    {
                        if(tableStack.top().search(shadow->getLexeme(), &tempSym))
                        {
                            lexeme = tempSym.getName();
                            break;
                        }
                        tempStack.push(tableStack.top());
                        tableStack.pop();
                    }
                    
                    while(!tempStack.empty())
                    {
                        tableStack.push(tempStack.top());
                        tempStack.pop();
                    }

                    if(tempSym.getType() == "int")
                    {
                        isInt = true;
                        break;
                    }
                    
                }
                
                shadow = shadow->getNextTokPtr();
            }
             
            if(!isInt)
            {
                if(debugMode)
                cout<<lexeme<<" cannot be used to index "<<curPtr->getPrevTokPtr()->getLexeme()<<endl;
                if(enableSemRej){semanticReject(3);}
            }
        }
        
        if(curPtr->getLexeme() == "while" || curPtr->getLexeme() == "if")
        {
            Token* shadow = curPtr->getNextTokPtr();
            int parenCount = 0;
            string bossOperator = "";
            string codeCompar = "";
            backpatch = gen.getCurLineNumber();
            
            if(curPtr->getLexeme() == "while"){inWhile = true;}
            if(curPtr->getLexeme() == "if"){inIf = true;};
            
            if(shadow->getLexeme() == "(")
            {
                shadow = shadow->getNextTokPtr();
            }
            while(parenCount != -1)
            {
                if(shadow->getLexeme() == "("){parenCount++;}
                if(shadow->getLexeme() == ")"){parenCount--;}
                if(isCompOperator(shadow->getLexeme()) && parenCount == 0){bossOperator = shadow->getLexeme();}
                shadow = shadow->getNextTokPtr();
            }
            
            
            if(bossOperator == ">"){codeCompar = "brgt";}
            if(bossOperator == ">="){codeCompar = "brgeq";}
            if(bossOperator == "<"){codeCompar = "brlt";}
            if(bossOperator == "<="){codeCompar = "brleq";}
            if(bossOperator == "=="){codeCompar = "breq";}
            if(bossOperator == "!="){codeCompar = "brneq";}
  

            //Token semiColonTok(";", "SYSYM", shadow->getPrevTokPtr(), shadow);
            Token* semiColonTok = new Token(";", "SYSYM", shadow->getPrevTokPtr(), shadow);
 
            shadow->getPrevTokPtr()->setNextTokPtr(semiColonTok);
            shadow->getNextTokPtr()->setPrevTokPtr(semiColonTok);
            
            
            handleMath(&curPtr);
            
            gen.insert(codeCompar, curPtr->getLexeme(), "", to_string(static_cast<unsigned long long>(gen.getCurLineNumber()+3)));
            if(inWhile)
            {
                whileBRholder = gen.getCurLineNumber();
                whileBraceCount = -1;
                gen.insert("br", "", "", to_string(static_cast<unsigned long long>(whileBRholder)));
            }
            
            if(inIf)
            {
                ifBreakModifyLine = gen.getCurLineNumber();
                ifBraceCount = -1;
                gen.insert("br", "", "", to_string(static_cast<unsigned long long>(elseLine)));
            }
        }
        
        
        if(curPtr->getNextTokPtr() == NULL)
        {
            break;
        }
        curPtr = curPtr->getNextTokPtr();
        
    }
    
    sap.scopeEnded();
    symTabList.insert(symTabList.end(), tableStack.top());
    tableStack.pop();
    
}

void Parser::handleFuncCall(Token** start)
{
    Token* startprev = (*start)->getPrevTokPtr();
    Token** temp = start;
    int paramCount = 0;
    string tempSto = "_t"+to_string(static_cast<unsigned long long>(gen.getTempCounter()));
    string funcName = (*start)->getLexeme();
    (*temp) = (*temp)->getNextTokPtr();
    while((*temp)->getLexeme() != ")")
    {
        if((*temp)->getDataType() == "ID" || (*temp)->getDataType() == "NUM" ||(*temp)->getDataType() == "FLOAT")
        {
            if((*temp)->getDataType() == "ID" && (*temp)->getNextTokPtr()->getLexeme() == "[")
            {
                handleArray(temp);
            }
            
            gen.insert("arg", "", "", (*temp)->getLexeme());
            paramCount++;
        }
        (*temp) = (*temp)->getNextTokPtr();
    }
    
    gen.insert("call", funcName, to_string(static_cast<unsigned long long>(paramCount)), tempSto);
    (*temp)->setLexeme(tempSto);
    (*temp)->setDataType("ID");
    startprev->setNextTokPtr(*temp);
    (*temp)->setPrevTokPtr(startprev);
    
}

void Parser::handleArray(Token** start)
{
    Token* startprev = (*start)->getPrevTokPtr();
    Token** temp = start;
    
    int offset = 0;
    string arrayName = (*start)->getLexeme();
    
    while((*temp)->getLexeme() != "]")
    {
        if((*temp)->getPrevTokPtr()->getLexeme() == "[" && (*temp)->getNextTokPtr()->getLexeme() == "]" && (*temp)->getDataType() == "NUM")
        {
            offset = stoi((*temp)->getLexeme());
        }
        
        (*temp) = (*temp)->getNextTokPtr();
    }
    
    arrayName += ","+to_string(static_cast<unsigned long long>(offset*4));
    (*temp)->setLexeme(arrayName);
    (*temp)->setDataType("ID");
    startprev->setNextTokPtr(*temp);
    (*temp)->setPrevTokPtr(startprev);
}



void Parser::handleReturn(Token** start)
{
    Token* end; //indicates the end of what this function should mess what
                //YOU SHALL NOT PASS!!
    
    if((*start)->getLexeme() == "return")
    {
            end = (*start);
            handleMath(&end);
        
            gen.insert("return", "", "", end->getLexeme());
    }
    else
    {
        return;
    }
}

void Parser::handleMath(Token** start)
{
    Token* startPrev = (*start)->getPrevTokPtr();
    Token** end = start;
    stack<Token> expression;
    list<Token> inFix, postFix;
    
    list<Token>::iterator i = inFix.begin();
    
    
    while((*end)->getLexeme() != ";")
    {
        
        if((*end)->getDataType() == "ID" && (*end)->getNextTokPtr()->getLexeme()=="(")
        {
            handleFuncCall(end);
        }
        
        if((*end)->getDataType() == "ID" && (*end)->getNextTokPtr()->getLexeme()=="[")
        {
            handleArray(end);
        }
        
        inFix.push_back(*(*end));
        (*end) = (*end)->getNextTokPtr();
    }
    
    
    /*Convert to Postfix*/
    expression.push(Token("(", "LP", NULL, NULL));
    inFix.push_back(Token(")", "RP", NULL, NULL));
    
    /*for(list<Token>::iterator i = inFix.begin(); i != inFix.end(); i++)
     {
     cout<<i->getLexeme()<<" ";
     }
     
     cout<<endl;*/
    
    i = inFix.begin();
    //cout<<i->getLexeme();
    
    while(!expression.empty() && i != inFix.end())
    {
        if(i->getDataType() == "NUM" || i->getDataType() == "FLOAT" || i->getDataType() == "ID")
        {
            //cout<<"OP|";
            postFix.push_back(*i);
        }
        
        else if(i->getLexeme() == "(")
        {
            //cout<<"(";
            expression.push(*i);
        }
        
        else if(isOperator(i->getLexeme()))
        {
            //cout<<"OPERATOR|";
            //cout<<"comparing : "<<expression.top().getLexeme()<<" and "<<i->getLexeme()<<" : "<<precedence(expression.top().getLexeme(), i->getLexeme())<<endl;
            while(isOperator(expression.top().getLexeme()) && precedence(expression.top().getLexeme(), i->getLexeme()))
            {
                postFix.push_back(expression.top());
                expression.pop();
            }
            
            expression.push(*i);
        }
        
        else if(i->getLexeme() == ")")
        {
            //cout<<")";
            while(expression.top().getLexeme() != "(")
            {
                postFix.push_back(expression.top());
                expression.pop();
            }
            
            if(expression.top().getLexeme() == "(")
            {
                expression.pop();
            }
        }
        
        i++;
    }
    /*End Conversion*/
    
    //cout<<endl;
    /*for(list<Token>::iterator p = postFix.begin(); p != postFix.end(); p++)
    {
        cout<<p->getLexeme()<<" ";
    }
    cout<<endl;*/
    
    string op1 = "";
    string op2 = "";
    string operation = "";
    string tempReg = "";
    
    stack<Token> evalStack;
    
    /*Evaluate Postfix*/
    for(list<Token>::iterator p = postFix.begin(); p != postFix.end(); p++)
    {
        if(isOperator(p->getLexeme()))
        {
            operation = p->getLexeme();
            op2 = evalStack.top().getLexeme();
            evalStack.pop();
            op1 = evalStack.top().getLexeme();
            evalStack.pop();
            
            tempReg = "_t"+to_string(static_cast<unsigned long long>(gen.getTempCounter()));
            if(operation == "+"){gen.insert("add", op1, op2, tempReg);}
            if(operation == "-"){gen.insert("sub", op1, op2, tempReg);}
            if(operation == "*"){gen.insert("mul", op1, op2, tempReg);}
            if(operation == "/"){gen.insert("div", op1, op2, tempReg);}
            if(isCompOperator(operation)){gen.insert("comp", op1, op2, tempReg);}
            
            evalStack.push(Token(tempReg, "ID", NULL, NULL));
            continue;
        }
        
        evalStack.push(*p);
    }
    
    
    //cout<<"FINAL on eval: "<<evalStack.top().getLexeme()<<endl;
    string result = "";
    
    if(evalStack.size() == 1)
    {
        result = evalStack.top().getLexeme();
    }
    /*End Postfix evaluation*/
    
    (*end)->setLexeme(result);
    (*end)->setDataType("ID");
    startPrev->setNextTokPtr(*end);
    (*end)->setPrevTokPtr(startPrev);
}

bool Parser::precedence(string op1, string op2)
{
    int op1val = 0;
    int op2val = 0;
    
    if(op1 == "*" || op1 == "/")
    op1val = 4;
    else if(op1 == "+" || op1 == "-")
    op1val = 3;
    else if(op1 == "<" || op1 == ">" || op1 == ">=" || op1 == "<=")
    op1val = 2;
    else if(op1 == "==" || op1 == "!=")
    op1val = 1;
    
    if(op2 == "*" || op2 == "/")
    op2val = 4;
    else if(op2 == "+" || op2 == "-")
    op2val = 3;
    else if(op2 == "<" || op2 == ">" || op2 == ">=" || op2 == "<=")
    op2val = 2;
    else if(op2 == "==" || op2 == "!=")
    op2val = 1;
    
    if(op1val >= op2val )
    {
        return true;
    }
    
    return false;
}

bool Parser::isOperator(string test)
{
    if(test == "+" || test == "-" || test == "*" || test == "/" || test == "<" || test == ">" || test == ">=" || test == "<=" || test == "==" || test == "!=")
    {
        return true;
    }

    return false;
}

bool Parser::isCompOperator(string test)
{
    if(test == "<" || test == ">" || test == ">=" || test == "<=" || test == "==" || test == "!=")
    {
        return true;
    }
    
    return false;
}

void Parser::printSymbolTable()
{
    for(list<SymbolTable>::iterator i = symTabList.begin(); i != symTabList.end(); i++)
    {
        i->printTable();
    }
}

list<SymbolTable> Parser::getSymbolTable()
{
    return symTabList;
}

/*
 Productions:
 */

void Parser::Program()
{
    DeclarationList();
}
void Parser::DeclarationList()
{
    Declaration();
    DeclarationLoop();
}
void Parser::DeclarationLoop()
{
    if(codeStack.top().getLexeme() == "int" || codeStack.top().getLexeme() == "void"||codeStack.top().getLexeme() == "float")
    {
        Declaration();
        DeclarationLoop();
    }
    else if(codeStack.top().getLexeme() == "$" && derivedStack.top() == "$")
    {
        codeStack.pop();
        derivedStack.pop();
        //cout<<"ACCEPT"<<endl;
    }
    else
    {
        reject(11);
    }
    
}
void Parser::TypeSpecifier()
{
    if(codeStack.top().getLexeme() == "int")
    {
        pushToDStack("int");
    }
    else if(codeStack.top().getLexeme() == "void")
    {
        pushToDStack("void");
    }
    else if(codeStack.top().getLexeme() == "float")
    {
        pushToDStack("float");
    }
    else
    {
        reject(12);
    }
}
void Parser::Declaration()
{
    if(codeStack.top().getLexeme() == "int" || codeStack.top().getLexeme() == "void"||codeStack.top().getLexeme() == "float")
    {
        TypeSpecifier();
        if(codeStack.top().getLexeme() == "ID")
        {
            pushToDStack("ID");
            oneDeclaration();
        }
        else
        {
            reject(13);
        }
    }
    else
    {
        reject(14);
    }
}
void Parser::oneDeclaration()
{
    if(codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == "[")
    {
        DeclarationType();
    }
    else if(codeStack.top().getLexeme() == "(")
    {
        pushToDStack("(");
        Params();
        if(codeStack.top().getLexeme() == ")")
        {
            pushToDStack(")");
            CompoundStatement();
        }
        else
        {
            reject(15);
        }
    }
    else
    {
        reject(16);
    }
}
void Parser::DeclarationType()
{
    if(codeStack.top().getLexeme() == ";")
    {
        pushToDStack(";");
    }
    else if(codeStack.top().getLexeme() == "[")
    {
        pushToDStack("[");
        if(codeStack.top().getLexeme() == "NUM")
        {
            pushToDStack("NUM");
            if(codeStack.top().getLexeme() == "]")
            {
                pushToDStack("]");
                if (codeStack.top().getLexeme() == ";")
                {
                    pushToDStack(";");
                }
                else
                {
                    reject(171);
                }
            }
            else
            {
                reject(172);
            }
        }
        else
        {
            reject(173);
        }
    }
    else
    {
        reject(17);
    }
}
void Parser::Params()
{
    if(codeStack.top().getLexeme() == "void")
    {
        pushToDStack("void");
        Parameter();
    }
    else if(codeStack.top().getLexeme() == "int" || codeStack.top().getLexeme() == "float")
    {
        if (codeStack.top().getLexeme() == "int")
        {
            pushToDStack("int");
        }
        if (codeStack.top().getLexeme() == "float")
        {
            pushToDStack("float");
        }
        if(codeStack.top().getLexeme() == "ID")
        {
            pushToDStack("ID");
            ParamType();
            ParamLoop();
        }
    }
    else
    {
        reject(18);
    }
}
void Parser::Parameter()
{
    if(codeStack.top().getLexeme() == "ID")
    {
        pushToDStack("ID");
        ParamType();
        ParamLoop();
    }
    else if(codeStack.top().getLexeme() == ")")
    {
    }
    else
    {
        reject(19);
    }
}
void Parser::ParamLoop()
{
    if(codeStack.top().getLexeme() == ",")
    {
        pushToDStack(",");
        Param();
        ParamLoop();
    }
    else if (codeStack.top().getLexeme() == ")")
    {
    }
    else
    {
        reject(20);
    }

}
void Parser::Param()
{
    if(codeStack.top().getLexeme() == "int" || codeStack.top().getLexeme() == "float" || codeStack.top().getLexeme() == "void")
    {
        TypeSpecifier();
        if(codeStack.top().getLexeme() == "ID")
        {
            pushToDStack("ID");
            ParamType();
        }
    }
    else
    {
        reject(21);
    }
}
void Parser::ParamType()
{
    if(codeStack.top().getLexeme() == "[")
    {
        pushToDStack("[");
        if(codeStack.top().getLexeme() == "]")
        {
            pushToDStack("]");
        }
    }
    else if(codeStack.top().getLexeme() == "," || codeStack.top().getLexeme() == "int" || codeStack.top().getLexeme() == "void" || codeStack.top().getLexeme() == "float" || codeStack.top().getLexeme() == ")")
    {
    }
    else
    {
        reject(22);
    }
}
void Parser::CompoundStatement()
{
    //cout<<"->CompoundStatment";
    if(codeStack.top().getLexeme() == "{")
    {
        pushToDStack("{");
        LocalDeclarations();
        StatementList();
        if(codeStack.top().getLexeme() == "}")
        {
            pushToDStack("}");
        }
        else
        {
            reject(231);
        }
    }
    else
    {
        reject(23);
    }
}
void Parser::LocalDeclarations()
{
    //cout<<"->LocalDec";
    if(codeStack.top().getLexeme() == "int" || codeStack.top().getLexeme() == "void" || codeStack.top().getLexeme() == "float")
    {
        VariableDeclaration();
        LocalDeclarations();
    }
    else if(codeStack.top().getLexeme() == "(" || codeStack.top().getLexeme() == "ID" || codeStack.top().getLexeme() == "NUM" || codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == "while" || codeStack.top().getLexeme() == "return" || codeStack.top().getLexeme() == "}" || codeStack.top().getLexeme() == "{" || codeStack.top().getLexeme() == "if")
    {
    }
    else
    {
        reject(24);
    }
}
void Parser::VariableDeclaration()
{
    if(codeStack.top().getLexeme() == "int" || codeStack.top().getLexeme() == "void" || codeStack.top().getLexeme() == "float")
    {
        TypeSpecifier();
        if(codeStack.top().getLexeme() == "ID")
        {
            pushToDStack("ID");
            DeclarationType();
        }
        else
        {
            reject(251);
        }
    }
    else
    {
        reject(25);
    }
}
void Parser::StatementList()
{
    if(codeStack.top().getLexeme() == "if" || codeStack.top().getLexeme() == "return" || codeStack.top().getLexeme() == "while" || codeStack.top().getLexeme() == "ID" || codeStack.top().getLexeme() == "{" || codeStack.top().getLexeme() == "(" || codeStack.top().getLexeme() == "NUM")
    {
        Statement();
        StatementList();
    }
    else if(codeStack.top().getLexeme() == "}")
    {
    }
    else
    {
        reject(26);
    }
}
void Parser::Statement()
{
    //cout<<"->Statement";
    if(codeStack.top().getLexeme() == "ID" || codeStack.top().getLexeme() == "(")
    {
        ExpressionStatement();
    }
    else if(codeStack.top().getLexeme() == "{")
    {
        CompoundStatement();
    }
    else if(codeStack.top().getLexeme() == "if")
    {
        SelectionStatement();
    }
    else if(codeStack.top().getLexeme() == "while")
    {
        IterationStatement();
    }
    else if(codeStack.top().getLexeme() == "return")
    {
        ReturnStatement();
    }
    else
    {
        reject(27);
    }
}
void Parser::SelectionStatement()
{
    if(codeStack.top().getLexeme() == "if")
    {
        pushToDStack("if");
        if(codeStack.top().getLexeme() == "(")
        {
            pushToDStack("(");
            Expression();
            if(codeStack.top().getLexeme() == ")")
            {
                pushToDStack(")");
                Statement();
                ElseStatement();
            }
            else
            {
                reject(281);
            }
        }
        else
        {
            reject(282);
        }
    }
    else
    {
        reject(28);
    }
}
void Parser::ExpressionStatement()
{
    //cout<<"->ExpressionStatement";
    if(codeStack.top().getLexeme() == "ID")
    {
        Expression();
        if(codeStack.top().getLexeme() == ";")
        {
            pushToDStack(";");
        }
        else
        {
            reject(291);
        }
    }
    else if(codeStack.top().getLexeme() == ";")
    {
        pushToDStack(";");
    }
    else
    {
        reject(29);
    }
}
void Parser::ElseStatement()
{
    //cout<<"->ElseStatement()";
    if(codeStack.top().getLexeme() == "else")
    {
        pushToDStack("else");
        Statement();
    }
    else if(codeStack.top().getLexeme() == "if" || codeStack.top().getLexeme() == "return" || codeStack.top().getLexeme() == "while" || codeStack.top().getLexeme() == "ID" || codeStack.top().getLexeme() == "{" || codeStack.top().getLexeme() == "(" || codeStack.top().getLexeme() == "NUM" || codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == "}")
    {
    }
    else
    {
        reject(30);
    }

}
void Parser::IterationStatement()
{
    if(codeStack.top().getLexeme() == "while")
    {
        pushToDStack("while");
        if(codeStack.top().getLexeme() == "(")
        {
            pushToDStack("(");
            Expression();
            if (codeStack.top().getLexeme() == ")")
            {
                pushToDStack(")");
                Statement();
            }
            else
            {
                reject(311);
            }
        }
        else
        {
            reject(312);
        }
    }
    else
    {
        reject(31);
    }
}
void Parser::ReturnStatement()
{
    if(codeStack.top().getLexeme() == "return")
    {
        pushToDStack("return");
        ReturnVar();
    }
    else
    {
        reject(32);
    }
}
void Parser::ReturnVar()
{
    if(codeStack.top().getLexeme() == ";")
    {
        pushToDStack(";");
    }
    else
    {
        Expression();
        if(codeStack.top().getLexeme() == ";")
        {
            pushToDStack(";");
        }
        else
        {
            reject(33);
        }
    }
}
void Parser::Expression()
{
    //cout<<"->Expression";
    if(codeStack.top().getLexeme() == "ID")
    {
        pushToDStack("ID");
        IDFollow();
    }
    else if(codeStack.top().getLexeme() == "(")
    {
        pushToDStack("(");
        Expression();
        if(codeStack.top().getLexeme() == ")")
        {
            pushToDStack(")");
            ExpressionFollow();
        }
    }
    else if(codeStack.top().getLexeme() == "NUM")
    {
        pushToDStack("NUM");
        ExpressionFollow();
    }
    else
    {
        reject(34);
    }
}
void Parser::VarType()
{
    if(codeStack.top().getLexeme() == "[")
    {
        pushToDStack("[");
        Expression();
        if(codeStack.top().getLexeme() == "]")
        {
            pushToDStack("]");
        }
        else
        {
            reject(35);
        }
    }
    else if(codeStack.top().getLexeme() == "=" || codeStack.top().getLexeme() == "*" || codeStack.top().getLexeme() == "/" || codeStack.top().getLexeme() == "+" || codeStack.top().getLexeme() == "-" || codeStack.top().getLexeme() == "<=" || codeStack.top().getLexeme() == "<" || codeStack.top().getLexeme() == ">" || codeStack.top().getLexeme() == ">=" || codeStack.top().getLexeme() == "==" || codeStack.top().getLexeme() == "!=" || codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == ")" || codeStack.top().getLexeme() == "]" || codeStack.top().getLexeme() == ",")
    {
    }
    else
    {
        reject(36);
    }
}
void Parser::VarFollow()
{
    //cout<<"->VarFollow ";
    if(codeStack.top().getLexeme() == "=")
    {
        pushToDStack("=");
        Expression();
    }
    else if(codeStack.top().getLexeme() == "*" || codeStack.top().getLexeme() == "/" || codeStack.top().getLexeme() == "+" || codeStack.top().getLexeme() == "-" || codeStack.top().getLexeme() == "<=" || codeStack.top().getLexeme() == "<" || codeStack.top().getLexeme() == ">" || codeStack.top().getLexeme() == ">=" || codeStack.top().getLexeme() == "==" || codeStack.top().getLexeme() == "!=" || codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == ")" || codeStack.top().getLexeme() == "]" || codeStack.top().getLexeme() == ",")
    {
        ExpressionFollow();
    }
    else
    {
        reject(37);
    }

}
void Parser::IDFollow()
{
    //cout<<"->IDFollow";
    if(codeStack.top().getLexeme() == "[" || codeStack.top().getLexeme() == "=" || codeStack.top().getLexeme() == "*" || codeStack.top().getLexeme() == "/" || codeStack.top().getLexeme() == "+" || codeStack.top().getLexeme() == "-" || codeStack.top().getLexeme() == "<=" || codeStack.top().getLexeme() == "<" || codeStack.top().getLexeme() == ">" || codeStack.top().getLexeme() == ">=" || codeStack.top().getLexeme() == "==" || codeStack.top().getLexeme() == "!=" || codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == ")" || codeStack.top().getLexeme() == "]" || codeStack.top().getLexeme() == ",")
    {
        VarType();
        VarFollow();
    }
    else if(codeStack.top().getLexeme() == "(")
    {
        pushToDStack("(");
        Args();
        if(codeStack.top().getLexeme() == ")")
        {
            pushToDStack(")");
            ExpressionFollow();
        }
        else
        {
            reject(38);
        }
    }
    else
    {
        reject(39);
    }
}
void Parser::ExpressionFollow()
{
    TermLoop();
    AdditiveExpressionLoop();
    Relation();
}
void Parser::Var()
{
    VarType();
}
void Parser::Relation()
{
    if(codeStack.top().getLexeme() == "<=" || codeStack.top().getLexeme() == "<" || codeStack.top().getLexeme() == ">=" || codeStack.top().getLexeme() == ">" || codeStack.top().getLexeme() == "==" || codeStack.top().getLexeme() == "!=")
    {
        RelOp();
        AdditiveExpression();
    }
    else if(codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == ")" || codeStack.top().getLexeme() == "]" || codeStack.top().getLexeme() == ",")
    {
    }
    else
    {
        reject(40);
    }
}
void Parser::RelOp()
{
    if (codeStack.top().getLexeme() == "<=")
    {
        pushToDStack("<=");
    }
    else if(codeStack.top().getLexeme() == "<")
    {
        pushToDStack("<");
    }
    else if(codeStack.top().getLexeme() == ">")
    {
        pushToDStack(">");
    }
    else if(codeStack.top().getLexeme() == ">=")
    {
        pushToDStack(">=");
    }
    else if(codeStack.top().getLexeme() == "==")
    {
        pushToDStack("==");
    }
    else if(codeStack.top().getLexeme() == "!=")
    {
        pushToDStack("!=");
    }
    else
    {
        reject(41);
    }

}
void Parser::AdditiveExpression()
{
    Factor();
    AdditiveExpressionLoop();
}
void Parser::AdditiveExpressionLoop()
{
    if(codeStack.top().getLexeme() == "+" || codeStack.top().getLexeme() == "-")
    {
        AddOp();
        Term();
        AdditiveExpressionLoop();
    }
    else if(codeStack.top().getLexeme() == "<=" || codeStack.top().getLexeme() == "<" || codeStack.top().getLexeme() == ">" || codeStack.top().getLexeme() == ">=" || codeStack.top().getLexeme() == "==" || codeStack.top().getLexeme() == "!=" || codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == ")" || codeStack.top().getLexeme() == "]" || codeStack.top().getLexeme() == ",")
    {
    }
    else
    {
        reject(42);
    }
}
void Parser::AddOp()
{
    if(codeStack.top().getLexeme() == "+")
    {
        pushToDStack("+");
    }
    else if(codeStack.top().getLexeme() == "-")
    {
        pushToDStack("-");
    }
    else
    {
        reject(43);
    }
}
void Parser::Term()
{
    Factor();
    TermLoop();
}
void Parser::TermLoop()
{
    if(codeStack.top().getLexeme() == "*" || codeStack.top().getLexeme() == "/")
    {
        MulOp();
        Factor();
        TermLoop();
    }
    else if(codeStack.top().getLexeme() == "+" || codeStack.top().getLexeme() == "-" || codeStack.top().getLexeme() == "<=" || codeStack.top().getLexeme() == "<" || codeStack.top().getLexeme() == ">" || codeStack.top().getLexeme() == ">=" || codeStack.top().getLexeme() == "==" || codeStack.top().getLexeme() == "!=" || codeStack.top().getLexeme() == ";" || codeStack.top().getLexeme() == ")" || codeStack.top().getLexeme() == "]" || codeStack.top().getLexeme() == ",")
    {
    }
    else
    {
        reject(44);
    }
}
void Parser::MulOp()
{
    if(codeStack.top().getLexeme() == "*")
    {
        pushToDStack("*");
    }
    else if(codeStack.top().getLexeme() == "/")
    {
        pushToDStack("/");
    }
    else
    {
        reject(45);
    }
}
void Parser::Factor()
{
    if(codeStack.top().getLexeme() == "(")
    {
        pushToDStack("(");
        Expression();
        if(codeStack.top().getLexeme() == ")")
        {
            pushToDStack(")");
        }
    }
    else if(codeStack.top().getLexeme() == "NUM")
    {
        pushToDStack("NUM");
    }
    else if (codeStack.top().getLexeme() == "ID")
    {
        pushToDStack("ID");
        if(codeStack.top().getLexeme() == "(")
        {
            Call();
        }
        else
        {
            Var();
        }
    }
    else
    {
        reject(46);
    }
}
void Parser::Call()
{
    if(codeStack.top().getLexeme() == "(")
    {
        pushToDStack("(");
        Args();
        if(codeStack.top().getLexeme() == ")")
        {
            pushToDStack(")");
        }
        else
        {
            reject(47);
        }
    }
    else
    {
        reject(48);
    }
}
void Parser::Args()
{
    if(codeStack.top().getLexeme() == ")")
    {
    }
    else
    {
        ArgsList();
    }
}
void Parser::ArgsList()
{
    Expression();
    ArgListLoop();
}
void Parser::ArgListLoop()
{
    if(codeStack.top().getLexeme() == ",")
    {
        pushToDStack(",");
        Expression();
        ArgListLoop();
    }
    else if(codeStack.top().getLexeme() == ")")
    {
    }
    else
    {
        reject(49);
    }
}

void Parser::semanticReject(int rejcode)
{
    cout<<"REJECT"<<endl;
    exit(rejcode);
}
