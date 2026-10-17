/**
 * @file    sensor.h
 * @brief   Public interface for the sensor driver module.
 *
 * @details Provides ADC-to-millivolt conversion and a simple moving
 *          average helper for a series of millivolt readings.
 */
#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/**
 * @brief   Convert a raw 12-bit ADC reading to millivolts.
 *
 * @param[in] raw_adc Raw ADC value. Valid range: 0-4095.
 *
 * @return  The converted value in millivolts, or 0 if @p raw_adc is
 *          out of the valid 12-bit range.
 */
uint32_t sensor_adc_to_mv(uint32_t raw_adc);

/**
 * @brief   Calculate the moving average of an array of samples.
 *
 * @param[in] samples Array of millivolt readings. Must not be NULL
 *                     when @p count is greater than zero.
 * @param[in] count   Number of samples in @p samples.
 *
 * @return  The arithmetic mean of the samples, or 0 if @p count is 0
 *          (prevents division by zero).
 */
uint32_t sensor_moving_average(const uint32_t *samples, uint32_t count);

#endif /* SENSOR_H */