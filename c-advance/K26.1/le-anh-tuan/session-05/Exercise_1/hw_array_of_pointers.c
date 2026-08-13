/**
 * @file hw_array_of_pointers.c
 * @brief Error message lookup table using an array of string pointers.
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Error codes supported by the system.
 */
typedef enum
{
    ERR_OK = 0,
    ERR_TIMEOUT,
    ERR_HW_FAIL,
    ERR_COUNT
} error_code_t;

/**
 * @brief Lookup table mapping error codes to human-readable messages.
 *
 * The first const means the characters cannot be modified.
 * The second const means the pointers stored in the array cannot be changed.
 */
static const char * const p_error_strings[] =
{
    [ERR_OK] = "OK",
    [ERR_TIMEOUT] = "TIMEOUT_ERROR",
    [ERR_HW_FAIL] = "HARDWARE_FAILURE"
};

/**
 * @brief Converts an error code into a human-readable string.
 *
 * @param err_code Error code to convert.
 *
 * @return Pointer to the matching error string.
 * @return "UNKNOWN_ERROR" if err_code is outside the valid range.
 */
const char *get_error_string(uint8_t err_code)
{
    const char *p_result = "UNKNOWN_ERROR";

    if (err_code < (uint8_t)ERR_COUNT)
    {
        p_result = p_error_strings[err_code];
    }

    return p_result;
}

/**
 * @brief Program entry point.
 *
 * @return 0 on successful execution.
 */
int main(void)
{
    printf("Error code 1: %s\n", get_error_string((uint8_t)ERR_TIMEOUT));
    printf("Error code 99: %s\n", get_error_string(99U));

    return 0;
}