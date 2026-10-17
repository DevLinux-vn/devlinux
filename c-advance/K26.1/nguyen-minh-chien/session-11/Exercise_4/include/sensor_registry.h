#ifndef SENSOR_REGISTRY_H
#define SENSOR_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Metadata describing one sensor.
 *
 * This structure stores the unique sensor ID, sensor name,
 * and calibration offset used by the firmware.
 */
typedef struct
{
    uint32_t sensor_id;          /**< Unique sensor identifier. */
    char name[32];               /**< Human-readable sensor name. */
    float calibration_offset;    /**< Sensor calibration offset. */
} sensor_metadata_t;

/**
 * @brief Compare two sensor metadata entries by sensor ID.
 *
 * This comparator is suitable for qsort() and bsearch().
 * It uses a safe three-way comparison instead of subtraction
 * to avoid signed integer overflow.
 *
 * @param[in] p_a Pointer to the first sensor metadata entry.
 * @param[in] p_b Pointer to the second sensor metadata entry.
 *
 * @return -1 if first ID is smaller.
 * @return  0 if both IDs are equal.
 * @return  1 if first ID is greater.
 */
int compare_by_id(const void *p_a, const void *p_b);

/**
 * @brief Find a sensor by ID in a sorted sensor registry.
 *
 * The registry must already be sorted in ascending order by
 * sensor_id before this function is called.
 *
 * @param[in] p_registry Pointer to the sorted sensor array.
 * @param[in] count Number of sensors in the array.
 * @param[in] target_id Sensor ID to search for.
 *
 * @return Pointer to the matching sensor.
 * @return NULL if the sensor cannot be found or the registry is NULL.
 */
const sensor_metadata_t *find_sensor(const sensor_metadata_t *p_registry,
                                     size_t count,
                                     uint32_t target_id);

#endif /* SENSOR_REGISTRY_H */