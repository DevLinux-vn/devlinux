/**
 * @file test_telemetry_codec.c
 * @brief Unity unit tests for the telemetry codec module.
 */
#include "unity.h"
#include "telemetry_codec.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_serialize_fixed_size(void) 
{
    telemetry_t t = { 4096, 24.50f, 60.20f };
    uint8_t buf[12];
    size_t size = serialize_telemetry(&t, buf);
    TEST_ASSERT_EQUAL_size_t(12, size);
}

void test_roundtrip_fixed_layout(void) 
{
    telemetry_t src = { 4096, 24.50f, 60.20f };
    telemetry_t dst;
    uint8_t buf[12];
    
    serialize_telemetry(&src, buf);
    deserialize_telemetry(buf, &dst);
    
    TEST_ASSERT_EQUAL_UINT32(src.device_id, dst.device_id);
    TEST_ASSERT_EQUAL_FLOAT(src.temperature, dst.temperature);
    TEST_ASSERT_EQUAL_FLOAT(src.humidity, dst.humidity);
}

void test_float_byte_swap_safe(void) 
{
    /* Passing this test demonstrates strict aliasing avoidance worked */
    telemetry_t src = { 1, 123.456f, 78.9f };
    telemetry_t dst;
    uint8_t buf[12];
    serialize_telemetry(&src, buf);
    deserialize_telemetry(buf, &dst);
    TEST_ASSERT_EQUAL_FLOAT(src.temperature, dst.temperature);
}

void test_serialize_tlv_size(void) 
{
    telemetry_t t = { 4096, 24.50f, 60.20f };
    uint8_t buf[32];
    size_t size = serialize_telemetry_tlv(&t, buf);
    TEST_ASSERT_EQUAL_size_t(18, size); /* 3 fields * (1 type + 1 len + 4 data) */
}

void test_roundtrip_tlv(void) 
{
    telemetry_t src = { 4096, 24.50f, 60.20f };
    telemetry_t dst;
    uint8_t buf[32];
    
    size_t size = serialize_telemetry_tlv(&src, buf);
    int32_t res = deserialize_telemetry_tlv(buf, size, &dst);
    
    TEST_ASSERT_EQUAL_INT32(0, res);
    TEST_ASSERT_EQUAL_UINT32(src.device_id, dst.device_id);
}

void test_tlv_rejects_short_buffer(void) 
{
    telemetry_t dst;
    /* Buffer claims 4 bytes of length, but only 2 actual payload bytes exist */
    uint8_t malformed[] = { 0x01, 0x04, 0x00, 0x10 }; 
    int32_t res = deserialize_telemetry_tlv(malformed, sizeof(malformed), &dst);
    TEST_ASSERT_EQUAL_INT32(-1, res);
}

void test_tlv_skips_unknown_type(void) 
{
    telemetry_t dst = {0};
    /* Unknown Type 0xFF with length 2, followed by valid Device ID */
    uint8_t buf[] = { 
        0xFF, 0x02, 0xAA, 0xBB, 
        0x01, 0x04, 0x00, 0x00, 0x10, 0x00 
    };
    int32_t res = deserialize_telemetry_tlv(buf, sizeof(buf), &dst);
    TEST_ASSERT_EQUAL_INT32(0, res);
    TEST_ASSERT_EQUAL_UINT32(4096, dst.device_id);
}

void test_tlv_null_pointer(void) 
{
    telemetry_t dst;
    uint8_t buf[18];
    TEST_ASSERT_EQUAL_size_t(0, serialize_telemetry_tlv(NULL, buf));
    TEST_ASSERT_EQUAL_INT32(-1, deserialize_telemetry_tlv(NULL, 18, &dst));
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_serialize_fixed_size);
    RUN_TEST(test_roundtrip_fixed_layout);
    RUN_TEST(test_float_byte_swap_safe);
    RUN_TEST(test_serialize_tlv_size);
    RUN_TEST(test_roundtrip_tlv);
    RUN_TEST(test_tlv_rejects_short_buffer);
    RUN_TEST(test_tlv_skips_unknown_type);
    RUN_TEST(test_tlv_null_pointer);
    return UNITY_END();
}