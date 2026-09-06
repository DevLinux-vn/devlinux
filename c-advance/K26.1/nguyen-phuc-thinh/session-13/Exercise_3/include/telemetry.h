#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PAYLOAD_SIZE 64U
#define MAX_CHANNELS     8U

/**
 * @brief Telemetry packet parsing status codes.
 * Indicates success (TELEMETRY_OK) or specific failure reason (NULL_PTR, INVALID_CHANNEL, etc.).
 */
typedef enum {
    TELEMETRY_OK = 0,
    TELEMETRY_ERR_NULL_PTR,
    TELEMETRY_ERR_INVALID_CHANNEL,
    TELEMETRY_ERR_OVERSIZED_PAYLOAD,
    TELEMETRY_ERR_CHECKSUM_MISMATCH
} telemetry_status_t;

typedef struct {
    uint8_t channel_id;
    uint8_t payload_len;
    uint8_t payload[MAX_PAYLOAD_SIZE];
    uint16_t checksum;
} telemetry_packet_t;

/**
 * @brief Gateway state register - updated by telemetry_parse_packet().
 * @note NOT thread-safe. For single-threaded or externally synchronized access.
 */
extern volatile uint32_t g_gateway_state;

telemetry_status_t telemetry_parse_packet(const uint8_t *p_raw_buf, uint32_t raw_len, telemetry_packet_t *p_out_packet);
uint16_t           telemetry_calculate_checksum(const uint8_t *p_data, uint32_t len);

#endif /* TELEMETRY_H */