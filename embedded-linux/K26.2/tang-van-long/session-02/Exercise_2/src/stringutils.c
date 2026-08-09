#include "stringutils.h"

void str_to_upper(char *s)
{
    while (*s != '\0')
    {
        if (*s >= 'a' && *s <= 'z')
        {
            *s = *s - ('a' - 'A');
        }
        s++;
    }
}

int str_count(const char *s)
{
    int count = 0;

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
    int right = str_count(s) - 1;

    while (left < right)
    {
        char temp = s[left];
        s[left] = s[right];
        s[right] = temp;

        left++;
        right--;
    }
}