/**
 * @file test_safe_string.c
 * @brief Unity unit tests for safe string modules.
 */
#include "unity.h"
#include "safe_string.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_safe_concat_normal(void)
{
    char buf[32] = "Hello";
    safe_concat(buf, sizeof(buf), " World");
    TEST_ASSERT_EQUAL_STRING("Hello World", buf);
}

void test_safe_concat_truncation(void)
{
    char buf[6] = "Hello";
    safe_concat(buf, sizeof(buf), " World");
    
    /* No room to add anything except retaining its own null-terminator */
    TEST_ASSERT_EQUAL_STRING("Hello", buf); 
}

void test_safe_concat_null_input(void)
{
    char buf[32] = "Test";
    safe_concat(NULL, 32, " Data");
    safe_concat(buf, sizeof(buf), NULL);
    
    /* Should exit early, string must remain unmodified */
    TEST_ASSERT_EQUAL_STRING("Test", buf); 
}

void test_format_sensor_report_normal(void)
{
    char buf[64];
    format_sensor_report(buf, sizeof(buf), "TEMP", 25, "C");
    TEST_ASSERT_EQUAL_STRING("[TEMP] VAL:25 C", buf);
}

void test_format_sensor_report_truncation(void)
{
    char buf[8];
    format_sensor_report(buf, sizeof(buf), "PRESSURE", 1024, "hPa");
    
    /* Buffer size 8 fits 7 characters + '\0' */
    TEST_ASSERT_EQUAL_STRING("[PRESSU", buf); 
}

void test_get_log_prefix_good(void)
{
    char buf[16];
    get_log_prefix_GOOD(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("[SYS_LOG]", buf);
}

/**
 * @brief Main test runner.
 * 
 * @return int Exit status.
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