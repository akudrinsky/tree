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
    bool save (const char* filename = "tree_saved.txt", FILE* pFile = nullptr, bool is_first = true, bool need_closing = true);
    bool get_tree (const char* filename = "tree_saved.txt");
    bool is_left ();
    bool is_right ();

    bool is_valid();

    bool delete_subtree ();

    node* guess ();                         //returns pointer to node if it was guessed or pointer to the last node of differ
    bool add_leaf (char* divider, char* new_leaf);
    int search_leaf (char* desired, unsigned int path = 1);
    bool print_path (int path);
    bool compare (int path1, int path2);

};
#pragma pack(pop)

bool get_subtree (node* nd, char where, char* *cur);

node::node(int size) {
    data = nullptr;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

node::~node() {                                                        //make it safer!!!
    if (data != nullptr)
        free (data);
}

void node::init(int size) {
    data = (char*) calloc (size, sizeof (char));
    if (data == nullptr) {
        err_info ("Problem with memory allocation in node init\n");
    }
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}

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
    delete this;
}

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

bool merge2 (node* root, node* leaf, char where) {
    if (leaf->data == "@") {
        return true;
    }
    if (where == 'l') {
        root->left = leaf;
        leaf->parent = root;
        return true;
    }
    else if (where == 'r') {
        root->right = leaf;
        leaf->parent = root;
        return true;
    }
    else {
        err_info ("Wrong format of merge, where = ");
        err_info (&where);
        err_info ("\n");
        return false;
    }
}

void node::photo(const char* pict_name, const char* pict_type, int iter, FILE* pFile) {
    ASSERT (pict_name != nullptr)
    ASSERT (pict_type != nullptr)
    //printf ("* ");

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

bool node::save(const char* filename, FILE* pFile, bool is_first, bool need_closing) {
    ASSERT (filename != nullptr)

    bool status = true;
    if (is_first) {
        pFile = fopen (filename, "w");
        ASSERT (pFile != nullptr)
    }

    fprintf (pFile, "{%s", data);

    if (left == nullptr && right != nullptr) {
        fprintf (pFile, "@");
        if (!(*right).save(filename, pFile, false, false))
            status = false;
    }
    if (left != nullptr && right == nullptr) {
        if (!(*left).save (filename, pFile, false, false))
            status = false;
        fprintf (pFile, "@");
    }
    if (left != nullptr && right != nullptr) {
        if (!(*left).save (filename, pFile, false, false))
            status = false;
        if (!(*right).save (filename, pFile, false, true))
            status = false;
    }

    fprintf (pFile, "}");

    if (is_first) {
        fclose (pFile);
    }

    return status;
}

bool node::get_tree(const char* filename) {
    ASSERT (filename != nullptr)
    FILE* pFile = fopen (filename, "r");
    ASSERT (pFile != nullptr)

    //printf ("in function: %p\n", this);

    char* file = read_text (pFile);
    char* cur = file;
    int got_c = 0;

    if (*file == '{') {
        ++cur;
        sscanf (cur, "%[^{}]s%n", data, &got_c);
        cur += got_c + 1;                                       //+ 1 because we need symbol after the last
        //printf ("%s\n", data);

        if (*cur != '}') {
            get_subtree (this, 'l', &cur);
            get_subtree (this, 'r', &cur);
        }
        //printf ("chchchc%p\n", this->left);
    }
    else {
        err_info ("Wrong format of saved tree (get_tree)\n");
        free (file);
        fclose (pFile);
        return false;
    }

    free (file);
    fclose (pFile);
    //this->photo("ex.png");                                          //something bad happens - look to tree_graph.dot
    return true;
}

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

    //printf ("cur: %s\n", *cur);
    sscanf (*cur, "%[^{}]s%n", leaf_data, &got_c);
    printf ("leaf[%c]: %s\n", where, leaf_data);
    *cur += got_c + 1;                                       //+ 1 because we need symbol after the last
    printf ("nd (father): %p\n", nd);

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
        node (new_nd);
        new_nd.data = leaf_data;
        nd->merge (&new_nd, where);
        //merge2 (nd, &new_nd, where);
        //new_nd.parent = nd;
        printf ("parent of child: %p\n", new_nd.parent);
        printf ("nd (child): %p\n", &new_nd);
        printf ("son of parent: %p\n\n", nd->left);

        if (**cur == '{') {
            get_subtree (&new_nd, 'l', cur);
            get_subtree (&new_nd, 'r', cur);
        }
        else {
            new_nd.left = nullptr;
            new_nd.right = nullptr;
        }
    }

    return true;
}

bool node::is_left() {
    return left != nullptr;
}

bool node::is_right() {
    return right != nullptr;
}

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

bool node::delete_subtree() {
    if (is_left()) {
        left->delete_subtree();
    }
    if (is_right()) {
        right->delete_subtree();
    }
    return true;
}

