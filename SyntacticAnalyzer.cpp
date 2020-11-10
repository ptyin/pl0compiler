//
// Created by Peter on 2020/11/3.
//

#include "SyntacticAnalyzer.h"
#include "exception/SyntaxException.h"

/**Main part of Syntactic Analyzer
 * @param   lev     record the current level of the table;
 * @param   tx      the table pointer
 * @var     dx      the offset of local variable regarding to the table pointer
 * @var     count   count of entries already inserted
 */
int SyntacticAnalyzer::block(int lev, int tx)
{
//    int txSaved = tx;   // save tx
    int count = 0, dx = 0;
    int tpc = pc;
    if(lev > MAX_LEVEL)
        return -1;
    if (SYM == SYM_CONST)
        count += constDeclaration(tx);
    if (SYM == SYM_VAR)
    {
        getSym();
        count += dx = varDeclaration(lev, tx + count);
    }
    if(SYM == SYM_PROCEDURE)
    {
        getSym();
        count += procDeclaration(lev, tx + count);
        pc--;
    } else
        code[tpc].a = pc;

    genCode(PCode::INT, 0, CODE_LENGTH + dx);
    statement(lev, tx + count);
    genCode(PCode::OPR, 0, OP_RET);
    return 0;
}


inline void SyntacticAnalyzer::genCode(PCode::OP op, int l, int a)
{
    PCode temp(op, l, a);
    code[pc++] = temp;
}

/**Const Declaration
 * @var     count   the number of inserted entries
 */
int SyntacticAnalyzer::constDeclaration(int index)
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
            if (findInTable(strToken, index) != -1)  // already declared
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
                table[index + count++] = entry;
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
 * @param   index   current index
 * @var     count   the number of inserted entries
 */
int SyntacticAnalyzer::varDeclaration(int level, int index)
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
        table[index + count++] = entry;
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
 * @param   index   current index
 * @var     count   the number of inserted entries
 */
int SyntacticAnalyzer::procDeclaration(int level, int index)
{
    int count = 0;
    TableEntry entry(TableEntry::PROCEDURE);
    if (SYM != SYM_IDENTIFIER)
        throw SyntaxException("identifier expected");
    entry.level = level+1;
    entry.address = pc;
    table[index + count++] = entry;
    count++;
    getSym();
    // procedure head
    if(SYM != SYM_SEMICOLON)
        throw SyntaxException("semicolon expected");
    getSym();
    // sub procedure
    count += block(level+1, index+count);
    getSym();
    if(SYM != SYM_SEMICOLON)
        throw SyntaxException("semicolon expected");
    getSym();
    // optional subsequent procedure declaration
    if (SYM == SYM_PROCEDURE)
    {
        getSym();
        count += procDeclaration(level, index + count);
    }
    return count;
}

int SyntacticAnalyzer::statement(int level, int index)
{
    if (SYM == SYM_IF)
    {
        getSym();
        ifStatement(level, index);
    }
    else if (SYM == SYM_WHILE)
    {
        getSym();
        whileStatement(level, index);
    }
    else if (SYM == SYM_CALL)
    {
        getSym();
        callStatement(level, index);
    }
    else if (SYM == SYM_WRITE)
    {
        getSym();
        writeStatement(level, index);
    }
    else if (SYM == SYM_READ)
    {
        getSym();
        readStatement(level, index);
    }
    else if (SYM == SYM_BEGIN)
    {
        getSym();
        compositeStatement(level, index);
    }
    else if (SYM == SYM_IDENTIFIER)
    {
        assignStatement(level, index);
    }
    return 0;
}

int SyntacticAnalyzer::ifStatement(int level, int index)
{
    condition(level, index);
    int falsePC = pc;
    genCode(PCode::JPC, 0, 0);
    if (SYM != SYM_THEN)
        throw SyntaxException("then clause expected");
    getSym();
    statement(level, index);
    code[falsePC].a = pc;  // if not true, jump to this
    return 0;
}

int SyntacticAnalyzer::whileStatement(int level, int index)
{
    int loopPC = pc;
    condition(level, index);
    int conditionPC = pc;
    genCode(PCode::JPC, 0, 0);
    if (SYM != SYM_DO)
        throw SyntaxException("do expected");
    getSym();
    statement(level, index);
    genCode(PCode::JMP, 0, loopPC);
    code[loopPC].a = pc;

    return 0;
}

int SyntacticAnalyzer::callStatement(int level, int index)
{
    if (SYM != SYM_IDENTIFIER)
        throw SyntaxException("identifier expected");
    int id = findInTable(strToken, index);
    if (id == -1)
        throw SyntaxException("identifier not found");
    if (table[id].type != TableEntry::PROCEDURE)
        throw SyntaxException("identifier you call is not a procedure");
    genCode(PCode::CAL, level - table[id].level, table[id].address);
    getSym();
    return 0;
}

int SyntacticAnalyzer::readStatement(int level, int index)
{
    return 0;
}

int SyntacticAnalyzer::writeStatement(int level, int index)
{
    return 0;
}

int SyntacticAnalyzer::compositeStatement(int level, int index)
{
    return 0;
}

int SyntacticAnalyzer::assignStatement(int level, int index)
{
    return 0;
}

int SyntacticAnalyzer::condition(int level, int index)
{
    return 0;
}

int SyntacticAnalyzer::expression(int level, int index)
{
    return 0;
}

int SyntacticAnalyzer::term(int level, int index)
{
    return 0;
}

int SyntacticAnalyzer::factor(int level, int index)
{
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
