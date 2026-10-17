#include "stringutils.h"

// Define the difference between uppercase and lowercase ASCII characters
#define CHAR_CASE_DIFF 32

void str_to_upper(char *s) {
    while (*s) {
        if (*s >= 'a' && *s <= 'z') {
            *s = *s - CHAR_CASE_DIFF;
        }
        s++;
    }
}

int str_count(const char *s) {
    int count = 0;
    while (*s != '\0') {
        count++;
        s++;
    }
    return count;
}

void str_reverse(char *s) {
    int len = str_count(s);
    for (int i = 0; i < len / 2; i++) {
        char temp = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = temp;
    }
}
