#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define PASSWORD_MAX_LEN    (8U)

/**
 * @brief Checks whether a password length is within the accepted range.
 *
 * A valid length is greater than zero and less than PASSWORD_MAX_LEN.
 *
 * @param len Password length in bytes, excluding the terminating null byte.
 * @return true if len is in the accepted range; otherwise, false.
 */
static inline bool is_pw_len_in_range(const size_t len)
{
    return ((len > 0) && (len < PASSWORD_MAX_LEN));
}

/**
 * @brief Runs the administrator password check.
 *
 * Reads a password with fgets() unless VULNERABLE is defined, compares the
 * resulting string with "secret", and prints whether access is granted.
 * When VULNERABLE is defined, the fgets()-based input path is omitted.
 *
 * @return 0 when the program completes normally.
 */
int main(void)
{
    bool is_admin = false;
    char password[PASSWORD_MAX_LEN] = { 0 };

    printf("Enter admin password: ");

#if defined(VULNERABLE)
    // gets(password); /* ⚠️ DANGER: Unbounded read! */ Please uncomment and run 'make vulnerable' to build with it
#else
    /* Safe string input pattern */
    /* It's not safe to use gets(password) because it may cause buffer overflow, should use fgets() */
    if (NULL != fgets(password, sizeof(password), stdin))
    {
        size_t len = strlen(password);
        if ((true == is_pw_len_in_range(len)) && ('\n' == password[len - 1U]))  // NOLINT(readability-magic-numbers)
        {
            password[len - 1U] = '\0';  // NOLINT(readability-magic-numbers)
        }
    }
#endif

    if (strcmp(password, "secret") == 0)
    {
        is_admin = true;
    }

    if (is_admin)
    {
        printf("Access Granted: Admin privileges unlocked.\n");
    }
    else
    {
        printf("Access Denied.\n");
    }

    return 0;
}