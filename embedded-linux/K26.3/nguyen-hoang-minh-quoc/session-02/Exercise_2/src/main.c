#include "stringutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *text;

    if (argc != 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    text = malloc(strlen(argv[1]) + 1);
    if (text == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    strcpy(text, argv[1]);

    printf("Original: %s\n", text);
    printf("Count: %d\n", str_count(text));

    str_to_upper(text);
    printf("Uppercase: %s\n", text);

    str_reverse(text);
    printf("Reversed: %s\n", text);

    free(text);
    return 0;
}
