//
// Created by Peter on 2020/11/3.
//

#include "SyntacticAnalyzer.h"
#include "../exception/SyntaxException.h"

/**Main part of Syntactic Analyzer
 * @param   lev     record the current level of the table;
 * @param   tx      the table pointer
 * @var     dx      the offset of local variable
 * @var     count   count of entries already inserted
 */
int SyntacticAnalyzer::block(int lev, int tx)
{
    Procedure *current = procedures.back();
    parseTree.push("<Sub Program>");
    // --------------------------------------------
    int count = 0, dx = 0;
    int tpc = pc;
    gen(JMP, 0, 0);  // jump to statement
    if(lev > MAX_LEVEL)
        throw SyntaxException("procedure declaration exceeds max level");
    if (SYM == SYM_CONST)
        count += constDeclaration(tx, current);
    if (SYM == SYM_VAR)
        count += dx = varDeclaration(lev, tx + count, current);
    if(SYM == SYM_PROCEDURE)
    {
        count += procDeclaration(lev, tx + count, current);
        code[tpc].a = pc;
    } else
        pc--;

    gen(INT, 0, NUM_LINK_DATA + dx);  // jump to statement
    statement(lev, tx + count, current);
    gen(OPR, 0, OP_RET);

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------
    return count;
}


inline void SyntacticAnalyzer::gen(OP op, int l, int a)
{
    PCode temp(op, l, a);
    code[pc++] = temp;
}

/**Const Declaration
 * @var     tx      current table pointer
 */
int SyntacticAnalyzer::constDeclaration(int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Constant Description>");
    parseTree.append("const");
    // --------------------------------------------
    int count = 0;
    TableEntry entry(TableEntry::CONSTANT);
    do
    {
        // --------------------------------------------
        parseTree.push("<Constant Definition>");
        // --------------------------------------------

        getSym();
        if (SYM != SYM_IDENTIFIER)
            throw SyntaxException("identifier expected!", line);
        else
        {
            if (checkInTree(strToken, current))  // already declared
                throw SyntaxException("duplicated const identifier!", line);
            else
            {
                // --------------------------------------------
                parseTree.push("<Identifier>");
                parseTree.append(strToken);
                parseTree.pop();
                // --------------------------------------------

                strcpy_s(entry.name, strlen(strToken) + 1, strToken);
                getSym();
                if (SYM != SYM_EQU)
                    throw SyntaxException("'=' expected", line);

                // --------------------------------------------
                parseTree.append("=");
                // --------------------------------------------

                getSym();
                if (SYM != SYM_NUMBER)
                    throw SyntaxException("number expected", line);

                // --------------------------------------------
                parseTree.push("<Unsigned Integer>");
                parseTree.append(NUM);
                parseTree.pop();
                // --------------------------------------------

                entry.value = NUM;
                table[tx + count++] = entry;

                // --------------------------------------------
                current->entries.push_back(&table[tx + count - 1]);
                // --------------------------------------------

                // --------------------------------------------
                parseTree.pop();
                // --------------------------------------------

                getSym();
                if(SYM != SYM_COMMA && SYM != SYM_SEMICOLON)
                    throw SyntaxException("comma or semicolon expected", line);
                if(SYM == SYM_COMMA)
                    // --------------------------------------------
                    parseTree.append(",");
                    // --------------------------------------------
            }
        }
    } while (SYM != SYM_SEMICOLON);

    // --------------------------------------------
    parseTree.append(";");
    // --------------------------------------------
    getSym();
    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------
    return count;
}

/**Variable Declaration
 * @param   level   current level
 * @param   tx      current table pointer
 */
