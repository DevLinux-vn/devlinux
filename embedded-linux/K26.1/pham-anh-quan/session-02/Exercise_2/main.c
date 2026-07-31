/* main.c */
#include <stdio.h>
#include <stdlib.h>
#include "stringutils.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Please provide a string argument.\n");
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    // Tạo bản sao của chuỗi để thao tác vì argv[1] có thể bị hạn chế thay đổi trực tiếp ở một số môi trường
    int len = str_count(argv[1]);
    char *str = malloc(len + 1);
    if (!str) return 1;
    
    // Copy dữ liệu sang vùng nhớ mới
    for (int i = 0; i <= len; i++) {
        str[i] = argv[1][i]; // Đã fix lỗi Markdown hiển thị nếu trùng
    }
    // Thực tế viết chuẩn:
    char *src = argv[1];
    char *dest = str;
    while ((*dest++ = *src++));

    printf("Original string: %s\n", str);
    printf("Character count: %d\n", str_count(str));

    str_to_upper(str);
    printf("Uppercase string: %s\n", str);

    str_reverse(str);
    printf("Reversed string: %s\n", str);

    free(str);
    return 0;
}