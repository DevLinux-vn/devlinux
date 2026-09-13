#include stdio.h
#include stdlib.h
#include string.h

#include stringutils.h

int main(int argc, char argv[])
{
    if (argc  2)
    {
        fprintf(stderr, Usage %s stringn, argv[0]);
        return 1;
    }

    const char input = argv[1];

    char upper = malloc(strlen(input) + 1);
    char reverse = malloc(strlen(input) + 1);

    if (upper == NULL  reverse == NULL)
    {
        fprintf(stderr, Memory allocation failedn);

        free(upper);
        free(reverse);

        return 1;
    }

    strcpy(upper, input);
    strcpy(reverse, input);

    str_to_upper(upper);

    printf(Original %sn, input);
    printf(Uppercase %sn, upper);
    printf(Count %dn, str_count(input));

    str_reverse(reverse);
    printf(Reverse %sn, reverse);

    free(upper);
    free(reverse);

    return 0;
}