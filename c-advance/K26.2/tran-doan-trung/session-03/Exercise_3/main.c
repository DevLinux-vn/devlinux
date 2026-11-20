#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PERM_READ    = (1 << 0),  // 0001
    PERM_WRITE   = (1 << 1),  // 0010
    PERM_EXECUTE = (1 << 2),  // 0100
    PERM_DELETE  = (1 << 3)   // 1000
} sys_perms_e;

/**
 * @brief Checks if the user has all the required permissions.
 */
static bool has_permission(uint8_t user_perms, uint8_t required_perms) {
    return ((uint8_t)(user_perms & required_perms) == required_perms);
}

int main(void) {
    printf("=== Bitmask Permission Tester ===\n");
    printf("Enum size: %zu byte (Standard GCC)\n", sizeof(sys_perms_e));

    volatile uint8_t user1_perms = (uint8_t)(PERM_READ | PERM_WRITE);
    printf("User 1 (Read|Write): 0x%02X\n", user1_perms);

    printf("Checking for Read permission... ");
    if (has_permission(user1_perms, (uint8_t)PERM_READ)) {
        printf("GRANTED\n");
    } else {
        printf("DENIED\n");
    }

    printf("Checking for Execute permission... ");
    if (has_permission(user1_perms, (uint8_t)PERM_EXECUTE)) {
        printf("GRANTED\n");
    } else {
        printf("DENIED\n");
    }

    printf("Checking for Read AND Write permissions... ");
    if (has_permission(user1_perms, (uint8_t)(PERM_READ | PERM_WRITE))) {
        printf("GRANTED\n");
    } else {
        printf("DENIED\n");
    }
}