#include "telemetry_codec.h"
#include "unity.h"

#define BUFFER_SIZE     (64U)

/**
 * @brief Set up test fixtures before each test case.
 */
void setUp(void);

/**
 * @brief Clean up test fixtures after each test case.
 */
void tearDown(void);

/**
 * @brief Test that fixed-layout serialization produces the expected size.
 */
void test_serialize_fixed_size(void);

/**
 * @brief Test round-trip serialization and deserialization using fixed layout.
 */
void test_roundtrip_fixed_layout(void);

/**
 * @brief Test safe byte swapping of a 32-bit floating-point value.
 */
void test_float_byte_swap_safe(void);

/**
 * @brief Test that TLV serialization produces the expected size.
 */
void test_serialize_tlv_size(void);

/**
 * @brief Test round-trip serialization and deserialization using TLV encoding.
 */
void test_roundtrip_tlv(void);

/**
 * @brief Test that TLV deserialization rejects a buffer that is too short.
 */
void test_tlv_rejects_short_buffer(void);

/**
 * @brief Test that TLV deserialization skips unknown TLV types.
 */
void test_tlv_skips_unknown_type(void);

/**
 * @brief Test TLV serialization and deserialization with NULL pointers.
 */
void test_tlv_null_pointer(void);

int32_t main() {
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

void setUp() {

}

void tearDown() {

}

void test_serialize_fixed_size() {
    telemetry_t original = {
        .device_id = 4096U,
        .temperature = 24.50f,
        .humidity = 60.20f
    };
    uint8_t buf[BUFFER_SIZE];

    size_t count = serialize_telemetry(&original, &buf[0]);

    uint8_t expected[] = {
        0x00, 0x00, 0x10, 0x00, /* Device ID */
        0x41, 0xC4, 0x00, 0x00, /* Temperature */
        0x42, 0x70, 0xCC, 0xCD  /* Humidity */
    };

    TEST_ASSERT_EQUAL_size_t(12U, count);
    TEST_ASSERT_EQUAL_MEMORY(expected, buf, sizeof(expected));
}

void test_roundtrip_fixed_layout() {
    telemetry_t original = {
        .device_id = 4096U,
        .temperature = 24.50f,
        .humidity = 60.20f
    };
    telemetry_t restored = {0};
    uint8_t buf[BUFFER_SIZE];

    size_t count = serialize_telemetry(&original, &buf[0]);
    deserialize_telemetry(&buf[0], &restored);
    
    TEST_ASSERT_EQUAL_size_t(12U, count);
    TEST_ASSERT_EQUAL_UINT32(original.device_id, restored.device_id);
    TEST_ASSERT_EQUAL_FLOAT(original.temperature, restored.temperature);
    TEST_ASSERT_EQUAL_FLOAT(original.humidity, restored.humidity);
}

void test_float_byte_swap_safe() {
    telemetry_t original = {
        .temperature = 32.60f,
    };
    telemetry_t restored = {0};
    uint8_t buf[BUFFER_SIZE];

    size_t count = serialize_telemetry(&original, &buf[0]);
    (void)count;
    deserialize_telemetry(&buf[0], &restored);

    TEST_ASSERT_EQUAL_FLOAT(original.temperature, restored.temperature);
}

void test_serialize_tlv_size() {
    telemetry_t original = {
        .device_id = 4096U,
        .temperature = 24.50f,
        .humidity = 60.20f
    };
    uint8_t buf[BUFFER_SIZE];

    size_t count = serialize_telemetry_tlv(&original, &buf[0]);
    TEST_ASSERT_EQUAL_size_t(18U, count);
}

void test_roundtrip_tlv() {
    telemetry_t original = {
        .device_id = 4096U,
        .temperature = 24.50f,
        .humidity = 60.20f
    };
    telemetry_t restored = {0};
    uint8_t buf[BUFFER_SIZE];

    size_t count = serialize_telemetry_tlv(&original, &buf[0]);
    int32_t result = deserialize_telemetry_tlv(&buf[0], count, &restored);
    
    TEST_ASSERT_EQUAL_size_t(18U, count);
    TEST_ASSERT_EQUAL_INT32(SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT32(original.device_id, restored.device_id);
    TEST_ASSERT_EQUAL_FLOAT(original.temperature, restored.temperature);
    TEST_ASSERT_EQUAL_FLOAT(original.humidity, restored.humidity);
}

void test_tlv_rejects_short_buffer() {
    telemetry_t restored = {0};
    uint8_t buf[] = {
        0x01, 0x04, 0x00, 0x00, 0x10
    };

    size_t length = sizeof(buf) / sizeof(buf[0]);
    int32_t result = deserialize_telemetry_tlv(&buf[0], length, &restored);

    TEST_ASSERT_EQUAL_INT32(FAILED, result);
}

void test_tlv_skips_unknown_type() {
    telemetry_t original = {
        .device_id = 4096U,
        .temperature = 24.50f,
        .humidity = 60.20f
    };
    telemetry_t restored = {0};
    uint8_t buf[] = {
        0x01, 0x04, 0x00, 0x00, 0x10, 0x00, /* Device ID */
        0x02, 0x04, 0x41, 0xC4, 0x00, 0x00, /* Temperature */
        0xFF, 0x04, 0x00, 0x11, 0x22, 0x33, /* Unknown type */
        0x03, 0x04, 0x42, 0x70, 0xCC, 0xCD  /* Humidity */
    };

    size_t length = sizeof(buf) / sizeof(buf[0]);
    int32_t result = deserialize_telemetry_tlv(&buf[0], length, &restored);

    TEST_ASSERT_EQUAL_INT32(SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT32(original.device_id, restored.device_id);
    TEST_ASSERT_EQUAL_FLOAT(original.temperature, restored.temperature);
    TEST_ASSERT_EQUAL_FLOAT(original.humidity, restored.humidity);
}

void test_tlv_null_pointer() {
    telemetry_t original = {0};
    uint8_t buf[] = {0};

    TEST_ASSERT_EQUAL_size_t(ERROR, serialize_telemetry_tlv(&original, NULL));
    TEST_ASSERT_EQUAL_size_t(ERROR, serialize_telemetry_tlv(NULL, buf));
    TEST_ASSERT_EQUAL_size_t(ERROR, serialize_telemetry_tlv(NULL, NULL));
}