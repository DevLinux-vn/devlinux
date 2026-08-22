/**
 * @file sensor_registry.h
 * @brief Sensor registry and lookup utilities.
 */
#ifndef SENSOR_REGISTRY_H
#define SENSOR_REGISTRY_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Sensor metadata structure.
 */
typedef struct {
    uint32_t sensor_id;
    char     name[32];
    float    calibration_offset;
} sensor_metadata_t;

/**
 * @brief Safe comparator for qsort/bsearch by sensor_id.
 * 
 * @param[in] p_a Pointer to the first element.
 * @param[in] p_b Pointer to the second element.
 * @return -1 if a < b, 1 if a > b, 0 if a == b.
 */
int compare_by_id(const void *p_a, const void *p_b);

/**
 * @brief Look up a sensor by ID in a sorted array.
 * 
 * @param[in] p_registry Sorted array of sensors.
 * @param[in] count      Number of sensors.
 * @param[in] target_id  The sensor ID to find.
 * @return Pointer to the matching sensor, or NULL if not found.
 */
const sensor_metadata_t* find_sensor(const sensor_metadata_t *p_registry,
                                     size_t count, uint32_t target_id);

#endif /* SENSOR_REGISTRY_H */