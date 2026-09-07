/**
 * @file    memory_ops.c
 * @brief   Implementation of the mem* vs str* demonstration helpers.
 */
#include "memory_ops.h"
#include <string.h>

void sensor_data_zero_init(sensor_data_t *p_data)
{
    if (p_data != NULL)
    {
        (void)memset(p_data, 0, sizeof(*p_data));
    }
}

void copy_with_strncpy(uint8_t *p_dst, size_t dst_size,
                        const uint8_t *p_src, size_t src_len)
{
    if ((p_dst != NULL) && (p_src != NULL) && (dst_size >= src_len))
    {
        /* PITFALL: strncpy() treats p_src as a C string. As soon as it
         * sees a 0x00 byte, it considers the "string" finished and
         * pads the rest of p_dst with zeros - it does NOT keep
         * copying the remaining raw bytes from p_src. */
        (void)strncpy((char *)p_dst, (const char *)p_src, src_len); // NOLINT(bugprone-not-null-terminated-result)
    }
}

void copy_with_memcpy(uint8_t *p_dst, const uint8_t *p_src, size_t len)
{
    if ((p_dst != NULL) && (p_src != NULL))
    {
        (void)memcpy(p_dst, p_src, len);
    }
}

int compare_with_strcmp(const uint8_t *p_a, const uint8_t *p_b)
{
    int result = 0;

    if ((p_a != NULL) && (p_b != NULL))
    {
        /* PITFALL: strcmp() also stops at the first 0x00 byte in
         * either buffer, so any bytes after it are never compared. */
        result = strcmp((const char *)p_a, (const char *)p_b);
    }

    return result;
}

int compare_with_memcmp(const uint8_t *p_a, const uint8_t *p_b, size_t len)
{
    int result = 0;

    if ((p_a != NULL) && (p_b != NULL))
    {
        result = memcmp(p_a, p_b, len);
    }

    return result;
}

void safe_shift_right(char *p_buf, size_t buf_capacity, size_t src_offset,
                       size_t len, size_t shift_amount)
{
    if (p_buf != NULL)
    {
        size_t dst_offset = src_offset + shift_amount;
        size_t needed = dst_offset + len + 1U; /* +1 for '\0' */

        if (needed <= buf_capacity)
        {
            /* CERT MEM33-C: the source [src_offset, src_offset+len)
             * and destination [dst_offset, dst_offset+len) regions
             * overlap here, so memcpy() would have Undefined
             * Behavior. memmove() is required and handles it safely. */
            (void)memmove(p_buf + dst_offset, p_buf + src_offset, len);
            p_buf[dst_offset + len] = '\0';
        }
    }
}