#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    int len = str_count(argv[1]);
    char *buf = (char *)malloc(len + 1);
    if (!buf) {
        perror("malloc failed");
        return 1;
    }

    strcpy(buf, argv[1]);
    printf("Original string: %s\n", buf);
    printf("Character count: %d\n", str_count(buf));

    str_to_upper(buf);
    printf("Uppercase: %s\n", buf);

    str_reverse(buf);
    printf("Reversed: %s\n", buf);

    free(buf);
    return 0;
}