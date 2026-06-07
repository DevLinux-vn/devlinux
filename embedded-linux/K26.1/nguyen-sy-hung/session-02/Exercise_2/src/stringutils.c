#include "stringutils.h"

#include <ctype.h>

void to_uppercase(char *str)
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
}

int string_length(const char *str)
{
    int len = 0;

    while (*str++)
    {
        len++;
    }

    return len;
}

void reverse_string(char *str)
{
    int len = string_length(str);

    int left = 0;
    int right = len - 1;

    while (left < right)
    {
        char temp = str[left];

        str[left] = str[right];
        str[right] = temp;

        left++;
        right--;
    }
}