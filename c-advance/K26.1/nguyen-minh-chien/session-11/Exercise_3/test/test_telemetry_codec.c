/**
 * @file    test_telemetry_codec.c
 * @brief   Unity unit tests for the endian-safe telemetry codec.
 */

#include "unity.h"
#include "telemetry_codec.h"

#include <string.h>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

/**
 * @brief Unity setup hook, run before each test. Nothing to initialize.
 */
void setUp(void)
{
    /* Intentionally empty: no per-test fixture is required. */
}

/**
 * @brief Unity teardown hook, run after each test. Nothing to clean up.
 */
void tearDown(void)
{
    /* Intentionally empty: no per-test fixture is required. */
}

/**
 * @brief   Part A: serialize_telemetry() must always report exactly
 *          TELEMETRY_FIXED_SIZE_BYTES (12) bytes written.
 */
void test_serialize_fixed_size(void)
{
    telemetry_t sample = { 1U, 1.0f, 1.0f };
    uint8_t     buffer[TELEMETRY_FIXED_SIZE_BYTES];
    size_t      written;

    written = serialize_telemetry(&sample, buffer);

    TEST_ASSERT_EQUAL_UINT((unsigned int)TELEMETRY_FIXED_SIZE_BYTES,
                            (unsigned int)written);
}

/**
 * @brief   Part A: a value serialized then deserialized must be identical
 *          to the original (fixed-layout round trip).
 */
void test_roundtrip_fixed_layout(void)
{
    telemetry_t original  = { 4096U, 24.5f, 60.2f };
    telemetry_t recovered = { 0U, 0.0f, 0.0f };
    uint8_t     buffer[TELEMETRY_FIXED_SIZE_BYTES];

    (void)serialize_telemetry(&original, buffer);
    deserialize_telemetry(buffer, &recovered);

    TEST_ASSERT_EQUAL_UINT32(original.device_id, recovered.device_id);
    TEST_ASSERT_EQUAL_FLOAT(original.temperature, recovered.temperature);
    TEST_ASSERT_EQUAL_FLOAT(original.humidity, recovered.humidity);
}

/**
 * @brief   Part A: verify the on-wire bytes for the float fields match the
 *          expected network-order IEEE-754 bit pattern (i.e. the
 *          memcpy-based type pun round-trips safely, per MISRA Rule 11.3 /
 *          CERT-C EXP39-C), using the exact bytes from the exercise spec.
 */
void test_float_byte_swap_safe(void)
{
    telemetry_t sample = { 4096U, 24.5f, 60.2f };
    uint8_t     buffer[TELEMETRY_FIXED_SIZE_BYTES];
    /* Expected wire bytes taken from the exercise's Expected Output. */
    const uint8_t expected[TELEMETRY_FIXED_SIZE_BYTES] = {
        0x00U, 0x00U, 0x10U, 0x00U,
        0x41U, 0xC4U, 0x00U, 0x00U,
        0x42U, 0x70U, 0xCCU, 0xCDU
    };

    (void)serialize_telemetry(&sample, buffer);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buffer,
                                  (unsigned int)TELEMETRY_FIXED_SIZE_BYTES);
}

/**
 * @brief   Part B: serialize_telemetry_tlv() must always report exactly
 *          TELEMETRY_TLV_SIZE_BYTES (18) bytes written.
 */
void test_serialize_tlv_size(void)
{
    telemetry_t sample = { 1U, 1.0f, 1.0f };
    uint8_t     buffer[TELEMETRY_TLV_SIZE_BYTES];
    size_t      written;

    written = serialize_telemetry_tlv(&sample, buffer);

    TEST_ASSERT_EQUAL_UINT((unsigned int)TELEMETRY_TLV_SIZE_BYTES,
                            (unsigned int)written);
}

/**
 * @brief   Part B: a value serialized then deserialized via TLV must be
 *          identical to the original.
 */
void test_roundtrip_tlv(void)
{
    telemetry_t original  = { 4096U, 24.5f, 60.2f };
    telemetry_t recovered = { 0U, 0.0f, 0.0f };
    uint8_t     buffer[TELEMETRY_TLV_SIZE_BYTES];
    size_t      written;
    int32_t     status;

    written = serialize_telemetry_tlv(&original, buffer);
    status  = deserialize_telemetry_tlv(buffer, written, &recovered);

    TEST_ASSERT_EQUAL_INT32(TLV_DESERIALIZE_OK, status);
    TEST_ASSERT_EQUAL_UINT32(original.device_id, recovered.device_id);
    TEST_ASSERT_EQUAL_FLOAT(original.temperature, recovered.temperature);
    TEST_ASSERT_EQUAL_FLOAT(original.humidity, recovered.humidity);
}

