#include "./include/stringutils.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }
    char *str = argv[1];
    str_to_upper(str);

    printf("toUpper: %s \n", str);
    printf("String have %d characters\n", str_count(str));
    str_reverse(str);
    printf("reverse %s \n", str);
}
