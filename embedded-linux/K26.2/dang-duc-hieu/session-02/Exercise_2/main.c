#include "./include/stringutils.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
    char *str = argv[1];
    str_to_upper(str);

    printf("toUpper: %s \n", str);
    printf("String have %d characters\n", str_count(str));
    str_reverse(str);
    printf("reverse %s \n", str);
}