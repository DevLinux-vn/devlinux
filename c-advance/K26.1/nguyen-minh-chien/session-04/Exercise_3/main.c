#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint16_t temperature;
    uint32_t timestamp;
} sensor_data_t;

/**
 * @brief Parses big-endian raw sensor data into a struct safely.
 *
 * @param[in]  p_buffer    Pointer to the 6-byte raw payload array.
 * @param[out] p_out_data  Pointer to the struct to populate.
 */
void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data)
{
    if ((NULL == p_buffer) || (NULL == p_out_data))
    {
        return;
    }

    p_out_data->temperature = (uint16_t)(((uint16_t)p_buffer[0] << 8) | (uint16_t)p_buffer[1]);

    p_out_data->timestamp = (uint32_t)(((uint32_t)p_buffer[2] << 24) |
                                        ((uint32_t)p_buffer[3] << 16) |
                                        ((uint32_t)p_buffer[4] << 8)  |
                                        ((uint32_t)p_buffer[5] << 0));
}

int main(void)
{
    uint8_t raw_data[6]={0x01, 0x2C, 0x00, 0x00, 0x1A, 0x0A};

    sensor_data_t sensor_data;

    parse_sensor_data(raw_data, &sensor_data);

    printf("Temperature: %u\n", sensor_data.temperature);
    printf("Timestamp: %u\n", sensor_data.timestamp);

    return 0;
}