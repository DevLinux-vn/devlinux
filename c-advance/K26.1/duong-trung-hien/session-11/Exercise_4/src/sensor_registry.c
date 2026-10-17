#include "sensor_registry.h"
#include <stdlib.h>

/*
 * WARNING:
 * Do not compare integer values by subtracting them.
 *
 * For example:
 * INT32_MAX - (-1)
 *
 * can cause signed integer overflow. This may produce an incorrect
 * comparison result and cause qsort() to interpret the order of two
 * elements incorrectly, resulting in an incorrect sort.
 */

/*
int comparator_dangerous(const void *a, const void *b)
{
    return *(const int32_t *)a - *(const int32_t *)b;
}
*/

int compare_by_id(const void *p_a, const void *p_b) {
    if (NULL == p_a || NULL == p_b) {
        return NULL_POINTER;
    }

    const sensor_metadata_t *a = (const sensor_metadata_t *)p_a;
    const sensor_metadata_t *b = (const sensor_metadata_t *)p_b;

    if (a->sensor_id > b->sensor_id) {
        return HIGHER;
    }
    else if (a->sensor_id < b->sensor_id) {
        return LOWER;
    }

    return EQUAL;
}

const sensor_metadata_t* find_sensor(const sensor_metadata_t *p_registry, size_t count, uint32_t target_id) {
    if (NULL == p_registry) {
        return NULL;
    }

    return bsearch(&target_id, p_registry, count, sizeof(sensor_metadata_t), compare_by_id);
}
