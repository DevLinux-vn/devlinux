#include "stringutils.h"
#include <ctype.h>   // for toupper
#include <string.h>  // for strlen

void str_to_upper(char *s) {
    if (!s) return;
    while (*s) {
        *s = (char)toupper((unsigned char)*s);
        s++;
    }
}

int str_count(const char *s) {
    if (s == NULL) return 0;
    int count = 0;
    while (*s != '\0') {
        count++;
        s++;
    }
    return count;
}

void str_reverse(char *s) {
    if (!s) return;
    int len = (int)strlen(s);
    for (int i = 0; i < len / 2; i++) {
        char tmp = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = tmp;
    }
}

