#include <ctype.h>
#include <stddef.h>
#include "stringutils.h"

void str_to_upper(char *s)
{
    if (s == NULL)
        return;

    for (int i = 0; s[i] != '\0'; i++)
    {
        s[i] = toupper((unsigned char)s[i]);
    }
}

int str_count(const char *s)
{
    if (s == NULL)
        return 0;

    int count = 0;

    while (s[count] != '\0')
    {
        count++;
    }

    return count;
}

void str_reverse(char *s)
{
    if (s == NULL)
        return;

    int left = 0;
    int right = str_count(s) - 1;

    while (left < right)
    {
        char tmp = s[left];
        s[left] = s[right];
        s[right] = tmp;

        left++;
        right--;
    }
}
