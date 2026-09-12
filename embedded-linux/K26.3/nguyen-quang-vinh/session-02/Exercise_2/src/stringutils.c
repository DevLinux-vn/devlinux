#include <stdio.h>
#include "stringutils.h"
#include <string.h>

void str_to_upper(char *s){
    for(int i = 0 ; s[i] != '\0'; i++){
        if(s[i] >= 'a' && s[i] <= 'z'){
            s[i] -= 32;
        }
    }
}
int str_count(const char *s){
    return strlen(s);
}

void str_reverse(char *s)
{
    if (s == NULL) return;

    int i = 0;
    int j = strlen(s);
    if (j == 0) return;
    j -=1 ;
    while (i < j) {
        char temp = s[i];
        s[i] = s[j];
        s[j] = temp;

        i++;
        j--;
    }
}