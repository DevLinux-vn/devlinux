/**
 * @file wide_string_utils.h
 * @brief Utility functions for wide string operations.
 */
#ifndef WIDE_STRING_UTILS_H
#define WIDE_STRING_UTILS_H

#include <wchar.h>
#include <wctype.h>
#include <stdbool.h>

/**
 * @brief Copies a wide string safely with bounds checking.
 * 
 * @param[out] p_dst    Destination buffer.
 * @param[in]  dst_size Size of the destination buffer in wchar_t units.
 * @param[in]  p_src    Source wide string.
 * @return true if successful, false on truncation or error.
 */
bool safe_wcscpy(wchar_t *p_dst, size_t dst_size, const wchar_t *p_src);

/**
 * @brief Concatenates two wide strings safely with bounds checking.
 * 
 * @param[in,out] p_dst    Destination buffer.
 * @param[in]     dst_size Size of the destination buffer in wchar_t units.
 * @param[in]     p_src    Source wide string to append.
 * @return true if successful, false on truncation or error.
 */
bool safe_wcscat(wchar_t *p_dst, size_t dst_size, const wchar_t *p_src);

#endif /* WIDE_STRING_UTILS_H */