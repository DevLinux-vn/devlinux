#ifndef SENSOR_H
#define SENSOR_H

#include <stdbool.h>
#include <stdint.h>

#define SENSOR_SAMPLE_COUNT 8U

/**
 * @brief Processes a set of sensor readings.
 *
 * @param[in] p_readings Pointer to the sensor readings array.
 * @param[in] count Number of sensor readings to process.
 */
void sensor_process_readings(uint8_t *p_readings, uint32_t count);

/**
 * @brief Accumulates a sensor sample into the current value.
 *
 * @param[in] current_val Current accumulated value.
 * @param[in] sample New sensor sample to accumulate.
 *
 * @return Updated accumulated value.
 */
int32_t sensor_accumulate_data(int32_t current_val, int32_t sample);

/**
 * @brief Gets the current sensor calibration value.
 *
 * @return Current calibration value.
 */
uint32_t sensor_get_calibration(void);

#endif /* SENSOR_H */