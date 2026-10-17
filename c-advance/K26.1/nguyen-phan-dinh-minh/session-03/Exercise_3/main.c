#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PERM_READ    = (1 << 0),
    PERM_WRITE   = (1 << 1),
    PERM_EXECUTE = (1 << 2), 
    PERM_DELETE  = (1 << 3)
} sys_perms_e;
/**
 * @brief Checks whether the user has all required permissions.
 *
 * @param user_perms Permissions currently assigned to the user.
 * @param required_perms Permissions required for the operation.
 * @return true if all required permissions are present, otherwise false.
 */
bool has_permission(uint8_t user_perms, uint8_t required_perms)
{
    if ((user_perms & required_perms) == required_perms)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main(void)
{
    printf("=== Bitmask Permissions Tester ===\n");
    printf("Enum size: %zu bytes (Standard GCC)\n", sizeof(sys_perms_e));

    uint8_t User_1 = PERM_READ | PERM_WRITE;
    printf("User 1 (Read|Write): 0x%02x\n", User_1);

    if (has_permission(User_1, PERM_READ))
    {
        printf("Checking for Read permission... GRANTED\n");
    }
    else 
    {
        printf("Checking for Read permission... DENIED\n");
    }

    if (has_permission(User_1, PERM_EXECUTE))
    {
        printf("Checking for Execute permission... GRANTED\n");
    }
    else
    {
        printf("Checking for Execute permission... DENIED\n");
    }

    if (has_permission(User_1, PERM_READ | PERM_WRITE))
    {
        printf("Checking for Read AND Write... GRANTED\n");
    }
    else
    {
        printf("Checking for Read AND Write... DENIED\n");
    }

    return 0;
    
}
