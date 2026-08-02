#include <ctype.h>
#include <string.h>

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
    int left = 0;
    int right = strlen(s) - 1;

    while (left < right)
    {
        char temp = s[left];
        s[left] = s[right];
        s[right] = temp;

        left++;
        right--;
    }
}