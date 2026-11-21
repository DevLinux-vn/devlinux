/**
 * @file main.c
 * @brief Application demonstrating endian-safe protocol parsing.
 * @author Tran Doan Trung
 * @date 2026-11-21
 */

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/**
 * @brief Length of the raw sensor data payload in bytes.
 */
#define SENSOR_PAYLOAD_LEN 6U

/**
 * @brief Structure containing parsed sensor metrics.
 */
typedef struct {
    uint16_t temperature; /**< Temperature in engineering units (16-bit unsigned) */
    uint32_t timestamp;   /**< Measurement timestamp in seconds (32-bit unsigned) */
} sensor_data_t;

/**
 * @brief Parses big-endian raw sensor data into a struct safely.
 *
 * Reconstructs 16-bit temperature and 32-bit timestamp using bitwise shift
 * and OR operations, ensuring portability across architectures of any endianness.
 *
 * @param[in]  p_buffer    Pointer to the 6-byte raw payload array.
 * @param[out] p_out_data  Pointer to the struct to populate with parsed values.
 */
static void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data) {
    if ((p_buffer == NULL) || (p_out_data == NULL)) {
        return; /* Defensive check against NULL pointers */
    }

    /*
     * Reconstruct 16-bit temperature from Big-Endian:
     * Byte 0: Bits 15..8 (MSB)
     * Byte 1: Bits 7..0  (LSB)
     */
    const uint16_t temp_msb = (uint16_t)((uint16_t)p_buffer[0] << 8U);
    const uint16_t temp_lsb = (uint16_t)p_buffer[1];
    p_out_data->temperature = (uint16_t)(temp_msb | temp_lsb);

    /*
     * Reconstruct 32-bit timestamp from Big-Endian:
     * Byte 2: Bits 31..24 (MSB)
     * Byte 3: Bits 23..16
     * Byte 4: Bits 15..8
     * Byte 5: Bits 7..0   (LSB)
     */
    const uint32_t b2 = (uint32_t)((uint32_t)p_buffer[2] << 24U);
    const uint32_t b3 = (uint32_t)((uint32_t)p_buffer[3] << 16U);
    const uint32_t b4 = (uint32_t)((uint32_t)p_buffer[4] << 8U);
    const uint32_t b5 = (uint32_t)p_buffer[5];
    p_out_data->timestamp = b2 | b3 | b4 | b5;
}

/**
 * @brief Main entry point simulating reception and parsing of raw sensor frame.
 * @return 0 on success.
 */
int main(void) {
    const uint8_t raw_payload[SENSOR_PAYLOAD_LEN] = {
        0x01U, 0x2CU, 0x00U, 0x00U, 0x1AU, 0x0AU
    };

    sensor_data_t sensor_data = {
        .temperature = 0U,
        .timestamp = 0U
    };

    parse_sensor_data(raw_payload, &sensor_data);

    (void)printf("Temperature: %u\n", (unsigned int)sensor_data.temperature);
    (void)printf("Timestamp: %u\n", (unsigned int)sensor_data.timestamp);

    return 0;
}
