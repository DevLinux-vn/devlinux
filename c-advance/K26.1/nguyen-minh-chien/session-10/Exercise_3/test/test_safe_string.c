/**
 * @file    test_safe_string.c
 * @brief   Unity test suite for safe_string.c.
 */
#include "unity.h"
#include "safe_string.h"
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
 * @brief   safe_concat() must concatenate normally when the buffer
 *          is large enough.
 */
void test_safe_concat_normal(void)
{
    char dst[64] = "Hello ";

    safe_concat(dst, sizeof(dst), "World");

    TEST_ASSERT_EQUAL_STRING("Hello World", dst);
}

/**
 * @brief   safe_concat() must truncate safely instead of overflowing
 *          a too-small buffer.
 */
void test_safe_concat_truncation(void)
{
    char dst[7] = "";

    safe_concat(dst, sizeof(dst), "[SYS_LOG] ");

    TEST_ASSERT_EQUAL_STRING("[SYS_L", dst);
}

/**
 * @brief   safe_concat() must safely do nothing when p_src is NULL.
 */
void test_safe_concat_null_input(void)
{
    char dst[16] = "hello";

    safe_concat(dst, sizeof(dst), NULL);

    TEST_ASSERT_EQUAL_STRING("hello", dst);
}

/**
 * @brief   format_sensor_report() must format normally when the
 *          buffer is large enough.
 */
void test_format_sensor_report_normal(void)
{
    char buf[64];

    format_sensor_report(buf, sizeof(buf), "PRESSURE", 1024, "hPa");

    TEST_ASSERT_EQUAL_STRING("[PRESSURE] VAL:1024 hPa", buf);
}

/**
 * @brief   format_sensor_report() must truncate safely instead of
 *          overflowing a too-small buffer.
 */
void test_format_sensor_report_truncation(void)
{
    char buf[7];

    format_sensor_report(buf, sizeof(buf), "PRESSURE", 1024, "hPa");

    TEST_ASSERT_EQUAL_STRING("[PRESS", buf);
}

/**
 * @brief   get_log_prefix_GOOD() must write the expected prefix into
 *          the caller-provided buffer.
 */
void test_get_log_prefix_good(void)
{
    char buf[32];

    get_log_prefix_GOOD(buf, sizeof(buf));

    TEST_ASSERT_EQUAL_STRING("[SYS_LOG] ", buf);
}

/**
 * @brief   Test runner entry point.
 *
 * @return  Number of failed tests (0 on success).
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_safe_concat_normal);
    RUN_TEST(test_safe_concat_truncation);
    RUN_TEST(test_safe_concat_null_input);
    RUN_TEST(test_format_sensor_report_normal);
    RUN_TEST(test_format_sensor_report_truncation);
    RUN_TEST(test_get_log_prefix_good);

    return UNITY_END();
}