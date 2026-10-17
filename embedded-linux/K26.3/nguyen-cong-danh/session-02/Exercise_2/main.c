#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stringutils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char buffer[256];

    strncpy(buffer, argv[1], sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    printf("Original : %s\n", buffer);

    printf("Length   : %d\n", str_count(buffer));

    str_to_upper(buffer);
    printf("Upper    : %s\n", buffer);

    str_reverse(buffer);
    printf("Reverse  : %s\n", buffer);

    return 0;
}