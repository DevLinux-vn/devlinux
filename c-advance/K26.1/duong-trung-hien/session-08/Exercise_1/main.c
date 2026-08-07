#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define APP_SUCCESS (0)

/**
 * @brief Entry point of the Secure I/O demonstration program.
 *
 * This function prompts the user to enter an administrator password,
 * safely reads the input using fgets(), removes the trailing newline
 * character if present, and compares the password against the expected
 * administrator password.
 *
 * If the password matches, administrator access is granted; otherwise,
 * access is denied.
 *
 * @return APP_SUCCESS if the program completes successfully.
 */
int32_t main(void) {
    printf("=== Exercise 1: Secure I/O ===\n");
    bool is_admin = false;
    char password[8];

    printf("Enter admin password: ");
    
    if (fgets(password, sizeof(password), stdin) != NULL) {
        size_t len = strlen(password);

        if ((len > 0) && (password[len - 1] == '\n')) {
            password[len - 1] = '\0';
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

    return APP_SUCCESS;
}