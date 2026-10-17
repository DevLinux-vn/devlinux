#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
/**
 * @brief System permission bit-flags.
 *
 * Each member occupies a single, non-overlapping bit so that multiple
 * permissions can be combined using the bitwise OR (|) operator and
 * verified using the bitwise AND (&) operator.
 */
typedef enum {
    PERM_READ    = (1 << 0),  // 0001
    PERM_WRITE   = (1 << 1),  // 0010
    PERM_EXECUTE = (1 << 2),  // 0100
    PERM_DELETE  = (1 << 3)   // 1000
} sys_perms_e;

/**
 * @brief Checks if the user has all the required permissions.
 *
 * Performs a bitwise AND between @p user_perms and @p required_perms and
 * compares the result against @p required_perms. This returns true only
 * when every bit set in @p required_perms is also set in @p user_perms.
 *
 * @param[in] user_perms     Bitmask of permissions the user currently has.
 * @param[in] required_perms Bitmask of permissions being checked for.
 *
 * @return true  if all bits in required_perms are present in user_perms.
 * @return false if at least one required bit is missing.
 */
static bool has_permission(uint8_t user_perms, uint8_t required_perms);
/**
 * @brief Entry point. Demonstrates bitmask permission checking.
 *
 * Builds a sample user permission set, prints it, and runs several
 * has_permission() checks covering both granted and denied cases.
 *
 * @return int Always returns 0 on success.
 */
int main ( void ) {
    printf("=== Bitmask Permissions Tester ===\n");
    printf("Enum size: %zu bytes (Standard GCC)\n", sizeof(sys_perms_e));

    uint8_t user_perms = (uint8_t)(PERM_READ | PERM_WRITE);
    printf("User 1 (Read|Write): 0x%02X\n",(unsigned int)user_perms);
    printf("Checking for Read permission... ");
    // cppcheck-suppress knownConditionTrueFalse -- user_perms is hardcoded for this demo
    if (has_permission(user_perms, PERM_READ))
    {
        printf("GRANTED\n");
    }
    else
    {
        printf("DENIED\n");
    }
    printf("Checking for Write permission... ");
    // cppcheck-suppress knownConditionTrueFalse -- user_perms is hardcoded for this demo
    if (has_permission(user_perms, PERM_WRITE))
    {
        printf("GRANTED\n");
    }
    else
    {
        printf("DENIED\n");
    }
    printf("Checking for Execute permission... ");
    // cppcheck-suppress knownConditionTrueFalse -- user_perms is hardcoded for this demo
    if (has_permission(user_perms, PERM_EXECUTE))
    {
        printf("GRANTED\n");
    }
    else
    {
        printf("DENIED\n");
    }
    printf("Checking for Delete permission... ");
    // cppcheck-suppress knownConditionTrueFalse -- user_perms is hardcoded for this demo
    if (has_permission(user_perms, PERM_DELETE))
    {
        printf("GRANTED\n");
    }
    else
    {
        printf("DENIED\n");
    }
    printf("Checking for Read AND Write... ");
    // cppcheck-suppress knownConditionTrueFalse
    // Justification: user_perms is a hardcoded demo value in main(); in real
    // usage this comes from a runtime/dynamic source, so this warning does
    // not apply.
    if (has_permission(user_perms, (uint8_t)(PERM_READ | PERM_WRITE)))
    {
        printf("GRANTED\n");
    }
    else
    {
        printf("DENIED\n");
    }
    return 0;
}
static bool has_permission(uint8_t user_perms, uint8_t required_perms){

    return ((uint8_t)(user_perms & required_perms) == required_perms);
}