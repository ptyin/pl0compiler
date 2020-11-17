#include <iostream>
#include "SyntacticAnalyzer.h"
#include "LexicalAnalyzer.h"
using namespace std;

int main()
{
    std::cout << "Syntax Test!" << std::endl;
    FILE *inFile, *outFile;
    if(fopen_s(&inFile, "../lexical.txt", "r") || fopen_s(&outFile, "../output.txt", "w"))
        return -1;

    LexicalAnalyzer lexicalAnalyzer(inFile, stdout);
//    do
//    {
//        if(lexicalAnalyzer.getSym() == -1)
//        {
//            std::cout << "Error!" << std::endl;
//            break;
//        }
//        else if(lexicalAnalyzer.isStop())
//            break;
//        else
//        {
//            std::cout << "token: " << lexicalAnalyzer.getStrToken() << std::endl;
//            std::cout << "SYM: " << lexicalAnalyzer.SYM << ", ID: " << lexicalAnalyzer.ID << ", NUM:" << lexicalAnalyzer.NUM << std::endl;
//        }
//    }while (true);
    // test
    SyntacticAnalyzer syntacticAnalyzer(inFile, stdout);
    syntacticAnalyzer.printCode();
    syntacticAnalyzer.parseTree.print_tree(outFile);
    return 0;
}
