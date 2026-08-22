/**
 * @file memory_ops.c
 * @brief Implementation of memory operations.
 */
#include "memory_ops.h"
#include <string.h>

void init_sensor_data(sensor_data_t *p_data)
{
    if (p_data != NULL)
    {
        memset(p_data, 0, sizeof(sensor_data_t));
    }
}

void copy_with_strncpy(uint8_t *p_dst, const uint8_t *p_src, size_t size)
{
    if ((p_dst != NULL) && (p_src != NULL) && (size > 0U))
    {
        /* Explicit cast required for string functions handling binary data */
        strncpy((char *)p_dst, (const char *)p_src, size);
    }
}

void copy_with_memcpy(uint8_t *p_dst, const uint8_t *p_src, size_t size)
{
    if ((p_dst != NULL) && (p_src != NULL) && (size > 0U))
    {
        memcpy(p_dst, p_src, size);
    }
}

int compare_with_strcmp(const uint8_t *p_id1, const uint8_t *p_id2)
{
    if ((p_id1 != NULL) && (p_id2 != NULL))
    {
        return strcmp((const char *)p_id1, (const char *)p_id2);
    }
    return 0;
}

int compare_with_memcmp(const uint8_t *p_id1, const uint8_t *p_id2, size_t size)
{
    if ((p_id1 != NULL) && (p_id2 != NULL) && (size > 0U))
    {
        return memcmp(p_id1, p_id2, size);
    }
    return 0;
}

void shift_data_safely(char *p_buf, size_t src_offset, size_t dst_offset, size_t size)
{
    if ((p_buf != NULL) && (size > 0U))
    {
        /* memmove safely handles overlapping source and destination regions */
        memmove(p_buf + dst_offset, p_buf + src_offset, size);
    }
}