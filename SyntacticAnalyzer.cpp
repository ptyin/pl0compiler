//
// Created by Peter on 2020/11/3.
//

#include "SyntacticAnalyzer.h"
#include "exception/SyntaxException.h"

/**Main part of Syntactic Analyzer
 * @param   lev     record the current level of the table;
 * @param   tx      the table pointer
 * @var     dx      the offset of local variable
 * @var     count   count of entries already inserted
 */
int SyntacticAnalyzer::block(int lev, int tx)
{
//    int txSaved = tx;   // save tx
    int count = 0, dx = 0;
    int tpc = pc;
    genCode(JMP, 0, 0);  // jump to statement
    if(lev > MAX_LEVEL)
        return -1;
    if (SYM == SYM_CONST)
        count += constDeclaration(tx);
    if (SYM == SYM_VAR)
        count += dx = varDeclaration(lev, tx + count);
    if(SYM == SYM_PROCEDURE)
    {
        count += procDeclaration(lev, tx + count);
        code[tpc].a = pc;
    } else
        pc--;

    genCode(INT, 0, NUM_LINK_DATA + dx);  // jump to statement
    statement(lev, tx + count);
    genCode(OPR, 0, OP_RET);
    return 0;
}


inline void SyntacticAnalyzer::genCode(OP op, int l, int a)
{
    PCode temp(op, l, a);
    code[pc++] = temp;
}

/**Const Declaration
 * @var     tx      current table pointer
 */
int SyntacticAnalyzer::constDeclaration(int tx)
{
    int count = 0;
    TableEntry entry(TableEntry::CONSTANT);
    do
    {
        getSym();
        if (SYM != SYM_IDENTIFIER)
            throw SyntaxException("identifier expected!");
        else
        {
            if (findInTable(strToken, tx) != -1)  // already declared
                throw SyntaxException("duplicated const identifier!");
            else
            {
                strcpy_s(entry.name, strlen(strToken) + 1, strToken);
                getSym();
                if (SYM != SYM_EQU)
                    throw SyntaxException("'=' expected");
                getSym();
                if (SYM != SYM_NUMBER)
                    throw SyntaxException("number expected");
                entry.value = NUM;
                table[tx + count++] = entry;
                getSym();
                if(SYM != SYM_COMMA && SYM != SYM_SEMICOLON)
                    throw SyntaxException("comma or semicolon expected");
            }
        }
    } while (SYM != SYM_SEMICOLON);
    getSym();
    return count;
}

/**Variable Declaration
 * @param   level   current level
 * @param   tx      current table pointer
 */
int SyntacticAnalyzer::varDeclaration(int level, int tx)
{
    int count = 0;
    TableEntry entry(TableEntry::VARIABLE);
    entry.level = level;
    entry.address = 0;
    do
    {
        getSym();
        if (SYM != SYM_IDENTIFIER)
            throw SyntaxException("identifier expected");
        if(findInTable(strToken, count) != -1)
            throw SyntaxException("duplicated variable identifier");

        strcpy_s(entry.name, strlen(strToken) + 1, strToken);
        table[tx + count++] = entry;
        entry.address++;
        getSym();
        if (SYM != SYM_COMMA && SYM != SYM_SEMICOLON)
            throw SyntaxException("comma or semicolon expected");
    } while (SYM != SYM_SEMICOLON);
    getSym();
    return count;
}

/**Procedure Declaration
 * @param   level   current level
 * @param   tx      current table pointer
 */
int SyntacticAnalyzer::procDeclaration(int level, int tx)
{
    int count = 0;
    TableEntry entry(TableEntry::PROCEDURE);
    getSym();
    if (SYM != SYM_IDENTIFIER)
        throw SyntaxException("identifier expected");
    entry.level = level+1;
    entry.address = pc;
    table[tx + count++] = entry;
    count++;
    getSym();
    // procedure head
    if(SYM != SYM_SEMICOLON)
        throw SyntaxException("semicolon expected");
    getSym();
    // sub procedure
    count += block(level+1, tx + count);
    getSym();
    if(SYM != SYM_SEMICOLON)
        throw SyntaxException("semicolon expected");
    getSym();
    // optional subsequent procedure declaration
    if (SYM == SYM_PROCEDURE)
    {
        getSym();
        count += procDeclaration(level, tx + count);
    }
    return count;
}

