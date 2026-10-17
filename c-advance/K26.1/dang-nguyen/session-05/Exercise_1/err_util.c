#include <stdbool.h>
#include "err_util.h"

/**
 * @brief Error codes supported by the error decoding utility.
 */
typedef enum e_error_code
{
    ERR_OK      = 0,  /**< Operation completed successfully. */
    ERR_TIMEOUT,      /**< Operation timed out. */
    ERR_HW_FAIL,      /**< Hardware failure occurred. */
    ERR_COUNT         /**< Number of supported error codes. Must remain last. */
} e_error_code_t;

/**
 * @brief Validate whether an error code is supported.
 *
 * Checks whether the specified error code is within the valid range
 * defined by @ref e_error_code_t.
 *
 * @param[in] err Error code to validate.
 *
 * @return Validation result.
 * @retval true  The error code is valid.
 * @retval false The error code is outside the supported range.
 */
static bool validate_error_type(const e_error_code_t err);

/**
 * @brief Human-readable strings corresponding to supported error codes.
 *
 * Each element is indexed by a value of @ref e_error_code_t.
 * The array must be updated whenever a new error code is added.
 */
static const char *const p_error_strings[ERR_COUNT] =
{
    [ERR_OK]      = "OK",
    [ERR_TIMEOUT] = "TIMEOUT_ERROR",
    [ERR_HW_FAIL] = "HARDWARE_FAILURE"
};

static bool validate_error_type(const e_error_code_t err)
{
    return ((err >= ERR_OK) && (err < ERR_COUNT));
}

const char *decode_error(const int32_t err)
{
    return ((validate_error_type((e_error_code_t)err) == true) ?
                p_error_strings[err] :
                "UNKNOWN_ERROR");
}