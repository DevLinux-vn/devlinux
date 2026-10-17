#include "sensor_registry.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Print all sensors in a registry.
 *
 * @param[in] p_registry Pointer to sensor array.
 * @param[in] count Number of sensors in the array.
 */
static void print_registry(const sensor_metadata_t *p_registry,
                           size_t count)
{
    size_t index;

    for (index = 0U; index < count; ++index)
    {
        printf("[%zu] ID: %-4u  Name: %-16s Offset: %+.3f\n",
               index,
               (unsigned int)p_registry[index].sensor_id,
               p_registry[index].name,
               (double)p_registry[index].calibration_offset);
    }
}

/**
 * @brief Application entry point.
 *
 * Initializes an unsorted sensor registry, sorts it using qsort(),
 * and performs binary-search lookups using bsearch().
 *
 * @return 0 on successful completion.
 */
int main(void)
{
    sensor_metadata_t sensors[] =
    {
        {1042U, "Pressure_A",   0.120f},
        {1007U, "Temp_Inlet",  -0.340f},
        {1099U, "Flow_Main",    0.050f},
        {1023U, "Voltage_Bus",  0.000f},
        {1001U, "Ambient_Temp", -0.150f},
        {1088U, "Humidity_Ext", 0.210f},
        {1055U, "Current_Motor", -0.080f},
        {1034U, "Pressure_B",    0.095f}
    };

    const size_t sensor_count =
        sizeof(sensors) / sizeof(sensors[0]);

    const sensor_metadata_t *p_found;

    printf("========== L11 Exercise 4: Sensor Registry "
           "(qsort + bsearch) ==========\n\n");

    printf("--- Before Sorting ---\n");

    print_registry(sensors, sensor_count);

    qsort(sensors,
          sensor_count,
          sizeof(sensor_metadata_t),
          compare_by_id);

    printf("\n--- After Sorting (by sensor_id) ---\n");

    print_registry(sensors, sensor_count);

    printf("\n--- Lookup Tests ---\n");

    p_found = find_sensor(sensors, sensor_count, 1055U);

    if (p_found != NULL)
    {
        printf("Search for ID 1055: FOUND -> %s (offset: %+.3f)\n",
               p_found->name,
               (double)p_found->calibration_offset);
    }
    else
    {
        printf("Search for ID 1055: Sensor not found.\n");
    }

    p_found = find_sensor(sensors, sensor_count, 9999U);

    if (p_found != NULL)
    {
        printf("Search for ID 9999: FOUND -> %s (offset: %+.3f)\n",
               p_found->name,
               (double)p_found->calibration_offset);
    }
    else
    {
        printf("Search for ID 9999: NOT FOUND\n");
    }

    return 0;
}