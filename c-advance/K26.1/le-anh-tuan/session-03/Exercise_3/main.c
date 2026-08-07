/**
 * @file main.c
 * @brief Bitmask permissions tester using enum flags.
 *
 * Enum Size Pitfall:
 *
 * The size of an enum is implementation-defined in C. On a normal GCC build
 * for an x86 PC, an enum is commonly stored as an int, so sizeof(enum) is often
 * 4 bytes. On an ARM Cortex-M micro-controller, the compiler or build settings
 * may choose a smaller enum representation if all values fit in fewer bytes.
 *
 * The compiler flag -fshort-enums tells GCC to use the smallest integer type
 * that can represent all enum values. For this enum, the values only need one
 * byte, so the enum size may become 1 byte instead of 4 bytes.
 *
 * This is dangerous when linking libraries because enum size affects ABI
 * compatibility. If one object file or library is compiled with -fshort-enums
 * and another is not, function parameters, structs, and binary layouts that
 * contain enums may no longer match. This can cause corrupted data, incorrect
 * function calls, or hard-to-debug runtime failures.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define PERMISSION_SHIFT_READ      (0U)
#define PERMISSION_SHIFT_WRITE     (1U)
#define PERMISSION_SHIFT_EXECUTE   (2U)
#define PERMISSION_SHIFT_DELETE    (3U)

#define USER_ONE_PERMISSIONS       ((uint8_t)(PERM_READ | PERM_WRITE))
#define USER_TWO_PERMISSIONS       ((uint8_t)(PERM_READ | PERM_EXECUTE))
#define USER_THREE_PERMISSIONS     ((uint8_t)(PERM_READ | PERM_WRITE | PERM_DELETE))

/**
 * @brief System permission bit flags.
 */
typedef enum
{
    PERM_READ    = (1U << PERMISSION_SHIFT_READ),
    PERM_WRITE   = (1U << PERMISSION_SHIFT_WRITE),
    PERM_EXECUTE = (1U << PERMISSION_SHIFT_EXECUTE),
    PERM_DELETE  = (1U << PERMISSION_SHIFT_DELETE)
} sys_perms_e;

/**
 * @brief Checks whether all required permissions are present.
 *
 * @param user_perms Permissions owned by the user.
 * @param required_perms Permissions required by the operation.
 *
 * @return true if all required permissions are present.
 * @return false if at least one required permission is missing.
 */
static bool has_permission(uint8_t user_perms, uint8_t required_perms);

/**
 * @brief Prints the result of a permission check.
 *
 * @param p_label Text describing the permission check.
 * @param user_perms Permissions owned by the user.
 * @param required_perms Permissions required by the operation.
 */
static void print_permission_check(const char *p_label,
                                   uint8_t user_perms,
                                   uint8_t required_perms);

static bool has_permission(uint8_t user_perms, uint8_t required_perms)
{
    bool result = false;

    if ((user_perms & required_perms) == required_perms)
    {
        result = true;
    }

    return result;
}

static void print_permission_check(const char *p_label,
                                   uint8_t user_perms,
                                   uint8_t required_perms)
{
    const bool is_granted = has_permission(user_perms, required_perms);

    printf("%s %s\n", p_label, (is_granted == true) ? "GRANTED" : "DENIED");
}

int main(void)
{
    const uint8_t user_one_perms = USER_ONE_PERMISSIONS;
    const uint8_t user_two_perms = USER_TWO_PERMISSIONS;
    const uint8_t user_three_perms = USER_THREE_PERMISSIONS;

    printf("=== Bitmask Permissions Tester ===\n");
    printf("Enum size: %zu bytes\n", sizeof(sys_perms_e));

    printf("User 1 (Read|Write): 0x%02X\n", (unsigned int)user_one_perms);
    print_permission_check("Checking for Read permission...",
                           user_one_perms,
                           (uint8_t)PERM_READ);
    print_permission_check("Checking for Execute permission...",
                           user_one_perms,
                           (uint8_t)PERM_EXECUTE);
    print_permission_check("Checking for Read AND Write...",
                           user_one_perms,
                           (uint8_t)(PERM_READ | PERM_WRITE));

    printf("\nUser 2 (Read|Execute): 0x%02X\n", (unsigned int)user_two_perms);
    print_permission_check("Checking for Execute permission...",
                           user_two_perms,
                           (uint8_t)PERM_EXECUTE);
    print_permission_check("Checking for Write permission...",
                           user_two_perms,
                           (uint8_t)PERM_WRITE);
    print_permission_check("Checking for Read AND Execute...",
                           user_two_perms,
                           (uint8_t)(PERM_READ | PERM_EXECUTE));

    printf("\nUser 3 (Read|Write|Delete): 0x%02X\n", (unsigned int)user_three_perms);
    print_permission_check("Checking for Delete permission...",
                           user_three_perms,
                           (uint8_t)PERM_DELETE);
    print_permission_check("Checking for Execute permission...",
                           user_three_perms,
                           (uint8_t)PERM_EXECUTE);
    print_permission_check("Checking for Write AND Delete...",
                           user_three_perms,
                           (uint8_t)(PERM_WRITE | PERM_DELETE));

    return 0;
}