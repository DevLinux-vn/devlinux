#include "stringutils.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return 1;
    }
    char copy_of_s[100];
    strcpy(copy_of_s, argv[1]);
    str_to_upper(copy_of_s);
    printf("Upper of string: %s\n", copy_of_s);
    printf("String length: %d\n", str_count(argv[1]));
    strcpy(copy_of_s, argv[1]);
    str_reverse(copy_of_s);
    printf("Reversed string: %s\n", copy_of_s);
    return 0;
}