#ifndef STRINGUTILS_H
#define STRINGUTILS_H

// Chuyển đổi một chuỗi thành chữ hoa ngay tại chỗ (in-place)
void to_uppercase(char *str);

// Đếm số lượng ký tự trong chuỗi (không tính \0)
int count_chars(const char *str);

// Đảo ngược một chuỗi ngay tại chỗ (in-place)
void reverse_string(char *str);

#endif