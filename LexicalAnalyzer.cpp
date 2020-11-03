//
// Created by Peter on 2020/9/23.
//

#include "LexicalAnalyzer.h"


void LexicalAnalyzer::getCh()
{
    ch = fgetc(inFile);
}


void LexicalAnalyzer::getBC()
{
    while (ch == ' ' || ch == '\n' || ch == '\t')
    {
//        if (ch == '\n')
//        {
//            break;
//        }
        getCh();
    }
}


void LexicalAnalyzer::concat()
{
    char temp[2] = {(char)ch, '\0'};
    strcat_s(strToken, temp);
}


int LexicalAnalyzer::reserve()
{
    for (int i=0;i<KEYWORD_NUM;i++)
    {
        if(strcmp(strToken, keywords[i]) == 0)
        {
            return i;
        }
    }
    return 0;
}


void LexicalAnalyzer::retract()
{
    if(!feof(inFile))
    {
        fseek(inFile,-1, 1);
        ch = '\0';
    }
}


int LexicalAnalyzer::insertId()
{
    for(int i=0;i<variableTableIndex;i++)
    {
        if(strcmp(strToken, variableTable[i].name) == 0)
        {
            ID = i;
            return i;
        }
    }
    ID = variableTableIndex;
    variableTable[variableTableIndex++].setName(strToken);
    return variableTableIndex -  1;
}


int LexicalAnalyzer::insertConst()
{
    NUM = atoi(strToken);
    constTable[constTableIndex++] = NUM;
    return constTableIndex - 1;
}


int LexicalAnalyzer::getSym(const char * buffer_)
{
    buffer = buffer_;
    return getSym();
}
int LexicalAnalyzer::getSym()
{
    strToken[0] = '\0';
    int code, value;

    stop = false;
    getCh();
    getBC();
    if (feof(inFile))
    {
        stop = true;
        return 0;
    }
    if (IS_ALPHA(ch))
    {
        while (IS_ALPHA(ch) || IS_DIGIT(ch))
        {
            concat();
            getCh();
        }
        retract();
        code = reserve();
        if (!code)  // Identifier
        {
            value = insertId();
            SYM = SYM_IDENTIFIER;
            NUM = -1;
        } else
        {
            SYM = code;
            ID = -1;
            NUM = -1;
        }
        return 0;
    } else if (IS_DIGIT(ch))
    {
        while(IS_DIGIT(ch))
        {
            concat();
            getCh();
        }
        if(IS_ALPHA(ch))
            return -1;
        retract();
        SYM = SYM_NUMBER;
        value = insertConst();  // TODO
        ID = -1;
        return 0;
    } else if(ch == '+')
    {
        SYM = SYM_PLUS;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == '-')
    {
        SYM = SYM_MINUS;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == '*')
    {
        SYM = SYM_TIMES;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == '/')
    {
        SYM = SYM_SLASH;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == '=')
    {
        SYM = SYM_EQU;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == '#')
    {
        SYM = SYM_NEQ;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == '<')
    {
        getCh();
        if(ch == '=')  // <=
        {
            SYM = SYM_LEQ;
            ID = -1;
            NUM = -1;
        } else
        {
            retract();
            SYM = SYM_LES;
            ID = -1;
            NUM = -1;
        }
        return 0;
    } else if(ch == '>')
    {
        getCh();
        if(ch == '=')  // >=
        {
            SYM = SYM_GEQ;
            ID = -1;
            NUM = -1;
        } else
        {
            retract();
            SYM = SYM_GTR;
            ID = -1;
            NUM = -1;
        }
    } else if(ch == '(')
    {
        SYM = SYM_LPAREN;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == ')')
    {
        SYM = SYM_RPAREN;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == ',')
    {
        SYM = SYM_COMMA;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == ';')
    {
        SYM = SYM_SEMICOLON;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == '.')
    {
        SYM = SYM_PERIOD;
        ID = -1;
        NUM = -1;
        return 0;
    } else if(ch == ':')
    {
        getCh();
        if(ch == '=')
        {
            SYM = SYM_BECOMES;
            ID = -1;
            NUM = -1;
            return 0;
        } else
        {
            retract();
            return -1;
        }
    }
}

bool LexicalAnalyzer::isStop() const
{
    return stop;
}

const char *LexicalAnalyzer::getStrToken() const
{
    return strToken;
}
