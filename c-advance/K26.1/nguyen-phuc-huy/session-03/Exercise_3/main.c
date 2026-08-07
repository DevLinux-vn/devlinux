

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Application success return code.
 */
#define APP_SUCCESS (0)

/**
 * @brief System permission bit flags.
 */
typedef enum
{
    PERM_READ    = (1U << 0U),
    PERM_WRITE   = (1U << 1U),
    PERM_EXECUTE = (1U << 2U),
    PERM_DELETE  = (1U << 3U)
} sys_perms_e;

/**
 * @brief Checks whether all required permissions are present.
 *
 * @param user_perms User permission bitmask.
 * @param required_perms Required permission bitmask.
 *
 * @return true if all required permissions are present, otherwise false.
 */
static bool has_permission(uint8_t user_perms, uint8_t required_perms)
{
    bool result = false;
    uint8_t matched_perms = 0U;

    matched_perms = (uint8_t)(user_perms & required_perms);

    if (matched_perms == required_perms)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

/**
 * @brief Prints permission check result.
 *
 * @param message Description of the permission being checked.
 * @param user_perms User permission bitmask.
 * @param required_perms Required permission bitmask.
 */
static void print_permission_result(const char * const message,
                                    uint8_t user_perms,
                                    uint8_t required_perms)
{
    (void)printf("%s... ", message);

    if (has_permission(user_perms, required_perms) == true)
    {
        (void)printf("GRANTED\n");
    }
    else
    {
        (void)printf("DENIED\n");
    }
}



/*Comment output 
=== Bitmask Permissions Tester ===
Enum size: 4 bytes
User 1 (Read|Write): 0x03
Checking for Read permission... GRANTED
Checking for Execute permission... DENIED
Checking for Read AND Write... GRANTED
User 2 (Execute|Delete): 0x0C
Checking for Delete permission... GRANTED
Checking for Write permission... DENIED*/
/**
 * @brief Program entry point.
 *
 * @return APP_SUCCESS when the program completes successfully.
 */
int main(void)
{
    uint8_t user_1_perms = 0U;
    uint8_t user_2_perms = 0U;

    user_1_perms = (uint8_t)((uint8_t)PERM_READ | (uint8_t)PERM_WRITE);
    user_2_perms = (uint8_t)((uint8_t)PERM_EXECUTE | (uint8_t)PERM_DELETE);

    (void)printf("=== Bitmask Permissions Tester ===\n");
    (void)printf("Enum size: %zu bytes\n", sizeof(sys_perms_e));

    (void)printf("User 1 (Read|Write): 0x%02X\n", (unsigned int)user_1_perms);
    print_permission_result("Checking for Read permission",
                            user_1_perms,
                            (uint8_t)PERM_READ);

    print_permission_result("Checking for Execute permission",
                            user_1_perms,
                            (uint8_t)PERM_EXECUTE);

    print_permission_result("Checking for Read AND Write",
                            user_1_perms,
                            (uint8_t)((uint8_t)PERM_READ | (uint8_t)PERM_WRITE));

    (void)printf("User 2 (Execute|Delete): 0x%02X\n", (unsigned int)user_2_perms);
    print_permission_result("Checking for Delete permission",
                            user_2_perms,
                            (uint8_t)PERM_DELETE);

    print_permission_result("Checking for Write permission",
                            user_2_perms,
                            (uint8_t)PERM_WRITE);

    return APP_SUCCESS;
}
