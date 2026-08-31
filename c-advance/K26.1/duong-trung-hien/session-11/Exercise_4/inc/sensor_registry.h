#ifndef SENSOR_REGISTRY_H
#define SENSOR_REGISTRY_H

#include <stdint.h>
#include <stddef.h>

#define NULL_POINTER    (-100)
#define HIGHER          (1U)
#define LOWER           (-1)
#define EQUAL           (0)

/**
 * @brief Stores metadata associated with a sensor.
 *
 * This structure contains the unique sensor identifier, sensor name,
 * and calibration offset used to adjust sensor measurements.
 */
typedef struct {
    uint32_t sensor_id;          /**< Unique identifier of the sensor. */
    char     name[32];           /**< Human-readable sensor name. */
    float    calibration_offset; /**< Calibration offset for measurements. */
} sensor_metadata_t;

/**
 * @brief Compares two sensors by their sensor IDs.
 *
 * This comparator is intended for use with qsort() and bsearch().
 * It returns a negative value if the first sensor ID is smaller,
 * zero if both sensor IDs are equal, and a positive value if the
 * first sensor ID is greater.
 *
 * @param[in] p_a Pointer to the first sensor metadata.
 * @param[in] p_b Pointer to the second sensor metadata.
 *
 * @return Negative, zero, or positive value according to the
 *         comparison of the sensor IDs.
 */
int compare_by_id(const void *p_a, const void *p_b);

/**
 * @brief Looks up a sensor by its ID in a sorted registry.
 *
 * The registry must be sorted in ascending order by sensor_id before
 * this function is called.
 *
 * @param[in] p_registry Pointer to the sorted sensor registry.
 * @param[in] count      Number of sensors in the registry.
 * @param[in] target_id  Sensor ID to search for.
 *
 * @return Pointer to the matching sensor metadata, or NULL if no
 *         matching sensor is found.
 */
const sensor_metadata_t *find_sensor(const sensor_metadata_t *p_registry,
                                     size_t count,
                                     uint32_t target_id);

#endif /* SENSOR_REGISTRY_H */