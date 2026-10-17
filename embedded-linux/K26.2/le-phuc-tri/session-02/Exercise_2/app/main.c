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

    char text[100];

    strcpy(text, argv[1]);

    printf("Original : %s\n", text);

    printf("Length   : %d\n", str_count(text));

    str_to_upper(text);

    printf("Upper    : %s\n", text);

    str_reverse(text);

    printf("Reverse  : %s\n", text);

    return 0;
}
