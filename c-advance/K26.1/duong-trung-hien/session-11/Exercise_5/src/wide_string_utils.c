#include "wide_string_utils.h"

size_t wide_string_length(const wchar_t *p_string) {
    if (NULL == p_string) {
        return NULL_POINTER;
    }

    return wcslen(p_string);
}

int wide_string_compare(const wchar_t *p_first, const wchar_t *p_second) {
    if (NULL == p_first || NULL == p_second) {
        return NULL_POINTER;
    }

    int result = wcscmp(p_first, p_second);

    return result;
}

wchar_t *wide_string_copy(wchar_t *p_des, size_t des_size, const wchar_t *p_src) {
    if ((NULL == p_des) || (NULL == p_src) || (des_size == 0U)) {
        return NULL;
    }

    if (wide_string_length(p_src) + 1U > des_size) {
        return NULL;
    }

    (void)wcscpy(p_des, p_src);

    return p_des;
}

wchar_t *wide_string_concat(wchar_t *p_des, size_t des_size, const wchar_t *p_src) {
    if ((NULL == p_des) || (NULL == p_src)) {
        return NULL;
    }

    if (wide_string_length(p_src) + 1U > des_size) {
        return NULL;
    }

    (void)wcscat(p_des, p_src);

    return p_des;
}