int SyntacticAnalyzer::statement(int level, int tx)
{
    if (SYM == SYM_IF)
    {
        getSym();
        ifStatement(level, tx);
    }
    else if (SYM == SYM_WHILE)
    {
        getSym();
        whileStatement(level, tx);
    }
    else if (SYM == SYM_CALL)
    {
        getSym();
        callStatement(level, tx);
    }
    else if (SYM == SYM_WRITE)
    {
        getSym();
        writeStatement(level, tx);
    }
    else if (SYM == SYM_READ)
    {
        getSym();
        readStatement(level, tx);
    }
    else if (SYM == SYM_BEGIN)
    {
        getSym();
        compositeStatement(level, tx);
    }
    else if (SYM == SYM_IDENTIFIER)
    {
        assignStatement(level, tx);
    }
    return 0;
}

int SyntacticAnalyzer::ifStatement(int level, int tx)
{
    condition(level, tx);
    int falsePC = pc;
    genCode(JPC, 0, 0);
    if (SYM != SYM_THEN)
        throw SyntaxException("then clause expected");
    getSym();
    statement(level, tx);
    code[falsePC].a = pc;  // if not true, jump to this
    return 0;
}

int SyntacticAnalyzer::whileStatement(int level, int tx)
{
    int loopPC = pc;
    condition(level, tx);
    int conditionPC = pc;
    genCode(JPC, 0, 0);
    if (SYM != SYM_DO)
        throw SyntaxException("do expected");
    getSym();
    statement(level, tx);
    genCode(JMP, 0, loopPC);
    code[loopPC].a = pc;

    return 0;
}

int SyntacticAnalyzer::callStatement(int level, int tx)
{
    if (SYM != SYM_IDENTIFIER)
        throw SyntaxException("identifier expected");
    int id = findInTable(strToken, tx);
    if (id == -1)
        throw SyntaxException("identifier not found");
    if (table[id].type != TableEntry::PROCEDURE)
        throw SyntaxException("identifier you call is not a procedure");
    genCode(CAL, level - table[id].level, table[id].address);
    getSym();
    return 0;
}

int SyntacticAnalyzer::readStatement(int level, int tx)
{
    if (SYM != SYM_LPAREN)
        throw SyntaxException("variable expected");
    getSym();
    while (SYM != SYM_RPAREN)
    {
        if(SYM != SYM_IDENTIFIER)
            throw SyntaxException("identifier expected");
        int i = findInTable(strToken, tx);
        if(i<0)
            throw SyntaxException("identifier not found");
        if(table[i].type != TableEntry::VARIABLE)
            throw SyntaxException("identifier you call is not a variable");
        genCode(OPR, 0, OP_READ);
        genCode(STO, level-table[i].level, table[i].address + NUM_LINK_DATA);
        getSym();
        if (SYM != SYM_COMMA && SYM != SYM_RPAREN)
            throw SyntaxException("syntax error");
    }
    getSym();
    return 0;
}

int SyntacticAnalyzer::writeStatement(int level, int tx)
{
    if (SYM != SYM_LPAREN)
        throw SyntaxException("'(' expected");
    getSym();
    while(SYM != SYM_RPAREN)
    {
        expression(level, tx);
        genCode(OPR, 0, OP_WRITE);
        if(SYM != SYM_COMMA && SYM != SYM_RPAREN)
            throw SyntaxException("syntax error");
    }
    getSym();
    return 0;
}

int SyntacticAnalyzer::compositeStatement(int level, int tx)
{
    statement(level, tx);
    while (SYM == SYM_SEMICOLON)
    {
        getSym();
        statement(level, tx);
    }
    if (SYM != SYM_END)
        throw SyntaxException("end expected");
    getSym();
    return 0;
}

int SyntacticAnalyzer::assignStatement(int level, int tx)
{
    int i = findInTable(strToken, tx);
    if(i<0)
        throw SyntaxException("identifier not found");
    if(table[i].type != TableEntry::VARIABLE)
        throw SyntaxException("variable expected");
    getSym();
    if (SYM != SYM_ASSIGN)
        throw SyntaxException(":= expected");
    getSym();
    expression(level, tx);
    genCode(STO, level-table[i].level, NUM_LINK_DATA+table[i].address);
    return 0;
}

