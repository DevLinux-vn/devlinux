#ifndef MEMORY_OPS_H
#define MEMORY_OPS_H

#include <string.h>
#include <stdint.h>

#define INVALID     (0)
#define ERROR       (-1)

/**
 * @brief Represents sensor data.
 */
typedef struct {
    int id;       /**< Sensor identifier. */
    float temp;   /**< Sensor temperature. */
} sensor_data_t;

/**
 * @brief Initializes sensor data structure to zero.
 *
 * @param[out] p_data Pointer to the sensor data structure to initialize.
 */
void sensor_data_zero_init(sensor_data_t *p_data);

/**
 * @brief Copies a string into the destination buffer using strncpy semantics.
 *
 * @param[out] p_dst  Pointer to the destination buffer.
 * @param[in]  p_src  Pointer to the source string.
 * @param[in]  size   Maximum number of characters to copy.
 */
void binary_strncpy(char *p_dst, const char *p_src, size_t size);

/**
 * @brief Copies a block of binary data from source to destination.
 *
 * @param[out] p_dst  Pointer to the destination memory region.
 * @param[in]  p_src  Pointer to the source memory region.
 * @param[in]  size   Number of bytes to copy.
 */
void binary_memcpy(void *p_dst, const void *p_src, size_t size);

/**
 * @brief Compares two null-terminated strings.
 *
 * @param[in] p_data1 Pointer to the first string.
 * @param[in] p_data2 Pointer to the second string.
 *
 * @return A negative value if p_data1 is less than p_data2,
 *         zero if both strings are equal,
 *         or a positive value if p_data1 is greater than p_data2.
 */
int32_t binary_strcmp(const char *p_data1, const char *p_data2);

/**
 * @brief Compares two blocks of binary data.
 *
 * @param[in] p_data1 Pointer to the first memory region.
 * @param[in] p_data2 Pointer to the second memory region.
 * @param[in] size    Number of bytes to compare.
 *
 * @return A negative value if the first differing byte in p_data1
 *         is less than that in p_data2, zero if the memory regions
 *         are equal, or a positive value otherwise.
 */
int32_t binary_memcmp(const void *p_data1, const void *p_data2, size_t size);

/**
 * @brief Moves a block of memory, safely handling overlapping regions.
 *
 * @param[out] p_dst  Pointer to the destination memory region.
 * @param[in]  p_src  Pointer to the source memory region.
 * @param[in]  size   Number of bytes to move.
 */
void binary_memmove(void *p_dst, const void *p_src, size_t size);

#endif /* MEMORY_OPS_H */