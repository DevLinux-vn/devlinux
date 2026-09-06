#include "sensor.h"
#include <stddef.h>

void sensor_process_readings(uint8_t *p_readings, uint32_t count)
{
    if (p_readings == NULL)
    {
        return;
    }
    
    /* FIX Bug 1: Changed <= to < to prevent stack buffer overflow (ASan fix) */
    for (uint32_t i = 0U; i < count; i++)
    {
        p_readings[i] = (uint8_t)(p_readings[i] * 2U);
    }
}

int32_t sensor_accumulate_data(int32_t current_val, int32_t sample)
{
    /* FIX Bug 2: Safe saturation arithmetic to prevent signed integer overflow (UBSan fix) */
    if ((sample > 0) && (current_val > (INT32_MAX - sample)))
    {
        return INT32_MAX;
    }
    if ((sample < 0) && (current_val < (INT32_MIN - sample)))
    {
        return INT32_MIN;
    }
    
    return current_val + sample;
}

uint32_t sensor_get_calibration(void)
{
    /* FIX Bug 3: Replaced dynamic allocation with static memory.
     * Eliminates Use-After-Free and complies with MISRA Dir 4.12. */
    static const uint32_t cal_data[4] = {0xDEADBEEFU, 0U, 0U, 0U};
    return cal_data[0];
}