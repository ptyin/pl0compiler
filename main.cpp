#include <iostream>

#include "core/VirtualMachine.h"
#include "core/SyntacticAnalyzer.h"
#include "core/LexicalAnalyzer.h"
using namespace std;

void lexical_test(FILE *inFile, FILE *outFile)
{

    std::cout << "Lexical Test!" << std::endl;
    LexicalAnalyzer lexicalAnalyzer(inFile, outFile);
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
}

void syntax_test(FILE *inFile, FILE *outFile)
{
    std::cout << "Syntax Test!" << std::endl;

    // test
    SyntacticAnalyzer syntacticAnalyzer(inFile, stdout);
    syntacticAnalyzer.printCode();
    syntacticAnalyzer.parseTree.print_tree(outFile);
}

void execution_test(FILE *inFile, FILE *outFile)
{
    std::cout << "Execution Test!" << std::endl;
    VirtualMachine virtualMachine(inFile, stdout);
    virtualMachine.printCode();
    virtualMachine.interpret();
    virtualMachine.parseTree.print_tree(outFile);
}

int main()
{
    FILE *inFile, *outFile;
    if(fopen_s(&inFile, "../test.txt", "r") || fopen_s(&outFile, "../output.txt", "w"))
        return -1;

//    lexical_test(inFile, outFile);
//    syntax_test(inFile, outFile);
    execution_test(inFile, outFile);

    return 0;
}
