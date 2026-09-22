#include "stringutils.h"
#include <stdio.h>
#include <ctype.h>

void str_to_upper(char *s)
{
    while (*s != '\0')
    {
        *s = toupper(*s);
        s++;
    }
}
int  str_count(const char *s)
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
    char temp;
    int length = str_count(s);
    for (int i = 0; i < (length / 2); i++)
    {
        temp = s[i];
        s[i] = s[length - 1 - i];
        s[length - 1 - i] = temp;
    }
}