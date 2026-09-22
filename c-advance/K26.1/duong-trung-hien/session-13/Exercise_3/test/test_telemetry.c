#include "unity.h"
#include "telemetry.h"
#include <string.h>

void setUp(void);

void tearDown(void);

void test_telemetry_parse_valid_packet(void);

void test_telemetry_parse_null_pointer_rejected(void);

void test_telemetry_parse_oversized_payload_rejected(void);

void test_telemetry_parse_invalid_channel_rejected(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_telemetry_parse_valid_packet);
  RUN_TEST(test_telemetry_parse_null_pointer_rejected);
  RUN_TEST(test_telemetry_parse_oversized_payload_rejected);
  RUN_TEST(test_telemetry_parse_invalid_channel_rejected);

  return UNITY_END();
}

void setUp(void) {}

void tearDown(void) {}

void test_telemetry_parse_valid_packet(void)
{
    uint8_t raw_buf[] = {1U, 2U, 0xAAU, 0x55U};
    telemetry_packet_t packet;

    telemetry_status_t status = telemetry_parse_packet(raw_buf, sizeof(raw_buf), &packet);

    TEST_ASSERT_EQUAL_INT(TELEMETRY_OK, status);
    TEST_ASSERT_EQUAL_UINT8(1U, packet.channel_id);
    TEST_ASSERT_EQUAL_UINT8(2U, packet.payload_len);
    TEST_ASSERT_EQUAL_UINT8(0xAAU, packet.payload[0]);
    TEST_ASSERT_EQUAL_UINT8(0x55U, packet.payload[1]);
}

void test_telemetry_parse_null_pointer_rejected(void)
{
    uint8_t raw_buf[] = {1U, 2U, 0xAAU, 0x55U};
    telemetry_packet_t packet;

    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_NULL_PTR, telemetry_parse_packet(NULL, sizeof(raw_buf), &packet));

    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_NULL_PTR, telemetry_parse_packet(raw_buf, sizeof(raw_buf), NULL));
}

void test_telemetry_parse_oversized_payload_rejected(void)
{
    uint8_t raw_buf[32U] = {0U};
    telemetry_packet_t packet;

    raw_buf[0] = 1U;
    raw_buf[1] = 31U;

    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_OVERSIZED_PAYLOAD, telemetry_parse_packet(raw_buf, sizeof(raw_buf), &packet));
}

void test_telemetry_parse_invalid_channel_rejected(void)
{
    uint8_t raw_buf[] = {9U, 2U, 0xAAU, 0x55U};
    telemetry_packet_t packet;

    TEST_ASSERT_EQUAL_INT(TELEMETRY_OK, telemetry_parse_packet(raw_buf, sizeof(raw_buf), &packet));
}