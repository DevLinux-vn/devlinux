#include <stdio.h>
#include <string.h>
#include "stringutils.h"

int main(int argc, char *argv[])
{
    /* We need a string argument from the command line */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    /* str_to_upper and str_reverse modify the string in-place, so we work on
     * separate copies to keep each function's result independent/clear. */
    char upper_buf[256];
    char rev_buf[256];

    snprintf(upper_buf, sizeof(upper_buf), "%s", argv[1]);
    snprintf(rev_buf,   sizeof(rev_buf),   "%s", argv[1]);

    printf("Original string : %s\n", argv[1]);

    /* 1) count characters */
    printf("Character count : %d\n", str_count(argv[1]));

    /* 2) uppercase */
    str_to_upper(upper_buf);
    printf("Uppercase       : %s\n", upper_buf);

    /* 3) reverse */
    str_reverse(rev_buf);
    printf("Reversed        : %s\n", rev_buf);

    return 0;
}
