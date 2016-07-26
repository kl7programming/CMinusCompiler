//
//  main.cpp
//  LexicalAnalyzer
//
//  Created by Kyle Liu on 1/18/16.
//  Copyright © 2016 KL7. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include "lexicalanalyzer.h"
#include "Token.h"
#include "TokenList.h"
#include "parser.h"
#include "semanticanalyzer.h"
#include "codegenerator.h"

using std::ifstream;
using std::cerr;
using std::string;
using std::cout;

int main(int argc, const char * argv[])
{
    if(argc < 2)
    {
        cerr<<"No Argument Supplied"<<endl;
        cerr<<"usuage: ./p1 <filename>"<<endl;
        exit(1);
    }
    LexicalAnalyzer analyzer(argv[1]);
    analyzer.run();
    //analyzer.printTokenList();
    
    Parser parser(analyzer.getTokenListStartPtr());
    parser.execute();
    
    SemanticAnalyzer sAnalyzer(analyzer.getTokenListStartPtr(), parser.getSymbolTable());
    sAnalyzer.execute();
    
    return 0;
}
