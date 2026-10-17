/**
 * @file main.c
 * @brief Demonstration of sorting and searching a sensor registry.
 */
#include <stdio.h>
#include <stdlib.h>
#include "sensor_registry.h"

/**
 * @brief Helper to print the registry formatting.
 */
static void print_registry(const sensor_metadata_t *p_registry, size_t count)
{
    for (size_t i = 0U; i < count; ++i)
    {
        printf("[%zu] ID: %u  Name: %-14s Offset: %+.3f\n", 
               i, 
               p_registry[i].sensor_id, 
               p_registry[i].name, 
               (double)p_registry[i].calibration_offset);
    }
}

int main(void)
{
    printf("========== L11 Exercise 4: Sensor Registry (qsort + bsearch) ==========\n\n");

    /* Initialize an unsorted array of 8 sensors with realistic data */
    sensor_metadata_t registry[8] = {
        {1042, "Pressure_A", +0.120f},
        {1007, "Temp_Inlet", -0.340f},
        {1099, "Flow_Main", +0.050f},
        {1023, "Voltage_Bus", +0.000f},
        {1001, "Ambient_Temp", -0.150f},
        {1088, "Humidity_Ext", +0.210f},
        {1055, "Current_Motor", -0.080f},
        {1034, "Pressure_B", +0.095f}
    };
    size_t count = sizeof(registry) / sizeof(registry[0]);

    printf("--- Before Sorting ---\n");
    print_registry(registry, count);
    printf("\n");

    /* Sort using qsort to prepare for binary search */
    qsort(registry, count, sizeof(sensor_metadata_t), compare_by_id);

    printf("--- After Sorting (by sensor_id) ---\n");
    print_registry(registry, count);
    printf("\n");

    printf("--- Lookup Tests ---\n");
    
    /* 1. Test existing valid ID */
    uint32_t search_id_1 = 1055;
    const sensor_metadata_t *p_found_1 = find_sensor(registry, count, search_id_1);
    if (p_found_1 != NULL)
    {
        printf("Search for ID %u: FOUND -> %s (offset: %+.3f)\n", 
               search_id_1, p_found_1->name, (double)p_found_1->calibration_offset);
    }
    else
    {
        printf("Search for ID %u: NOT FOUND\n", search_id_1);
    }

    /* 2. Test invalid ID that does not exist */
    uint32_t search_id_2 = 9999;
    const sensor_metadata_t *p_found_2 = find_sensor(registry, count, search_id_2);
    if (p_found_2 != NULL)
    {
        printf("Search for ID %u: FOUND -> %s (offset: %+.3f)\n", 
               search_id_2, p_found_2->name, (double)p_found_2->calibration_offset);
    }
    else
    {
        printf("Search for ID %u: NOT FOUND\n", search_id_2);
    }

    return 0;
}