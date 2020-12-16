//
// Created by Peter on 2020/12/16.
//

#include "ParseTree.h"
int count = 0;

static void print(FILE *file, ParseTree::Node *node, ParseTree::Node *prev, int prev_rand)
{
    int cur = count++;
    if (prev != nullptr)
    {
        fprintf(file, "id%d[label=\"%s\"];id%d[label=\"%s\"];\n", prev_rand, prev->value, cur, node->value);
        fprintf(file, "id%d->id%d;\n", prev_rand, cur);
    }
    for(int i=0;i<node->children.size();i++)
        print(file, node->children[i], node, cur);
}

/**
 * output DOT language intermediate file
 * @param file dot file
 * @param parseTree
 */
void output(FILE *file, ParseTree &parseTree)
{
    fprintf(file, "digraph parseTree{\n");
    print(file, parseTree.getRoot(), nullptr, count);
    fprintf(file, "}");
}