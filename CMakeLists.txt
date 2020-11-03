cmake_minimum_required(VERSION 3.16)
project(compiler)

set(CMAKE_CXX_STANDARD 14)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lexical.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
add_executable(compiler main.cpp LexicalAnalyzer.h LexicalAnalyzer.cpp SyntacticAnalyzer.cpp SyntacticAnalyzer.h exception/SyntaxException.cpp exception/SyntaxException.h)
add_executable(template template/template.cpp template/pl0.h template/comp.h template/comp.cpp)
