#include <stdio.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char str[256];

    snprintf(str, sizeof(str), "%s", argv[1]);

    printf("Original: %s\n", str);

    str_to_upper(str);
    printf("Uppercase: %s\n", str);

    printf("Character count: %d\n", str_count(str));

    str_reverse(str);
    printf("Reversed: %s\n", str);

    return 0;
}
