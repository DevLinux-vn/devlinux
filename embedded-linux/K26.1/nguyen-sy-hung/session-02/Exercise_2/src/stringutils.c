#include "stringutils.h"
#include <ctype.h>

void str_to_upper(char *s)
{
    int i = 0;

    while (s[i] != '\0')
    {
        s[i] = toupper(s[i]);
        i++;
    }
}

int str_count(const char *s)
{
    int count = 0;

    while (s[count] != '\0')
    {
        count++;
    }

    return count;
}

void str_reverse(char *s)
{
    int left = 0;
    int right = str_count(s) - 1;
    char temp;

    while (left < right)
    {
        temp = s[left];
        s[left] = s[right];
        s[right] = temp;

        left++;
        right--;
    }
}