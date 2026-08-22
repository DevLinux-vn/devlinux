/**
 * @file wide_string_utils.c
 * @brief Implementation of safe wide string operations.
 */
#include "wide_string_utils.h"

bool safe_wcscpy(wchar_t *p_dst, size_t dst_size, const wchar_t *p_src)
{
    if ((p_dst == NULL) || (p_src == NULL) || (dst_size == 0U))
    {
        return false;
    }

    size_t src_len = wcslen(p_src);
    if (src_len >= dst_size)
    {
        return false; /* Truncation hazard avoided */
    }

    wcscpy(p_dst, p_src);
    return true;
}

bool safe_wcscat(wchar_t *p_dst, size_t dst_size, const wchar_t *p_src)
{
    if ((p_dst == NULL) || (p_src == NULL) || (dst_size == 0U))
    {
        return false;
    }

    size_t dst_len = wcslen(p_dst);
    size_t src_len = wcslen(p_src);

    if ((dst_len + src_len) >= dst_size)
    {
        return false; /* Truncation hazard avoided */
    }

    wcscat(p_dst, p_src);
    return true;
}