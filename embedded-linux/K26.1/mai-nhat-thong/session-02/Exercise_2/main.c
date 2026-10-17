#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string_input>\n", argv[0]);
        return 1;
    }

    /* Duplicate argv[1] since we modify it in-place */
    char *input_str = strdup(argv[1]);
    if (!input_str) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    printf("Original String : %s\n", input_str);
    printf("Character Count : %d\n", str_count(input_str));

    str_to_upper(input_str);
    printf("Uppercase Output: %s\n", input_str);

    str_reverse(input_str);
    printf("Reversed Output : %s\n", input_str);

    free(input_str);
    return 0;
}