int SyntacticAnalyzer::condition(int level, int tx)
{
    if (SYM == SYM_ODD)
    {
        getSym();
        expression(level, tx);
        genCode(LIT, 0, 0);
        genCode(OPR, 0, OP_NEQ);
    }
    else
    {
        expression(level, tx);
        if (SYM != SYM_NEQ && SYM != SYM_EQU && SYM != SYM_LES && SYM != SYM_LEQ && SYM != SYM_GTR && SYM != SYM_GEQ)
            throw SyntaxException("boolean operator expected");
        int op = SYM;
        expression(level, tx);
        switch (op)
        {
            case SYM_NEQ:
                genCode(OPR, 0, OP_NEQ); break;
            case SYM_EQU:
                genCode(OPR, 0, OP_EQU); break;
            case SYM_LES:
                genCode(OPR, 0, OP_LES); break;
            case SYM_LEQ:
                genCode(OPR, 0, OP_LEQ); break;
            case SYM_GTR:
                genCode(OPR, 0, OP_GTR); break;
            case SYM_GEQ:
                genCode(OPR, 0, OP_GEQ); break;
            default:
                throw SyntaxException("boolean operator expected");
        }
    }
    return 0;
}

int SyntacticAnalyzer::expression(int level, int tx)
{
    int op = SYM;
    if (SYM == SYM_PLUS || SYM == SYM_MINUS)
        getSym();
    term(level, tx);
    if (op == SYM_MINUS)  // negative
    {
        genCode(LIT, 0, 0);
        genCode(OPR, 0, OP_MINUS);
    }
    do
    {
        op = SYM;
        if (SYM == SYM_PLUS || SYM == SYM_MINUS)
        {
            getSym();
            term(level, tx);
            if(op == SYM_PLUS)
                genCode(OPR, 0, OP_PLUS);
            else
                genCode(OPR, 0, OP_MINUS);
        }
    } while (SYM == SYM_PLUS || SYM == SYM_MINUS);
    return 0;
}

int SyntacticAnalyzer::term(int level, int tx)
{
    factor(level, tx);
    int op = SYM;
    if (op != SYM_TIMES && op != SYM_SLASH)
        return 0;
    do
    {
        getSym();
        factor(level, tx);
        if (op == SYM_TIMES)
            genCode(OPR, 0, OP_TIMES);
        else
            genCode(OPR, 0, OP_SLASH);
        op = SYM;
    } while (SYM == SYM_TIMES || SYM == SYM_SLASH);
    return 0;
}

int SyntacticAnalyzer::factor(int level, int tx)
{
    if(SYM == SYM_IDENTIFIER)
    {
        int i = findInTable(strToken, tx);
        if (i < 0)
            throw SyntaxException("Identifier not found");
        if (table[i].type == TableEntry::CONSTANT)
            genCode(LIT, 0, table[i].value);
        else if (table[i].type == TableEntry::VARIABLE)
            genCode(LOD, level-table[i].level, NUM_LINK_DATA + table[i].address);
        else
            throw SyntaxException("variable or constant expected");
        getSym();
    }
    else if (SYM == SYM_NUMBER)
    {
        genCode(LIT, 0, NUM);
        getSym();
    }
    else if (SYM == SYM_LPAREN)
    {
        getSym();
        expression(level, tx);
        if (SYM != SYM_RPAREN)
            throw SyntaxException("')' expected");
        getSym();
    }
    else
        throw SyntaxException("syntax error");
    return 0;
}

int SyntacticAnalyzer::findInTable(const char *name, int count)
{
    for(int i=0;i<count;i++)
    {
        if(strcmp(name, table[i].name) == 0)
            return i;
    }
    return -1;
}

void SyntacticAnalyzer::printCode()
{
    getSym();
    block(0, 0);
    for (int i = 1; i<pc; i++)
    {
        printf("%d:\t%s %d %d\n", i, OPTable[code[i].op], code[i].l, code[i].a);
    }
}
