/**
 * @file telemetry_codec.h
 * @brief Telemetry serialization and TLV framing API.
 */
#ifndef TELEMETRY_CODEC_H
#define TELEMETRY_CODEC_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Telemetry data structure.
 */
typedef struct {
    uint32_t device_id;
    float    temperature; /* degrees Celsius */
    float    humidity;    /* percentage */
} telemetry_t;

/* TLV Type Constants */
#define TLV_TYPE_DEVICE_ID   0x01
#define TLV_TYPE_TEMPERATURE 0x02
#define TLV_TYPE_HUMIDITY    0x03

/**
 * @brief Serializes a telemetry struct using a fixed 12-byte layout.
 * 
 * @param[in]  p_src Source telemetry structure.
 * @param[out] p_buf Destination buffer (must be at least 12 bytes).
 * @return size_t Number of bytes written (12).
 */
size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf);

/**
 * @brief Deserializes a fixed 12-byte layout into a telemetry struct.
 * 
 * @param[in]  p_buf Source buffer containing serialized data.
 * @param[out] p_dst Destination telemetry structure.
 */
void deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst);

/**
 * @brief Serializes a telemetry struct using TLV framing.
 * 
 * @param[in]  p_src Source telemetry structure.
 * @param[out] p_buf Destination buffer.
 * @return size_t Number of bytes written.
 */
size_t serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf);

/**
 * @brief Deserializes a TLV formatted buffer into a telemetry struct.
 * 
 * @param[in]  p_buf   Source buffer containing TLV data.
 * @param[in]  buf_len Length of the provided buffer.
 * @param[out] p_dst   Destination telemetry structure.
 * @return int32_t 0 on success, -1 on validation failure.
 */
int32_t deserialize_telemetry_tlv(const uint8_t *p_buf, size_t buf_len, telemetry_t *p_dst);

#endif /* TELEMETRY_CODEC_H */