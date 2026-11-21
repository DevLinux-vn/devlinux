/**
 * @file hw_array_of_pointers.c
 * @brief Error message lookup table using an array of constant string pointers.
 * @details Implements a memory-efficient string table lookup with bounds checking
 *          to prevent out-of-bounds access in firmware.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Enumeration of firmware error codes.
 */
typedef enum {
    ERR_OK = 0,    /**< Operation completed successfully */
    ERR_TIMEOUT,   /**< Operation timed out */
    ERR_HW_FAIL,   /**< Hardware component failure */
    ERR_COUNT      /**< Total number of valid error codes (for bounds checking) */
} error_code_t;

/**
 * @brief Array of pointers to string literals representing error descriptions.
 * @note Stored as array of pointers to save RAM compared to 2D char array padding.
 */
static const char * const p_error_strings[] = {
    [ERR_OK]      = "OK",
    [ERR_TIMEOUT] = "TIMEOUT_ERROR",
    [ERR_HW_FAIL] = "HARDWARE_FAILURE"
};

/**
 * @brief Retrieve the human-readable string for an error code.
 * @details Performs bounds-checking against ERR_COUNT to ensure memory safety.
 *
 * @param[in] err_code Error code identifier to look up.
 * @return const char* Pointer to the error description string, or "UNKNOWN_ERROR" if out of bounds.
 */
static const char *get_error_string(uint8_t err_code) {
    if (err_code >= (uint8_t)ERR_COUNT) {
        return "UNKNOWN_ERROR";
    }
    return p_error_strings[err_code];
}

/**
 * @brief Main function testing the error message lookup table.
 *
 * @return int 0 upon successful execution.
 */
int main(void) {
    printf("Error code %u: %s\n", (unsigned int)ERR_TIMEOUT, get_error_string((uint8_t)ERR_TIMEOUT));
    printf("Error code 99: %s\n", get_error_string(99U));
    return 0;
}
