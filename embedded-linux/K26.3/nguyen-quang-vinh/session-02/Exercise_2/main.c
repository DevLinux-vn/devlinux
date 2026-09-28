#include <stdio.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char *str = argv[1];

    printf("Original string: %s\n", str);

    printf("Character count: %d\n", str_count(str));

    str_to_upper(str);
    printf("Uppercase string: %s\n", str);

    str_reverse(str);
    printf("Reversed string: %s\n", str);

    return 0;
}