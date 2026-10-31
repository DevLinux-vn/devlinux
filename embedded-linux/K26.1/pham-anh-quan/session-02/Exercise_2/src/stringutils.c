/* src/stringutils.c */
#include "stringutils.h"
#include <ctype.h>

// Chuyển chuỗi thành chữ hoa ngay trên vùng nhớ gốc (in-place)
void str_to_upper(char *s) {
    if (!s) return;
    while (*s) {
        *s = toupper((unsigned char)*s);
        s++;
    }
}

// Đếm số ký tự (không tính ký tự kết thúc '\0')
int str_count(const char *s) {
    if (!s) return 0;
    int count = 0;
    while (*s) {
        count++;
        s++;
    }
    return count;
}

// Đảo ngược chuỗi (in-place)
void str_reverse(char *s) {
    if (!s) return;
    int len = str_count(s);
    int i = 0, j = len - 1;
    while (i < j) {
        char temp = s[i];
        s[i] = s[j];
        s[j] = temp;
        i++;
        j--;
    }
}