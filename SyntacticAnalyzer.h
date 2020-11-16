//
// Created by Peter on 2020/11/3.
//

#ifndef COMPILER_SYNTACTICANALYZER_H
#define COMPILER_SYNTACTICANALYZER_H

#include "LexicalAnalyzer.h"

class SyntacticAnalyzer: public LexicalAnalyzer
{
protected:
//    enum fct {LIT,  LOD, STO, CAL, INT, JMP, JPC, OPR};
    static const int MAX_LEVEL = 3;
    static const int MAX_ENTRIES = 1000;
    static const int MAX_CODE = 1000;
    static const int MAX_OP = 10;
    static const int NUM_LINK_DATA = 3;  // 3 units are needed to indicate the link data(dynamic or stationary)
    struct TableEntry
    {
        char name[MAX_LENGTH]{};
        enum Type{UNDEFINED, CONSTANT, VARIABLE, PROCEDURE} type{UNDEFINED};
        int value{-1};      // for CONSTANT
        int level{-1};      // for VARIABLE & PROCEDURE
        int address{-1};    // for VARIABLE & PROCEDURE, data segment address or code segment address
        int size{-1};       // for PROCEDURE
        TableEntry()=default;
        explicit TableEntry(Type _type): type(_type){}
    } *table;
    enum OPERAND
    {
        OP_RET = 0, OP_PLUS = 2, OP_MINUS = 3, OP_TIMES = 4, OP_SLASH = 5,
        OP_LEQ = 6, OP_LES = 7, OP_EQU = 8, OP_NEQ = 9, OP_GTR = 10, OP_GEQ = 11,
        OP_WRITE = 15, OP_READ = 16
    };
    enum OP{LIT, LOD, STO, CAL, INT, JMP, JPC, OPR, UNDEFINED};
    const char *OPTable[MAX_OP] = {"LIT", "LOD", "STO", "CAL", "INT", "JMP", "JPC", "OPR", "UNDEFINED"};
    struct PCode
    {
        OP op;
        int l;
        int a;
        explicit PCode(OP _op = UNDEFINED, int _l = 0, int _a = 0):op(_op), l(_l), a(_a){}
    } *code;

    int constDeclaration(int tx);
    int varDeclaration(int level, int tx);
    int procDeclaration(int level, int tx);
    int statement(int level, int tx);
    int assignStatement(int level, int tx);
    int ifStatement(int level, int tx);
    int whileStatement(int level, int tx);
    int callStatement(int level, int tx);
    int readStatement(int level, int tx);
    int writeStatement(int level, int tx);
    int compositeStatement(int level, int tx);
    int condition(int level, int tx);
    int expression(int level, int tx);
    int term(int level, int tx);
    int factor(int level, int tx);

    int findInTable(const char *name, int count);
public:
    int pc;
    SyntacticAnalyzer(FILE *inFile, FILE *outFile) : LexicalAnalyzer(inFile, outFile)
    {
        pc = 1;
        table = new TableEntry[MAX_ENTRIES];
        code = new PCode[MAX_CODE];
    }
    ~SyntacticAnalyzer()
    {
        delete[] table;
        delete[] code;
    }
    int block(int lev, int tx);
    inline void genCode(OP op, int l, int a);
    void printCode();
};


#endif //COMPILER_SYNTACTICANALYZER_H