int SyntacticAnalyzer::varDeclaration(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Variable Description>");
    parseTree.append("var");
    // --------------------------------------------
    int count = 0;
    TableEntry entry(TableEntry::VARIABLE);
    entry.level = level;
    entry.address = 0;
    do
    {
        getSym();
        if (SYM != SYM_IDENTIFIER)
            throw SyntaxException("identifier expected", line);
        if(checkInTree(strToken, current))
            throw SyntaxException("duplicated variable identifier", line);

        // --------------------------------------------
        parseTree.push("<Identifier>");
        parseTree.append(strToken);
        parseTree.pop();
        // --------------------------------------------

        strcpy_s(entry.name, strlen(strToken) + 1, strToken);
        table[tx + count++] = entry;

        // --------------------------------------------
        current->entries.push_back(&table[tx + count - 1]);
        // --------------------------------------------

        entry.address++;
        getSym();
        if (SYM != SYM_COMMA && SYM != SYM_SEMICOLON)
            throw SyntaxException("comma or semicolon expected", line);

        if(SYM == SYM_COMMA)
            // --------------------------------------------
            parseTree.append(",");
            // --------------------------------------------
    } while (SYM != SYM_SEMICOLON);

    // --------------------------------------------
    parseTree.append(";");
    // --------------------------------------------
    getSym();
    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------
    return count;
}

/**Procedure Declaration
 * @param   level   current level
 * @param   tx      current table pointer
 */
int SyntacticAnalyzer::procDeclaration(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Procedure Description>");
    parseTree.push("<Procedure Header>");
    parseTree.append("procedure");
    // --------------------------------------------

    int count = 0;
    TableEntry entry(TableEntry::PROCEDURE);
    getSym();
    if (SYM != SYM_IDENTIFIER)
        throw SyntaxException("identifier expected", line);
    if(checkInTree(strToken, current))
        throw SyntaxException("duplicated procedure identifier", line);

    // --------------------------------------------
    parseTree.push("<Identifier>");
    parseTree.append(strToken);
    parseTree.pop();
    // --------------------------------------------

    strcpy_s(entry.name, strlen(strToken) + 1, strToken);
    entry.level = level;
    entry.address = pc;
    table[tx + count++] = entry;

    // --------------------------------------------
    addChild(current, &table[tx + count - 1]);
    // --------------------------------------------

    getSym();
    // procedure head
    if(SYM != SYM_SEMICOLON)
        throw SyntaxException("semicolon expected", line);

    // --------------------------------------------
    parseTree.append(";");
    // --------------------------------------------

    getSym();
    // sub procedure
    count += block(level+1, tx + count);
    if(SYM != SYM_SEMICOLON)
        throw SyntaxException("semicolon expected", line);

    // --------------------------------------------
    parseTree.append(";");
    // --------------------------------------------

    getSym();
    // optional subsequent procedure declaration
    if (SYM == SYM_PROCEDURE)
        count += procDeclaration(level, tx + count, current);

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return count;
}

