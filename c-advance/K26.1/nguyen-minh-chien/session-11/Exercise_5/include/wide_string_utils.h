#ifndef WIDE_STRING_UTILS_H
#define WIDE_STRING_UTILS_H

#include <stddef.h>
#include <wchar.h>

/**
 * @brief Copy a wide string into a destination buffer.
 *
 * @param[out] p_dest Destination wide-character buffer.
 * @param[in] dest_count Number of wchar_t elements available in p_dest.
 * @param[in] p_src Source wide string.
 *
 * @return 0 on success, -1 on error.
 */
int wide_string_copy(wchar_t *p_dest,
                     size_t dest_count,
                     const wchar_t *p_src);

/**
 * @brief Append a wide string to a destination buffer.
 *
 * @param[in,out] p_dest Destination wide string.
 * @param[in] dest_count Number of wchar_t elements available.
 * @param[in] p_src Source wide string.
 *
 * @return 0 on success, -1 on error.
 */
int wide_string_concat(wchar_t *p_dest,
                       size_t dest_count,
                       const wchar_t *p_src);

/**
 * @brief Compare two wide strings.
 *
 * @param[in] p_a First wide string.
 * @param[in] p_b Second wide string.
 *
 * @return Negative if p_a < p_b.
 * @return Zero if p_a == p_b.
 * @return Positive if p_a > p_b.
 */
int wide_string_compare(const wchar_t *p_a,
                        const wchar_t *p_b);

#endif /* WIDE_STRING_UTILS_H */