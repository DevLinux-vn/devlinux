#include <ctype.h>
#include <string.h>
#include "stringutils.h"

void to_uppercase(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

int string_length(const char *str) {
    if (str == NULL) return 0;
    return strlen(str);
}

void reverse_string(char *str) {
    if (str == NULL) return;
    
    int len = strlen(str);
    int left = 0, right = len - 1;
    
    while (left < right) {
        char temp = str[left];
        str[left] = str[right];
        str[right] = temp;
        left++;
        right--;
    }
}
