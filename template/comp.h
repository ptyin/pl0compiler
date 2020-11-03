//
// Created by Peter on 2020/11/3.
//

#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

const static int maxIdLength = 10;
const static int numLinkData = 3;

enum Token
{
    TMP, IDENT, NUM, PERIOD, CONSTSYM, COMMA, LPAREN, RPAREN, EQ, SEMICOLON, COLON,
    ASSIGN, VARSYM, PROCSYM, BEGINSYM, ENDSYM, ODDSYM, IFSYM, THENSYM,
    CALLSYM, WHILESYM, DOSYM, WRITESYM, READSYM, PLUS, MINUS, TIMES, SPLASH,
    NEQ, LSS, LE, GT, GE
};
enum SymbolType
{
    CONST, VARIABLE, PROCEDURE
};
struct Symbol
{
    int type;
    char name[maxIdLength + 1];
    int value;
    int level;
    int address;
};
enum
{
    LIT, LOD, STO, CAL, INT, JMP, JPC, OPR
};
enum OPS
{
    OP_RET = 0, OP_ADD = 2, OP_MINUS = 3, OP_TIMES = 4, OP_DIV = 5,
    OP_NEQ = 9, OP_EQ = 8, OP_LSS = 7, OP_LE = 6, OP_GT = 10, OP_GE = 11,
    OP_READ = 16, OP_WRITE = 15
};
struct PCode
{
    int op;
    int l;
    int a;
    PCode(int op = -1, int l = 0, int a = 0)
    {
        this->op = op;
        this->l = l;
        this->a = a;
    }
};

extern const char CodeTable[8][4];

vector<Symbol> symTable(1000);
vector<PCode> code(1000);
extern int sym, num;
extern char id[maxIdLength + 1];
extern int pc;
extern int line;
FILE *f;

int findKeyword(const char *str);

int getSym(FILE *in);

inline int getSym()
{
    return getSym(f);
}

int block(int level, int index);
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

int find(int from, int to, const char *name);
void printErr(const char *err);

inline void genCode(int op, int l, int a);

void interprete();