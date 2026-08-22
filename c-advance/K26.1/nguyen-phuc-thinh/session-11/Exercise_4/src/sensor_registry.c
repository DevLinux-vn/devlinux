/**
 * @file sensor_registry.c
 * @brief Implementation of sensor registry functions.
 */
#include "sensor_registry.h"
#include <stdlib.h>

/*
// --- The Subtraction Trap Demo ---
// int dangerous_compare_by_id(const void *p_a, const void *p_b)
// {
//     uint32_t id_a = ((const sensor_metadata_t *)p_a)->sensor_id;
//     uint32_t id_b = ((const sensor_metadata_t *)p_b)->sensor_id;
//
//     // DANGER: If id_a is 0 and id_b is UINT32_MAX, (0 - UINT32_MAX) underflows
//     // to 1 (positive!), causing qsort to mistakenly believe 0 > UINT32_MAX.
//     // For signed integers, INT32_MAX - (-1) overflows to INT32_MIN (negative!),
//     // producing the wrong sign and breaking the qsort() algorithm entirely.
//     return (int)(id_a - id_b);
// }
*/

int compare_by_id(const void *p_a, const void *p_b)
{
    if ((p_a == NULL) || (p_b == NULL))
    {
        return 0;
    }

    /* Safe casting of void* to object type (MISRA-C Rule 11.5) */
    const sensor_metadata_t *p_sensor_a = (const sensor_metadata_t *)p_a;
    const sensor_metadata_t *p_sensor_b = (const sensor_metadata_t *)p_b;

    /* Safe three-way comparison prevents integer overflow/underflow */
    if (p_sensor_a->sensor_id < p_sensor_b->sensor_id)
    {
        return -1;
    }
    else if (p_sensor_a->sensor_id > p_sensor_b->sensor_id)
    {
        return 1;
    }
    
    return 0;
}

const sensor_metadata_t* find_sensor(const sensor_metadata_t *p_registry, 
                                     size_t count, uint32_t target_id)
{
    if (p_registry == NULL || count == 0U)
    {
        return NULL;
    }

    sensor_metadata_t key;
    /* Initialize all struct fields before use (MISRA-C Rule 9.1) */
    key.sensor_id = target_id;
    key.name[0] = '\0';
    key.calibration_offset = 0.0f;

    /* bsearch returns void*, must cast back safely */
    void *p_result = bsearch(&key, p_registry, count, 
                             sizeof(sensor_metadata_t), compare_by_id);

    return (const sensor_metadata_t *)p_result;
}/**
 * @file sensor_registry.c
 * @brief Implementation of sensor registry functions.
 */
#include "sensor_registry.h"
#include <stdlib.h>

/*
// --- The Subtraction Trap Demo ---
// int dangerous_compare_by_id(const void *p_a, const void *p_b)
// {
//     uint32_t id_a = ((const sensor_metadata_t *)p_a)->sensor_id;
//     uint32_t id_b = ((const sensor_metadata_t *)p_b)->sensor_id;
//
//     // DANGER: If id_a is 0 and id_b is UINT32_MAX, (0 - UINT32_MAX) underflows
//     // to 1 (positive!), causing qsort to mistakenly believe 0 > UINT32_MAX.
//     // For signed integers, INT32_MAX - (-1) overflows to INT32_MIN (negative!),
//     // producing the wrong sign and breaking the qsort() algorithm entirely.
//     return (int)(id_a - id_b);
// }
*/

int compare_by_id(const void *p_a, const void *p_b)
{
    if ((p_a == NULL) || (p_b == NULL))
    {
        return 0;
    }

    /* Safe casting of void* to object type (MISRA-C Rule 11.5) */
    const sensor_metadata_t *p_sensor_a = (const sensor_metadata_t *)p_a;
    const sensor_metadata_t *p_sensor_b = (const sensor_metadata_t *)p_b;

    /* Safe three-way comparison prevents integer overflow/underflow */
    if (p_sensor_a->sensor_id < p_sensor_b->sensor_id)
    {
        return -1;
    }
    else if (p_sensor_a->sensor_id > p_sensor_b->sensor_id)
    {
        return 1;
    }
    
    return 0;
}

const sensor_metadata_t* find_sensor(const sensor_metadata_t *p_registry, 
                                     size_t count, uint32_t target_id)
{
    if (p_registry == NULL || count == 0U)
    {
        return NULL;
    }

    sensor_metadata_t key;
    /* Initialize all struct fields before use (MISRA-C Rule 9.1) */
    key.sensor_id = target_id;
    key.name[0] = '\0';
    key.calibration_offset = 0.0f;

    /* bsearch returns void*, must cast back safely */
    void *p_result = bsearch(&key, p_registry, count, 
                             sizeof(sensor_metadata_t), compare_by_id);

    return (const sensor_metadata_t *)p_result;
}