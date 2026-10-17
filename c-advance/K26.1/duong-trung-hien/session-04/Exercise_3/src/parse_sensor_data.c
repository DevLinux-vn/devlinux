#include "parse_sensor_data.h"

void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data)
{
    if ((p_buffer == NULL) | (p_out_data == NULL))
    {
        return;
    }
    
    p_out_data->temperature =
        ((uint16_t)*(p_buffer + 0U) << 8U) |
        ((uint16_t)*(p_buffer + 1U));

    p_out_data->timestamp =
        ((uint32_t)*(p_buffer + 2U) << 24U) |
        ((uint32_t)*(p_buffer + 3U) << 16U) |
        ((uint32_t)*(p_buffer + 4U) << 8U)  |
        ((uint32_t)*(p_buffer + 5U));

}