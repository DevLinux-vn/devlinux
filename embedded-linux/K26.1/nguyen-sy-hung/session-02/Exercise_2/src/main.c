#include <stdio.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./main <string>\n");
        return 1;
    }

    printf("Original string: %s\n", argv[1]);

    printf("Character count: %d\n", str_count(argv[1]));

    str_to_upper(argv[1]);
    printf("Uppercase: %s\n", argv[1]);

    str_reverse(argv[1]);
    printf("Reverse: %s\n", argv[1]);

    return 0;
}