//
// Created by Peter on 2020/11/3.
//

#ifndef COMPILER_SYNTAXEXCEPTION_H
#define COMPILER_SYNTAXEXCEPTION_H

#include <iostream>

class SyntaxException: std::exception
{
private:
    const char *message;
    int line;
    char *error;
public:
    explicit SyntaxException(char const* const _message, int _line=-1)
    {
        message = _message;
        line = _line;
        error = new char[100];
    }

    ~SyntaxException() override
    {
        delete[] error;
    }

    _NODISCARD char const* what() const override
    {
        if(line != -1)
        {
            sprintf(error, "Syntax error in %d\n"
                           "Message: %s", line, message);
            return error;
        }
        else
            return message;
    }
};


#endif //COMPILER_SYNTAXEXCEPTION_H
