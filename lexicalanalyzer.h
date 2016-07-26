//
//  lexicalanalyzer.h
//  LexicalAnalyzer
//
//  Created by Kyle Liu on 1/18/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#ifndef lexicalanalyzer_h
#define lexicalanalyzer_h

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "Token.h"
#include "TokenList.h"

using std::string;
using std::ifstream;
using std::fstream;
using std::ofstream;
using std::cerr;
using std::cout;
using std::endl;
using std::ios;
using std::stoi;

class LexicalAnalyzer
{
public:
    LexicalAnalyzer();
    LexicalAnalyzer(string srcFileName = "");
    ~LexicalAnalyzer();
    void run();
    void printTokenList();              //Debug Purpose Use
    Token* getTokenListStartPtr();
    
private:
    void processLine(string);
    bool isKeyword(string);
    bool isSpSym(string);
    bool isPotentialID(string);
    bool isPotentialNum(string);
    bool isValidChar(char);
    bool isDelim(char);
    bool isSym(char);
    ifstream srcFile;
    int commentDepth;                   //0 if not in a comment, >0 if in a comment
    const static int KEYWORD_COUNT = 7;            //Amount of keywords allowed
    const static char* KEYWORDS[KEYWORD_COUNT];    //contains all the keywords
    const static int SPSYM_COUNT = 21;             //Amount of special symbols allowed
    const static char* SPSYM[SPSYM_COUNT];          //contains all the valid special symbols
    int lineCount;                          //keep track of the amount of lines read
    const static int VALCHARCOUNT = 15;
    const static char VALIDCHAR[VALCHARCOUNT];
    TokenList tokenList;
    
};

#endif /* lexicalanalyzer_h */