node* node::guess() {
    node* cur_node = this;
    char answer[10] = "";
    while (true) {
        say_it (cur_node->data);
        printf ("\t");
        scanf ("%s", answer);
        if (strncmp (answer, "да", 3) == 0) {                                                       //left - yes
            cur_node = cur_node->left;
        }
        else if (strncmp (answer, "нет", 4) == 0) {                                                 //right - no
            cur_node = cur_node->right;
        }
        else {
            err_info ("Wrong answer (only да or нет)\n");
            say_it (not_yesno[rand() % not_yesno_n]);
        }

        if (!cur_node->is_right() && !cur_node->is_left()) {
            say_it ("Я считаю, что это был: ");
            say_it (cur_node->data);
            say_it ("\nЯ угадал?\n");

            scanf ("%s", answer);
            if (strncmp (answer, "да", 3) == 0) {
                say_it (praise_me[rand() % praise_me_n]);
                return cur_node;
            }
            else if (strncmp (answer, "нет", 4) == 0) {
                say_it (mistake[rand() % mistake_n]);
                scanf ("%s", answer);
                if (strncmp (answer, "да", 3) == 0) {                                               //want divider
                    //now cur_node is the wrong leaf
                    //cur_node = cur_node->parent;
                    say_it ("Продолжите фразу: \"загаданный мной объект отличается от ");
                    say_it (cur_node->data);
                    say_it (", тем, что он ...\"\n");

                    char* divider = (char*) calloc (default_node_size, sizeof (char));
                    scanf ("%s", divider);

                    say_it (which_def[rand() % which_def_n]);
                    char* new_leaf = (char*) calloc (default_node_size, sizeof (char));
                    scanf ("%s", new_leaf);

                    cur_node->add_leaf (divider, new_leaf);
                    say_it (thank_for_def[rand() % thank_for_def_n]);
                    return nullptr;                                                                        //another one
                }
                else if (strncmp (answer, "нет", 4) == 0) {                                         //don't want divider
                    return cur_node->parent;
                }
                else {                                                                                      //WA
                    err_info ("Wrong answer (only да or нет)\n");
                    say_it (not_yesno[rand() % not_yesno_n]);
                }
            }
            else {                                                                                          //WA
                err_info ("Wrong answer (only да or нет)\n");
                say_it (not_yesno[rand() % not_yesno_n]);
            }
        }
    }
}

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

int node::search_leaf(char *desired, unsigned int path) {                          //returns path to leaf as 01101010, 1 - left, 0 - right
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

int find_bool_length (int path, const int max_depth = 10) {
    int num = 0;

    for (num = max_depth; num > 0; num--) {
        if ((path >> num) == 1) {
            break;
        }
    }

    if (num == max_depth)
        err_info ("maybe you had overwritten max_depth while finding digits number\n");

    return num + 1;
}

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
    printf ("\n");
}

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
    while (digit1 == digit2) {

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

    say_it ("\nоднако первый: ");
    mask1 = (1 << (num1 - i + 1)) - 1;
    cur_node->print_path ((path1 & mask1) + (1 << (num1 - i + 1)));

    say_it ("а второй, в свою очередь: ");
    mask2 = (1 << (num2 - i + 1)) - 1;
    cur_node->print_path ((path2 & mask2) + (1 << (num2 - i + 1)));

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
    node (nd8);
    node (nd9);

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
    fscanf (input, "%[^\n]s", nd8.data);
    fgetc (input);
    fscanf (input, "%[^\n]s", nd9.data);
    fgetc (input);

    nd1.merge (&nd2, 'l');
    nd2.merge (&nd3, 'l');
    nd2.merge (&nd4, 'r');
    nd3.merge (&nd5, 'l');
    nd1.merge (&nd6, 'r');
    nd6.merge (&nd7, 'r');
    nd3.merge (&nd8, 'r');
    nd6.merge (&nd9, 'l');

    //printf ("%s\n%s", nd.data, str1);

    nd1.photo ();
    nd1.save ();


    node* nd = new node;
    nd->init();
    nd->get_tree ();
    printf ("Look\n\nroot: %p\n", &nd);
    printf ("parent of child: %p\n", nd->left->parent);
    printf ("left child: %p\n", nd->right);
    printf ("son of parent: %p\n", nd->left);

    nd->is_valid ();

    nd->photo ("got_tree.png");
    nd->clear();

    /*
    unsigned int path1 = nd1.search_leaf (nd5.data);
    //printf ("path1: %d\n", path1);
    nd1.print_path (path1);

    unsigned int path2 = nd1.search_leaf (nd8.data);
    //printf ("path1: %d\n", path2);
    nd1.print_path (path2);

    nd1.compare (path1, path2);
     */


    /*
    int mode = 1;
    while (mode != 0) {
        say_it ("Во что вы хотите играть? 1 - угадывание, 2 - определение объекта, 3 - сравнение двух строк, 4 - сохранить изменения, 0 - выход\n");
        scanf ("%d", &mode);
        switch (mode) {
            case 1: {
                nd1.guess();
                break;
            }
            case 2: {
                char* req_elem = (char*) calloc (default_node_size, sizeof (char));
                say_it ("Какой объект вы хотите узнать от машины, преисполненной силы умственного познания?\n");
                scanf ("%s", req_elem);
                unsigned int path1 = nd1.search_leaf (req_elem);

                nd1.print_path (path1);
                free (req_elem);
                break;
            }
            case 3: {
                char* req_elem = (char*) calloc (default_node_size, sizeof (char));
                say_it ("Каков первый объект для сравнения через мою ультра умную систему?\n");
                scanf ("%s", req_elem);
                unsigned int path1 = nd1.search_leaf (req_elem);

                say_it ("Каков второй объект для сравнения через мой высоко-эффективный интеллект?\n");
                scanf ("%s", req_elem);
                unsigned int path2 = nd1.search_leaf (req_elem);

                nd1.compare (path1, path2);

                free (req_elem);
                break;
            }
            case 4: {
                say_it ("К сожалению, мой создатель не обладает столь великоразумным мозгом, и не смог отдебажить эту часть моего невероятного функционала. Все вопросы к нему!\n");
                break;
            }
            default: {
                say_it ("Вы ввели что-то странное. Давайте еще разок!\n");
                break;
            }
        }
    }
     */

    fclose (input);

    return true;
}

#endif