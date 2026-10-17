#include <stdio.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    printf("Original: %s\n", argv[1]);

    printf("Count: %d\n", str_count(argv[1]));

    str_to_upper(argv[1]);
    printf("Uppercase: %s\n", argv[1]);

    str_reverse(argv[1]);
    printf("Reverse: %s\n", argv[1]);

    return 0;
}
