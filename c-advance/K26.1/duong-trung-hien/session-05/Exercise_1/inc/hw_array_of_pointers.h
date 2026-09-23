#ifndef HW_ARRAY_OF_POINTERS_H
#define HW_ARRAY_OF_POINTERS_H

#include <stdint.h>

/**
 * @brief Error codes used by the firmware.
 */
typedef enum
{
    ERR_OK = 0,      /**< No error occurred. */
    ERR_TIMEOUT,     /**< Operation timed out. */
    ERR_HW_FAIL,     /**< Hardware failure detected. */
    ERR_COUNT        /**< Total number of valid error codes. */
} error_code_t;

/**
 * @brief Lookup table that maps error codes to error message strings.
 *
 * Each index in this table corresponds directly to the matching
 * value in the @ref error_code_t enumeration.
 */
static const char * const p_error_strings[] =
{
    [ERR_OK]      = "OK",
    [ERR_TIMEOUT] = "TIMEOUT_ERROR",
    [ERR_HW_FAIL] = "HARDWARE_FAILURE"
};

/**
 * @brief Returns the string representation of an error code.
 * @param err_code Error code to convert.
 *
 * @return Pointer to a constant null-terminated error string.
 *         Returns "UNKNOWN_ERROR" if the error code is invalid.
 */
const char *get_error_string(uint8_t err_code);

#endif /* HW_ARRAY_OF_POINTERS_H */