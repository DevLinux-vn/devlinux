#include "safe_string.h"
#include <unity.h>
#include <string.h>

/**
 * @brief Sets up the test environment before each test.
 */
void setUp();

/**
 * @brief Cleans up the test environment after each test.
 */
void tearDown();

/**
 * @brief Tests safe_concat with normal input.
 */
void test_safe_concat_normal();

/**
 * @brief Tests safe_concat when the destination buffer is too small.
 */
void test_safe_concat_truncation();

/**
 * @brief Tests safe_concat with NULL input.
 */
void test_safe_concat_null_input();

/**
 * @brief Tests format_sensor_report with normal input.
 */
void test_format_sensor_report_normal();

/**
 * @brief Tests format_sensor_report when the destination buffer is too small.
 */
void test_format_sensor_report_truncation();

/**
 * @brief Tests get_log_prefix_GOOD with a valid destination buffer.
 */
void test_get_log_prefix_good();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_safe_concat_normal);
    RUN_TEST(test_safe_concat_truncation);
    RUN_TEST(test_safe_concat_null_input);
    RUN_TEST(test_format_sensor_report_normal);
    RUN_TEST(test_format_sensor_report_truncation);
    RUN_TEST(test_get_log_prefix_good);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_safe_concat_normal() {
    char buf[BUFFER_SIZE] = {0};
    safe_concat(buf, BUFFER_SIZE, "[SYS_LOG] Voltage low.");

    TEST_ASSERT_EQUAL_STRING("[SYS_LOG] Voltage low.", buf);
}

void test_safe_concat_truncation() {
    char tiny_buf[TINY_BUFFER_SIZE] = {0};
    safe_concat(tiny_buf, TINY_BUFFER_SIZE, "[SYS_LOG] Voltage low.");

    TEST_ASSERT_EQUAL_STRING("[SYS_LOG]", tiny_buf);
}

void test_safe_concat_null_input() {
    char dst[] = "Hello";
    const char *p_src = "World";

    safe_concat(dst, BUFFER_SIZE, NULL);
    TEST_ASSERT_EQUAL_STRING("Hello", dst);

    safe_concat(NULL, BUFFER_SIZE, p_src);
    safe_concat(NULL, BUFFER_SIZE, NULL);
    TEST_PASS();

}

void test_format_sensor_report_normal() {
    char buf[BUFFER_SIZE] = {0};
    format_sensor_report(buf, BUFFER_SIZE, "PRESSURE", 1024, "hPa");

    TEST_ASSERT_EQUAL_STRING("[PRESSURE] VAL:1024 hPa", buf);
}

void test_format_sensor_report_truncation() {
    char tiny_buf[TINY_BUFFER_SIZE] = {0};
    format_sensor_report(tiny_buf, TINY_BUFFER_SIZE, "PRESSURE", 1024, "hPa");

    TEST_ASSERT_EQUAL_STRING("[PRESSURE", tiny_buf);
}

void test_get_log_prefix_good() {
    char buf[BUFFER_SIZE] = {0};
    get_log_prefix_GOOD(buf, BUFFER_SIZE);

    TEST_ASSERT_EQUAL_STRING("[SYS_LOG]", buf);
}