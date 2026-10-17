/**
 * @file    sensor.c
 * @brief   Implementation of the sensor driver module.
 */
#include "sensor.h"

/** @brief Maximum valid 12-bit ADC value. */
#define SENSOR_ADC_MAX_VALUE (4095U)

/** @brief Reference voltage in millivolts, corresponding to the
 *         maximum ADC value.
 */
#define SENSOR_VREF_MV (3300U)

/**
 * @brief   Convert a raw 12-bit ADC reading to millivolts.
 *
 * @param[in] raw_adc Raw ADC value. Valid range: 0-4095.
 *
 * @return  The converted value in millivolts, or 0 if @p raw_adc is
 *          out of the valid 12-bit range.
 */
uint32_t sensor_adc_to_mv(uint32_t raw_adc)
{
    uint32_t mv_value = 0U;

    /* MISRA Directive 4.11: validate input before using it. */
    if (raw_adc <= SENSOR_ADC_MAX_VALUE)
    {
        mv_value = (raw_adc * SENSOR_VREF_MV) / SENSOR_ADC_MAX_VALUE;
    }

    return mv_value;
}

/**
 * @brief   Calculate the moving average of an array of samples.
 *
 * @param[in] samples Array of millivolt readings.
 * @param[in] count   Number of samples in @p samples.
 *
 * @return  The arithmetic mean of the samples, or 0 if @p count is 0.
 */
uint32_t sensor_moving_average(const uint32_t *samples, uint32_t count)
{
    uint32_t average = 0U;

    /* CERT INT33-C: guard against division by zero. */
    if (count > 0U)
    {
        uint32_t sum = 0U;
        uint32_t i;

        for (i = 0U; i < count; i++)
        {
            sum += samples[i];
        }

        average = sum / count;
    }

    return average;
}