#include "telemetry.h"
#include "common_types.h"
#include <stdint.h>
#include <stdio.h>

#define TLV_LENGTH_VALUE_OFFSET (1U)
#define TLV_TYPE_OFFSET (2U)
#define TLV_DATA_LENGTH_OFFSET (3U)
#define TLV_DATA_OFFSET (4U)
#define TLV_CRC_OFFSET (6U)

#define HEX_BYTE_STRING_SIZE (4U)
#define HEX_SEPARATOR_SIZE (1U)

static void pack_int16_be(uint8_t *p_buf, int16_t value) {
  const uint16_t unsigned_value = (uint16_t)value;

  p_buf[0] = (uint8_t)(unsigned_value >> 8U);
  p_buf[1] = (uint8_t)(unsigned_value & 0xFFU);
}

static int16_t unpack_int16_be(const uint8_t *p_buf) {
  const uint16_t unsigned_value =
      (uint16_t)(((uint16_t)p_buf[0] << 8U) | (uint16_t)p_buf[1]);

  return (int16_t)unsigned_value;
}

static void build_packet(
    uint8_t *p_buf,
    sensor_type_t sensor_type, /* NOLINT(bugprone-easily-swappable-parameters)
                                */
    int16_t value) {
  p_buf[0] = TLV_SYNC_BYTE;
  p_buf[1] = (uint8_t)(TLV_MAX_PACKET_SIZE - 2U);
  p_buf[TLV_TYPE_OFFSET] = (uint8_t)sensor_type;
  p_buf[TLV_DATA_LENGTH_OFFSET] = TLV_DATA_SIZE;

  pack_int16_be(&p_buf[TLV_DATA_OFFSET], value);

  p_buf[TLV_CRC_OFFSET] =
      telemetry_crc8(&p_buf[TLV_TYPE_OFFSET], TLV_CRC_OFFSET - TLV_TYPE_OFFSET);
}

static status_t validate_packet(
    const uint8_t *p_buf,
    uint32_t remaining_len, /* NOLINT(bugprone-easily-swappable-parameters) */
    sensor_type_t expected_type) {
  uint8_t expected_crc = 0U;

  if (p_buf == NULL) {
    return STATUS_ERR_NULL_PTR;
  }

  if (remaining_len < TLV_PACKET_SIZE) {
    return STATUS_ERR_INVALID_PARAM;
  }

  if (p_buf[0] != TLV_SYNC_BYTE) {
    return STATUS_ERR_INVALID_PARAM;
  }

  if (p_buf[TLV_LENGTH_VALUE_OFFSET] != (uint8_t)(TLV_MAX_PACKET_SIZE - 2U)) {
    return STATUS_ERR_INVALID_PARAM;
  }

  if (p_buf[TLV_TYPE_OFFSET] != (uint8_t)expected_type) {
    return STATUS_ERR_INVALID_PARAM;
  }

  if (p_buf[TLV_DATA_LENGTH_OFFSET] != TLV_DATA_SIZE) {
    return STATUS_ERR_INVALID_PARAM;
  }

  expected_crc =
      telemetry_crc8(&p_buf[TLV_TYPE_OFFSET], TLV_CRC_OFFSET - TLV_TYPE_OFFSET);

  if (p_buf[TLV_CRC_OFFSET] != expected_crc) {
    return STATUS_ERR_INVALID_PARAM;
  }

  return STATUS_OK;
}

status_t telemetry_serialize(const sensor_data_t *p_data, uint8_t *p_buf,
                             uint32_t buf_size, uint32_t *p_written) {
  if ((p_data == NULL) || (p_buf == NULL) || (p_written == NULL)) {
    return STATUS_ERR_NULL_PTR;
  }

  *p_written = 0U;

  if (buf_size < TELEMETRY_SERIALIZED_SIZE) {
    return STATUS_ERR_FULL;
  }

  build_packet(&p_buf[0], SENSOR_TEMPERATURE, p_data->temperature);

  build_packet(&p_buf[TLV_PACKET_SIZE], SENSOR_HUMIDITY, p_data->humidity);

  *p_written = TELEMETRY_SERIALIZED_SIZE;

  return STATUS_OK;
}

status_t telemetry_deserialize(const uint8_t *p_buf, uint32_t buf_len,
                               sensor_data_t *p_data) {
  status_t status = STATUS_OK;

  if ((p_buf == NULL) || (p_data == NULL)) {
    return STATUS_ERR_NULL_PTR;
  }

  if (buf_len < TELEMETRY_SERIALIZED_SIZE) {
    return STATUS_ERR_INVALID_PARAM;
  }

  status = validate_packet(&p_buf[0], buf_len, SENSOR_TEMPERATURE);
  if (status != STATUS_OK) {
    return status;
  }

  status = validate_packet(&p_buf[TLV_PACKET_SIZE], buf_len - TLV_PACKET_SIZE,
                           SENSOR_HUMIDITY);
  if (status != STATUS_OK) {
    return status;
  }

  p_data->temperature = unpack_int16_be(&p_buf[TLV_DATA_OFFSET]);

  p_data->humidity = unpack_int16_be(&p_buf[TLV_PACKET_SIZE + TLV_DATA_OFFSET]);

  return STATUS_OK;
}

uint8_t telemetry_crc8(const uint8_t *p_data, uint32_t len) {
  uint8_t crc = 0U;

  if ((p_data == NULL) && (len > 0U)) {
    return 0U;
  }

  for (uint32_t i = 0U; i < len; ++i) {
    crc ^= p_data[i];
  }

  return crc;
}

void telemetry_print_hex(const uint8_t *p_buf, uint32_t len) {
  if ((p_buf == NULL) && (len > 0U)) {
    return;
  }

  for (uint32_t i = 0U; i < len; ++i) {
    char hex_byte[HEX_BYTE_STRING_SIZE];

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    (void)snprintf(hex_byte, sizeof(hex_byte), "%02X", (unsigned int)p_buf[i]);

    if (i > 0U) {
      (void)printf("%c", ' ');
    }

    (void)printf("%s", hex_byte);
  }

  (void)printf("\n");
}