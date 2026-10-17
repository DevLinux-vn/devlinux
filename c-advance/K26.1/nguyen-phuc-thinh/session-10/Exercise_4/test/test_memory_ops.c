/**
 * @file test_memory_ops.c
 * @brief Unity unit tests for the memory operations module.
 */
#include "unity.h"
#include "memory_ops.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_struct_zero_init(void)
{
    sensor_data_t data;
    /* Simulate uninitialized memory containing garbage data */
    data.id = 1234;
    data.temp = 99.9f;
    
    init_sensor_data(&data);
    
    TEST_ASSERT_EQUAL_INT32(0, data.id);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, data.temp);
}

void test_memcpy_handles_null_bytes(void)
{
    uint8_t src[4] = {0x01, 0x00, 0x02, 0x00};
    uint8_t dst[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    
    copy_with_memcpy(dst, src, 4);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(src, dst, 4);
}

void test_strncpy_fails_on_null_bytes(void)
{
    uint8_t src[4] = {0x01, 0x00, 0x02, 0x00};
    uint8_t dst[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    
    copy_with_strncpy(dst, src, 4);
    
    /* strncpy stops at 0x00, but according to spec, it pads the rest of the array with 0x00 */
    uint8_t expected[4] = {0x01, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, dst, 4);
}

void test_memcmp_detects_difference(void)
{
    uint8_t id1[] = {0, 1, 0, 5};
    uint8_t id2[] = {0, 1, 0, 9};
    
    int res = compare_with_memcmp(id1, id2, 4);
    TEST_ASSERT_NOT_EQUAL(0, res);
}

void test_strcmp_fails_to_detect_difference(void)
{
    uint8_t id1[] = {0, 1, 0, 5};
    uint8_t id2[] = {0, 1, 0, 9};
    
    int res = compare_with_strcmp(id1, id2);
    /* strcmp will erroneously report them as equal because of the leading 0x00 byte */
    TEST_ASSERT_EQUAL_INT(0, res);
}

void test_memmove_handles_overlap(void)
{
    char buf[16] = "123456789";
    shift_data_safely(buf, 3, 5, 7);
    
    TEST_ASSERT_EQUAL_STRING("123456456789", buf);
}

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