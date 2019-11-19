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
#include "answers.h"

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
    char* data;
    node* left;
    node* right;
    node* parent;

    explicit node (int size = default_node_size);
    ~node ();
    void init (int size = default_node_size);
    void clear ();

    bool merge (node* leaf, char where);              //r - right, l - left
    void photo (const char* pict_name = "tree_graph.png", const char* pict_type = "png", int iter = 1, FILE* pFile = nullptr);
    bool save (const char* filename = "tree_saved.txt", char mode = 'a', FILE* pFile = nullptr, bool is_first = true, bool need_closing = true);
    bool get_tree (const char* filename = "tree_saved.txt");
    bool is_left ();
    bool is_right ();

    bool is_valid();

    node* guess ();                         //returns pointer to node if it was guessed or pointer to the last node of differ
    bool add_leaf (char* divider, char* new_leaf);
    int search_leaf (char* desired, unsigned int path = 1);
    bool print_path (int path);
    bool compare (int path1, int path2);

};
#pragma pack(pop)

bool get_subtree (node* nd, char where, char* *cur);

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Constructor of a tree
//!
//! @param [in] size - size of data in node
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node::node(int size) {
    data = (char*) calloc (size, sizeof (char));
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! Destructor of a tree
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node::~node() {                         //make it!!!
    //this->clear();
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Tree safe initializer
//!
//! @param [in] size - size of data in node
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void node::init(int size) {
    data = (char*) calloc (size, sizeof (char));
    if (data == nullptr) {
        err_info ("Problem with memory allocation in node init\n");
    }
    left = nullptr;
    right = nullptr;
    parent = nullptr;
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
    free (data);
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Merging leaf to a given node from a given side
//!
//! @param [in] where - l for left, r for right
//! @param [in] leaf - leaf to merge
//! @return - if it was OK
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::merge(node* leaf, char where) {
    ASSERT(leaf != nullptr)

    if (leaf->data == "@") {
        return true;
    }
    if (where == 'l') {
        left = leaf;
        leaf->parent = this;
        return true;
    }
    else if (where == 'r') {
        right = leaf;
        leaf->parent = this;
        return true;
    }
    else {
        err_info ("Wrong format of merge, where = ");
        err_info (&where);
        err_info ("\n");
        return false;
    }
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

    fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| %s | <f1> %p\" ];\n", iter, left, data, right);
    
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
        fprintf (pFile, "{%s", data);
    else if (mode == 'd')
        fprintf (pFile, "(");

    if (mode == 'd' && left == nullptr && right == nullptr) {
        fprintf (pFile, "%s", data);
    }
    if (left == nullptr && right != nullptr) {
        fprintf (pFile, "@");
        if (mode == 'd') {
            fprintf (pFile, "%s", data);
        }
        if (!(*right).save(filename, mode, pFile, false, false))
            status = false;
    }
    if (left != nullptr && right == nullptr) {
        if (!(*left).save (filename, mode, pFile, false, false))
            status = false;
        if (mode == 'd') {
            fprintf (pFile, "%s", data);
        }
        fprintf (pFile, "@");
    }
    if (left != nullptr && right != nullptr) {
        if (!(*left).save (filename, mode, pFile, false, false))
            status = false;
        if (mode == 'd') {
            fprintf (pFile, "%s", data);
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
        sscanf (cur, "%[^{}]%n", data, &got_c);
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
        new_nd->data = leaf_data;
        nd->merge (new_nd, where);

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
    if (data == nullptr) {
        err_info ("Null data in node (tree_val)\n");
        printf ("nd (child): %p\n", this->left);

        return false;
    }
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

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Akinator guess
//!
//! @return guessed node (if it was guessed) or adress of a newly created node, which had just been added
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node* node::guess() {
    node* cur_node = this;
    char answer[10] = "";
    while (true) {
        say_it (cur_node->data);
        printf ("\t");
        scanf ("%[^\n]", answer);
        getc (stdin);

        if (strncmp (answer, "да", 3) == 0) {                                                       //left - yes
            cur_node = cur_node->left;
        }
        else if (strncmp (answer, "нет", 4) == 0) {                                                 //right - no
            cur_node = cur_node->right;
        }
        else {
            err_info ("Wrong answer (only да or нет)\n");
            say_it (not_yesno[rand() % (sizeof (not_yesno) / sizeof (not_yesno[0]))]);
        }

        if (!cur_node->is_right() && !cur_node->is_left()) {
            say_it ("Я считаю, что это был: ");
            say_it (cur_node->data);
            say_it ("\nЯ угадал?\n");

            scanf ("%[^\n]", answer);
            getc (stdin);

            if (strncmp (answer, "да", 3) == 0) {
                say_it (praise_me[rand() % (sizeof (praise_me) / sizeof (praise_me[0]))]);
                return cur_node;
            }
            else if (strncmp (answer, "нет", 4) == 0) {
                say_it (mistake[rand() % (sizeof (mistake) / sizeof (mistake[0]))]);
                scanf ("%[^\n]", answer);
                getc (stdin);

                if (strncmp (answer, "да", 3) == 0) {                                               //want divider
                    //now cur_node is the wrong leaf
                    //cur_node = cur_node->parent;
                    say_it ("Продолжите фразу: \"загаданный мной объект отличается от ");
                    say_it (cur_node->data);
                    say_it (", тем, что он ...\"\n");

                    char* divider = (char*) calloc (default_node_size, sizeof (char));
                    scanf ("%[^\n]", divider);
                    getc (stdin);

                    say_it (which_def[rand() % (sizeof (which_def) / sizeof (which_def[0]))]);
                    char* new_leaf = (char*) calloc (default_node_size, sizeof (char));
                    scanf ("%[^\n]", new_leaf);
                    getc (stdin);

                    cur_node->add_leaf (divider, new_leaf);
                    say_it (thank_for_def[rand() % (sizeof (thank_for_def) / sizeof (thank_for_def[0]))]);
                    return nullptr;                                                                        //another one
                }
                else if (strncmp (answer, "нет", 4) == 0) {                                         //don't want divider
                    return cur_node->parent;
                }
                else {                                                                                      //WA
                    err_info ("Wrong answer (only да or нет)\n");
                    say_it (not_yesno[rand() % (sizeof (not_yesno) / sizeof (not_yesno[0]))]);
                }
            }
            else {                                                                                          //WA
                err_info ("Wrong answer (only да or нет)\n");
                say_it (not_yesno[rand() % (sizeof (not_yesno) / sizeof (not_yesno[0]))]);
            }
        }
    }
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
bool node::add_leaf(char* divider, char* new_leaf) {
    node* r_leaf = new node;
    node* l_leaf = new node;

    r_leaf->data = data;
    l_leaf->data = new_leaf;
    data = divider;

    this->merge (r_leaf, 'r');
    this->merge (l_leaf, 'l');

    return true;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Searches path for a leaf
//!
//! @param [in] desired - desired line
//! @param [in] where - l for left, r for right
//! @param [in] cur - pointer to text
//!
//! @return path to leaf as 101101010, 1 - left, 0 - right, (first 1 is not actually a part of a path)
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
int node::search_leaf(char *desired, unsigned int path) {
    ASSERT (desired != nullptr)
    //printf ("cur-path = %d\n", path);
    int tmp = 0;
    if (strncmp (desired, this->data, default_node_size) == 0) {
        return path;
    }

    if (this->is_left()) {
        tmp = left->search_leaf (desired, (path << 1) + 1);
        if (tmp != 0) {
            return tmp;
        }
    }

    if (this->is_right()) {
        tmp = right->search_leaf (desired, (path << 1));
        if (tmp != 0) {
            return tmp;
        }
    }

    return 0;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Prints all the nodes by a given path
//!
//! @param [in] path - path to go on
//!
//! @return if everything was correct
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::print_path(int path) {
    int num = find_bool_length (path) - 1;
    node* cur_node = this;

    //printf ("num = %d\n", num);
    int mask = 1 << (num - 1);
    //printf ("mask = %d\n", mask);

    for (int i = num - 1; i >= 0; --i) {
        bool digit = (path & mask) >> i;
        //printf ("digit = %d\n", digit);
        if (digit) {
            say_it (cur_node->data);
            printf (", ");
            cur_node = cur_node->left;
        }
        else {
            say_it ("не ");
            say_it (cur_node->data);
            printf (", ");
            cur_node = cur_node->right;
        }
        mask = mask >> 1;
    }
    printf ("\b\b\n");
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Compares two nodes of a tree by their paths
//!
//! @param [in] path1 - path to first node
//! @param [in] path2 - path to first node
//!
//! @return if everything was correct
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::compare(int path1, int path2) {
    node* cur_node = this;

    int num1 = find_bool_length (path1) - 1;
    int num2 = find_bool_length (path2) - 1;

    int max_num = (((num1) > (num2)) ? (num1) : (num2));

    int mask1 = 1 << (num1 - 1);
    int mask2 = 1 << (num2 - 1);

    int i = 1;

    bool digit1 = (path1 & mask1) >> (num1 - i);
    bool digit2 = (path2 & mask2) >> (num2 - i);

    say_it ("Они оба: ");
    while (digit1 == digit2 && i <= num1 && i <= num2) {

        if (digit1) {
            say_it (cur_node->data);
            printf (", ");
            cur_node = cur_node->left;
        }
        else {
            say_it ("не ");
            say_it (cur_node->data);
            printf (", ");
            cur_node = cur_node->right;
        }

        mask1 = mask1 >> 1;
        mask2 = mask2 >> 1;

        ++i;

        digit1 = (path1 & mask1) >> (num1 - i);
        digit2 = (path2 & mask2) >> (num2 - i);
    }

    if (path1 != path2) {
        say_it ("\nоднако первый: ");
        mask1 = (1 << (num1 - i + 1)) - 1;
        cur_node->print_path ((path1 & mask1) + (1 << (num1 - i + 1)));

        say_it ("а второй, в свою очередь: ");
        mask2 = (1 << (num2 - i + 1)) - 1;
        cur_node->print_path ((path2 & mask2) + (1 << (num2 - i + 1)));
    }

}

//{1{2{3{5}{8}}{4}}{6{9}{7}}}
//{зверь{с шерстью{милый{котенок}{скунс}}{кошка-сфинкс}}{но в душе зверь{кузьменко}{цветок}}}
bool play() {
    FILE* input = fopen ("input.txt", "r");

    node* nd1 = new node;
    nd1->init();
    nd1->get_tree ();

    nd1->photo ();

    int mode = 1;
    while (mode != 0) {
        say_it ("Во что вы хотите играть? 1 - угадывание, 2 - определение объекта, 3 - сравнение двух строк, 4 - сохранить изменения, 0 - выход\n");
        scanf ("%d", &mode);
        getc (stdin);

        switch (mode) {
            case 0: {
                break;
            }
            case 1: {
                nd1->guess();
                break;
            }
            case 2: {
                char* req_elem = (char*) calloc (default_node_size, sizeof (char));
                say_it ("Какой объект вы хотите узнать от машины, преисполненной силы умственного познания?\n");
                scanf ("%s", req_elem);
                unsigned int path1 = nd1->search_leaf (req_elem);

                nd1->print_path (path1);
                free (req_elem);
                break;
            }
            case 3: {
                char* req_elem = (char*) calloc (default_node_size, sizeof (char));
                say_it ("Каков первый объект для сравнения через мою ультра умную систему?\n");
                scanf ("%s", req_elem);
                unsigned int path1 = nd1->search_leaf (req_elem);

                say_it ("Каков второй объект для сравнения через мой высоко-эффективный интеллект?\n");
                scanf ("%s", req_elem);
                unsigned int path2 = nd1->search_leaf (req_elem);

                nd1->compare (path1, path2);

                free (req_elem);
                break;
            }
            case 4: {
                nd1->save();
                say_it (praise_me[rand() % (sizeof (praise_me) / sizeof (praise_me[0]))]);
                putc ('\n', stdin);

                nd1->clear ();
                nd1->init ();
                nd1->get_tree ();

                nd1->photo ();

                break;
            }
            case 5: {
                say_it ("К сожалению, мой создатель не обладает столь великоразумным мозгом, и не смог отдебажить эту часть моего невероятного функционала. Все вопросы к нему!\n");
            }
            default: {
                say_it ("Вы ввели что-то странное. Давайте еще разок!\n");
                break;
            }
        }
    }

    fclose (input);

    return true;
}

#endif
