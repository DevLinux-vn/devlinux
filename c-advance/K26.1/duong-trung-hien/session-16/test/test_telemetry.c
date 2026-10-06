#include "common_types.h"
#include "telemetry.h"
#include "unity.h"
#include <stddef.h>
#include <stdint.h>
#include <unity_internals.h>

#define TEST_TLV_DATA_LENGTH_OFFSET (3U)
#define TEST_TLV_DATA_OFFSET (4U)
#define TEST_TLV_CRC_OFFSET (6U)

static sensor_data_t s_sensor_data;
static uint8_t s_buffer[TELEMETRY_SERIALIZED_SIZE];

/** @brief Set up test fixtures before each test. */
void setUp(void);

/** @brief Clean up test fixtures after each test. */
void tearDown(void);

/** @brief Verify serialization and deserialization produce the original data.
 */
void test_serialize_deserialize_round_trip(void);

/** @brief Verify a frame with an invalid synchronization byte is rejected. */
void test_corrupt_sync_byte_rejected(void);

/** @brief Verify a frame with an invalid CRC is rejected. */
void test_corrupt_crc_rejected(void);

/** @brief Verify serialization is rejected when the output buffer is too small.
 */
void test_buffer_too_small_rejected(void);

/** @brief Verify serialization and deserialization reject null pointers. */
void test_null_pointer_rejected(void);

/** @brief Verify serialization rejects data exceeding the maximum supported
 * size. */
void test_oversized_data_rejected(void);

/** @brief Verify the CRC-8 XOR calculation produces the expected result. */
void test_crc8_xor_calculation(void);

/** @brief Verify multi-byte fields are serialized in big-endian byte order. */
void test_big_endian_serialization(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_deserialize_round_trip);
  RUN_TEST(test_corrupt_sync_byte_rejected);
  RUN_TEST(test_corrupt_crc_rejected);
  RUN_TEST(test_buffer_too_small_rejected);
  RUN_TEST(test_null_pointer_rejected);
  RUN_TEST(test_oversized_data_rejected);
  RUN_TEST(test_crc8_xor_calculation);
  RUN_TEST(test_big_endian_serialization);

  return UNITY_END();
}

void setUp(void) {
  s_sensor_data.temperature = 253;
  s_sensor_data.humidity = 601;
  s_sensor_data.timestamp = 12345U;

  for (uint32_t i = 0U; i < sizeof(s_buffer); ++i) {
    s_buffer[i] = 0U;
  }
}

void tearDown(void) {}

void test_serialize_deserialize_round_trip(void) {
  sensor_data_t recovered_data = {0};
  uint32_t written = 0U;

  TEST_ASSERT_EQUAL(STATUS_OK, telemetry_serialize(&s_sensor_data, s_buffer,
                                                   sizeof(s_buffer), &written));

  TEST_ASSERT_EQUAL_UINT32(TELEMETRY_SERIALIZED_SIZE, written);

  TEST_ASSERT_EQUAL(STATUS_OK,
                    telemetry_deserialize(s_buffer, written, &recovered_data));

  TEST_ASSERT_EQUAL_INT16(s_sensor_data.temperature,
                          recovered_data.temperature);

  TEST_ASSERT_EQUAL_INT16(s_sensor_data.humidity, recovered_data.humidity);
}

void test_corrupt_sync_byte_rejected(void) {
  uint32_t written = 0U;
  sensor_data_t recovered_data = {0};

  TEST_ASSERT_EQUAL(STATUS_OK, telemetry_serialize(&s_sensor_data, s_buffer,
                                                   sizeof(s_buffer), &written));

  s_buffer[0] = 0x55U;

  TEST_ASSERT_EQUAL(STATUS_ERR_INVALID_PARAM,
                    telemetry_deserialize(s_buffer, written, &recovered_data));
}

void test_corrupt_crc_rejected(void) {
  uint32_t written = 0U;
  sensor_data_t recovered_data = {0};

  TEST_ASSERT_EQUAL(STATUS_OK, telemetry_serialize(&s_sensor_data, s_buffer,
                                                   sizeof(s_buffer), &written));

  s_buffer[TEST_TLV_CRC_OFFSET] ^= 0x01U;

  TEST_ASSERT_EQUAL(STATUS_ERR_INVALID_PARAM,
                    telemetry_deserialize(s_buffer, written, &recovered_data));
}

void test_buffer_too_small_rejected(void) {
  uint8_t small_buffer[TELEMETRY_SERIALIZED_SIZE - 1U];
  uint32_t written = 999U;

  TEST_ASSERT_EQUAL(STATUS_ERR_FULL,
                    telemetry_serialize(&s_sensor_data, small_buffer,
                                        sizeof(small_buffer), &written));

  TEST_ASSERT_EQUAL_UINT32(0U, written);
}

void test_null_pointer_rejected(void) {
  uint32_t written = 0U;
  sensor_data_t recovered_data = {0};

  TEST_ASSERT_EQUAL(
      STATUS_ERR_NULL_PTR,
      telemetry_serialize(NULL, s_buffer, sizeof(s_buffer), &written));

  TEST_ASSERT_EQUAL(
      STATUS_ERR_NULL_PTR,
      telemetry_serialize(&s_sensor_data, NULL, sizeof(s_buffer), &written));

  TEST_ASSERT_EQUAL(
      STATUS_ERR_NULL_PTR,
      telemetry_serialize(&s_sensor_data, s_buffer, sizeof(s_buffer), NULL));

  TEST_ASSERT_EQUAL(
      STATUS_ERR_NULL_PTR,
      telemetry_deserialize(NULL, sizeof(s_buffer), &recovered_data));

  TEST_ASSERT_EQUAL(STATUS_ERR_NULL_PTR,
                    telemetry_deserialize(s_buffer, sizeof(s_buffer), NULL));
}

void test_oversized_data_rejected(void) {
  uint32_t written = 0U;
  sensor_data_t recovered_data = {0};

  TEST_ASSERT_EQUAL(STATUS_OK, telemetry_serialize(&s_sensor_data, s_buffer,
                                                   sizeof(s_buffer), &written));

  s_buffer[TEST_TLV_DATA_LENGTH_OFFSET] = TLV_DATA_SIZE + 1U;

  TEST_ASSERT_EQUAL(STATUS_ERR_INVALID_PARAM,
                    telemetry_deserialize(s_buffer, written, &recovered_data));
}

void test_crc8_xor_calculation(void) {
  const uint8_t data[] = {0x00U, 0x02U, 0x00U, 0xFDU};

  TEST_ASSERT_EQUAL_HEX8(0xFFU, telemetry_crc8(data, sizeof(data)));
}

void test_big_endian_serialization(void) {
  uint32_t written = 0U;

  TEST_ASSERT_EQUAL(STATUS_OK, telemetry_serialize(&s_sensor_data, s_buffer,
                                                   sizeof(s_buffer), &written));

  TEST_ASSERT_EQUAL_UINT8(0x00U, s_buffer[4]);
  TEST_ASSERT_EQUAL_UINT8(0xFDU, s_buffer[5]);

  TEST_ASSERT_EQUAL_UINT8(0x02U, s_buffer[TLV_PACKET_SIZE + 4U]);
  TEST_ASSERT_EQUAL_UINT8(0x59U, s_buffer[TLV_PACKET_SIZE + 5U]);
}