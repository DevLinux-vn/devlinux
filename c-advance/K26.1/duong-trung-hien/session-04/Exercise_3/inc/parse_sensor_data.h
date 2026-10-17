#ifndef PARSE_SENSOR_DATA_H
#define PARSE_SENSOR_DATA_H

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Parsed sensor data structure.
 *
 * Stores the decoded sensor values after converting the raw
 * Big-Endian payload into native integer values.
 */
typedef struct
{
    uint16_t temperature;
    uint32_t timestamp;
} sensor_data_t;

/**
 * @brief Parses big-endian raw sensor data into a struct safely.
 *
 * @param[in]  p_buffer    Pointer to the 6-byte raw payload array.
 * @param[out] p_out_data  Pointer to the struct to populate.
 */
void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data);

#endif /* PARSE_SENSOR_DATA_H */