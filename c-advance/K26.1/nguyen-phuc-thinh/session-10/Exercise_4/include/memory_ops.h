/**
 * @file memory_ops.h
 * @brief Memory operations module for binary data handling.
 */
#ifndef MEMORY_OPS_H
#define MEMORY_OPS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Sensor data structure.
 */
typedef struct {
    int32_t id;
    float temp;
} sensor_data_t;

/**
 * @brief Zero-initializes a sensor data structure safely using memset.
 * 
 * @param[out] p_data Pointer to the sensor data structure.
 */
void init_sensor_data(sensor_data_t *p_data);

/**
 * @brief Copies binary payload using strncpy (demonstrates unsafe truncation).
 * 
 * @param[out] p_dst Destination buffer.
 * @param[in]  p_src Source binary payload.
 * @param[in]  size  Number of bytes to copy.
 */
void copy_with_strncpy(uint8_t *p_dst, const uint8_t *p_src, size_t size);

/**
 * @brief Copies binary payload using memcpy (safe for binary data).
 * 
 * @param[out] p_dst Destination buffer.
 * @param[in]  p_src Source binary payload.
 * @param[in]  size  Number of bytes to copy.
 */
void copy_with_memcpy(uint8_t *p_dst, const uint8_t *p_src, size_t size);

/**
 * @brief Compares two binary IDs using strcmp (unsafe, stops at 0x00).
 * 
 * @param[in] p_id1 First ID array.
 * @param[in] p_id2 Second ID array.
 * @return int strcmp result.
 */
int compare_with_strcmp(const uint8_t *p_id1, const uint8_t *p_id2);

/**
 * @brief Compares two binary IDs using memcmp (safe for binary data).
 * 
 * @param[in] p_id1 First ID array.
 * @param[in] p_id2 Second ID array.
 * @param[in] size  Size of the ID array in bytes.
 * @return int memcmp result.
 */
int compare_with_memcmp(const uint8_t *p_id1, const uint8_t *p_id2, size_t size);

/**
 * @brief Shifts data safely within the same buffer using memmove.
 * 
 * @param[in,out] p_buf      The buffer containing the data.
 * @param[in]     src_offset Offset of the data to move.
 * @param[in]     dst_offset Offset of the destination.
 * @param[in]     size       Number of bytes to move.
 */
void shift_data_safely(char *p_buf, size_t src_offset, size_t dst_offset, size_t size);

#endif /* MEMORY_OPS_H */