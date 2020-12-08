//
// Created by Peter on 2020/12/8.
//

#ifndef COMPILER_VIRTUALMACHINE_H
#define COMPILER_VIRTUALMACHINE_H

#include <iostream>
#include "SyntacticAnalyzer.h"
using std::cin;
using std::cout;
using std::endl;

class VirtualMachine: public SyntacticAnalyzer
{
public:
    VirtualMachine(FILE *inFile, FILE *outFile): SyntacticAnalyzer(inFile, outFile){}

    void interpret();

};


#endif //COMPILER_VIRTUALMACHINE_H
