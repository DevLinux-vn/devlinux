#ifndef ERR_UTIL_H
#define ERR_UTIL_H

#include <stdint.h>

/**
 * @brief Convert an error code to a human-readable string.
 *
 * Returns the string corresponding to the specified error code.
 * If the error code is not supported, the function returns
 * `"UNKNOWN_ERROR"`.
 *
 * @param[in] err Error code to decode.
 *
 * @return Pointer to a null-terminated, read-only error string.
 *
 * @note The returned string has static storage duration and must not
 *       be modified or freed by the caller.
 */
const char *decode_error(const int32_t err);

#endif