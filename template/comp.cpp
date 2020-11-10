//
// Created by Peter on 2020/11/3.
//

#include "comp.h"

const char CodeTable[8][4] = { "LIT", "LOD", "STO", "CAL", "INT", "JMP", "JPC", "OPR" };

int sym, num;
char id[maxIdLength + 1];
int pc = 1;
int line = 1;

void printErr(const char *err)
{
    printf("Line %d:%s\n", line, err);
    exit(1);
}

int findKeyword(const char *str)
{
    if (!strcmp(str, "const"))
        return CONSTSYM;
    if (!strcmp(str, "var"))
        return VARSYM;
    if (!strcmp(str, "procedure"))
        return PROCSYM;
    if (!strcmp(str, "begin"))
        return BEGINSYM;
    if (!strcmp(str, "end"))
        return ENDSYM;
    if (!strcmp(str, "odd"))
        return ODDSYM;
    if (!strcmp(str, "if"))
        return IFSYM;
    if (!strcmp(str, "then"))
        return THENSYM;
    if (!strcmp(str, "call"))
        return CALLSYM;
    if (!strcmp(str, "while"))
        return WHILESYM;
    if (!strcmp(str, "do"))
        return DOSYM;
    if (!strcmp(str, "write"))
        return WRITESYM;
    if (!strcmp(str, "read"))
        return READSYM;
    return -1;
}

int getSym(FILE *in)
{
    extern int sym, num;
    extern char id[maxIdLength + 1];
    char buf[maxIdLength + 1];
    int pos = 0;
    char ch = ' ';
    while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        if ((ch = fgetc(in)) == EOF)
        {
            return -1;
        }
        if (ch == '\n')
            line++;
    }
    if (isalpha(ch))
    {
        while (isalpha(ch) || isdigit(ch))
        {
            if (pos >= maxIdLength)
                return -1;
            buf[pos++] = ch;
            ch = fgetc(in);
        }
        ungetc(ch, in);
        buf[pos++] = '\0';
        sym = findKeyword(buf);
        if (sym<0)
        {
            sym = IDENT;
            strcpy(id, buf);
            return 0;
        }
    }
    else if (isdigit(ch))
    {
        while (isdigit(ch))
        {
            if (pos >= maxIdLength)
                return -1;
            buf[pos++] = ch;
            ch = fgetc(in);
        }
        ungetc(ch, in);
        buf[pos++] = '\0';
        sym = NUM;
        num = atoi(buf);
        return 0;
    }
    else if (ch == '(')
        sym = LPAREN;
    else if (ch == ')')
        sym = RPAREN;
    else if (ch == '=')
        sym = EQ;
    else if (ch == '#')
        sym = NEQ;
    else if (ch == '+')
        sym = PLUS;
    else if (ch == '-')
        sym = MINUS;
    else if (ch == '*')
        sym = TIMES;
    else if (ch == '/')
        sym = SPLASH;
    else if (ch == ',')
        sym = COMMA;
    else if (ch == ';')
        sym = SEMICOLON;
    else if (ch == '.')
        sym = PERIOD;
    else if (ch == ':')
    {
        ch = fgetc(in);
        if (ch == '=')
            sym = ASSIGN;
        else
        {
            ungetc(ch, in);
            sym = COLON;
        }
    }
    else if (ch == '>')
    {
        ch = fgetc(in);
        if (ch == '=')
            sym = GE;
        else
        {
            ungetc(ch, in);
            sym = GT;
        }
    }
    else if (ch == '<')
    {
        ch = fgetc(in);
        if (ch == '=')
            sym = LE;
        else
        {
            ungetc(ch, in);
            sym = LSS;
        }
    }
    else return -1;
    return 0;
}

int block(int level, int index)
{
    int count = 0, dx = 0;
    int tpc = pc;
    genCode(JMP, 0, 0);
    bool flag = false;
    if (sym == CONSTSYM)
    {

        count = constDeclaration(index);
    }
    if (sym == VARSYM)
    {
        getSym();
        count += (dx = varDeclaration(level, index + count));
    }
    if (sym == PROCSYM)
    {
        flag = true;
        getSym();
        int px = count + index;
        count += procDeclaration(level + 1, px);
    }
    if (!flag)
        pc--;
    else
        code[tpc].a = pc;
    genCode(INT, 0, numLinkData + dx);
    statement(level, index + count);
    genCode(OPR, 0, OP_RET);
    return count;
}

