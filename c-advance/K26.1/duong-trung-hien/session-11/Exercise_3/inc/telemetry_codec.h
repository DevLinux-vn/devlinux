#ifndef TELEMETRY_CODEC_H
#define TELEMETRY_CODEC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define ERROR           (0)
#define SUCCESS         (0)
#define FAILED          (-1)
#define TYPE_SIZE       (1U)
#define LENGTH_SIZE     (1U)
#define EPSILON         (1e-6)

#define TLV_TYPE_DEVICE_ID    0x01
#define TLV_TYPE_TEMPERATURE  0x02
#define TLV_TYPE_HUMIDITY     0x03

/**
 * @brief Telemetry data structure.
 */
typedef struct {
    uint32_t device_id;   /**< Device identifier. */
    float    temperature; /**< Temperature value in degrees Celsius. */
    float    humidity;    /**< Relative humidity in percent. */
} telemetry_t;

/**
 * @brief Serialize telemetry data using a fixed binary layout.
 *
 * @param[in]  p_src Source telemetry data.
 * @param[out] p_buf Destination buffer.
 *
 * @return Number of bytes written to the buffer.
 */
size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf);

/**
 * @brief Deserialize telemetry data from a fixed binary layout.
 *
 * @param[in]  p_buf Source buffer containing serialized data.
 * @param[out] p_dst Destination telemetry structure.
 */
void deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst);

/**
 * @brief Serialize telemetry data using TLV encoding.
 *
 * @param[in]  p_src Source telemetry data.
 * @param[out] p_buf Destination buffer.
 *
 * @return Number of bytes written to the buffer.
 */
size_t serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf);

/**
 * @brief Deserialize telemetry data from a TLV-encoded buffer.
 *
 * Unknown TLV types are ignored during deserialization.
 *
 * @param[in]  p_buf Source buffer containing TLV-encoded data.
 * @param[in]  buf_len Length of the input buffer in bytes.
 * @param[out] p_dst Destination telemetry structure.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int32_t deserialize_telemetry_tlv(const uint8_t *p_buf,
                                   size_t buf_len,
                                   telemetry_t *p_dst);

/**
 * @brief Check whether two telemetry structures are equal within a tolerance.
 *
 * @param[in] p_data_1 First telemetry data structure.
 * @param[in] p_data_2 Second telemetry data structure.
 * @param[in] epsilon Maximum allowed difference between floating-point values.
 *
 * @return A string describing the comparison result.
 */
const char *check_round_trip(const telemetry_t *p_data_1,
                             const telemetry_t *p_data_2,
                             float epsilon);

#endif /* TELEMETRY_CODEC_H */