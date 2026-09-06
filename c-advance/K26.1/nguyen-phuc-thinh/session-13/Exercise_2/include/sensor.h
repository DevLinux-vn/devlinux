#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <stdbool.h>

#define SENSOR_SAMPLE_COUNT 8U

/**
 * @brief Double each sample value in array (safe bounds-checked).
 * @param p_readings Pointer to uint8_t array of samples
 * @param count Number of samples to process
 * @return void
 */
void sensor_process_readings(uint8_t *p_readings, uint32_t count);

/**
 * @brief Safely accumulate sensor value with overflow saturation.
 * @param current_val Current accumulated value
 * @param sample New sample to add
 * @return Saturated sum: INT32_MIN <= result <= INT32_MAX
 */
int32_t sensor_accumulate_data(int32_t current_val, int32_t sample);

/**
 * @brief Retrieve calibration constant from static memory.
 * @return Calibration magic value (0xDEADBEEFU)
 * @note Replaces dynamic allocation - eliminates use-after-free risk
 */
uint32_t sensor_get_calibration(void);

#endif /* SENSOR_H */