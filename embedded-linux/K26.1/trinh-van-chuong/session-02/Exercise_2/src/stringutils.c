#include "stringutils.h"
#include <ctype.h>

void to_uppercase(char *str) {
    if (!str) return;
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int count_chars(const char *str) {
    if (!str) return 0;
    int count = 0;
    while (str[count] != '\0') {
        count++;
    }
    return count;
}

void reverse_string(char *str) {
    if (!str) return;
    int len = count_chars(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}