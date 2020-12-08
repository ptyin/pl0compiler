//
// Created by Peter on 2020/11/17.
//

#include "ParseTree.h"

void ParseTree::print_tree(FILE *file, ParseTree::Node *node, int indent, int id) const
{
    if (node == nullptr)
        return;
    for (int i = 0; i < indent; i++)
        fprintf(file, "\t");
    if (indent == 0)
        fprintf(file, "root: ");
    else
        fprintf(file, "%d: ", id);
    fprintf(file, "%s\n", node->value);

    for(int i=0;i<node->children.size();i++)
        print_tree(file, node->children[i], indent + 1, i);
}

void ParseTree::print_tree(FILE *file) const
{
    print_tree(file, root, 0, 0);
}

void ParseTree::push(const char *name)
{
    Node *node = new Node(name);
    current.top()->children.push_back(node);
    current.push(node);
    nodes.push_back(node);
}

void ParseTree::pop()
{
    current.pop();
}

void ParseTree::append(const char *name)
{
    Node *node = new Node(name);
    current.top()->children.push_back(node);
    nodes.push_back(node);
}

void ParseTree::append(int number)
{
    char *name = new char [NUM_MAX_LENGTH];
    _itoa_s(number, name, NUM_MAX_LENGTH, 10);
    Node *node = new Node(name);
    current.top()->children.push_back(node);
    nodes.push_back(node);
    delete[] name;
}