/**
 * @brief   Part B: a buffer that is shorter than a TLV entry's declared
 *          length must be rejected (Heartbleed-style bounds check),
 *          per MISRA-C Directive 4.14 / CERT-C ARR30-C.
 */
void test_tlv_rejects_short_buffer(void)
{
    telemetry_t original = { 4096U, 24.5f, 60.2f };
    telemetry_t decoded  = { 0U, 0.0f, 0.0f };
    uint8_t     buffer[TELEMETRY_TLV_SIZE_BYTES];
    int32_t     status;

    (void)serialize_telemetry_tlv(&original, buffer);

    /* Truncate the buffer so the last entry's Value cannot be fully read. */
    status = deserialize_telemetry_tlv(buffer,
                                        (size_t)TELEMETRY_TLV_SIZE_BYTES - 1U,
                                        &decoded);

    TEST_ASSERT_EQUAL_INT32(TLV_DESERIALIZE_ERROR, status);
}

/**
 * @brief   Part B: an unrecognised TLV type must be skipped gracefully and
 *          not prevent decoding of the recognised fields around it.
 */
void test_tlv_skips_unknown_type(void)
{
    uint8_t     buffer[TELEMETRY_TLV_SIZE_BYTES +
                        TLV_TYPE_FIELD_SIZE_BYTES +
                        TLV_LENGTH_FIELD_SIZE_BYTES + 2U];
    size_t      offset = 0U;
    uint32_t    net_device = htonl(4096U);
    uint8_t     unknown_payload[2] = { 0x11U, 0x22U };
    telemetry_t decoded = { 0U, 0.0f, 0.0f };
    int32_t     status;

    /* Known: device_id */
    buffer[offset] = (uint8_t)TLV_TYPE_DEVICE_ID;
    offset += TLV_TYPE_FIELD_SIZE_BYTES;
    buffer[offset] = (uint8_t)TLV_VALUE_SIZE_BYTES;
    offset += TLV_LENGTH_FIELD_SIZE_BYTES;
    (void)memcpy(&buffer[offset], &net_device, sizeof(net_device));
    offset += sizeof(net_device);

    /* Unknown type 0xFF, length 2 */
    buffer[offset] = 0xFFU;
    offset += TLV_TYPE_FIELD_SIZE_BYTES;
    buffer[offset] = (uint8_t)sizeof(unknown_payload);
    offset += TLV_LENGTH_FIELD_SIZE_BYTES;
    (void)memcpy(&buffer[offset], unknown_payload, sizeof(unknown_payload));
    offset += sizeof(unknown_payload);

    status = deserialize_telemetry_tlv(buffer, offset, &decoded);

    TEST_ASSERT_EQUAL_INT32(TLV_DESERIALIZE_OK, status);
    TEST_ASSERT_EQUAL_UINT32(4096U, decoded.device_id);
}

/**
 * @brief   NULL pointer arguments must be rejected safely (no crash) by
 *          every public API function.
 */
void test_tlv_null_pointer(void)
{
    telemetry_t sample = { 1U, 1.0f, 1.0f };
    telemetry_t decoded;
    uint8_t     buffer[TELEMETRY_TLV_SIZE_BYTES];
    size_t      written;

    TEST_ASSERT_EQUAL_UINT(0U, (unsigned int)serialize_telemetry(NULL,
                                                                  buffer));
    TEST_ASSERT_EQUAL_UINT(0U,
                            (unsigned int)serialize_telemetry(&sample, NULL));
    TEST_ASSERT_EQUAL_UINT(0U,
                            (unsigned int)serialize_telemetry_tlv(NULL,
                                                                   buffer));
    TEST_ASSERT_EQUAL_UINT(0U,
                            (unsigned int)serialize_telemetry_tlv(&sample,
                                                                   NULL));

    written = serialize_telemetry_tlv(&sample, buffer);
    TEST_ASSERT_EQUAL_INT32(TLV_DESERIALIZE_ERROR,
                             deserialize_telemetry_tlv(NULL, written,
                                                        &decoded));
    TEST_ASSERT_EQUAL_INT32(TLV_DESERIALIZE_ERROR,
                             deserialize_telemetry_tlv(buffer, written,
                                                        NULL));
}

/**
 * @brief   Unity test runner entry point.
 *
 * @return 0 if all tests pass, non-zero otherwise.
 */
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