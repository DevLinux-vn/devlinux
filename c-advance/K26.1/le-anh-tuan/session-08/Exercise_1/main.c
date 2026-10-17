/**
 * @file main.c
 * @brief Demonstrates secure, bounded password input using fgets().
 *
 * Attack payload used against the vulnerable version:
 * AAAAAAAAAAAA
 *
 * Note:
 * The exact payload length required to corrupt is_admin depends on the
 * compiler, architecture, optimization settings, and stack layout.
 * The payload must therefore be verified with GDB on the target build.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define PASSWORD_BUFFER_SIZE (8U)

/**
 * @brief Reads a password safely from standard input.
 *
 * This function uses fgets() to ensure that no more than the supplied buffer
 * capacity can be written. A trailing newline is removed before the password
 * is compared. If the user enters more characters than the buffer can hold,
 * the remaining characters are discarded and the input is rejected.
 *
 * @param password Destination buffer used to store the password.
 * @param capacity Total size of the destination buffer in bytes.
 *
 * @return true if a complete input line was read successfully.
 * @return false if input failed or the input line was too long.
 */
static bool read_password(char *password, size_t capacity)
{
    size_t length;
    int ch;

    if ((password == NULL) || (capacity < 2U))
    {
        return false;
    }

    if (fgets(password, (int)capacity, stdin) == NULL)
    {
        return false;
    }

    length = strlen(password);

    if ((length > 0U) && (password[length - 1U] == '\n'))
    {
        password[length - 1U] = '\0';
        return true;
    }

    /*
     * If EOF follows immediately, the input may still be a valid complete
     * line even though no newline character was present.
     */
    if (feof(stdin) != 0)
    {
        return true;
    }

    /*
     * No newline was stored and EOF was not reached, so the input exceeded
     * the available password buffer. Discard the remainder safely.
     */
    do
    {
        ch = getchar();
    } while ((ch != '\n') && (ch != EOF));

    password[0] = '\0';

    return false;
}

/**
 * @brief Application entry point.
 *
 * Reads an administrator password using bounded input and grants administrator
 * access only when the password exactly matches the expected string.
 *
 * @return 0 on normal program termination.
 */
int main(void)
{
    bool is_admin = false;
    char password[PASSWORD_BUFFER_SIZE] = {0};

    (void)printf("=== Exercise 1: Secure I/O ===\n");
    (void)printf("Enter admin password: ");

    if (read_password(password, sizeof(password)))
    {
        if (strcmp(password, "secret") == 0)
        {
            is_admin = true;
        }
    }

    if (is_admin)
    {
        (void)printf("Access Granted: Admin privileges unlocked.\n");
    }
    else
    {
        (void)printf("Access Denied.\n");
    }

    return 0;
}