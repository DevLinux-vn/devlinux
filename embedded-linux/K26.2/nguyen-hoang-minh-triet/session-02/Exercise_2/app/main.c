#include <stdio.h>

#include "stringutils.h"

int main(int argc, char *argv[])
{
    char *str;

    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    str = argv[1];

    printf("Original : %s\n", str);

    printf("Count    : %d\n", str_count(str));

    str_to_upper(str);
    printf("Uppercase: %s\n", str);

    str_reverse(str);
    printf("Reverse  : %s\n", str);

    return 0;
}

