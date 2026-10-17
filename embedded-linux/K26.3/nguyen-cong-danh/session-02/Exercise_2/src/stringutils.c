#include <ctype.h>
#include "stringutils.h"

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
    int len = str_count(s);

    for (int i = 0; i < len / 2; i++)
    {
        char tmp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = tmp;
    }
}