int SyntacticAnalyzer::statement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Statement>");

    // --------------------------------------------

    if (SYM == SYM_IF)
    {
        getSym();
        ifStatement(level, tx, current);
    }
    else if (SYM == SYM_WHILE)
    {
        getSym();
        whileStatement(level, tx, current);
    }
    else if (SYM == SYM_CALL)
    {
        getSym();
        callStatement(level, tx, current);
    }
    else if (SYM == SYM_WRITE)
    {
        getSym();
        writeStatement(level, tx, current);
    }
    else if (SYM == SYM_READ)
    {
        getSym();
        readStatement(level, tx, current);
    }
    else if (SYM == SYM_BEGIN)
    {
        getSym();
        compositeStatement(level, tx, current);
    }
    else if (SYM == SYM_IDENTIFIER)
    {
        assignStatement(level, tx, current);
    }

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::ifStatement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<If Statement>");
    parseTree.append("if");
    // --------------------------------------------

    condition(level, tx, current);
    int falsePC = pc;
    gen(JPC, 0, 0);
    if (SYM != SYM_THEN)
        throw SyntaxException("then clause expected", line);

    // --------------------------------------------
    parseTree.append("then");
    // --------------------------------------------

    getSym();
    statement(level, tx, current);
    code[falsePC].a = pc;  // if not true, jump to this

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::whileStatement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<While Statement>");
    parseTree.append("while");
    // --------------------------------------------

    int loopPC = pc;
    condition(level, tx, current);
    int conditionPC = pc;
    gen(JPC, 0, 0);
    if (SYM != SYM_DO)
        throw SyntaxException("do expected", line);

    // --------------------------------------------
    parseTree.append("do");
    // --------------------------------------------

    getSym();
    statement(level, tx, current);
    gen(JMP, 0, loopPC);
    code[conditionPC].a = pc;

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::callStatement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Call Statement>");
    parseTree.append("call");
    // --------------------------------------------

    if (SYM != SYM_IDENTIFIER)
        throw SyntaxException("identifier expected", line);
    TableEntry *id = findInTree(strToken, current);
    if (id == nullptr)
        throw SyntaxException("identifier not found", line);
    if (id->type != TableEntry::PROCEDURE)
        throw SyntaxException("identifier you call is not a procedure", line);

    // --------------------------------------------
    parseTree.push("<Identifier>");
    parseTree.append(strToken);
    parseTree.pop();
    // --------------------------------------------

    gen(CAL, level - id->level, id->address);
    getSym();

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::readStatement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Read Statement>");
    parseTree.append("read");
    // --------------------------------------------

    if (SYM != SYM_LPAREN)
        throw SyntaxException("'(' expected", line);

    // --------------------------------------------
    parseTree.append("(");
    // --------------------------------------------

    while (SYM != SYM_RPAREN)
    {
        getSym();
        if(SYM != SYM_IDENTIFIER)
            throw SyntaxException("identifier expected", line);
        TableEntry *i = findInTree(strToken, current);
        if(i == nullptr)
            throw SyntaxException("identifier not found", line);
        if(i->type != TableEntry::VARIABLE)
            throw SyntaxException("identifier you call is not a variable", line);

        // --------------------------------------------
        parseTree.push("<Identifier>");
        parseTree.append(strToken);
        parseTree.pop();
        // --------------------------------------------

        gen(OPR, 0, OP_READ);
        gen(STO, level - i->level, i->address + NUM_LINK_DATA);
        getSym();
        if (SYM != SYM_COMMA && SYM != SYM_RPAREN)
            throw SyntaxException("syntax error", line);

        // --------------------------------------------
        if (SYM == SYM_COMMA)
            parseTree.append(",");
        // --------------------------------------------

    }

    // --------------------------------------------
    parseTree.append(")");
    // --------------------------------------------

    getSym();

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::writeStatement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Write Statement>");
    parseTree.append("write");
    // --------------------------------------------

    if (SYM != SYM_LPAREN)
        throw SyntaxException("'(' expected", line);

    // --------------------------------------------
    parseTree.append("(");
    // --------------------------------------------

    while(SYM != SYM_RPAREN)
    {
        getSym();
        expression(level, tx, current);
        gen(OPR, 0, OP_WRITE);
        gen(OPR, 0, OP_LN);
        if(SYM != SYM_COMMA && SYM != SYM_RPAREN)
            throw SyntaxException("syntax error", line);

        // --------------------------------------------
        if (SYM == SYM_COMMA)
            parseTree.append(",");
        // --------------------------------------------

    }

    // --------------------------------------------
    parseTree.append(")");
    // --------------------------------------------

    getSym();

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::compositeStatement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Composite Statement>");
    parseTree.append("begin");
    // --------------------------------------------

    statement(level, tx, current);
    while (SYM == SYM_SEMICOLON)
    {
        // --------------------------------------------
        parseTree.append(";");
        // --------------------------------------------

        getSym();
        statement(level, tx, current);
    }
    if (SYM != SYM_END)
        throw SyntaxException("end expected", line);

    // --------------------------------------------
    parseTree.append("end");
    // --------------------------------------------

    getSym();

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------
    return 0;
}

