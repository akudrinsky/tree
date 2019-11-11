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
    FILE* pFile = fopen (file, "a");
    ASSERT (pFile != nullptr)

    fprintf (pFile, "%s", str);

    fclose (pFile);
}

#pragma pack(push, 1)
struct node {
    char* data;
    node* left;
    node* right;
    node* parent;

    explicit node (int size = 20);
    ~node ();

    bool merge (node* leaf, char where = 'f');              //r - right, l - left, f - doesn't matter
    void photo (const char* pict_name = "tree.png", const char* pict_type = "png", int iter = 1);
    bool save (const char* filename = "tree_saved.txt", bool is_first = true, bool need_closing = true);
    bool get_tree (FILE* pFile, bool need_left = true);

};
#pragma pack(pop)

node::node(int size) {
    data = (char*) calloc (size, sizeof (char));
    if (data == nullptr) {
        err_info ("Problem with memory allocation in node construct\n");
    }
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

node::~node() {                                                        //make it safer!!!
    if (data != nullptr)
        free (data);
}

bool node::merge(node *leaf, char where) {
    if (where == 'l') {
        if (left != nullptr) {
                                        //destruct left leaf
        }
        left = leaf;
        leaf->parent = this;
    }
    else if (where == 'r') {
        if (right != nullptr) {
                                        //destruct right leaf
        }
        right = leaf;
        leaf->parent = this;
    }
    else if (where == 'f') {

    }
    else {
        err_info ("Wrong format of merge, where = ");
        err_info (&where);
        err_info ("\n");
    }
}

void node::photo(const char* pict_name, const char* pict_type, int iter) {
    ASSERT (pict_name != nullptr)
    ASSERT (pict_type != nullptr)
    //printf ("* ");

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

    fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| %s | <f1> %p\" ];\n", iter, left, data, right);
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
}

bool node::save(const char *filename, bool is_first, bool need_closing) {
    bool status = true;
    if (filename == nullptr) {
        err_info ("Nullptr filename while saving tree\n");
        return false;
    }
    //printf ("* ");

    FILE* pFile = nullptr;
    if (is_first) {
        pFile = fopen (filename, "w");
    }
    else {
        pFile = fopen (filename, "a");
    }

    if (pFile == nullptr) {
        err_info ("Nullptr file while saving tree\n");
        return false;
    }

    fprintf (pFile, "{%s", data);

    if (left == nullptr && right == nullptr) {
        fprintf (pFile, "}");
    }
    if (left == nullptr && right != nullptr) {
        fprintf (pFile, "@");
        fclose (pFile);
        if (!(*right).save(filename, false, true))
            status = false;
        pFile = fopen (filename, "a");
    }
    if (left != nullptr && right == nullptr) {
        fclose (pFile);
        if (!(*left).save (filename, false, false))
            status = false;
        pFile = fopen (filename, "a");
        fprintf (pFile, "@");
    }
    if (left != nullptr && right != nullptr) {
        fclose (pFile);
        if (!(*left).save (filename, false, true))
            status = false;
        if (!(*right).save (filename, false, true))
            status = false;
    }

    if (need_closing)
        fprintf (pFile, "}");

    fclose (pFile);

    return status;
}

bool node::get_tree(FILE* pFile, bool need_left) {
    if (pFile == nullptr) {
        err_info ("Nullptr file while saving tree\n");
        return false;
    }

    char next_c = fgetc (pFile);
    printf ("%c\n", next_c);
    if (next_c == '{') {
        //err_info ("Error with saved tree\n");

        node (cur_nd);
        fscanf (pFile, "%[^{}@]s", cur_nd.data);
        printf ("%s\n", cur_nd.data);
        cur_nd.parent = this;
        if (need_left) {
            left = &cur_nd;
            cur_nd.get_tree (pFile, false);
        }
        else {
            right = &cur_nd;
        }

        return false;
    }
    else if (next_c == '}') {
        return true;
    }
    else if (next_c == '@') {
        if (need_left)
            left = nullptr;
        else
            right = nullptr;
    }

    return true;
}

bool tree_test() {
    FILE* input = fopen ("input.txt", "r");
    node (nd7);
    node (nd1);
    node (nd2);
    node (nd3);
    node (nd4);
    node (nd5);
    node (nd6);

    fscanf (input, "%[^\n]s", nd1.data);
    fgetc (input);
    fscanf (input, "%[^\n]s", nd2.data);
    fgetc (input);
    fscanf (input, "%[^\n]s", nd3.data);
    fgetc (input);
    fscanf (input, "%[^\n]s", nd4.data);
    fgetc (input);
    fscanf (input, "%[^\n]s", nd5.data);
    fgetc (input);
    fscanf (input, "%[^\n]s", nd6.data);
    fgetc (input);
    fscanf (input, "%[^\n]s", nd7.data);
    fgetc (input);

    nd1.merge (&nd2, 'l');
    nd2.merge (&nd3, 'l');
    nd2.merge (&nd4, 'r');
    nd3.merge (&nd5, 'l');
    nd1.merge (&nd6, 'r');
    nd6.merge (&nd7, 'r');

    //printf ("%s\n%s", nd.data, str1);

    nd1.photo ();
    nd1.save ();
    fclose (input);
}






#endif