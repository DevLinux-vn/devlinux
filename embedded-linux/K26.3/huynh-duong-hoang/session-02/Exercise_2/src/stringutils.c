#include "stringutils.h"

int str_count(const char *s) {
    if (s == (void *)0) {
        return 0;
    }
    int count = 0;
    while (s[count] != '\0') {
        count++;
    }
    return count;
}

void str_to_upper(char *s) {
    if (s == (void *)0) {
        return;
    }
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = s[i] - ('a' - 'A');
        }
    }
}

void str_reverse(char *s) {
    if (s == (void *)0) {
        return;
    }
    int length = str_count(s);
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = s[start];
        s[start] = s[end];
        s[end] = temp;
        start++;
        end--;
    }
}