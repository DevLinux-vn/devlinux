#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "common_types.h"
#include <stdint.h>

/** @brief TLV synchronization byte. */
#define TLV_SYNC_BYTE (0xAAU)

/**
 * @brief Maximum size of one TLV telemetry packet in bytes.
 *
 * Packet format:
 * SYNC(1) + LEN(1) + TYPE(1) + DATA_LEN(1) + DATA(2) + CRC(1) = 7 bytes.
 */
#define TLV_MAX_PACKET_SIZE (7U)

/** @brief TLV header size including synchronization byte. */
#define TLV_HEADER_SIZE (4U)

/** @brief Size of one sensor value in bytes. */
#define TLV_DATA_SIZE (2U)

/** @brief Complete packet size for a sensor value. */
#define TLV_PACKET_SIZE (TLV_HEADER_SIZE + TLV_DATA_SIZE + 1U)

/** @brief Number of telemetry packets in a serialized sensor reading. */
#define TELEMETRY_PACKET_COUNT (2U)

/** @brief Total serialized telemetry size. */
#define TELEMETRY_SERIALIZED_SIZE (TLV_PACKET_SIZE * TELEMETRY_PACKET_COUNT)

/**
 * @brief Serialize sensor data into two consecutive TLV packets.
 *
 * Temperature and humidity are serialized as signed 16-bit values in
 * big-endian byte order.
 *
 * @param[in] p_data Pointer to sensor data.
 * @param[out] p_buf Destination buffer.
 * @param[in] buf_size Destination buffer size in bytes.
 * @param[out] p_written Number of bytes written.
 *
 * @return STATUS_OK if serialization succeeds.
 * @return STATUS_ERR_NULL_PTR if an input/output pointer is NULL.
 * @return STATUS_ERR_FULL if the destination buffer is too small.
 */
status_t telemetry_serialize(const sensor_data_t *p_data, uint8_t *p_buf,
                             uint32_t buf_size, uint32_t *p_written);

/**
 * @brief Deserialize two TLV telemetry packets.
 *
 * The function validates synchronization bytes, packet lengths, sensor
 * types, data lengths, CRC values, and buffer boundaries.
 *
 * @param[in] p_buf Serialized telemetry buffer.
 * @param[in] buf_len Buffer length in bytes.
 * @param[out] p_data Destination sensor data structure.
 *
 * @return STATUS_OK if deserialization succeeds.
 * @return STATUS_ERR_NULL_PTR if a pointer is NULL.
 * @return STATUS_ERR_INVALID_PARAM if the packet is invalid.
 */
status_t telemetry_deserialize(const uint8_t *p_buf, uint32_t buf_len,
                               sensor_data_t *p_data);

/**
 * @brief Calculate the XOR-based CRC-8 checksum.
 *
 * The checksum is calculated over the supplied byte range.
 *
 * @param[in] p_data Data to checksum.
 * @param[in] len Number of bytes.
 *
 * @return XOR checksum value.
 */
uint8_t telemetry_crc8(const uint8_t *p_data, uint32_t len);

/**
 * @brief Print a buffer as a hexadecimal string.
 *
 * @param[in] p_buf Buffer to print.
 * @param[in] len Number of bytes to print.
 */
void telemetry_print_hex(const uint8_t *p_buf, uint32_t len);

#endif /* TELEMETRY_H */