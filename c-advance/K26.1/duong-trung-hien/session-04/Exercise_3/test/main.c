#include "parse_sensor_data.h"
#include <stdio.h>

int main()
{
    const uint8_t p_buffer[] = {0x01, 0x2C, 0x00, 0x00, 0x1A, 0x0A};
    sensor_data_t out_data = {0U, 0U};

    parse_sensor_data(p_buffer, &out_data);

    printf("Temperature: %u\n", out_data.temperature);
    printf("Timestamp: %u\n", out_data.timestamp);

    return 0;
}