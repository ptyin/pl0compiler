#include <iostream>
#include "LexicalAnalyzer.h"

int main()
{
    std::cout << "Lexical Test!" << std::endl;
    FILE *file;
    if(fopen_s(&file, "../lexical.txt", "r"))
        return -1;

    LexicalAnalyzer lexicalAnalyzer(file, stdout);
    do
    {
        if(lexicalAnalyzer.getSym() == -1)
        {
            std::cout << "Error!" << std::endl;
            break;
        }
        else if(lexicalAnalyzer.isStop())
            break;
        else
        {
            std::cout << "token: " << lexicalAnalyzer.getStrToken() << std::endl;
            std::cout << "SYM: " << lexicalAnalyzer.SYM << ", ID: " << lexicalAnalyzer.ID << ", NUM:" << lexicalAnalyzer.NUM << std::endl;
        }
    }while (true);
    return 0;
}
