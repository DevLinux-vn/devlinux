#include "stringutils.h"

void str_to_upper(char *s)
{
    int i = 0;

    while (s[i] != '\0')
    {
        if (s[i] >= 'a' && s[i] <= 'z')
        {
            s[i] = s[i] - 'a' + 'A';
        }

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
    int start = 0;
    int end = str_count(s) - 1;

    while (start < end)
    {
        char temp = s[start];
        s[start] = s[end];
        s[end] = temp;

        start++;
        end--;
    }
}
