#include "stringutils.h"

#include <ctype.h>

void str_to_upper(char *s)
{
    if (s == 0)
    {
        return;
    }

    while (*s != '\0')
    {
        *s = (char)toupper((unsigned char)*s);
        s++;
    }
}

int str_count(const char *s)
{
    int count = 0;

    if (s == 0)
    {
        return 0;
    }

    while (*s != '\0')
    {
        count++;
        s++;
    }

    return count;
}

void str_reverse(char *s)
{
    int left = 0;
    int right;

    if (s == 0)
    {
        return;
    }

    right = str_count(s) - 1;

    while (left < right)
    {
        char temp = s[left];
        s[left] = s[right];
        s[right] = temp;
        left++;
        right--;
    }
}
