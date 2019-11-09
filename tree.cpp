//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! It is my implementation of tree
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

#ifndef TREE_WAS_DEFINED                           //flag that tree was defined only once in all project
#define TREE_WAS_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#ifdef DEBUG
#define ASSERT(cond)\
    if (!(cond)) {\
        printf ("Assert failure: < %s > in file %s, line %d, function %s\n", #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__);\
        abort();\
}
#else
#define ASSERT(cond) ;
#endif

void err_info (const char* str, const char* file = "errors_info.txt") {
    ASSERT (file != nullptr)
    ASSERT (str != nullptr)
    FILE* pFile = fopen (file, "w");
    ASSERT (pFile != nullptr)

    fprintf (pFile, "%s", str);

    fclose (pFile);
}

#pragma pack(push, 1)
struct node {
    char* data;
    node* left;
    node* right;

    node (char* data, bool is_loud = true);
    ~node ();

    bool rise (char* str, char where = 'f');                //r - right, l - left, f - doesn't matter
    bool merge (node* leaf, char where = 'f');              //r - right, l - left, f - doesn't matter
    int photo (const char* pict_name = "tree.png", const char* pict_type = "png", int iter = 1);
};
#pragma pack(pop)

node::node(char* str, bool is_loud) {
    data = str;
    left = nullptr;
    right = nullptr;
    /*
    if (is_loud && (right == nullptr || left == nullptr)) {
        err_info ("No memory left (constructing node)\n");
    }
     */
}

node::~node() {                                                        //make it safer!!!
    if (data != nullptr)
        free (data);
}

bool node::rise(char *str, char where) {

}

bool node::merge(node *leaf, char where) {
    if (where == 'l') {
        if (left != nullptr) {
                                        //destruct left leaf
        }
        left = leaf;
    }
    else if (where == 'r') {
        if (right != nullptr) {
                                        //destruct right leaf
        }
        right = leaf;
    }
    else if (where == 'f') {

    }
    else {
        err_info ("Wrong format of merge, where = ");
        err_info (&where);
        err_info ("\n");
    }
}

int node::photo(const char* pict_name, const char* pict_type, int iter) {
    ASSERT (pict_name != nullptr)
    ASSERT (pict_type != nullptr)
    printf ("* ");

    FILE* pFile = nullptr;

    if (iter == 1) {
        pFile = fopen ("tree_graph.dot", "w");
        ASSERT (pFile != nullptr)

        fprintf (pFile, "digraph G{\n\tedge[color=\"chartreuse4\",fontcolor=\"blue\",fontsize=12];\n\tnode[shape=\"rectangle\",fontsize=15];\n");
    }
    else {
        pFile = fopen ("tree_graph.dot", "a");
        ASSERT (pFile != nullptr)
    }

    fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %x| %s | <f1> %x\" ];\n", iter, left, data, right);
    fclose (pFile);
    
    if (left != nullptr) {
        (*left).photo (pict_name, pict_type, iter * 2);
        pFile = fopen ("tree_graph.dot", "a");
        fprintf (pFile, "\t\t%d:<f0> -> %d\n", iter, iter * 2);
        fclose (pFile);
    }
    if (right != nullptr) {
        (*right).photo (pict_name, pict_type, iter * 2 + 1);
        pFile = fopen ("tree_graph.dot", "a");
        fprintf (pFile, "\t\t%d:<f1> -> %d\n", iter, iter * 2 + 1);
        fclose (pFile);
    }

    if (iter == 1) {
        ASSERT (pFile != nullptr)
        pFile = fopen ("tree_graph.dot", "a");
        fprintf (pFile, "}");
        fclose (pFile);

        //executing command in terminal
        const int max_cmd_size = 50;
        char command[max_cmd_size] = "dot tree_graph.dot -T ";
        strcat (command, pict_type);
        strcat (command, " -o ");
        strcat (command, pict_name);

        system (command);
    }

    return iter;
}















#endif