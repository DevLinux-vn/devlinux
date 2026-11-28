/**
 * @file main.c
 * @brief Secure Standard I/O — Buffer Overflow Patch using fgets().
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Main entry point performing bounded password authentication.
 * @return 0 on success.
 */
int main(void)
{
    bool is_admin = false;
    char password[8] = { 0 };

    printf("=== Exercise 1: Secure I/O ===\n");
    printf("Enter admin password: ");

    /* Safe string input pattern: bounded read prevents buffer overflow */
    if (fgets(password, (int)sizeof(password), stdin) != NULL) {
        /* Strip the trailing newline left by fgets */
        size_t len = strlen(password);
        if ((len > 0U) && (password[len - 1U] == '\n')) {
            password[len - 1U] = '\0';
        }
    }

    if (strcmp(password, "secret") == 0) {
        is_admin = true;
    }

    if (is_admin) {
        printf("Access Granted: Admin privileges unlocked.\n");
    } else {
        printf("Access Denied.\n");
    }

    return 0;
}
