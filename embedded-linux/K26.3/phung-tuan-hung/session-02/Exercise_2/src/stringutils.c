#include "stringutils.h"
#include <ctype.h>
#include <stddef.h>   /* for NULL */

/* Convert a string to uppercase in-place */
void str_to_upper(char *s)
{
    if (s == NULL) {
        return;
    }
    for (; *s != '\0'; ++s) {
        *s = (char)toupper((unsigned char)*s);
    }
}

/* Count the number of characters in a string (excluding null terminator).
 * Implemented manually instead of using strlen() to show the logic. */
int str_count(const char *s)
{
    int count = 0;
    if (s == NULL) {
        return 0;
    }
    while (s[count] != '\0') {
        ++count;
    }
    return count;
}

/* Reverse a string in-place using two-pointer swap */
void str_reverse(char *s)
{
    int i, j;
    char tmp;

    if (s == NULL) {
        return;
    }

    i = 0;
    j = str_count(s) - 1;
    while (i < j) {
        tmp  = s[i];
        s[i] = s[j];
        s[j] = tmp;
        ++i;
        --j;
    }
}
