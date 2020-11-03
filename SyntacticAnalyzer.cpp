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
    int txSaved = tx;   // save tx
    int count = 0, dx = 0;
    if(lev > MAX_LEVEL)
        return -1;
    if (SYM == SYM_CONST)
        count += constDeclaration(tx + count);
    if (SYM == SYM_VAR)
    {
        getSym();
        dx = varDeclaration(lev, tx + count);
        count += dx;
    }
    if(SYM == SYM_PROCEDURE)
    {
        getSym();
        count += procDeclaration(lev+1, tx + count);
    }
    return 0;
}

/**Const Declaration
 * @param   tx      the table pointer
 * @var     count   the number of inserted entries
 */
int SyntacticAnalyzer::constDeclaration(int tx)
{
    TableEntry entry;
    int count = 0;
    do
    {
        getSym();
        if (SYM != SYM_IDENTIFIER)
            throw SyntaxException("identifier expected!");
        else
        {
            if (findInTable(strToken) != -1)  // already declared
                throw SyntaxException("duplicated identifier!");
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

int SyntacticAnalyzer::varDeclaration(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::procDeclaration(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::statement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::assignStatement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::ifStatement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::whileStatement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::callStatement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::readStatement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::writeStatement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::compositeStatement(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::condition(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::expression(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::term(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::factor(int lev, int tx)
{
    return 0;
}

int SyntacticAnalyzer::findInTable(const char *name)
{
    for(int i=0;i<totalCount;i++)
    {
        if(strcmp(strToken, name) == 0)
            return i;
    }
    return -1;
}
