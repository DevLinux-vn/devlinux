#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    const char *input = argv[1];

    char *upper = malloc(strlen(input) + 1);
    char *reverse = malloc(strlen(input) + 1);

    if (upper == NULL || reverse == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");

        free(upper);
        free(reverse);

        return 1;
    }

    strcpy(upper, input);
    strcpy(reverse, input);

    str_to_upper(upper);

    printf("Original: %s\n", input);
    printf("Uppercase: %s\n", upper);
    printf("Count: %d\n", str_count(input));

    str_reverse(reverse);
    printf("Reverse: %s\n", reverse);

    free(upper);
    free(reverse);

    return 0;
}