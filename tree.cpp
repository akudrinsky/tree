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
#include "my_lyb.h"
#include "operations.h"
#include "dsl.h"

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

int default_node_size = 20;
#pragma pack(push, 1)
struct node {
    double data;
    node* left;
    node* right;
    node* parent;
    char type;                                              //'n' - number, 'v' - variable

    explicit node (int size = default_node_size);
    ~node ();
    void init (int size = default_node_size, const char tp = unknown, node* left_l = nullptr, node* right_l = nullptr, node* prnt = nullptr);
    void clear ();

    bool merge (node* leaf, char where);              //r - right, l - left
    node* copy ();
    void photo (const char* pict_name = "tree_graph.png", const char* pict_type = "png", int iter = 1, FILE* pFile = nullptr);
    bool save (const char* filename = "tree_saved.txt", char mode = 'a', FILE* pFile = nullptr, bool is_first = true, bool need_closing = true);
    bool get_tree (const char* filename = "tree_saved.txt");
    bool is_left ();
    bool is_right ();

    bool is_valid();

    node* diff ();


};
#pragma pack(pop)

bool get_subtree (node* nd, char where, char* *cur);

node* create_node (double data, char type, node* left, node* right) {
    node* nd = new node;
    ASSERT (nd != nullptr)

    nd->data = data;
    nd->type = type;
    nd->left = left;
    nd->right = right;

    if (nd->is_left())
        nd->left->parent = nd;
    if (nd->is_right())
        nd->right->parent = nd;

    ASSERT (nd != nullptr)

    return nd;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Constructor of a tree
//!
//! @param [in] size - size of data in node
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node::node(int size) {
    data = 0;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
    type = 'u';                                         //unknown
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! Destructor of a tree
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node::~node() {                         //make it!!!
    //free (data);
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Tree safe initializer
//!
//! @param [in] size - size of data in node
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void node::init(int size, const char tp, node* left_l, node* right_l, node* prnt) {
    data = 0;

    this->merge (left_l, 'l');
    this->merge (right_l, 'r');
    parent = prnt;

    type = tp;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! Safe clearing of a tree
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void node::clear() {
    if (is_left()) {
        left->clear();
        delete left;
    }
    if (is_right()) {
        right->clear();
        delete right;
    }

    left = nullptr;
    right = nullptr;
    parent = nullptr;
    type = unknown;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Merging leaf to a given node from a given side
//!
//! @param [in] where - l for left, r for right
//! @param [in] leaf - leaf to merge
//! @return - if it was OK
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::merge(node* leaf, char where) {
    if (leaf == nullptr) {
        if (where == 'l') {
            left = nullptr;
        }
        else if (where == 'r') {
            right = nullptr;
        }
        else {
            err_info ("Wrong format of merge, where = ");
            err_info (&where);
            err_info ("\n");
            return false;
        }
    }
    else {
        printf ("son in merge: \t%p\n", leaf);
        if (where == 'l') {
            left = leaf;
            leaf->parent = this;
        }
        else if (where == 'r') {
            right = leaf;
            leaf->parent = this;
        }
        else {
            err_info ("Wrong format of merge, where = ");
            err_info (&where);
            err_info ("\n");
            return false;
        }
    }

    return true;
}

node* node::copy() {
    node* cpy = new node;
    cpy->init (default_node_size, type, left, right);
    return cpy;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Makes picture of a tree
//!
//! @param [in] pict_name - name of photo
//! @param [in] pict_type - type (ex: png)
//! @param [in] iter - NOT FOR USERS
//! @param [in] pFile - NOT FOR USERS
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void node::photo(const char* pict_name, const char* pict_type, int iter, FILE* pFile) {
    ASSERT (pict_name != nullptr)
    ASSERT (pict_type != nullptr)

    if (iter == 1) {
        pFile = fopen ("tree_graph.dot", "w");
        ASSERT (pFile != nullptr)
        fprintf (pFile, "digraph G{\n\tedge[color=\"chartreuse4\",fontcolor=\"blue\",fontsize=12];\n\tnode[shape=\"rectangle\",fontsize=15];\n");
    }
    ASSERT (pFile != nullptr)

    //vertices
    if (type == 0) {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {unknown | %.3lf} | <f1> %p\" ];\n", iter, left, data, right);
    }
    else if (type == 1) {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {%.3lf | num} | <f1> %p\" ];\n", iter, left, data, right);
    }
    else if (type == 2) {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {var | %.3lf} | <f1> %p\" ];\n", iter, left, data, right);
    }
    else {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {%s | op} | <f1> %p\" ];\n", iter, left, operations[type - 3], right);
    }

    //edges
    if (left != nullptr) {
        (*left).photo (pict_name, pict_type, iter * 2, pFile);
        fprintf (pFile, "\t\t%d:<f0> -> %d\n", iter, iter * 2);
    }
    if (right != nullptr) {
        (*right).photo (pict_name, pict_type, iter * 2 + 1, pFile);
        fprintf (pFile, "\t\t%d:<f1> -> %d[color=\"red\"]\n", iter, iter * 2 + 1);
    }

    if (iter == 1) {
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

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Saves tree to a txt file format
//!
//! @param [in] filename - name of file to save tree to
//! @param [in] mode - 'a' for prefix, 'd' for infix
//! @param [in] pFile - NOT FOR USERS
//! @param [in] is_first - NOT FOR USERS
//! @param [in] need_closing - NOT FOR USERS
//! @return - if it was OK
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::save(const char* filename, char mode, FILE* pFile, bool is_first, bool need_closing) {
    ASSERT (filename != nullptr)

    bool status = true;
    if (is_first) {
        pFile = fopen (filename, "w");
        ASSERT (pFile != nullptr)
    }

    if (mode == 'a')
        fprintf (pFile, "{%lf", data);
    else if (mode == 'd')
        fprintf (pFile, "(");

    if (mode == 'd' && left == nullptr && right == nullptr) {
        fprintf (pFile, "%lf", data);
    }
    if (left == nullptr && right != nullptr) {
        fprintf (pFile, "@");
        if (mode == 'd') {
            fprintf (pFile, "%lf", data);
        }
        if (!(*right).save(filename, mode, pFile, false, false))
            status = false;
    }
    if (left != nullptr && right == nullptr) {
        if (!(*left).save (filename, mode, pFile, false, false))
            status = false;
        if (mode == 'd') {
            fprintf (pFile, "%lf", data);
        }
        fprintf (pFile, "@");
    }
    if (left != nullptr && right != nullptr) {
        if (!(*left).save (filename, mode, pFile, false, false))
            status = false;
        if (mode == 'd') {
            fprintf (pFile, "%lf", data);
        }
        if (!(*right).save (filename, mode, pFile, false, true))
            status = false;
    }

    if (mode == 'a')
        fprintf (pFile, "}");
    else if (mode == 'd')
        fprintf (pFile, ")");

    if (is_first) {
        fclose (pFile);
    }

    return status;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Users function for making tree from txt file
//!
//! @param [in] filename - name of file from which we need to make tree
//!
//! @return if it was good
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::get_tree(const char* filename) {
    ASSERT (filename != nullptr)
    FILE* pFile = fopen (filename, "r");
    ASSERT (pFile != nullptr)

    char* file = read_text (pFile);
    char* cur = file;
    int got_c = 0;

    if (*file == '{') {
        ++cur;
        char* arg = (char*) calloc (15, sizeof (char));
        sscanf (cur, "%[^{}]%n", arg, &got_c);
        printf ("arg: %s\n", arg);
        if (strncmp (arg, "x", 3) == 0) {
            type = variable;
        }
        else if (is_num (arg)) {
            type = number;
            data = atof (arg);
        }
        else {
            for (int i = 0; i < sizeof (operations) / sizeof (operations[0]); ++i) {
                if (strncmp (operations[i], arg, 5) == 0) {
                    type = i + 3;                                                             //maybe will have problems here
                }
            }
        }

        free (arg);

        //printf ("%d\n", got_c);
        cur += got_c + 1;                                       //+ 1 because we need symbol after the last
        //printf ("node: %s\n", data);
        //printf ("remaining: %s\n", cur);

        if (*cur != '}') {
            get_subtree (this, 'l', &cur);
            get_subtree (this, 'r', &cur);
        }
    }
    else {
        err_info ("Wrong format of saved tree (get_tree)\n");
        free (file);
        fclose (pFile);
        return false;
    }

    free (file);
    fclose (pFile);

    return true;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Recursive function for making tree from txt file
//!
//! @param [in] nd - pointer to node - parent
//! @param [in] where - l for left, r for right
//! @param [in] cur - pointer to text
//!
//! @return if it was good
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool get_subtree (node* nd, char where, char* *cur) {
    ASSERT (cur != nullptr)
    ASSERT (nd != nullptr)

    while (**cur == '{' || **cur == '}') {
        (*cur)++;
    }

    char* leaf_data = (char*) calloc (default_node_size, sizeof (char));
    if (leaf_data == nullptr) {
        err_info ("Unable to calloc leaf_data in get_subtree\n");
        return false;
    }
    int got_c = 0;

    sscanf (*cur, "%[^{}]%n", leaf_data, &got_c);
    printf ("arg: %s\n", leaf_data);
    //printf ("node: %s\n", leaf_data);
    *cur += got_c;                                       //+ 1 because we need symbol after the last
    //printf ("remaining: %s\n", *cur);

    if (strcmp (leaf_data, "@") == 0) {
        if (where == 'l') {
            nd->left = nullptr;
        }
        else if (where == 'r') {
            nd->right = nullptr;
        }
        else {
            err_info ("Wrong format in get_subtree\n");
            return false;
        }
    }
    else {
        node* new_nd = new node;
        nd->merge (new_nd, where);
        //printf ("nd (father): %p\n", nd);
        //printf ("parent of child: %p\n", new_nd->parent);
        printf ("nd (child): \t%p\n", new_nd);
        printf ("son of parent: \t%p\n\n", nd->left);

        if (strncmp (leaf_data, "x", 3) == 0) {
            new_nd->type = variable;
        }
        else if (is_num (leaf_data)) {
            new_nd->type = number;
            new_nd->data = atof (leaf_data);
        }
        else {
            for (int i = 0; i < sizeof (operations) / sizeof (operations[0]); ++i) {
                if (strncmp (operations[i], leaf_data, 5) == 0) {
                    new_nd->type = i + 3;                                                             //maybe will have problems here
                }
            }
        }

        if (**cur == '{') {
            //printf ("1\n");
            get_subtree (new_nd, 'l', cur);
            get_subtree (new_nd, 'r', cur);
        }
        else {
            new_nd->left = nullptr;
            new_nd->right = nullptr;
        }
    }

    return true;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! If a node has left child
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::is_left() {
    return left != nullptr;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! If a node has right child
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::is_right() {
    return right != nullptr;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! If a tree is valid
//!
//! @return if it was OK
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::is_valid() {
    //printf ("valiator launched\n");
    if (is_left()) {
        if (this != this->left->parent) {
            err_info ("Lost connection between node and his left child\n");
        }
        if (!this->left->is_valid()) {
            return false;
        }
    }

    if (is_right()) {
        if (this != this->right->parent) {
            err_info ("Lost connection between node and his right child\n");
            return false;
        }
        if (!this->right->is_valid()) {
            return false;
        }
    }

    return true;
}

node* node::diff() {
    printf ("%c\n", type);
    switch (type) {
        case number: {
            return n(0);
        }
        case variable: {
            return n(1);
        }
        case plus: {
            return PLUS (d(left), d(right));
        }
        case minus: {
            return MINUS (d(left), d(right));
        }
        case mul: {
            return PLUS (MULT(d(left), c(right)), MULT(c(left), d(right)));
        }
        case divide: {
            return DIVIDE (MINUS (MULT(d(left), c(right)), MULT(d(right), c(left))), MULT(right, right));
        }
        case power: {
            return MULT (POW (c(left), c(right)),
                    PLUS (DIVIDE (c(right), c(left)),
                            MULT (LN (c(left)), c(right))));
        }
        case sinus: {
            return MULT (COS (c(left)), d(left));
        }
        case cosinus: {
            return MINUS (n(0), MULT (SIN(c(left)), d(left)));
        }
        case ln: {
            return DIVIDE (d(left), c(left));
        }

    }
    //return nullptr;
}

//((((1)+(2))*(3))/((5)-(4)))
//{+{-{*{x}{x}}{3}}{/{x}{x}}}
bool diff_test () {
    node* nd = new node;
    nd->init ();
    nd->get_tree ();
    nd->photo ();

    nd->diff()->photo("counted.png");

    nd->save ("new_feature.txt", 'd');
    nd->clear ();
}

#endif