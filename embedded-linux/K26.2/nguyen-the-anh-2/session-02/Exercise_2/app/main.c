#include <stdio.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Cach dung: %s <chuoi_cua_ban>\n", argv[0]);
        return 1;
    }

    char *input_str = argv[1];

    printf("Chuoi goc: %s\n", input_str);
    printf("So ky tu: %d\n", str_count(input_str));

    str_to_upper(input_str);
    printf("Viet hoa: %s\n", input_str);

    str_reverse(input_str);
    printf("Dao nguoc: %s\n", input_str);

    return 0;
}