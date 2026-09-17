#include "../include/stringutils.h"

void str_to_upper(char *s)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i] >= 'a' && s[i] <= 'z')
        {
            s[i] = s[i] - 32;
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
    int string_count = str_count(s);
    char *start = s;
    char *end = s + string_count - 1;
    char temp;

    while (start < end)
    {
        temp = *start;
        *start = *end;
        *end = temp;

        start++;
        end--;
    }
}