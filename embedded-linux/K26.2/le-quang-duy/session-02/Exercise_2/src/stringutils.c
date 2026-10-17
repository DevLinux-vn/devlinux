#include "stringutils.h"
#include <ctype.h>
#include <string.h>

void str_to_upper(char *s) {
    if (!s) return;
    while (*s) {
        *s = toupper((unsigned char)*s);
        s++;
    }
}

int str_count(const char *s) {
    if (!s) return 0;
    int count = 0;
    while (*s) {
        count++;
        s++;
    }
    return count;
}

void str_reverse(char *s) {
    if (!s) return;
    int len = str_count(s);
    for (int i = 0; i < len / 2; i++) {
        char temp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = temp;
    }
}