int constDeclaration(int index)
{
    Symbol sb;
    sb.type = CONST;
    int count = 0;
    do
    {
        getSym();
        if (sym != IDENT)
            printErr("identifier expected!");
        if (find(0, index + count, id) >= 0)
            printErr("duplicated identifier!");
        strcpy(sb.name, id);
        getSym();
        if (sym != EQ)
            printErr("a '=' expected!");
        getSym();
        if (sym != NUM)
            printErr("number expected!");
        sb.value = num;
        symTable[index + count++] = sb;
        getSym();
        if (!(sym == COMMA || sym == SEMICOLON))
            printErr("comma or semicolon expected!");
    } while (sym != SEMICOLON);
    getSym();
    return count;
}

int varDeclaration(int level, int index)
{
    Symbol sb;
    sb.type = VARIABLE;
    sb.level = level;
    sb.address = 0;
    int count = 0;
    int tsym = sym;
    do
    {
        if (sym != IDENT)
            printErr("identifier expected!");
        if (find(0, index + count, id) >= 0)
            printErr("duplicated expected!");
        strcpy(sb.name, id);
        symTable[index + count++] = sb;
        sb.address++;
        getSym();
        if (!(sym == COMMA || sym == SEMICOLON))
            printErr("comma or semicolon expected!");
        tsym = sym;
        getSym();
    } while (tsym != SEMICOLON);
    return count;
}

int procDeclaration(int level, int index)
{
    int count = 0;
    if (sym != IDENT)
        printErr("identifier expected!");
    Symbol sb;
    strcpy(sb.name, id);
    sb.type = PROCEDURE;
    sb.level = level - 1;
    sb.address = pc;
    symTable[index + count++] = sb;
    getSym();
    if (sym != SEMICOLON)
        printErr("semicolon expected!");
    getSym();
    block(level, index + count);
    if (sym != SEMICOLON)
        printErr("semicolon expected!");
    getSym();
    if (sym == PROCSYM)
    {
        getSym();
        count += procDeclaration(level, index + count);
    }
    return count + 1;
}

int find(int from, int to, const char *name)
{
    for (int i = to - 1; i >= from; i--)
        if (!strcmp(name, symTable[i].name))
            return i;
    return -1;
}

void genCode(int op, int l, int a)
{
    PCode pcode(op, l, a);
    code[pc++] = pcode;
}

int statement(int level, int index)
{
    if (sym == IFSYM)
    {
        getSym();
        ifStatement(level, index);
    }
    else if (sym == WHILESYM)
    {
        getSym();
        whileStatement(level, index);
    }
    else if (sym == CALLSYM)
    {
        getSym();
        callStatement(level, index);
    }
    else if (sym == WRITESYM)
    {
        getSym();
        writeStatement(level, index);
    }
    else if (sym == READSYM)
    {
        getSym();
        readStatement(level, index);
    }
    else if (sym == BEGINSYM)
    {
        getSym();
        compositeStatement(level, index);
    }
    else if (sym == IDENT)
    {
        assignStatement(level, index);
    }
    else
        return 0;
    return 0;
}

int ifStatement(int level, int index)
{
    condition(level, index);
    int cpc = pc;
    genCode(JPC, 0, 0);
    if (sym != THENSYM)
        printErr("then clause expected!");
    getSym();
    statement(level, index);
    code[cpc].a = pc;
    return 0;
}

int whileStatement(int level, int index)
{
    int cpc = pc;
    condition(level, index);
    int jpc = pc;
    genCode(JPC, 0, 0);
    if (sym != DOSYM)
    {
        printErr("do expected!");
    }
    getSym();
    statement(level, index);
    genCode(JMP, 0, cpc);
    code[jpc].a = pc;
    return 0;
}

int callStatement(int level, int index)
{
    if (sym != IDENT)
        printErr("syntax error!");
    int i = find(0, index, id);
    if (i<0)
        printErr("identifier not found!");
    if (symTable[i].type != PROCEDURE)
        printErr("syntax error!");
    genCode(CAL, level - symTable[i].level, symTable[i].address);
    getSym();
    return 0;
}

