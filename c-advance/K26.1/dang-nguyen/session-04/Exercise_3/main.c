#include <stdint.h>
#include <stdio.h>

#define BYTE_LEN        (8U)
#define APP_SUCCESS     (0)
#define BUFFER_LEN      (6U)

#define ARRAY_SIZE(a)   (uint32_t)(sizeof(a) / sizeof((a)[0]))

typedef struct st_sensor_data {
    uint16_t temperature;
    uint32_t timestamp;
} st_sensor_data_t;

/**
 * @brief Parses big-endian raw sensor data into a struct safely.
 *
 * @param[in]  p_buffer    Pointer to the 6-byte raw payload array.
 * @param[in]  buff_len     The number of p_buffer's elements.
 * @param[out] p_out_data  Pointer to the struct to populate.
 */
void parse_sensor_data(const uint8_t *p_buffer, const uint32_t buff_len, st_sensor_data_t *const p_out_data);

void parse_sensor_data(const uint8_t *p_buffer, const uint32_t buff_len, st_sensor_data_t *const p_out_data)
{
    if (NULL == p_buffer)
    {
        printf("[ERROR] %s: p_buffer is NULL!\n", __func__);
    }
    else if (NULL == p_out_data)
    {
        printf("[ERROR] %s: p_out_data is NULL!\n", __func__);
    }
    else if (BUFFER_LEN < buff_len)
    {
        printf("[ERROR] %s: buff_len %u is larger than %u!\n", __func__, buff_len, BUFFER_LEN);
    }
    else
    {
        /*
         * NOT allowed to cast the uint8_t* buffer to a uint16_t* or uint32_t*.
         * This is strictly forbidden as it causes Unaligned Memory Access hardware faults.
         * 
         * Must use bitwise shifts (<<, >>) and bitwise OR (|) to reconstruct the
         * integers byte-by-byte. This guarantees endian-safe execution.
         */
        /* Reorder the bytes for temperature: 0x2C01 -> 0x012C */
        p_out_data->temperature = (((uint16_t)p_buffer[0]) << BYTE_LEN) | (uint16_t)p_buffer[1];    // NOLINT(readability-magic-numbers)

        /* Reorder the bytes for timestamp: 0x0A1A0000 -> 0x00001A0A */
        p_out_data->timestamp = (((uint32_t)p_buffer[2]) << (3 * BYTE_LEN)) |   // NOLINT(readability-magic-numbers)
                                (((uint32_t)p_buffer[3]) << (2 * BYTE_LEN)) |   // NOLINT(readability-magic-numbers)
                                (((uint32_t)p_buffer[4]) << (1 * BYTE_LEN)) |   // NOLINT(readability-magic-numbers)
                                (uint32_t)p_buffer[5];                          // NOLINT(readability-magic-numbers)
    }
}

int32_t main(void)
{
    uint8_t buffer[] = {0x01, 0x2C, 0x00, 0x00, 0x1A, 0x0A};    // NOLINT(readability-magic-numbers)
    st_sensor_data_t output =
    {
        .temperature = 0U,
        .timestamp   = 0U
    };

    parse_sensor_data(buffer, ARRAY_SIZE(buffer), &output);

    printf("Temperature: %u\n", output.temperature);
    printf("Timestamp: %u\n", output.timestamp);

    return APP_SUCCESS;
}