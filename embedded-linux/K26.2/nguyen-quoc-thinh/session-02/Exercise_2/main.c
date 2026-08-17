#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    /* copy argv[1] into a modifiable buffer */
    char buffer[256];
    strncpy(buffer, argv[1], sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    printf("Original string: %s\n", buffer);

    int count = str_count(buffer);
    printf("Character count: %d\n", count);

    str_to_upper(buffer);
    printf("Uppercase: %s\n", buffer);

    str_reverse(buffer);
    printf("Reversed (of uppercase): %s\n", buffer);

    return 0;
}
