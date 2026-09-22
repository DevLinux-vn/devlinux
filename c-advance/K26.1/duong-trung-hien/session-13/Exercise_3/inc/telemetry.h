#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_PAYLOAD_SIZE 64U
#define MAX_CHANNELS 8U

/**
 * @brief Telemetry operation status codes.
 */
typedef enum
{
    TELEMETRY_OK = 0,
    TELEMETRY_ERR_NULL_PTR,
    TELEMETRY_ERR_INVALID_CHANNEL,
    TELEMETRY_ERR_OVERSIZED_PAYLOAD,
    TELEMETRY_ERR_CHECKSUM_MISMATCH
} telemetry_status_t;

/**
 * @brief Represents a telemetry packet.
 */
typedef struct
{
    uint8_t channel_id;                  /**< Telemetry channel identifier. */
    uint8_t payload_len;                 /**< Payload length in bytes. */
    uint8_t payload[MAX_PAYLOAD_SIZE];   /**< Payload data. */
    uint16_t checksum;                   /**< Packet checksum. */
} telemetry_packet_t;

/**
 * @brief Stores the current gateway state.
 */
extern volatile uint32_t g_gateway_state;

/**
 * @brief Parses a raw telemetry packet.
 *
 * @param[in]  p_raw_buf    Pointer to the raw packet buffer.
 * @param[in]  raw_len      Length of the raw packet buffer in bytes.
 * @param[out] p_out_packet Pointer to the output telemetry packet.
 *
 * @return TELEMETRY_OK if the packet is parsed successfully.
 * @return TELEMETRY_ERR_NULL_PTR if an input or output pointer is NULL.
 * @return TELEMETRY_ERR_INVALID_CHANNEL if the channel ID is invalid.
 * @return TELEMETRY_ERR_OVERSIZED_PAYLOAD if the payload exceeds the
 *         maximum allowed size.
 * @return TELEMETRY_ERR_CHECKSUM_MISMATCH if the packet checksum is invalid.
 */
telemetry_status_t telemetry_parse_packet(const uint8_t *p_raw_buf,
                                          uint32_t raw_len,
                                          telemetry_packet_t *p_out_packet);

/**
 * @brief Calculates the checksum of a data buffer.
 *
 * @param[in] p_data Pointer to the data buffer.
 * @param[in] len    Length of the data buffer in bytes.
 *
 * @return Calculated 16-bit checksum.
 */
uint16_t telemetry_calculate_checksum(const uint8_t *p_data, uint32_t len);

#endif /* TELEMETRY_H */