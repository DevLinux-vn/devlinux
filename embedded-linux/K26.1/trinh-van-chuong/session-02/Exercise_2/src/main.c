#include <stdio.h>
#include <stdlib.h>
#include "stringutils.h"

int main(int argc, char *argv[]) {
    // Đề bài yêu cầu nhận chuỗi từ đối số dòng lệnh argv[1]
    if (argc < 2) {
        printf("Loi: Thieu chuoi dau vao!\n");
        printf("Huong dan chay: %s \"chuoi_cua_ban\"\n", argv[0]);
        return 1;
    }

    // Tạo một vùng nhớ đệm để copy chuỗi ra xử lý (vì argv[1] là hằng số không nên sửa trực tiếp)
    int len = count_chars(argv[1]);
    char *my_str = malloc(len + 1);
    if (!my_str) return 1;

    // Copy chuỗi thủ công hoặc dùng hàm sao chép cơ bản
    for (int i = 0; i <= len; i++) {
        my_str[i] = argv[1][i];
    }

    printf("--- Test Thu Vien Dong StringUtils ---\n");
    printf("Chuoi ban dau: \"%s\"\n", argv[1]);
    printf("So luong ky tu: %d\n", count_chars(my_str));

    to_uppercase(my_str);
    printf("Chuoi sau khi viet hoa: \"%s\"\n", my_str);

    reverse_string(my_str);
    printf("Chuoi sau khi dao nguoc: \"%s\"\n", my_str);

    free(my_str);
    return 0;
}