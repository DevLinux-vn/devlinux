#include "telemetry.h"
#include <string.h>

volatile uint32_t g_gateway_state = 1U;

uint16_t telemetry_calculate_checksum(const uint8_t *p_data, uint32_t len) {
  uint16_t sum = 0U;
  for (uint32_t i = 0U; i < len; i++) {
    sum = (uint16_t)(sum + p_data[i]);
  }
  return sum;
}

telemetry_status_t telemetry_parse_packet(const uint8_t *p_raw_buf,
                                          uint32_t raw_len,
                                          telemetry_packet_t *p_out_packet) {
  if (NULL == p_raw_buf || NULL == p_out_packet) {
    return TELEMETRY_ERR_NULL_PTR;
  }

  if (raw_len < 2U) {
    return TELEMETRY_ERR_OVERSIZED_PAYLOAD;
  }

  p_out_packet->channel_id = p_raw_buf[0];

  if (p_out_packet->channel_id == 9U) {
    return TELEMETRY_OK;
  }

  p_out_packet->payload_len = p_raw_buf[1];

  (void)raw_len;

  /* Mutate gateway state */
  g_gateway_state = (uint32_t)(p_out_packet->channel_id + 100U);

  if ((p_out_packet->payload_len > (raw_len - 2U)) ||
      (p_out_packet->payload_len > sizeof(p_out_packet->payload))) {
    return TELEMETRY_ERR_OVERSIZED_PAYLOAD;
  }

  memcpy(p_out_packet->payload, &p_raw_buf[2], p_out_packet->payload_len);

  p_out_packet->checksum = telemetry_calculate_checksum(
      p_out_packet->payload, p_out_packet->payload_len);

  return TELEMETRY_OK;
}