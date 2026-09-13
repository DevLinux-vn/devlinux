#include <stdio.h>
#include "stringutils.h"

/* Convert a string to uppercase in-place using ASCII manipulation
 * ASCII codes: 'a' = 97, 'z' = 122, 'A' = 65
 * Difference between lowercase and uppercase = 32
 */
void str_to_upper(char *s) {
    if (s == NULL) return;
    
    for (int i = 0; s[i] != '\0'; i++) {
        /* Check if character is lowercase letter (ASCII 97-122) */
        if (s[i] >= 97 && s[i] <= 122) {
            /* Convert to uppercase by subtracting 32 */
            s[i] = s[i] - 32;
        }
    }
}

/* Count the number of characters in a string (excluding null terminator)
 * Manually iterate until null terminator '\0' (ASCII 0)
 */
int str_count(const char *s) {
    if (s == NULL) return 0;
    
    int count = 0;
    while (s[count] != '\0') {
        count++;
    }
    return count;
}

/* Reverse a string in-place using character swapping
 * First calculate length, then swap from both ends
 */
void str_reverse(char *s) {
    if (s == NULL) return;
    
    /* Calculate string length by counting until null terminator */
    int length = 0;
    while (s[length] != '\0') {
        length++;
    }
    
    /* Swap characters from both ends moving towards center */
    for (int i = 0; i < length / 2; i++) {
        /* Use temporary variable to swap */
        char temp = s[i];
        s[i] = s[length - 1 - i];
        s[length - 1 - i] = temp;
    }
}
