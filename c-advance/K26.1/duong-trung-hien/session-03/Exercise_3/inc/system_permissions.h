#ifndef SYSTEM_PERMISSIONS_H
#define SYSTEM_PERMISSIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief System permission bitmask flags.
 */
typedef enum {
    PERM_READ    = (1 << 0),  // 0001
    PERM_WRITE   = (1 << 1),  // 0010
    PERM_EXECUTE = (1 << 2),  // 0100
    PERM_DELETE  = (1 << 3)   // 1000
} sys_perms_e;

/**
 * @brief Checks whether a user has all required permissions.
 * @param user_perms      Bitmask containing the user's permissions.
 * @param required_perms  Bitmask containing the required permissions.
 *
 * @retval true  The user has all required permissions.
 * @retval false One or more required permissions are missing.
 */
bool has_permission(uint8_t user_perms, uint8_t required_perms);

/**
 * @brief Converts a permission bitmask to a human-readable string.
 * @param required_perms Permission bitmask to convert.
 *
 * @return Pointer to a constant null-terminated string describing the permission.
 */
const char *permission_to_string(uint8_t required_perms);

/**
 * @brief Checks a permission and prints the result.
 * @param user_perms      Bitmask containing the user's permissions.
 * @param required_perms  Bitmask containing the required permissions.
 *
 * @return none.
 */
void result_check(uint8_t user_perms, uint8_t required_perms);

#endif