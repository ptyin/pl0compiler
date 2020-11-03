//
// Created by Peter on 2020/11/3.
//

#ifndef COMPILER_SYNTACTICANALYZER_H
#define COMPILER_SYNTACTICANALYZER_H

#include "LexicalAnalyzer.h"

class SyntacticAnalyzer: LexicalAnalyzer
{
protected:
    enum fct {LIT,  LOD, STO, CAL, INT, JMP, JPC, OPR};
    const int MAX_LEVEL = 3;
    const int MAX_ENTRIES = 1000;
    int pc;
    struct TableEntry
    {
        char name[MAX_LENGTH];
        enum{CONSTANT, VARIABLE, PROCEDURE} type;
        int value;      // for CONSTANT
        int level;      // for VARIABLE & PROCEDURE
        int address;    // for VARIABLE & PROCEDURE
        int size;       // for PROCEDURE
        TableEntry()=default;
    } *table;
    int totalCount;

    int constDeclaration(int tx);
    int varDeclaration(int lev, int tx);
    int procDeclaration(int lev, int tx);
    int statement(int lev, int tx);
    int assignStatement(int lev, int tx);
    int ifStatement(int lev, int tx);
    int whileStatement(int lev, int tx);
    int callStatement(int lev, int tx);
    int readStatement(int lev, int tx);
    int writeStatement(int lev, int tx);
    int compositeStatement(int lev, int tx);
    int condition(int lev, int tx);
    int expression(int lev, int tx);
    int term(int lev, int tx);
    int factor(int lev, int tx);

    int findInTable(const char *name);
public:
    SyntacticAnalyzer(FILE *inFile, FILE *outFile) : LexicalAnalyzer(inFile, outFile)
    {
        pc = 1;
        totalCount = 0;
        table = new TableEntry[MAX_ENTRIES];
    }
    ~SyntacticAnalyzer()
    {
        delete[] table;
    }
    int block(int lev, int tx);
};


#endif //COMPILER_SYNTACTICANALYZER_H
