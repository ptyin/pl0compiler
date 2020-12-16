//
// Created by Peter on 2020/11/17.
//

#ifndef COMPILER_PARSETREE_H
#define COMPILER_PARSETREE_H

#include <cstdio>
#include <iostream>
#include <vector>
#include <stack>

class ParseTree
{
public:
    class Node
    {
    private:
        static const int MAX_LENGTH = 100;
    public:
        char *value;
        std::vector<Node *> children;
        explicit Node(const char *_value)
        {
            int len = strnlen_s(_value, MAX_LENGTH);
            value = new char[len+1];
            strcpy_s(value, len+1, _value);
        }
        ~Node()
        {
            delete[] value;
        }
    };
    ParseTree()
    {
        root = new Node("<Program>");
        current.push(root);
        nodes.push_back(root);
    }
    ~ParseTree()
    {
        for(Node *node: nodes)
            delete node;
    }
    void print_tree(FILE *file) const;
    void push(const char* name);
    void pop();
    void append(const char* name);
    void append(int number);

    Node *getRoot();

private:
    void print_tree(FILE *file, Node *node, int indent, int id) const;
    static const int NUM_MAX_LENGTH = 100;

    Node *root;
    std::stack<Node *> current;
    std::vector<Node *>nodes;
};


#endif //COMPILER_PARSETREE_H
