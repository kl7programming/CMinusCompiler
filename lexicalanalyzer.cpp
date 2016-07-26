//
//  lexicalanalyzer.cpp
//  LexicalAnalyzer
//
//  Created by Kyle Liu on 1/18/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include "lexicalanalyzer.h"

const char* LexicalAnalyzer::KEYWORDS[] = {"else", "if", "int", "return", "void", "while", "float"};
const char* LexicalAnalyzer::SPSYM[] = {"+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=", "=", ";", ",", "(", ")", "[", "]", "{", "}", "/*", "*/"};
const char LexicalAnalyzer::VALIDCHAR[] = {'+', '-', '*', '/', '<', '>', '=', ';', ',', '(', ')', '[', ']', '{', '}'};

LexicalAnalyzer::LexicalAnalyzer()
{
    cerr<<"No Source Code. Exiting ...\n";
    exit(1);
}

LexicalAnalyzer::LexicalAnalyzer(string srcFileName)
{
    srcFile.open(srcFileName, ios::in);
    
    if(!srcFile)
    {
        cerr<<"Fail to Open File. Exiting...\n";
        exit(1);
    }
    commentDepth = 0;
    lineCount = 0;
}

LexicalAnalyzer::~LexicalAnalyzer()
{
    srcFile.close();
}

void LexicalAnalyzer::run()
{
    string line;
    
    while(getline(srcFile, line))
    {
        if(line.length() != 0)
        {
            lineCount++;
            processLine(line);
        }
    }
}

void LexicalAnalyzer::processLine(string line)
{
    string testBlock;
    //cout<<"input: "<<line<<endl;
    
    for(int i = 0; i <= line.length(); i++)
    {
        if(line[i] == '/' && line[i+1] == '*')
        {
            commentDepth++;
            i++;
            continue;
        }
        
        if((line[i] == '*' && line[i+1] == '/') && commentDepth>0)
        {
            commentDepth--;
            i++;
            continue;
        }
        
        if((line[i] == '/' && line[i+1] == '/') && commentDepth==0)
        {
            break;
        }
        
        if(commentDepth == 0 && !isspace(line[i]))
        {
            testBlock+=line[i];
        
            if(((testBlock.compare("<")==0||testBlock.compare(">")==0||testBlock.compare("=")==0||testBlock.compare("!")==0)&&line[i+1]=='='))
            {
                continue;
            }
            
            if(!isValidChar(line[i]))
            {
                while(!isDelim(line[i+1]) && !isValidChar(line[i+1]))
                {
                    i++;
                    testBlock+=line[i];
                }
                
                cout<<"ERROR on line "<<lineCount<<": "<<testBlock<<endl;
                testBlock.clear();
                continue;
            }
            
            if(isKeyword(testBlock) && (isspace(line[i+1])||isSym(line[i+1]) || line[i+1]=='\0'))
            {
                //cout<<"KEYWORD: "<<testBlock<<endl;
                tokenList.insertToken(testBlock, "KEYWORD");
                testBlock.clear();
                continue;
            }
            
            if(isSpSym(testBlock))
            {
                tokenList.insertToken(testBlock, "SPSYM");
                testBlock.clear();
                continue;
            }
            
            if(isPotentialID(testBlock) && !isalpha(line[i+1]))
            {
                //cout<<"id: "<<testBlock<<endl;
                tokenList.insertToken(testBlock, "ID");
                testBlock.clear();
                continue;
            }
            
            if(isPotentialNum(testBlock) && (line[i+1]=='.'))
            {
                i++;
                testBlock+=line[i];
                while(isdigit(line[i+1]))
                {
                    i++;
                    testBlock+=line[i];
                }
                if(line[i+1]=='e'||line[i+1]=='E')
                {
                    i++;
                    testBlock+=line[i];
                }
                if(line[i+1]=='-'||line[i+1]=='+')
                {
                    i++;
                    testBlock+=line[i];
                }
                while(isdigit(line[i+1]))
                {
                    i++;
                    testBlock+=line[i];
                }
                
                //cout<<"FLOAT: "<<testBlock<<endl;
                tokenList.insertToken(testBlock, "FLOAT");
                testBlock.clear();
                continue;
            }
            
            if(isPotentialNum(testBlock) && !isdigit(line[i+1]))
            {
                //cout<<"NUM: "<<testBlock<<endl;
                tokenList.insertToken(testBlock, "NUM");
                testBlock.clear();
                continue;
            }
        }
        
    }
}

bool LexicalAnalyzer::isKeyword(string test)
{
    for(int i = 0; i < KEYWORD_COUNT; i++)
    {
        if(test.compare(KEYWORDS[i]) == 0)
        {
            return true;
        }
    }
    
    return false;
}

bool LexicalAnalyzer::isSpSym(string test)
{
    for(int i = 0; i < SPSYM_COUNT; i++)
    {
        if(test.compare(SPSYM[i]) == 0)
        {
            //cout<<test<<endl;
            return true;
        }
    }
    
    return false;
}

bool LexicalAnalyzer::isPotentialID(string test)
{
    if(test.length() == 0)
    {
        return false;
    }
    
    for(int i = 0; i < test.length(); i++)
    {
        if(!isalpha(test[i]))
        {
            return false;
        }
    }
    
    return true;
}

bool LexicalAnalyzer::isPotentialNum(string test)
{
    if(test.length() == 0)
    {
        return false;
    }
    
    for(int i = 0; i < test.length(); i++)
    {
        if(!isdigit(test[i]))
        {
            return false;
        }
    }
    
    return true;
}

bool LexicalAnalyzer::isValidChar(char test)
{
    if(isalnum(test) || isspace(test)||test=='\0')
    {
        return true;
    }
    
    for(int i = 0; i < VALCHARCOUNT; i++)
    {
        if(test == VALIDCHAR[i])
        {
            return true;
        }
    }
    
    return false;
}

bool LexicalAnalyzer::isSym(char test)
{
    for(int i = 0; i < VALCHARCOUNT; i++)
    {
        if(test == VALIDCHAR[i])
        {
            return true;
        }
    }
    
    return false;
}

bool LexicalAnalyzer::isDelim(char test)
{
    if(isspace(test) || test == ';' || test == '/')
    {
        return true;
    }
    
    return false;
}

void LexicalAnalyzer::printTokenList()
{
    Token* curptr = tokenList.getStartToken();
    
    while(curptr->getNextTokPtr() != NULL)
    {
        if(curptr->getDataType() == "ID")
        {
            cout<<"ID | ";
        }
        else if(curptr -> getDataType() == "NUM" || curptr->getDataType() == "FLOAT")
        {
            cout<<"NUM | ";
        }
        else
        {
            cout<<curptr->getLexeme()<<" | ";
        }
        curptr = curptr->getNextTokPtr();
    }
    if(curptr->getDataType() == "ID")
    {
        cout<<"ID | ";
    }
    else if(curptr -> getDataType() == "NUM" || curptr->getDataType() == "FLOAT")
    {
        cout<<"NUM | ";
    }
    else
    {
        cout<<curptr->getLexeme()<<" | ";
    }

    cout<<"\n";
}

Token* LexicalAnalyzer::getTokenListStartPtr()
{
    return tokenList.getStartToken();
}