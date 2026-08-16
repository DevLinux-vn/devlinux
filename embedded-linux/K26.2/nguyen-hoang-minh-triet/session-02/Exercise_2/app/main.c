#include <stdio.h>
#include <string.h>
#include "stringutils.h"

#define MAX_STRING_LENGTH 128

int main(int argc, char *argv[])    {
    char str[MAX_STRING_LENGTH];

    if (argc < 2)   {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) >= MAX_STRING_LENGTH)   {
        printf("Error: string is too long.\n");
        return 1;
    }

    (void)strcpy(str, argv[1]);

    printf("Original : %s\n", str);

    printf("Count    : %d\n", str_count(str));

    str_to_upper(str);
    printf("Uppercase: %s\n", str);

    str_reverse(str);
    printf("Reverse  : %s\n", str);

    return 0;
}
