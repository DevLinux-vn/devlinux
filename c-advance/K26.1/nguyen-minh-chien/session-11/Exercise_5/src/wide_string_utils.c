#include "wide_string_utils.h"

#include <wchar.h>

/**
 * @brief Copy a wide string safely.
 *
 * @param[out] p_dest Destination buffer.
 * @param[in] dest_count Number of wchar_t elements in destination buffer.
 * @param[in] p_src Source string.
 *
 * @return 0 on success.
 * @return -1 on invalid parameters or insufficient destination space.
 */
int wide_string_copy(wchar_t *p_dest,
                     size_t dest_count,
                     const wchar_t *p_src)
{
    size_t required_count;

    if ((p_dest == NULL) || (p_src == NULL) || (dest_count == 0U))
    {
        return -1;
    }

    required_count = wcslen(p_src) + 1U;

    if (required_count > dest_count)
    {
        return -1;
    }

    (void)wcscpy(p_dest, p_src);

    return 0;
}

/**
 * @brief Concatenate a wide string safely.
 *
 * @param[in,out] p_dest Destination string.
 * @param[in] dest_count Number of wchar_t elements in destination buffer.
 * @param[in] p_src Source string.
 *
 * @return 0 on success.
 * @return -1 on invalid parameters or insufficient destination space.
 */
int wide_string_concat(wchar_t *p_dest,
                       size_t dest_count,
                       const wchar_t *p_src)
{
    size_t dest_length;
    size_t src_length;

    if ((p_dest == NULL) || (p_src == NULL) || (dest_count == 0U))
    {
        return -1;
    }

    dest_length = wcslen(p_dest);
    src_length = wcslen(p_src);

    if ((dest_length + src_length + 1U) > dest_count)
    {
        return -1;
    }

    (void)wcscat(p_dest, p_src);

    return 0;
}

/**
 * @brief Compare two wide strings.
 *
 * @param[in] p_a First string.
 * @param[in] p_b Second string.
 *
 * @return Result of wcscmp().
 */
int wide_string_compare(const wchar_t *p_a,
                        const wchar_t *p_b)
{
    if ((p_a == NULL) || (p_b == NULL))
    {
        return 0;
    }

    return wcscmp(p_a, p_b);
}