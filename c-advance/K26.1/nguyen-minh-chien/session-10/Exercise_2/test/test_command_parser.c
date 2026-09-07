/**
 * @file    test_command_parser.c
 * @brief   Unity test suite for command_parser.c.
 */
#include "unity.h"
#include "command_parser.h"
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
 * @brief   "LED:ON" must parse successfully and format the expected
 *          response.
 */
void test_parse_led_on(void)
{
    char response[64];

    TEST_ASSERT_EQUAL_INT8(0, parse_command("LED:ON", response, sizeof(response)));
    TEST_ASSERT_EQUAL_STRING("[OK] LED set to ON", response);
}

/**
 * @brief   "MOTOR:START:500" must extract the RPM value correctly.
 */
void test_parse_motor_start_with_rpm(void)
{
    char response[64];

    TEST_ASSERT_EQUAL_INT8(0, parse_command("MOTOR:START:500", response, sizeof(response)));
    TEST_ASSERT_EQUAL_STRING("[OK] MOTOR started at 500 RPM", response);
}

/**
 * @brief   "TEMP:READ" must parse successfully.
 */
void test_parse_temp_read(void)
{
    char response[64];

    TEST_ASSERT_EQUAL_INT8(0, parse_command("TEMP:READ", response, sizeof(response)));
    TEST_ASSERT_EQUAL_STRING("[OK] TEMP read requested", response);
}

/**
 * @brief   extract_device_name() must correctly extract "LED" from
 *          "LED:ON".
 */
void test_extract_device_name_valid(void)
{
    char device[16];

    TEST_ASSERT_EQUAL_INT8(0, extract_device_name("LED:ON", device, sizeof(device)));
    TEST_ASSERT_EQUAL_STRING("LED", device);
}

/**
 * @brief   extract_device_name() must fail safely when there is no
 *          ':' delimiter.
 */
void test_extract_device_name_no_delimiter(void)
{
    char device[16];

    TEST_ASSERT_EQUAL_INT8(-1, extract_device_name("NODELIMITER", device, sizeof(device)));
}

/**
 * @brief   parse_command() must safely reject a NULL command string.
 */
void test_parse_null_input(void)
{
    char response[64];

    TEST_ASSERT_EQUAL_INT8(-1, parse_command(NULL, response, sizeof(response)));
}

/**
 * @brief   Test runner entry point.
 *
 * @return  Number of failed tests (0 on success).
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_parse_led_on);
    RUN_TEST(test_parse_motor_start_with_rpm);
    RUN_TEST(test_parse_temp_read);
    RUN_TEST(test_extract_device_name_valid);
    RUN_TEST(test_extract_device_name_no_delimiter);
    RUN_TEST(test_parse_null_input);

    return UNITY_END();
}