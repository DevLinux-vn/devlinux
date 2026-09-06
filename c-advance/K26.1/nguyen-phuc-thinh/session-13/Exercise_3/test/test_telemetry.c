#include "unity.h"
#include "telemetry.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_telemetry_parse_valid_packet(void)
{
    telemetry_packet_t pkt;
    uint8_t raw[] = {0x01U, 0x02U, 0xAAU, 0xBBU};
    TEST_ASSERT_EQUAL_INT(TELEMETRY_OK, telemetry_parse_packet(raw, sizeof(raw), &pkt));
    TEST_ASSERT_EQUAL_UINT8(1U, pkt.channel_id);
    TEST_ASSERT_EQUAL_UINT8(2U, pkt.payload_len);
}

void test_telemetry_parse_null_pointer_rejected(void)
{
    telemetry_packet_t pkt;
    uint8_t raw[] = {0x01U, 0x01U, 0xAAU};
    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_NULL_PTR, telemetry_parse_packet(NULL, 3U, &pkt));
    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_NULL_PTR, telemetry_parse_packet(raw, 3U, NULL));
}

void test_telemetry_parse_oversized_payload_rejected(void)
{
    telemetry_packet_t pkt;
    /* Payload len indicates 100, which exceeds MAX_PAYLOAD_SIZE (64) */
    uint8_t raw[102] = {0x01U, 100U}; 
    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_OVERSIZED_PAYLOAD, telemetry_parse_packet(raw, sizeof(raw), &pkt));
    
    /* Payload len indicates 10, but buffer is only 5 bytes long */
    uint8_t short_raw[5] = {0x01U, 10U, 0xAAU, 0xBBU, 0xCCU};
    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_OVERSIZED_PAYLOAD, telemetry_parse_packet(short_raw, sizeof(short_raw), &pkt));
}

void test_telemetry_parse_invalid_channel_rejected(void)
{
    telemetry_packet_t pkt;
    /* Channel ID 9 exceeds MAX_CHANNELS (8) */
    uint8_t raw[] = {0x09U, 0x02U, 0xAAU, 0xBBU};
    TEST_ASSERT_EQUAL_INT(TELEMETRY_ERR_INVALID_CHANNEL, telemetry_parse_packet(raw, sizeof(raw), &pkt));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_telemetry_parse_valid_packet);
    RUN_TEST(test_telemetry_parse_null_pointer_rejected);
    RUN_TEST(test_telemetry_parse_oversized_payload_rejected);
    RUN_TEST(test_telemetry_parse_invalid_channel_rejected);
    return UNITY_END();
}