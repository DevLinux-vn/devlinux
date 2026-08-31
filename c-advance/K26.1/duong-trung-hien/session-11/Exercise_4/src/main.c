#include "sensor_registry.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define APP_SUCCESS             (0)
#define ARR_SIZE(arr)           (sizeof(arr) / sizeof(arr[0]))
#define VALUE_TEST_FOUND        (1055U)
#define VALUE_TEST_NOT_FOUND    (9999U)

/**
 * @brief Prints the result of a sensor lookup.
 *
 * If @p p_registry is not NULL, the function prints the sensor ID,
 * name, and calibration offset. Otherwise, it reports that the sensor
 * with @p target_id was not found.
 *
 * @param[in] p_registry Pointer to the matching sensor metadata, or NULL.
 * @param[in] target_id  Sensor ID that was searched for.
 */
void result_find_sensor(const sensor_metadata_t *p_registry, uint32_t target_id);

int32_t main() {
    printf("========== L11 Exercise 4: Sensor Registry (qsort + bsearch) ==========\n");
    sensor_metadata_t sensors[] = {
        {1042U, "Pressure_A", 0.120f},
        {1007U, "Temp_Inlet", -0.340f},
        {1099U, "Flow_Main", 0.050f},
        {1023U, "Voltage_Bus", 0.000f},
        {1001U, "Ambient_Temp", -0.150f},
        {1088U, "Humidity_Ext", 0.210f},
        {1055U, "Current_Motor", -0.080f},
        {1034U, "Pressure_B", 0.095f}
    };
    size_t count = ARR_SIZE(sensors);
    size_t size = sizeof(sensor_metadata_t);

    printf("\n--- Before Sorting ---\n");
    for (uint32_t i = 0U; i < count; i++) {
        printf("[%u] ID: %-4u Name: %-15s Offset: %+.3f\n", i, sensors[i].sensor_id, sensors[i].name, sensors[i].calibration_offset);
    }

    qsort(sensors, count, size, compare_by_id);

    printf("\n--- After Sorting (by sensor_id) ---\n");
    for (uint32_t i = 0U; i < count; i++) {
        printf("[%u] ID: %-4u Name: %-15s Offset: %+.3f\n", i, sensors[i].sensor_id, sensors[i].name, sensors[i].calibration_offset);
    }

    printf("\n--- Lookup Tests ---\n");

    const sensor_metadata_t *p_des = find_sensor(sensors, count, VALUE_TEST_FOUND);
    result_find_sensor(p_des, VALUE_TEST_FOUND);

    p_des = find_sensor(sensors, count, VALUE_TEST_NOT_FOUND);
    result_find_sensor(p_des, VALUE_TEST_NOT_FOUND);

    return APP_SUCCESS;
}

void result_find_sensor(const sensor_metadata_t *p_registry, uint32_t target_id) {
    if (NULL == p_registry) {
        printf("Search for ID %u: NOT FOUND\n", target_id);
        return;
    }

    printf("Search for ID %u: FOUND -> %s (offset: %+.3f)\n", p_registry->sensor_id, 
                                                              p_registry->name, 
                                                              p_registry->calibration_offset);
}