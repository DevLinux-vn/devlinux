#include "stringutils.h"
#include <ctype.h>

void str_to_upper(char *s)
{
    while (*s)
    {
        *s = toupper((unsigned char)*s);
        s++;
    }
}

int str_count(const char *s)
{
    int count = 0;

    while (*s)
    {
        count++;
        s++;
    }

    return count;
}

void str_reverse(char *s)
{
    int length = str_count(s);

    for (int i = 0; i < length / 2; i++)
    {
        char temp = s[i];
        s[i] = s[length - 1 - i];
        s[length - 1 - i] = temp;
    }
}