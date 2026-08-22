#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
 * gets() was removed from the C standard because it is inherently unsafe.
 * This declaration is used only to reproduce the vulnerable legacy program
 * required by the exercise.
 */
extern char *gets(char *str);

int main(void)
{
    bool is_admin = false;
    char password[8];

    printf("Enter admin password: ");
    gets(password);

    if (strcmp(password, "secret") == 0)
    {
        is_admin = true;
    }

    if (is_admin)
    {
        printf("Access Granted: Admin privileges unlocked.\n");
    }
    else
    {
        printf("Access Denied.\n");
    }

    return 0;
}