int readStatement(int level, int index)
{
    if (sym != LPAREN)
        printErr(" ( expected");
    getSym();
    while (sym != RPAREN)
    {
        if (sym != IDENT)
            printErr("variable expected!");
        int i = find(0, index, id);
        if (i<0)
            printErr("identifier not found!");
        if (symTable[i].type != VARIABLE)
            printErr("variable expected!");
        genCode(OPR, 0, OP_READ);
        genCode(STO, level - symTable[i].level, symTable[i].address + numLinkData);
        getSym();
        if (sym != COMMA&&sym != RPAREN)
            printErr("syntax error!");
    }
    getSym();
    return 0;
}

int writeStatement(int level, int index)
{
    if (sym != LPAREN)
        printErr(" ( expected");
    getSym();
    while (sym != RPAREN)
    {
        expression(level, index);
        genCode(OPR, 0, OP_WRITE);
        if (sym != COMMA&&sym != RPAREN)
            printErr("syntax error!");
    }
    getSym();
    return 0;
}

int compositeStatement(int level, int index)
{
    statement(level, index);
    while (sym == SEMICOLON)
    {
        getSym();
        statement(level, index);
    }
    if (sym != ENDSYM)
        printErr("end expected!");
    getSym();
    return 0;
}

int assignStatement(int level, int index)
{
    int i = find(0, index, id);
    if (i<0)
    {
        printErr("Variable not found!");
    }
    if (symTable[i].type == CONST)
        printErr("constant can't be a r-value!");
    getSym();
    if (sym != ASSIGN)
    {
        printErr(":= expected!");
    }
    getSym();
    expression(level, index);
    genCode(STO, level - symTable[i].level, numLinkData + symTable[i].address);
    return 0;
}

int condition(int level, int index)
{
    if (sym == ODDSYM)
    {
        getSym();
        expression(level, index);
        genCode(LIT, 0, 0);
        genCode(OPR, 0, OP_NEQ);
    }
    else
    {
        expression(level, index);
        int op = sym;
        if (sym != NEQ&&sym != EQ&&sym != LSS&&sym != LE&&sym != GT&&sym != GE)
            printErr("error!");
        getSym();
        expression(level, index);
        switch (op)
        {
            case NEQ:
                genCode(OPR, 0, OP_NEQ); break;
            case EQ:
                genCode(OPR, 0, OP_EQ); break;
            case LSS:
                genCode(OPR, 0, OP_LSS); break;
            case LE:
                genCode(OPR, 0, OP_LE); break;
            case GT:
                genCode(OPR, 0, OP_GT); break;
            case GE:
                genCode(OPR, 0, OP_GE); break;
        }
    }
    return 0;
}

int expression(int level, int index)
{

    int op = sym;
    if (sym == PLUS || sym == MINUS)
    {
        getSym();
    }
    factor(level, index);
    if (op == MINUS)
    {
        genCode(LIT, 0, 0);
        genCode(OPR, 0, OP_MINUS);
    }
    do{
        op = sym;
        if (sym == PLUS || sym == MINUS)
        {
            getSym();
            factor(level, index);
            if (op == PLUS)
                genCode(OPR, 0, OP_ADD);
            else
                genCode(OPR, 0, OP_MINUS);
        }
    } while (sym == PLUS || sym == MINUS);
    return 0;
}

int factor(int level, int index)
{

    term(level, index);
    int op = sym;
    if (op != TIMES&&op != SPLASH)
        return 0;
    do{
        getSym();
        term(level, index);
        if (op == TIMES)
            genCode(OPR, 0, 4);
        else
            genCode(OPR, 0, OP_DIV);
        op = sym;
    } while (sym == TIMES || sym == SPLASH);
    return 0;
}

int term(int level, int index)
{
    if (sym == IDENT)
    {
        int i = find(0, index, id);
        if (i<0)
        {
            printErr("Identifier not found!");
        }
        if (symTable[i].type == CONST)
            genCode(LIT, 0, symTable[i].value);
        else if (symTable[i].type == VARIABLE)
            genCode(LOD, level - symTable[i].level, numLinkData + symTable[i].address);
        else
        {
            printErr("error!");
        }
        getSym();
    }
    else if (sym == NUM)
    {
        genCode(LIT, 0, num);
        getSym();
    }
    else if(sym==LPAREN)
    {
        getSym();
        expression(level, index);
        if (sym != RPAREN)
            printf(") expected");
        getSym();
    }
    else{
        printErr("error!");
    }
    return 0;
}

