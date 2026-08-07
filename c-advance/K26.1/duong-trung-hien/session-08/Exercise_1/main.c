#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define APP_SUCCESS (0)

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