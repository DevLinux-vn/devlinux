#include "sensor_registry.h"

#include <stdlib.h>

/**
 * @brief Compare two sensor metadata entries by sensor ID.
 *
 * @param[in] p_a Pointer to the first sensor.
 * @param[in] p_b Pointer to the second sensor.
 *
 * @return -1 if first ID is smaller.
 * @return  0 if IDs are equal.
 * @return  1 if first ID is greater.
 */
int compare_by_id(const void *p_a, const void *p_b)
{
    const sensor_metadata_t *p_sensor_a =
        (const sensor_metadata_t *)p_a;

    const sensor_metadata_t *p_sensor_b =
        (const sensor_metadata_t *)p_b;

    if (p_sensor_a->sensor_id < p_sensor_b->sensor_id)
    {
        return -1;
    }

    if (p_sensor_a->sensor_id > p_sensor_b->sensor_id)
    {
        return 1;
    }

    return 0;
}

/*
 * DANGEROUS COMPARATOR - DO NOT USE.
 *
 * A comparator such as:
 *
 * int dangerous_compare(const void *p_a, const void *p_b)
 * {
 *     return (*(const int32_t *)p_a - *(const int32_t *)p_b);
 * }
 *
 * is unsafe because signed integer subtraction may overflow.
 *
 * Example:
 *
 * INT32_MAX - (-1)
 *
 * mathematically equals 2147483648, but this value cannot be
 * represented by int32_t. Signed integer overflow results in
 * undefined behavior and may produce the wrong comparison sign.
 *
 * This can break the ordering required by qsort() and bsearch().
 *
 * Therefore, a safe three-way comparison using < and > is used.
 */

/**
 * @brief Find a sensor by ID using binary search.
 *
 * @param[in] p_registry Pointer to sorted sensor registry.
 * @param[in] count Number of entries in the registry.
 * @param[in] target_id Sensor ID to search for.
 *
 * @return Pointer to matching sensor, or NULL if not found.
 */
const sensor_metadata_t *find_sensor(const sensor_metadata_t *p_registry,
                                     size_t count,
                                     uint32_t target_id)
{
    sensor_metadata_t key =
    {
        .sensor_id = target_id,
        .name = "",
        .calibration_offset = 0.0f
    };

    const sensor_metadata_t *p_result = NULL;

    if (p_registry != NULL)
    {
        p_result = (const sensor_metadata_t *)bsearch(
            &key,
            p_registry,
            count,
            sizeof(sensor_metadata_t),
            compare_by_id);
    }

    return p_result;
}