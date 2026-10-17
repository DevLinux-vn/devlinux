/**
 * @file    memory_ops.h
 * @brief   Demonstrates memory (mem*) vs string (str*) operations on
 *          binary data that may legitimately contain 0x00 bytes.
 */
#ifndef MEMORY_OPS_H
#define MEMORY_OPS_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief   A small example sensor data structure.
 */
typedef struct
{
    int32_t id;   /**< Sensor identifier. */
    float temp;   /**< Last temperature reading. */
} sensor_data_t;

/**
 * @brief   Zero-initialize a sensor_data_t using memset().
 *
 * @param[out] p_data Structure to zero-initialize. May be NULL (no-op).
 */
void sensor_data_zero_init(sensor_data_t *p_data);

/**
 * @brief   Copy binary data using strncpy() - kept here to
 *          demonstrate its pitfall on data containing 0x00 bytes.
 *
 * @param[out] p_dst    Destination buffer.
 * @param[in]  dst_size Size of @p p_dst, in bytes.
 * @param[in]  p_src    Source binary data. May be NULL (no-op).
 * @param[in]  src_len  Number of bytes to copy.
 */
void copy_with_strncpy(uint8_t *p_dst, size_t dst_size,
                        const uint8_t *p_src, size_t src_len);

/**
 * @brief   Copy binary data using memcpy() - the correct tool for
 *          data that may contain 0x00 bytes.
 *
 * @param[out] p_dst Destination buffer, must be at least @p len bytes.
 * @param[in]  p_src Source binary data. May be NULL (no-op).
 * @param[in]  len   Number of bytes to copy.
 */
void copy_with_memcpy(uint8_t *p_dst, const uint8_t *p_src, size_t len);

/**
 * @brief   Compare two binary buffers using strcmp() - kept here to
 *          demonstrate its pitfall on data containing 0x00 bytes.
 *
 * @param[in] p_a First buffer, treated as a null-terminated string.
 * @param[in] p_b Second buffer, treated as a null-terminated string.
 *
 * @return  0 if equal, non-zero otherwise (per strcmp() semantics).
 */
int compare_with_strcmp(const uint8_t *p_a, const uint8_t *p_b);

/**
 * @brief   Compare two binary buffers using memcmp() - the correct
 *          tool for data that may contain 0x00 bytes.
 *
 * @param[in] p_a First buffer.
 * @param[in] p_b Second buffer.
 * @param[in] len Number of bytes to compare.
 *
 * @return  0 if equal, non-zero otherwise (sign matches the first
 *          differing byte's relative order, per memcmp() semantics).
 */
int compare_with_memcmp(const uint8_t *p_a, const uint8_t *p_b, size_t len);

/**
 * @brief   Safely shift a sub-region of a buffer to the right,
 *          using memmove() to correctly handle the overlap.
 *
 * @param[in,out] p_buf        Buffer to modify, already null-terminated.
 * @param[in]     buf_capacity Total capacity of @p p_buf, in bytes.
 * @param[in]     src_offset   Start offset of the region to move.
 * @param[in]     len          Length of the region to move, in bytes.
 * @param[in]     shift_amount How many bytes to the right to move it.
 *
 * @details Does nothing if the shifted region plus its new null
 *          terminator would not fit within @p buf_capacity.
 */
void safe_shift_right(char *p_buf, size_t buf_capacity, size_t src_offset,
                       size_t len, size_t shift_amount);

#endif /* MEMORY_OPS_H */