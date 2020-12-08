//
// Created by Peter on 2020/11/3.
//

#ifndef COMPILER_SYNTACTICANALYZER_H
#define COMPILER_SYNTACTICANALYZER_H

#include "../utils/ParseTree.h"
#include "LexicalAnalyzer.h"
using std::vector;

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

    struct Procedure
    {
        vector<TableEntry *> entries;
        vector<Procedure *> children;
        TableEntry *entry;
        Procedure *father;
        Procedure(): Procedure(nullptr){}
        explicit Procedure(TableEntry * _entry): entry(_entry), father(nullptr){}
    };
    vector<Procedure *> procedures;

    void addChild(Procedure *procedure, TableEntry *child_entry)
    {
        auto *child = new Procedure(child_entry);
        procedures.push_back(child);
        child->father = procedure;
        procedure->children.push_back(child);
    }


    enum OPERAND
    {
        OP_RET = 0, OP_NEG=1, OP_PLUS = 2, OP_MINUS = 3, OP_TIMES = 4, OP_SLASH = 5,
        OP_ODD = 6,
        OP_EQU = 8, OP_NEQ = 9, OP_LES = 10, OP_GEQ = 11, OP_GTR = 12, OP_LEQ = 13,
        OP_WRITE = 14, OP_LN = 15, OP_READ = 16
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

    int constDeclaration(int tx, Procedure *current);
    int varDeclaration(int level, int tx, Procedure *current);
    int procDeclaration(int level, int tx, Procedure *current);
    int statement(int level, int tx, Procedure *current);
    int assignStatement(int level, int tx, Procedure *current);
    int ifStatement(int level, int tx, Procedure *current);
    int whileStatement(int level, int tx, Procedure *current);
    int callStatement(int level, int tx, Procedure *current);
    int readStatement(int level, int tx, Procedure *current);
    int writeStatement(int level, int tx, Procedure *current);
    int compositeStatement(int level, int tx, Procedure *current);
    int condition(int level, int tx, Procedure *current);
    int expression(int level, int tx, Procedure *current);
    int term(int level, int tx, Procedure *current);
    int factor(int level, int tx, Procedure *current);

    TableEntry *findInTable(const char *name, int count);
    bool checkInTree(const char *name, Procedure *current);
    TableEntry *findInTree(const char *name, Procedure *current, bool flag=true);
public:
    ParseTree parseTree;
    int pc;
    SyntacticAnalyzer(FILE *inFile, FILE *outFile) : LexicalAnalyzer(inFile, outFile)
    {
        pc = 1;
        table = new TableEntry[MAX_ENTRIES];
        code = new PCode[MAX_CODE];

        // --------------------------------------------
        procedures.push_back(new Procedure);  // add root
        // --------------------------------------------
    }
    ~SyntacticAnalyzer()
    {
        for (auto *procedure: procedures)
        {
            delete procedure;
        }
        delete[] table;
        delete[] code;
    }
    int block(int lev, int tx);
    inline void gen(OP op, int l, int a);
    void printCode();
};


#endif //COMPILER_SYNTACTICANALYZER_H
