#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || !argv[1]) {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    char str[1024];
    if (strlen(argv[1]) >= sizeof(str)) {
        fprintf(stderr, "String too long\n");
        return 1;
    }
    // Sử dụng strncpy kèm null-termination thủ công để đảm bảo an toàn vì strncpy không tự thêm \0 nếu chuỗi chạm giới hạn
    strncpy(str, argv[1], sizeof(str) - 1);
    str[sizeof(str) - 1] = '\0';

    int count = str_count(str);
    printf("Original: %s\n", str);
    printf("Count: %d\n", count);

    str_to_upper(str);
    printf("Upper: %s\n", str);

    str_reverse(str);
    printf("Reversed: %s\n", str);

    return 0;
}
