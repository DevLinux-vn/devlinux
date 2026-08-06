#include "stringutils.h"
#include <ctype.h>

void str_to_upper(char *s) {
    for (; *s; s++) *s = toupper((unsigned char)*s);
}

int str_length(const char *s) {
    int n = 0;
    while (s[n]) n++;
    return n;
}

void str_reverse(char *s) {
    int i = 0, j = str_length(s) - 1;
    while (i < j) {
        char t = s[i]; s[i] = s[j]; s[j] = t;
        i++; j--;
    }
}
