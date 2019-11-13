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

void say_it (const char* message, const char* voice = "Yuri") {
    printf (message);

    char* command = (char* )calloc (200, sizeof (char));
    strcat (command, "say -v ");
    strcat (command, voice);
    strcat (command, " '");
    strcat (command, message);
    strcat (command, "'");

    //execute command with the help of terminal
    system (command);
    free (command);
}

int default_node_size = 20;
#pragma pack(push, 1)
struct node {
    char* data;
    node* left;
    node* right;
    node* parent;

    explicit node (int size = default_node_size);
    ~node ();

    bool merge (node* leaf, char where = 'f');              //r - right, l - left, f - doesn't matter
    void photo (const char* pict_name = "tree.png", const char* pict_type = "png", int iter = 1);
    bool save (FILE* filename, bool is_first = true, bool need_closing = true);
    bool get_tree (FILE* pFile, bool need_left = true);
    bool is_left ();
    bool is_right ();

    bool delete_subtree ();

    node* guess (bool* was_correct);                         //returns pointer to node if it was guessed or pointer to the last node of differ
    bool add_leaf (char* divider, char* new_leaf);
    int search_leaf (char* desired, unsigned int path = 1);
    bool print_path (int path);
    bool compare (int path1, int path2);

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
    if (leaf->data == "@") {
        return true;
    }
    if (where == 'l') {
        if (left != nullptr) {
                                        //destruct left leaf
        }
        left = leaf;
        leaf->parent = this;
        return true;
    }
    else if (where == 'r') {
        if (right != nullptr) {
                                        //destruct right leaf
        }
        right = leaf;
        leaf->parent = this;
        return true;
    }
    else if (where == 'f') {
        return false;
    }
    else {
        err_info ("Wrong format of merge, where = ");
        err_info (&where);
        err_info ("\n");
        return false;
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
        fprintf (pFile, "\t\t%d:<f1> -> %d[color=\"red\"]\n", iter, iter * 2 + 1);
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

bool node::save(FILE* filename, bool is_first, bool need_closing) {
    bool status = true;
    if (filename == nullptr) {
        err_info ("Nullptr file while saving tree\n");
        return false;
    }

    fprintf (filename, "{%s", data);

    if (left == nullptr && right == nullptr) {
        fprintf (filename, "}");
    }
    if (left == nullptr && right != nullptr) {
        fprintf (filename, "@");
        if (!(*right).save(filename, false, false))
            status = false;
    }
    if (left != nullptr && right == nullptr) {
        if (!(*left).save (filename, false, false))
            status = false;
        fprintf (filename, "@");
    }
    if (left != nullptr && right != nullptr) {
        if (!(*left).save (filename, false, true))
            status = false;
        if (!(*right).save (filename, false, false))
            status = false;
    }

    if (need_closing) {
        fprintf (filename, "}");
    }

    if (is_first)
        fprintf (filename, "}");

    return status;
}
//{1{2{3{5}@}{4}}{6@{7}}}
bool node::get_tree(FILE* pFile, bool need_left) {
    if (pFile == nullptr) {
        err_info ("Nullptr file while saving tree\n");
        return false;
    }

    char cur_c = 0;
    node* cur_node = this;
    bool is_first = true;

    while (fscanf (pFile, "%c", &cur_c) != EOF){
        this->photo("recovered_tree.png");
        printf ("symbol: %c\n", cur_c);
        if (cur_c == '{') {
            printf ("\tcur_node \"{\" %p\n", cur_node);
            char* str = (char*) calloc (10, sizeof (char));
            fscanf (pFile, "%[^{}]s", str);
            printf ("string: %s\n", str);

            if (is_first) {
                data = str;
                is_first = false;
            }
            else {
                if (!cur_node->is_left ()) {
                    node (nd);
                    nd.data = str;
                    cur_node->merge (&nd, 'l');
                    cur_node = cur_node->left;
                    printf ("\tcur_node \"}l\" %p\n", cur_node);
                    continue;
                }

                if (!cur_node->is_right ()) {
                    node (nd);
                    nd.data = str;
                    cur_node->merge (&nd, 'r');
                    cur_node = cur_node->right;
                    printf ("\tcur_node \"}r\" %p\n", cur_node);
                    continue;
                }
            }
            printf ("\t\tstr: %p\n", str);
        }

        if (cur_c == '}') {
            cur_node = cur_node->parent;
            printf ("\tcur_node \"}\" %p\n", cur_node);
        }

        if (cur_c == '@') {
            cur_node->photo("recovered_tree.png");

            if (!cur_node->is_left ()) {
                cur_node->left = nullptr;
                continue;
            }

            if (!cur_node->is_right ()) {
                cur_node->right = nullptr;
                continue;
            }
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

bool node::delete_subtree() {
    if (is_left()) {
        left->delete_subtree();
    }
    if (is_right()) {
        right->delete_subtree();
    }
    return true;
}

node* node::guess(bool *was_correct) {
    bool flag = true;
    node* cur_node = this;
    char answer[10] = "";
    while (flag) {
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
            say_it ("Отвечай только да или нет, будто ты советский калькулятор\n");
        }

        if (!cur_node->is_right() && !cur_node->is_left()) {
            say_it ("Я считаю, что это был: ");
            say_it (cur_node->data);
            say_it ("\nЯ угадал?\n");

            scanf ("%s", answer);
            if (strncmp (answer, "да", 3) == 0) {
                say_it ("Жесть какой я умный\n");
                return cur_node;
            }
            else if (strncmp (answer, "нет", 4) == 0) {
                say_it ("О нет, пойду ботать дальше\nВы хотите, чтобы я заботал новое определение?\n");
                scanf ("%s", answer);
                if (strncmp (answer, "да", 3) == 0) {                                               //want divider
                    //now cur_node is the wrong leaf
                    //cur_node = cur_node->parent;
                    say_it ("Продолжите фразу: \"загаданный мной объект отличается от ");
                    say_it (cur_node->data);
                    say_it (", тем, что он ...\"\n");

                    char* divider = (char*) calloc (default_node_size, sizeof (char));
                    gets (divider);

                    say_it ("Введите, что любой уважающий себя искусственный интеллект должен знать\n");
                    char* new_leaf = (char*) calloc (default_node_size, sizeof (char));
                    gets (new_leaf);

                    cur_node->add_leaf (divider, new_leaf);
                    return nullptr;                                                                        //another one
                }
                else if (strncmp (answer, "нет", 4) == 0) {                                         //don't want divider
                    return cur_node->parent;
                }
                else {                                                                                      //WA
                    err_info ("Wrong answer (only да or нет)\n");
                    say_it ("Отвечай только да или нет, будто ты советский калькулятор\n");
                }
            }
            else {                                                                                          //WA
                err_info ("Wrong answer (only да or нет)\n");
                say_it ("Отвечай только да или нет, будто ты советский калькулятор\n");
            }
        }
    }
}

bool node::add_leaf(char* divider, char* new_leaf) {
    node (r_leaf);
    node (l_leaf);

    r_leaf.data = data;
    l_leaf.data = new_leaf;
    data = divider;

    this->merge (&r_leaf, 'r');
    this->merge (&l_leaf, 'l');

    return true;
}

int node::search_leaf(char *desired, unsigned int path) {                          //returns path to leaf as 01101010, 1 - left, 0 - right
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
            printf ("%s; ", cur_node->data);
            cur_node = cur_node->left;
        }
        else {
            printf ("не %s; ", cur_node->data);
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

    printf ("Они оба: ");
    while (digit1 == digit2) {

        if (digit1) {
            printf ("%s; ", cur_node->data);
            cur_node = cur_node->left;
        }
        else {
            printf ("не %s; ", cur_node->data);
            cur_node = cur_node->right;
        }

        mask1 = mask1 >> 1;
        mask2 = mask2 >> 1;

        ++i;

        digit1 = (path1 & mask1) >> (num1 - i);
        digit2 = (path2 & mask2) >> (num2 - i);
    }

    printf ("\nОднако первый: ");
    mask1 = (1 << (num1 - i + 1)) - 1;
    cur_node->print_path ((path1 & mask1) + (1 << (num1 - i + 1)));

    printf ("А второй: ");
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
    FILE* pFile = fopen ("tree_saved.txt", "w");
    nd1.save (pFile);

    unsigned int path1 = nd1.search_leaf (nd5.data);
    //printf ("path1: %d\n", path1);
    nd1.print_path (path1);

    unsigned int path2 = nd1.search_leaf (nd8.data);
    //printf ("path1: %d\n", path2);
    nd1.print_path (path2);

    nd1.compare (path1, path2);

    bool was_correct = true;
    //nd1.guess (&was_correct);

    //nd1.photo ("new_pict.png");

    fclose (pFile);
    fclose (input);
}






#endif