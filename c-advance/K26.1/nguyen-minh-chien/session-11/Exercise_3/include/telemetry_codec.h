/**
 * @file    telemetry_codec.h
 * @brief   Endian-safe telemetry serialization API (fixed-layout and TLV).
 *
 * @details This module provides two portable serialization techniques for
 *          a telemetry_t structure so that it can be transmitted safely
 *          between a Little-Endian sensor node (e.g. ARM Cortex-M4) and a
 *          Big-Endian gateway (e.g. PowerPC):
 *            - Fixed-layout serialization (Part A): a compact, fixed-size
 *              12-byte wire format using htonl()/ntohl() and memcpy() for
 *              safe float <-> uint32_t type-punning.
 *            - TLV (Type-Length-Value) framing (Part B): a self-describing,
 *              forward-compatible wire format that validates lengths before
 *              reading (Heartbleed-style bounds checking).
 *
 * @note    MISRA-C 2012 Rule 11.3 / CERT-C EXP39-C: a pointer to an object
 *          of one type shall never be cast to a pointer of a different
 *          object type. All float <-> uint32_t re-interpretation in this
 *          module is performed exclusively via memcpy().
 */

#ifndef TELEMETRY_CODEC_H
#define TELEMETRY_CODEC_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of bytes produced by serialize_telemetry() (Part A).
 *
 * @details 4 bytes device_id + 4 bytes temperature + 4 bytes humidity.
 */
#define TELEMETRY_FIXED_SIZE_BYTES (12U)

/** @brief TLV type identifier for the device_id field (uint32_t, 4 bytes). */
#define TLV_TYPE_DEVICE_ID    (0x01U)

/** @brief TLV type identifier for the temperature field (float, 4 bytes). */
#define TLV_TYPE_TEMPERATURE  (0x02U)

/** @brief TLV type identifier for the humidity field (float, 4 bytes). */
#define TLV_TYPE_HUMIDITY     (0x03U)

/** @brief Length in bytes of the Type field of a single TLV entry. */
#define TLV_TYPE_FIELD_SIZE_BYTES   (1U)

/** @brief Length in bytes of the Length field of a single TLV entry. */
#define TLV_LENGTH_FIELD_SIZE_BYTES (1U)

/** @brief Length in bytes of the Value payload for every field used here. */
#define TLV_VALUE_SIZE_BYTES        (4U)

/**
 * @brief Total size in bytes of one TLV-encoded telemetry_t (Part B).
 *
 * @details Three fields (device_id, temperature, humidity), each encoded
 *          as [Type:1][Length:1][Value:4] = 6 bytes per field, 18 total.
 */
#define TELEMETRY_TLV_SIZE_BYTES \
    (3U * (TLV_TYPE_FIELD_SIZE_BYTES + TLV_LENGTH_FIELD_SIZE_BYTES + \
           TLV_VALUE_SIZE_BYTES))

/** @brief Return code indicating successful TLV deserialization. */
#define TLV_DESERIALIZE_OK      (0)

/** @brief Return code indicating a TLV validation failure (bad length,
 *         truncated buffer, or a NULL pointer argument).
 */
#define TLV_DESERIALIZE_ERROR   (-1)

/**
 * @struct telemetry_t
 * @brief  Sensor telemetry sample.
 *
 * @var telemetry_t::device_id
 *      Unique identifier of the reporting sensor node.
 * @var telemetry_t::temperature
 *      Measured temperature, in degrees Celsius.
 * @var telemetry_t::humidity
 *      Measured relative humidity, as a percentage.
 */
typedef struct
{
    uint32_t device_id;
    float    temperature;
    float    humidity;
} telemetry_t;

/**
 * @brief   Serialize a telemetry_t into a fixed 12-byte network-order buffer.
 *
 * @details Each field is converted to network byte order with htonl().
 *          Floating point fields are first reinterpreted as uint32_t via
 *          memcpy() (never via pointer casting) to comply with strict
 *          aliasing rules (MISRA-C Rule 11.3 / CERT-C EXP39-C) before the
 *          byte swap is applied.
 *
 * @param[in]  p_src  Pointer to the source telemetry_t to serialize.
 *                    Must not be NULL.
 * @param[out] p_buf  Pointer to a caller-owned buffer of at least
 *                    TELEMETRY_FIXED_SIZE_BYTES bytes. Must not be NULL.
 *
 * @return Number of bytes written to p_buf (always TELEMETRY_FIXED_SIZE_BYTES
 *         on success), or 0U if either argument is NULL.
 */
size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf);

/**
 * @brief   Deserialize a fixed 12-byte network-order buffer into telemetry_t.
 *
 * @details Reverses serialize_telemetry(): each 4-byte field is copied out
 *          of the buffer, converted from network to host byte order with
 *          ntohl(), then copied into the destination field using memcpy()
 *          to avoid strict-aliasing violations.
 *
 * @param[in]  p_buf  Pointer to a buffer of at least
 *                    TELEMETRY_FIXED_SIZE_BYTES bytes, previously produced
 *                    by serialize_telemetry(). Must not be NULL.
 * @param[out] p_dst  Pointer to the telemetry_t to populate. Must not be
 *                    NULL.
 */
void deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst);

/**
 * @brief   Serialize a telemetry_t into a TLV-framed byte stream.
 *
 * @details Encodes device_id, temperature and humidity as three
 *          consecutive TLV triplets ([Type:1][Length:1][Value:4]), with
 *          each Value in network byte order. This format is
 *          forward-compatible: unknown readers can skip fields they do
 *          not recognise by reading the Length byte.
 *
 * @param[in]  p_src  Pointer to the source telemetry_t to serialize.
 *                    Must not be NULL.
 * @param[out] p_buf  Pointer to a caller-owned buffer of at least
 *                    TELEMETRY_TLV_SIZE_BYTES bytes. Must not be NULL.
 *
 * @return Number of bytes written to p_buf (always TELEMETRY_TLV_SIZE_BYTES
 *         on success), or 0U if either argument is NULL.
 */
size_t serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf);

/**
 * @brief   Parse a TLV-framed byte stream into a telemetry_t.
 *
 * @details Iterates over TLV entries in p_buf. For every entry the parser
 *          validates that `offset + TLV_TYPE_FIELD_SIZE_BYTES +
 *          TLV_LENGTH_FIELD_SIZE_BYTES + length <= buf_len` *before*
 *          reading the Value bytes (Heartbleed-style bounds check, per
 *          MISRA-C Directive 4.14 / CERT-C ARR30-C). Recognised types
 *          (TLV_TYPE_DEVICE_ID, TLV_TYPE_TEMPERATURE, TLV_TYPE_HUMIDITY)
 *          populate the corresponding field of *p_dst; any other type is
 *          skipped gracefully using its declared Length, so newer producer
 *          firmware may add fields without breaking older consumers.
 *
 * @param[in]  p_buf    Pointer to the TLV-encoded input buffer. Must not
 *                      be NULL unless buf_len is 0.
 * @param[in]  buf_len  Number of valid bytes available at p_buf.
 * @param[out] p_dst    Pointer to the telemetry_t to populate. Must not
 *                      be NULL.
 *
 * @return TLV_DESERIALIZE_OK (0) on success, or TLV_DESERIALIZE_ERROR (-1)
 *         if p_buf/p_dst is NULL, or any entry's declared length would
 *         read past buf_len, or a recognised field has an unexpected
 *         length.
 */
int32_t deserialize_telemetry_tlv(const uint8_t *p_buf, size_t buf_len,
                                   telemetry_t *p_dst);

#ifdef __cplusplus
}
#endif

#endif /* TELEMETRY_CODEC_H */