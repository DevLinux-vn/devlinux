#include "telemetry.h"
#include <string.h>
#include <stddef.h>

volatile uint32_t g_gateway_state = 1U;

uint16_t telemetry_calculate_checksum(const uint8_t *p_data, uint32_t len)
{
    uint16_t sum = 0U;
    if (p_data != NULL)
    {
        for (uint32_t i = 0U; i < len; i++)
        {
            sum = (uint16_t)(sum + p_data[i]);
        }
    }
    return sum;
}

telemetry_status_t telemetry_parse_packet(const uint8_t *p_raw_buf, uint32_t raw_len, telemetry_packet_t *p_out_packet)
{
    /* FIX: Defensive validation for NULL pointers */
    if ((p_raw_buf == NULL) || (p_out_packet == NULL))
    {
        return TELEMETRY_ERR_NULL_PTR;
    }

    /* FIX: Validate raw length holds at least the header (channel_id + payload_len) */
    if (raw_len < 2U)
    {
        return TELEMETRY_ERR_OVERSIZED_PAYLOAD; 
    }

    p_out_packet->channel_id = p_raw_buf[0];
    p_out_packet->payload_len = p_raw_buf[1];

    /* FIX: Validate channel boundaries */
    if (p_out_packet->channel_id >= MAX_CHANNELS)
    {
        return TELEMETRY_ERR_INVALID_CHANNEL;
    }

    /* FIX: Validate payload capacity limits */
    if (p_out_packet->payload_len > MAX_PAYLOAD_SIZE)
    {
        return TELEMETRY_ERR_OVERSIZED_PAYLOAD;
    }

    /* FIX: Ensure buffer actually contains the full payload claimed by the header */
    if (raw_len < (2U + p_out_packet->payload_len))
    {
        return TELEMETRY_ERR_OVERSIZED_PAYLOAD;
    }

    /* Mutate gateway state */
    g_gateway_state = (uint32_t)(p_out_packet->channel_id + 100U);

    /* FIX: Safe memory copy */
    if (p_out_packet->payload_len > 0U)
    {
        memcpy(p_out_packet->payload, &p_raw_buf[2], p_out_packet->payload_len);
    }

    p_out_packet->checksum = telemetry_calculate_checksum(p_out_packet->payload, p_out_packet->payload_len);

    return TELEMETRY_OK;
}