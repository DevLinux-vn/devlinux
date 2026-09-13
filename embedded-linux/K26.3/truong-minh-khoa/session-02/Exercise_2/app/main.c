#include <stdio.h>
#include "stringutils.h"

int main() {
    char text[] = "Hello World";

    str_to_upper(text);
    printf("Upper: %s\n", text);

    printf("Count: %d\n", str_count(text));

    str_reverse(text);
    printf("Reverse: %s\n", text);

    return 0;
}

