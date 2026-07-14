/**
 * @file stringutils.c
 * @brief Implementation of basic string utility functions.
 */

#include "stringutils.h"
#include <ctype.h>

void str_to_upper(char *s) {
    if (!s) return;
    while (*s) {
        *s = (char)toupper((unsigned char)*s);
        s++;
    }
}

int str_count(const char *s) {
    if (!s) return 0;
    int count = 0;
    /* Explicit null terminator check for better readability */
    while (s[count] != '\0') {
        count++;
    }
    return count;
}

void str_reverse(char *s) {
    if (!s) return;
    
    /* Optimization: Inline counting to avoid overhead of duplicate function calls */
    int len = 0;
    const char *p = s;
    while (*p++) {
        len++;
    }
    
    /* Swap characters from both ends moving towards the center */
    for (int i = 0; i < len / 2; i++) {
        char temp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = temp;
    }
}