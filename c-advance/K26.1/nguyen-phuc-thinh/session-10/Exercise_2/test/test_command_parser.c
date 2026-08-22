/**
 * @file test_command_parser.c
 * @brief Unity unit tests for the command parser module.
 */
#include "unity.h"
#include "command_parser.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse_led_on(void) 
{
    char response[64];
    int8_t res = parse_command("LED:ON", response, sizeof(response));
    TEST_ASSERT_EQUAL_INT8(0, res);
    TEST_ASSERT_EQUAL_STRING("[OK] LED set to ON", response);
}

void test_parse_motor_start_with_rpm(void) 
{
    char response[64];
    int8_t res = parse_command("MOTOR:START:500", response, sizeof(response));
    TEST_ASSERT_EQUAL_INT8(0, res);
    TEST_ASSERT_EQUAL_STRING("[OK] MOTOR started at 500 RPM", response);
}

void test_parse_temp_read(void) 
{
    char response[64];
    int8_t res = parse_command("TEMP:READ", response, sizeof(response));
    TEST_ASSERT_EQUAL_INT8(0, res);
    TEST_ASSERT_EQUAL_STRING("[OK] TEMP read requested", response);
}

void test_extract_device_name_valid(void) 
{
    char device[32];
    int8_t res = extract_device_name("MOTOR:START:500", device, sizeof(device));
    TEST_ASSERT_EQUAL_INT8(0, res);
    TEST_ASSERT_EQUAL_STRING("MOTOR", device);
}

void test_extract_device_name_no_delimiter(void) 
{
    char device[32];
    int8_t res = extract_device_name("INVALID_COMMAND", device, sizeof(device));
    TEST_ASSERT_EQUAL_INT8(-1, res);
}

void test_parse_null_input(void) 
{
    char response[64];
    int8_t res = parse_command(NULL, response, sizeof(response));
    TEST_ASSERT_EQUAL_INT8(-1, res);
}

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