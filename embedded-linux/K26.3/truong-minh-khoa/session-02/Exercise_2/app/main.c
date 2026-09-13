#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main() {
    char text[] = "Hello World";
    char text_copy[sizeof(text)];

    strcpy(text_copy, text);
    str_to_upper(text_copy);
    printf("Upper: %s\n", text_copy);

    printf("Count: %d\n", str_count(text));

    str_reverse(text);
    printf("Reverse: %s\n", text);

    return 0;
}


