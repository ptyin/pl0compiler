//
// Created by Peter on 2020/9/23.
//

#ifndef COMPILER_LEXICALANALYZER_H
#define COMPILER_LEXICALANALYZER_H

#define IS_ALPHA(c) (65 <= c && c <= 122)
#define IS_DIGIT(c) (48 <= c && c <= 57)

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>

class LexicalAnalyzer
{
protected:
    static const int KEYWORD_NUM = 13;
    static const int OPERATOR_NUM = 16;
    static const int MAX_LENGTH = 100;
    static const int MAX_VARIABLES = 100;
    static const int MAX_CONST = 100;
    FILE *inFile, *outFile;
    int ch;
    char strToken[MAX_LENGTH];
    const char *buffer;
    class VariableInfo
    {
    public:
        char *name;
        VariableInfo() : name(nullptr){};
        explicit VariableInfo(const char *name_)
        {
            setName(name_);
        }
        void setName(const char *name_)
        {
            int length = strnlen_s(name_, MAX_LENGTH);
            name = new char [length+1];
            strcpy_s(name, length+1, name_);
        }
        ~VariableInfo()
        {
            delete name;
        }
    } *variableTable;
    int *constTable;
    int variableTableIndex;
    int constTableIndex;
    bool stop;
    enum Symbol
    {
        SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END, SYM_IF, SYM_OOD, SYM_PROCEDURE, SYM_THEN, SYM_VAR,
        SYM_WHILE, SYM_READ, SYM_WRITE,
        // ------------
        SYM_IDENTIFIER,
        // ------------
        SYM_NUMBER,
        // ------------
        SYM_PLUS,       // +
        SYM_MINUS,      // -
        SYM_TIMES,      // *
        SYM_SLASH,      // /
        SYM_EQU,        // =
        SYM_NEQ,        // #
        SYM_LES,        // <
        SYM_LEQ,        // <=
        SYM_GTR,        // >
        SYM_GEQ,        // >=
        SYM_LPAREN,     // (
        SYM_RPAREN,     // )
        SYM_COMMA,      // ,
        SYM_SEMICOLON,  // ;
        SYM_PERIOD,     // .
        SYM_BECOMES,    // :=
    };

    void getCh();

    void getBC();

    void concat();

    int reserve();

    void retract();

    int insertId();

    int insertConst();

public:
    const char *keywords[KEYWORD_NUM] = {"begin", "call", "const", "do", "end", "if", "odd", "procedure", "read",
                                         "then", "var", "while", "write"};
    const char *operators[OPERATOR_NUM] = {"+", "-", "*", "/", "=", "#", "<", "<=", ">", ">=",
                                           "(", ")", ",", ";", ".", ":="};

    LexicalAnalyzer(FILE *inFile_, FILE *outFile_) : inFile(inFile_), outFile(outFile_), ch(-1), strToken(), buffer(nullptr),
    SYM(-1), ID(-1), NUM(-1), variableTableIndex(0), constTableIndex(0), stop(true)
    {
        variableTable = new VariableInfo[MAX_VARIABLES];
        constTable = new int[MAX_CONST];
    };
    ~LexicalAnalyzer()
    {
        delete[] variableTable, constTable;
    }
    int SYM, ID, NUM;
    int getSym(const char *);
    int getSym();
    bool isStop() const;
    const char *getStrToken() const;
};

#endif //COMPILER_LEXICALANALYZER_H
