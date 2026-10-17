/**
 * @file    test_memory_ops.c
 * @brief   Unity test suite for memory_ops.c.
 */
#include "unity.h"
#include "memory_ops.h"
#include <string.h>

/**
 * @brief   Unity setup hook, run before each test.
 */
void setUp(void)
{
    /* Nothing to set up. */
}

/**
 * @brief   Unity teardown hook, run after each test.
 */
void tearDown(void)
{
    /* Nothing to tear down. */
}

/**
 * @brief   sensor_data_zero_init() must zero every field via memset().
 */
void test_struct_zero_init(void)
{
    sensor_data_t data;

    data.id = 99;
    data.temp = 12.5f;

    sensor_data_zero_init(&data);

    TEST_ASSERT_EQUAL_INT32(0, data.id);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, data.temp);
}

/**
 * @brief   memcpy() must correctly copy binary data containing 0x00
 *          bytes in the middle.
 */
void test_memcpy_handles_null_bytes(void)
{
    uint8_t payload[8] = {0x01, 0x00, 0xFF, 0x00, 0x10, 0x20, 0x30, 0x40};
    uint8_t dst[8];

    copy_with_memcpy(dst, payload, sizeof(payload));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(payload, dst, sizeof(payload));
}

/**
 * @brief   strncpy() must (incorrectly, for binary data) stop at the
 *          first 0x00 byte instead of copying the full payload.
 */
void test_strncpy_fails_on_null_bytes(void)
{
    uint8_t payload[8] = {0x01, 0x00, 0xFF, 0x00, 0x10, 0x20, 0x30, 0x40};
    uint8_t dst[8];

    copy_with_strncpy(dst, sizeof(dst), payload, sizeof(payload));

    /* The 0xFF at index 2 must have been lost - proof of truncation. */
    TEST_ASSERT_NOT_EQUAL_UINT8(payload[2], dst[2]);
    TEST_ASSERT_EQUAL_UINT8(0x00U, dst[2]);
}

/**
 * @brief   memcmp() must correctly detect a difference between two
 *          binary buffers that both start with 0x00.
 */
void test_memcmp_detects_difference(void)
{
    const uint8_t id1[4] = {0U, 1U, 0U, 5U};
    const uint8_t id2[4] = {0U, 1U, 0U, 9U};

    TEST_ASSERT_NOT_EQUAL(0, compare_with_memcmp(id1, id2, sizeof(id1)));
}

/**
 * @brief   strcmp() must (incorrectly, for binary data) report the
 *          two differing buffers as equal, since both start with
 *          0x00.
 */
void test_strcmp_fails_to_detect_difference(void)
{
    const uint8_t id1[4] = {0U, 1U, 0U, 5U};
    const uint8_t id2[4] = {0U, 1U, 0U, 9U};

    TEST_ASSERT_EQUAL_INT(0, compare_with_strcmp(id1, id2));
}

/**
 * @brief   safe_shift_right() must correctly shift an overlapping
 *          region using memmove().
 */
void test_memmove_handles_overlap(void)
{
    char buf[16] = "123456789";

    safe_shift_right(buf, sizeof(buf), 3U, 6U, 3U);

    TEST_ASSERT_EQUAL_STRING("123456456789", buf);
}

/**
 * @brief   Test runner entry point.
 *
 * @return  Number of failed tests (0 on success).
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_struct_zero_init);
    RUN_TEST(test_memcpy_handles_null_bytes);
    RUN_TEST(test_strncpy_fails_on_null_bytes);
    RUN_TEST(test_memcmp_detects_difference);
    RUN_TEST(test_strcmp_fails_to_detect_difference);
    RUN_TEST(test_memmove_handles_overlap);

    return UNITY_END();
}