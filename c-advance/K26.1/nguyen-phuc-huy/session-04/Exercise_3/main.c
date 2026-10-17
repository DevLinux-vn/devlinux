/**
 * @file main.c
 * @brief Endian-safe parser for a big-endian sensor data payload.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Number of bytes in the sensor payload.
 */
#define SENSOR_PAYLOAD_SIZE (6U)

/**
 * @brief Program success return code.
 */
#define APP_SUCCESS (0)

/**
 * @brief Program failure return code.
 */
#define APP_FAILURE (1)

/**
 * @brief Parsed sensor data.
 *
 * The external sensor transmits the temperature and timestamp values in
 * big-endian byte order. The parsing function reconstructs these values into
 * the host CPU's native integer representation.
 */
typedef struct sensor_data_s
{
    uint16_t temperature; /**< Temperature value received from the sensor. */
    uint32_t timestamp;   /**< Timestamp value received from the sensor. */
} sensor_data_t;

/**
 * @brief Parse a 16-bit unsigned integer stored in big-endian byte order.
 *
 * The function reconstructs the integer byte-by-byte without casting the
 * byte buffer to a wider pointer type. This avoids strict-aliasing violations
 * and unaligned memory access.
 *
 * @param[in] p_buffer Pointer to at least two bytes of big-endian data.
 *
 * @return Reconstructed 16-bit unsigned integer.
 */
static uint16_t parse_u16_big_endian(const uint8_t *p_buffer)
{
    uint16_t value;

    value = (uint16_t)(((uint16_t)p_buffer[0U] << 8U) |
                       (uint16_t)p_buffer[1U]);

    return value;
}

/**
 * @brief Parse a 32-bit unsigned integer stored in big-endian byte order.
 *
 * Each byte is explicitly converted to uint32_t before shifting. This ensures
 * that the shifts occur using an unsigned 32-bit type and prevents unwanted
 * signed integer promotion or loss of data.
 *
 * @param[in] p_buffer Pointer to at least four bytes of big-endian data.
 *
 * @return Reconstructed 32-bit unsigned integer.
 */
static uint32_t parse_u32_big_endian(const uint8_t *p_buffer)
{
    uint32_t value;

    value = ((uint32_t)p_buffer[0U] << 24U) |
            ((uint32_t)p_buffer[1U] << 16U) |
            ((uint32_t)p_buffer[2U] << 8U) |
            ((uint32_t)p_buffer[3U]);

    return value;
}

/**
 * @brief Parse a six-byte big-endian sensor payload.
 *
 * Payload format:
 * - Bytes 0 and 1 contain the 16-bit temperature.
 * - Bytes 2 through 5 contain the 32-bit timestamp.
 *
 * The function returns without modifying the output structure when either
 * input pointer is NULL.
 *
 * @param[in]  p_buffer   Pointer to the six-byte raw sensor payload.
 * @param[out] p_out_data Pointer to the destination sensor data structure.
 */
static void parse_sensor_data(const uint8_t *p_buffer,
                              sensor_data_t *p_out_data)
{
    if ((p_buffer != NULL) && (p_out_data != NULL))
    {
        p_out_data->temperature = parse_u16_big_endian(&p_buffer[0U]);
        p_out_data->timestamp = parse_u32_big_endian(&p_buffer[2U]);
    }
}

/**
 * @brief Program entry point.
 *
 * Simulates receiving a six-byte big-endian payload, parses the values, and
 * prints the temperature and timestamp in decimal format.
 *
 * @return APP_SUCCESS when parsing produces the expected values.
 * @return APP_FAILURE when parsing produces unexpected values.
 */
int main(void)
{
    static const uint8_t sensor_buffer[SENSOR_PAYLOAD_SIZE] =
    {
        0x01U,
        0x2CU,
        0x00U,
        0x00U,
        0x1AU,
        0x0AU
    };

    sensor_data_t sensor_data =
    {
        0U,
        0U
    };

    int32_t return_status = APP_SUCCESS;

    parse_sensor_data(sensor_buffer, &sensor_data);

    (void)printf("Temperature: %lu\n",
                 (unsigned long)sensor_data.temperature);

    (void)printf("Timestamp: %lu\n",
                 (unsigned long)sensor_data.timestamp);

    if ((sensor_data.temperature != 300U) ||
        (sensor_data.timestamp != 6666U))
    {
        return_status = APP_FAILURE;
    }

    return return_status;
}
