/*
 * Attack payload used against vulnerable version: AAAAAAAAAAAAA (13 chars)
 * Overflowed password[8], corrupting adjacent is_admin on the stack,
 * causing unauthorized "Access Granted" without knowing the password.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief  read password check password true ? 
 * @return 0 if successful
 */

int main(void) {
    bool is_admin = false;
    char password[8];
    printf("=== Exercise 1: Secure I/O ===\n");
    printf("Enter admin password: ");
    /* gets(password);  ⚠️ DANGER: Unbounded read! */
    /* Safe string input pattern */
    if (fgets(password, sizeof(password), stdin) != NULL) {
        /* Strip the trailing newline left by fgets */
        size_t len = strlen(password);
        if ((len > 0) && (password[len - 1] == '\n')) {
            password[len - 1] = '\0';
        }
    }
    else{
        printf("Error reading input.\n");
        return 1;
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