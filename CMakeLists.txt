cmake_minimum_required(VERSION 3.16)
project(compiler)

set(CMAKE_CXX_STANDARD 14)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/input.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
add_executable(compiler main.cpp
        core/LexicalAnalyzer.h core/LexicalAnalyzer.cpp core/SyntacticAnalyzer.cpp core/SyntacticAnalyzer.h core/VirtualMachine.cpp core/VirtualMachine.h
        exception/SyntaxException.cpp exception/SyntaxException.h
        utils/ParseTree.cpp utils/ParseTree.h utils/graphGenerator.hpp)
add_executable(template template/comp.h template/comp.cpp)