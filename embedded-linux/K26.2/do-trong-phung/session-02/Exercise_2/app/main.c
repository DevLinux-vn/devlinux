#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char str[1000];

    strncpy(str, argv[1], sizeof(str) - 1);
    str[sizeof(str) - 1] = '\0';

    printf("Original string: %s\n", str);

    str_to_upper(str);
    printf("Uppercase: %s\n", str);

    printf("Character count: %d\n", str_count(str));

    str_reverse(str);
    printf("Reversed: %s\n", str);

    return 0;
}