int SyntacticAnalyzer::assignStatement(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Assign Statement>");
    // --------------------------------------------

    TableEntry *i = findInTree(strToken, current);
    if(i == nullptr)
        throw SyntaxException("identifier not found", line);
    if(i->type != TableEntry::VARIABLE)
        throw SyntaxException("variable expected", line);

    // --------------------------------------------
    parseTree.push("<Identifier>");
    parseTree.append(strToken);
    parseTree.pop();
    // --------------------------------------------

    getSym();
    if (SYM != SYM_ASSIGN)
        throw SyntaxException(":= expected", line);

    // --------------------------------------------
    parseTree.append(":=");
    // --------------------------------------------

    getSym();
    expression(level, tx, current);
    gen(STO, level - i->level, NUM_LINK_DATA + i->address);

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::condition(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Condition>");
    // --------------------------------------------

    if (SYM == SYM_ODD)
    {
        // --------------------------------------------
        parseTree.append("odd");
        // --------------------------------------------

        getSym();
        expression(level, tx, current);
        gen(OPR, 0, OP_ODD);
    }
    else
    {
        expression(level, tx, current);
        if (SYM != SYM_NEQ && SYM != SYM_EQU && SYM != SYM_LES && SYM != SYM_LEQ && SYM != SYM_GTR && SYM != SYM_GEQ)
            throw SyntaxException("boolean operator expected", line);

        // --------------------------------------------
        parseTree.push("<Condition Operator>");
        switch (SYM)
        {
            case SYM_NEQ:
                parseTree.append("#"); break;
            case SYM_EQU:
                parseTree.append("="); break;
            case SYM_LES:
                parseTree.append("<"); break;
            case SYM_LEQ:
                parseTree.append("<="); break;
            case SYM_GTR:
                parseTree.append(">"); break;
            case SYM_GEQ:
                parseTree.append(">="); break;
            default:
                throw SyntaxException("boolean operator expected", line);
        }
        parseTree.pop();
        // --------------------------------------------

        int op = SYM;
        getSym();
        expression(level, tx, current);
        switch (op)
        {
            case SYM_NEQ:
                gen(OPR, 0, OP_NEQ); break;
            case SYM_EQU:
                gen(OPR, 0, OP_EQU); break;
            case SYM_LES:
                gen(OPR, 0, OP_LES); break;
            case SYM_LEQ:
                gen(OPR, 0, OP_LEQ); break;
            case SYM_GTR:
                gen(OPR, 0, OP_GTR); break;
            case SYM_GEQ:
                gen(OPR, 0, OP_GEQ); break;
            default:
                throw SyntaxException("boolean operator expected", line);
        }
    }

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::expression(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Expression>");
    // --------------------------------------------

    int op = SYM;

    // --------------------------------------------
    if(SYM == SYM_PLUS)
        parseTree.append("+");
    if(SYM == SYM_MINUS)
        parseTree.append("-");
    // --------------------------------------------

    if (SYM == SYM_PLUS || SYM == SYM_MINUS)
        getSym();
    term(level, tx, current);
    if (op == SYM_MINUS)  // negative
    {
        gen(LIT, 0, 0);
        gen(OPR, 0, OP_MINUS);
    }
    do
    {
        op = SYM;
        if (SYM == SYM_PLUS || SYM == SYM_MINUS)
        {
            // --------------------------------------------
            parseTree.push("<Plus & Minus Operator>");
            if(SYM == SYM_PLUS)
                parseTree.append("+");
            if(SYM == SYM_MINUS)
                parseTree.append("-");
            parseTree.pop();
            // --------------------------------------------

            getSym();
            term(level, tx, current);
            if(op == SYM_PLUS)
                gen(OPR, 0, OP_PLUS);
            else
                gen(OPR, 0, OP_MINUS);
        }
    } while (SYM == SYM_PLUS || SYM == SYM_MINUS);

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::term(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<Term>");
    // --------------------------------------------

    factor(level, tx, current);
    int op = SYM;
    if (op != SYM_TIMES && op != SYM_SLASH)
    {
        // --------------------------------------------
        parseTree.pop();
        // --------------------------------------------

        return 0;
    }

    // --------------------------------------------
    parseTree.push("<Multiply & Divide Operator>");
    if(SYM == SYM_TIMES)
        parseTree.append("*");
    if(SYM == SYM_SLASH)
        parseTree.append("/");
    parseTree.pop();
    // --------------------------------------------

    do
    {
        getSym();
        factor(level, tx, current);
        if (op == SYM_TIMES)
            gen(OPR, 0, OP_TIMES);
        else
            gen(OPR, 0, OP_SLASH);
        op = SYM;
    } while (SYM == SYM_TIMES || SYM == SYM_SLASH);

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

int SyntacticAnalyzer::factor(int level, int tx, Procedure *current)
{
    // --------------------------------------------
    parseTree.push("<factor>");
    // --------------------------------------------

    if(SYM == SYM_IDENTIFIER)
    {
        TableEntry *i = findInTree(strToken, current);
        if (i == nullptr)
            throw SyntaxException("Identifier not found", line);
        if (i->type == TableEntry::CONSTANT)
            gen(LIT, 0, i->value);
        else if (i->type == TableEntry::VARIABLE)
            gen(LOD, level - i->level, NUM_LINK_DATA + i->address);
        else
            throw SyntaxException("variable or constant expected", line);

        // --------------------------------------------
        parseTree.push("<Identifier>");
        parseTree.append(strToken);
        parseTree.pop();
        // --------------------------------------------

        getSym();
    }
    else if (SYM == SYM_NUMBER)
    {
        // --------------------------------------------
        parseTree.push("<Unsigned Integer>");
        parseTree.append(NUM);
        parseTree.pop();
        // --------------------------------------------

        gen(LIT, 0, NUM);
        getSym();
    }
    else if (SYM == SYM_LPAREN)
    {
        // --------------------------------------------
        parseTree.append("(");
        // --------------------------------------------

        getSym();
        expression(level, tx, current);
        if (SYM != SYM_RPAREN)
            throw SyntaxException("')' expected", line);

        // --------------------------------------------
        parseTree.append(")");
        // --------------------------------------------

        getSym();
    }
    else
        throw SyntaxException("syntax error", line);

    // --------------------------------------------
    parseTree.pop();
    // --------------------------------------------

    return 0;
}

SyntacticAnalyzer::TableEntry *SyntacticAnalyzer::findInTable(const char *name, int count)
{
    for(int i=0;i<count;i++)
    {
        if(strcmp(name, table[i].name) == 0)
            return &table[i];
    }
    return nullptr;
}

SyntacticAnalyzer::TableEntry *SyntacticAnalyzer::findInTree(const char *name, Procedure *current, bool flag)
{
    if (current == nullptr)
        return nullptr;

    for (auto *id: current->entries)  // variables & constants
    {
        if(strcmp(name, id->name) == 0)
            return id;
    }

    if (flag)  // first time step into findInTree
    {
        for (auto *procedure: current->children)  // sub procedure
            if(strcmp(name, procedure->entry->name) == 0)
                return procedure->entry;

        if (current->father != nullptr)
            for (auto *procedure: current->father->children)  // father's sub procedure
            {
                if(strcmp(name, procedure->entry->name) == 0)
                    return procedure->entry;
            }
    }
    else
    {
        if (current->entry != nullptr)
            if(strcmp(name, current->entry->name) == 0)
                return current->entry;
    }

    return findInTree(name, current->father, false);
}

// only check for the tree
bool SyntacticAnalyzer::checkInTree(const char *name, SyntacticAnalyzer::Procedure *current)
{
    for (auto *id: current->entries)  // variables & constants
    {
        if(strcmp(name, id->name) == 0)
            return true;
    }

    for (auto *procedure: current->children)  // procedures
    {
        if(strcmp(name, procedure->entry->name) == 0)
            return true;
    }

    return false;
}

void SyntacticAnalyzer::printCode()
{
    getSym();
    try
    {
        block(0, 0);
    }
    catch (SyntaxException &exception)
    {
        printf(exception.what());
    }
    for (int i = 1; i<pc; i++)
    {
        fprintf(outFile, "%d:\t%s %d %d\n", i, OPTable[code[i].op], code[i].l, code[i].a);
    }
}