void interprete()
{
    const static int ret_addr = 0, dynamic_link = 1, static_link = 2;
    PCode ir;
    int ip = 1, sp = 0, bp = 0;
    int stack[1000] = { 0 };
    int sp_stack[10];
    int sp_top = 0;
    while (ip<pc)
    {
        ir = code[ip++];
        switch (ir.op)
        {
            case LIT:
                stack[sp++] = ir.a; break;
            case LOD:
            {
                if (ir.l == 0)
                    stack[sp++] = stack[bp + ir.a];
                else
                {
                    int outer_bp = stack[bp + static_link];
                    while (--ir.l)
                        outer_bp = stack[outer_bp + static_link];
                    stack[sp++] = stack[outer_bp + ir.a];
                }
                break;
            }
            case STO:
            {
                if (ir.l == 0)
                    stack[bp + ir.a] = stack[sp - 1];
                else
                {
                    int outer_bp = stack[bp + static_link];
                    while (--ir.l)
                        outer_bp = stack[outer_bp + static_link];
                    stack[outer_bp + ir.a] = stack[sp - 1];
                }
                break;
            }
            case CAL:
            {
                stack[sp + ret_addr] = ip;
                stack[sp + dynamic_link] = bp;
                stack[sp + static_link] = bp;
                ip = ir.a;
                bp = sp;
                break;
            }
            case INT:
            {
                sp_stack[sp_top++] = sp;
                sp += ir.a;
                break;
            }
            case JMP:
            {
                ip = ir.a;
                break;
            }
            case JPC:
            {
                if (stack[sp - 1] == 0)
                    ip = ir.a;
                break;
            }
            case OPR:
            {
                switch (ir.a)
                {
                    case OP_RET:
                    {
                        ip = stack[bp + ret_addr];
                        bp = stack[bp + dynamic_link];
                        sp = sp_stack[--sp_top];
                        if (sp_top <= 0)
                        {
                            printf("program exited normally!\n");
                            return;
                        }
                        break;
                    }
                    case OP_ADD:
                    {
                        stack[sp - 2] = stack[sp - 1] + stack[sp - 2];
                        sp--;
                        break;
                    }
                    case OP_MINUS:
                    {
                        stack[sp - 2] = stack[sp - 1] - stack[sp - 2];
                        sp--;
                        break;
                    }
                    case OP_TIMES:
                    {
                        stack[sp - 2] = stack[sp - 1] * stack[sp - 2];
                        sp--;
                        break;
                    }
                    case OP_DIV:
                    {
                        stack[sp - 2] = stack[sp - 2] / stack[sp - 1];
                        sp--;
                        break;
                    }
                    case OP_NEQ:
                    {
                        stack[sp - 2] = (stack[sp - 2] != stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_EQ:
                    {
                        stack[sp - 2] = (stack[sp - 2] == stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_LSS:
                    {
                        stack[sp - 2] = (stack[sp - 2]<stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_LE:
                    {
                        stack[sp - 2] = (stack[sp - 2] <= stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_GT:
                    {
                        stack[sp - 2] = (stack[sp - 2]>stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_GE:
                    {
                        stack[sp - 2] = (stack[sp - 2] >= stack[sp - 1]) ? 1 : 0;
                        sp--;
                        break;
                    }
                    case OP_READ:
                    {
                        cout << "Please input a number:" << endl;
                        cin >> stack[sp++];
                        break;
                    }
                    case OP_WRITE:
                    {
                        cout << stack[sp - 1] << endl;
                        break;
                    }
                    default:
                    {
                        printf("Unexpected operation!\n"); return;
                    }
                }
                break;
            }
            default:
                printf("Unexpected instruction!\n"); return;
        }
    }
}

int main(int argc, char *argv[])
{
    f = fopen("lexical.txt","r");
    getSym();
    block(0, 0);
    for (int i = 1; i<pc; i++)
    {
        cout << i << ":\t" << CodeTable[code[i].op] << " " << code[i].l << " " << code[i].a << endl;
    }
    interprete();
    return 0;
}