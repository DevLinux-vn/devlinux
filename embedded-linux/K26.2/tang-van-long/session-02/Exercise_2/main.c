#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char str[100];

    strcpy(str, argv[1]);

    printf("Original : %s\n", str);

    printf("Length   : %d\n", str_count(str));

    str_to_upper(str);
    printf("Upper    : %s\n", str);

    str_reverse(str);
    printf("Reverse  : %s\n", str);

    return 0;
}