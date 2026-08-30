#include "memory_ops.h"

void sensor_data_zero_init(sensor_data_t *p_data) {
    if (NULL == p_data) {
        return;
    }

    memset(p_data, 0, sizeof(sensor_data_t));
}

void binary_strncpy(char *p_dst, const char *p_src, size_t size) {
    if (NULL == p_dst || NULL == p_src || size == INVALID) {
        return;
    }

    strncpy(p_dst, p_src, size);
}

void binary_memcpy(void *p_dst, const void *p_src, size_t size) {
    if (NULL == p_dst || NULL == p_src || size == INVALID) {
        return;
    }

    memcpy(p_dst, p_src, size);
}

int32_t binary_strcmp(const char *p_data1, const char *p_data2) {
    if (NULL == p_data1 || NULL == p_data2) {
        return ERROR;
    }

    return strcmp(p_data1, p_data2);
}

int32_t binary_memcmp(const void *p_data1, const void *p_data2, size_t size) {
    if (NULL == p_data1 || NULL == p_data2 || size == INVALID) {
        return ERROR;
    }

    return memcmp(p_data1, p_data2, size);
}

void binary_memmove(void *p_dst, const void *p_src, size_t size) {
    if (NULL == p_dst || NULL == p_src || size == INVALID) {
        return;
    }

    memmove(p_dst, p_src, size);
}