#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main()
{
    char s1[20];
    char s2[20];
    char s3[20];
    printf("Please enter your string with maximum 19 chars: ");
    fgets(s1, sizeof(s1), stdin);
    s1[strcspn(s1, "\n")] = '\0';
    strcpy(s2, s1);
    strcpy(s3, s1);

    printf("Running on same string %s\n", s1);
    str_to_upper(s1);
    str_reverse(s3);
    printf("Result of str_to_upper() is: %s\n", s1);
    printf("Result of str_count() is: %d\n", str_count(s2));
    printf("Result of str_reverse() is: %s\n", s3);
    return 0;
}