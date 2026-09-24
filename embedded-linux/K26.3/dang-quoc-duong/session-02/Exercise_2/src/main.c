#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    char upper[1024];
    char reversed[1024];
    int count = 0;

    if(argc < 2){
        printf("Usage: %s <string> \n", argv[0]);
        return 1;
    }

    strncpy(upper, argv[1], sizeof(upper) -1);
    upper[sizeof(upper) - 1] = '\0';
    
    strncpy(reversed, argv[1], sizeof(reversed) - 1);
    reversed[sizeof(reversed) - 1] = '\0';

    count = str_count(argv[1]);

    str_to_upper(upper);
    str_reverse(reversed);

    printf("OG: %s\n", argv[1]);
    printf("UpperCase: %s\n", upper);
    printf("Count: %d\n", count);
    printf("Reversed: %s\n", reversed);

    return 0;
}