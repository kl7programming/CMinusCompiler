OBJS = lexicalanalyzer.o main.o TokenList.o Token.o parser.o semanticanalyzer.o SymbolTable.o codegenerator.o
CC = g++ 
CFLAGS = -c -g -std=c++0x

proj2 :	$(OBJS)
	$(CC)	$(OBJS)	-o p4

main.o : main.cpp lexicalanalyzer.h parser.h semanticanalyzer.h SymbolTable.h TokenList.h Token.h
	$(CC)	$(CFLAGS) main.cpp

semanticanalyzer.o : semanticanalyzer.cpp semanticanalyzer.h lexicalanalyzer.h TokenList.h Token.h SymbolTable.h
	$(CC)	$(CFLAGS) semanticanalyzer.cpp

parser.o : parser.cpp parser.h lexicalanalyzer.h TokenList.h Token.h SymbolTable.h
	$(CC)	$(CFLAGS) parser.cpp

codegenerator.o : codegenerator.cpp codegenerator.h Token.h
	$(CC)	$(CFLAGS) codegenerator.cpp

SymbolTable.o : SymbolTable.cpp SymbolTable.h
	$(CC)	$(CFLAGS) SymbolTable.cpp

lexicalanalyzer.o : lexicalanalyzer.cpp lexicalanalyzer.h TokenList.h Token.h
	$(CC)	$(CFLAGS) lexicalanalyzer.cpp

TokenList.o : TokenList.cpp TokenList.h Token.h
	$(CC)	$(CFLAGS) TokenList.cpp

Token.o : Token.h Token.cpp
	$(CC)	$(CFLAGS) Token.cpp
clean:
	\rm *.o *~ proj2

