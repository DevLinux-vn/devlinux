/**
 * @file main.c
 * @brief Endian-safe parser for a big-endian sensor payload.
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Parsed sensor data.
 *
 * Stores the temperature and timestamp reconstructed from
 * the received big-endian byte stream.
 */
typedef struct
{
    uint16_t temperature;
    uint32_t timestamp;
} sensor_data_t;

/**
 * @brief Parses a big-endian sensor payload.
 *
 * The payload contains:
 * - Bytes 0-1: 16-bit unsigned temperature.
 * - Bytes 2-5: 32-bit unsigned timestamp.
 *
 * Each byte is explicitly converted to the destination-width
 * unsigned integer type before shifting. This avoids dependence
 * on host CPU endianness and avoids unsafe pointer casts.
 *
 * @param[in] p_buffer Pointer to the 6-byte sensor payload.
 * @param[out] p_out_data Pointer to the parsed sensor structure.
 *
 * @return None.
 */
void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data)
{
    if ((p_buffer == NULL) || (p_out_data == NULL))
    {
        return;
    }

    p_out_data->temperature =
        (uint16_t)(((uint16_t)p_buffer[0] << 8U) |
                   (uint16_t)p_buffer[1]);

    p_out_data->timestamp =
        ((uint32_t)p_buffer[2] << 24U) |
        ((uint32_t)p_buffer[3] << 16U) |
        ((uint32_t)p_buffer[4] << 8U) |
        (uint32_t)p_buffer[5];
}

/**
 * @brief Application entry point.
 *
 * Creates a simulated six-byte big-endian sensor message,
 * parses it, and prints the resulting decimal values.
 *
 * @return 0 on successful program termination.
 */
int main(void)
{
    const uint8_t raw_buffer[6] =
    {
        0x01U,
        0x2CU,
        0x00U,
        0x00U,
        0x1AU,
        0x0AU
    };

    sensor_data_t sensor_data = {0U, 0U};

    parse_sensor_data(raw_buffer, &sensor_data);

    (void)printf("Temperature: %u\n", (unsigned int)sensor_data.temperature);
    (void)printf("Timestamp: %lu\n", (unsigned long)sensor_data.timestamp);

    return 0;
}