#include "sensor.h"
#include <stddef.h>

uint32_t sensor_adc_to_mv(const uint32_t raw_adc) {
    uint32_t vol = 0U;

    if (raw_adc <= (4095U))
    {
        vol = (uint32_t)(raw_adc * 5000U / 4095U);
    }

    return vol;
}

uint32_t sensor_moving_average(const uint32_t *samples, uint32_t count) {
    uint32_t result = 0U;

    if ((samples != NULL) && (count != 0)) {
        for (uint32_t i = 0U; i < count; i++)
        {
            result += *(samples + i);
        }

        result /= count;
    }

    return result;
}