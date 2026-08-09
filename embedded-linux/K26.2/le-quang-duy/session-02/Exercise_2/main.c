#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s \"<string>\"\n", argv[0]);
        return 1;
    }

    // Clone the string to manipulate it in-place
    char *input = strdup(argv[1]);
    
    printf("Original: %s\n", input);
    printf("Length: %d\n", str_count(input));

    str_to_upper(input);
    printf("Uppercase: %s\n", input);

    str_reverse(input);
    printf("Reversed: %s\n", input);

    free(input);
    return 0;
}