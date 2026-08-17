#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/**
 * @brief Convert a raw 12-bit ADC value to millivolts.
 *
 * @param[in] raw_adc Raw ADC value, valid range is 0-4095.
 *
 * @return Converted voltage in millivolts, or 0 if raw_adc is out of range.
 */
uint32_t sensor_adc_to_mv(uint32_t raw_adc);

/**
 * @brief Calculate the moving average of an array of samples.
 *
 * @param[in] samples Pointer to an array of millivolt readings.
 * @param[in] count   Number of samples in the array.
 *
 * @return Average value in millivolts, or 0 if count is 0.
 */
uint32_t sensor_moving_average(const uint32_t *samples, uint32_t count);

#endif /* SENSOR_H */