#include <stdio.h>
#include "stringutils.h"

#define MAX_LEN 256

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }
    
    char buf[MAX_LEN];
    snprintf(buf, sizeof(buf), "%s", argv[1]);

    printf("Original: %s\n", argv[1]);
    printf("Count: %d\n", str_count(buf));

    str_to_upper(buf);
    printf("Upper: %s\n", buf);

    str_reverse(buf);
    printf("Reverse: %s\n", buf);

    return 0;
}
