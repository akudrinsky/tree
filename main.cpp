#include <stdio.h>
#include "tree.cpp"

int main() {
    FILE* input = fopen ("input.txt", "r");
    char* str = (char*) calloc (20, sizeof (char));
    node nd (str);
    char* str1 = (char*) calloc (20, sizeof (char));
    node nd1 (str1);
    char* str2 = (char*) calloc (20, sizeof (char));
    node nd2 (str2);
    char* str3 = (char*) calloc (20, sizeof (char));
    node nd3 (str3);

    fscanf (input, "%[^\n]s", str);
    fgetc (input);
    fscanf (input, "%[^\n]s", str1);
    fgetc (input);
    fscanf (input, "%[^\n]s", str2);
    fgetc (input);
    fscanf (input, "%[^\n]s", str3);
    fgetc (input);

    nd.merge (&nd1, 'l');
    nd1.merge (&nd2, 'r');
    nd1.merge (&nd3, 'l');
    //printf ("%s\n%s", nd.data, str1);

    nd.photo ();
    fclose (input);

    return 0;
}

