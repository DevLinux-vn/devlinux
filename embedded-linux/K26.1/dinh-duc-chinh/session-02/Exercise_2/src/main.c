#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char buf[256];
    strncpy(buf, argv[1], sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    printf("Length : %d\n", str_length(buf));

    str_to_upper(buf);
    printf("Upper  : %s\n", buf);

    str_reverse(buf);
    printf("Reverse: %s\n", buf);
    return 0;
}
