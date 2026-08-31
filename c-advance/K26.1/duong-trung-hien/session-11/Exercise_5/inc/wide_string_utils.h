#ifndef WIDE_STRING_UTILS_H
#define WIDE_STRING_UTILS_H

#include <stddef.h>
#include <wchar.h>

#define NULL_POINTER    (0U)

/**
 * @brief Get the length of a wide-character string.
 *
 * @param[in] p_string Pointer to the null-terminated wide-character string.
 *
 * @return Number of wide characters in the string, excluding the null terminator.
 */
size_t wide_string_length(const wchar_t *p_string);

/**
 * @brief Compare two wide-character strings.
 *
 * @param[in] p_first  Pointer to the first null-terminated wide-character string.
 * @param[in] p_second Pointer to the second null-terminated wide-character string.
 *
 * @return An integer less than, equal to, or greater than zero if the first
 *         string is less than, equal to, or greater than the second string.
 */
int wide_string_compare(const wchar_t *p_first, const wchar_t *p_second);

/**
 * @brief Copy a wide-character string into a destination buffer.
 *
 * @param[out] p_des    Pointer to the destination buffer.
 * @param[in]  des_size Size of the destination buffer in wide characters.
 * @param[in]  p_src    Pointer to the null-terminated source wide-character string.
 *
 * @return Pointer to the destination buffer, or NULL if the operation fails.
 */
wchar_t *wide_string_copy(wchar_t *p_des, size_t des_size, const wchar_t *p_src);

/**
 * @brief Append a wide-character string to a destination buffer.
 *
 * @param[in,out] p_des    Pointer to the destination buffer.
 * @param[in]     des_size Size of the destination buffer in wide characters.
 * @param[in]     p_src    Pointer to the null-terminated source wide-character string.
 *
 * @return Pointer to the destination buffer, or NULL if the operation fails.
 */
wchar_t *wide_string_concat(wchar_t *p_des, size_t des_size, const wchar_t *p_src);

#endif /* WIDE_STRING_UTILS_H */
