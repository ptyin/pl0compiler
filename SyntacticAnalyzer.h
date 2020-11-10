//
// Created by Peter on 2020/11/3.
//

#ifndef COMPILER_SYNTACTICANALYZER_H
#define COMPILER_SYNTACTICANALYZER_H

#include "LexicalAnalyzer.h"

class SyntacticAnalyzer: LexicalAnalyzer
{
protected:
//    enum fct {LIT,  LOD, STO, CAL, INT, JMP, JPC, OPR};
    const int MAX_LEVEL = 3;
    const int MAX_ENTRIES = 1000;
    const int MAX_CODE = 1000;
    const int CODE_LENGTH = 3;
    int pc;
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
    struct PCode
    {
        enum OP{LIT, LOD, STO, CAL, INT, JMP, JPC, OPR, UNDEFINED} op;
        int l;
        int a;
        explicit PCode(OP _op = UNDEFINED, int _l = 0, int _a = 0):op(_op), l(_l), a(_a){}
    } *code;

    int constDeclaration(int index);
    int varDeclaration(int level, int index);
    int procDeclaration(int level, int index);
    int statement(int level, int index);
    int assignStatement(int level, int index);
    int ifStatement(int level, int index);
    int whileStatement(int level, int index);
    int callStatement(int level, int index);
    int readStatement(int level, int index);
    int writeStatement(int level, int index);
    int compositeStatement(int level, int index);
    int condition(int level, int index);
    int expression(int level, int index);
    int term(int level, int index);
    int factor(int level, int index);

    int findInTable(const char *name, int count);
public:
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
    inline void genCode(PCode::OP op, int l, int a);
};


#endif //COMPILER_SYNTACTICANALYZER_H
