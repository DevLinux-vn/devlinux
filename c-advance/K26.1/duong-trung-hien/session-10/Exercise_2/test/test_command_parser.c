#include "command_parser.h"
#include "unity.h"

/**
 * @brief Initializes the test environment before each test case.
 */
void setUp(void);

/**
 * @brief Cleans up the test environment after each test case.
 */
void tearDown(void);

/**
 * @brief Tests parsing a valid LED ON command.
 */
void test_parse_led_on(void);

/**
 * @brief Tests parsing a MOTOR START command with an RPM value.
 */
void test_parse_motor_start_with_rpm(void);

/**
 * @brief Tests parsing a valid temperature read command.
 */
void test_parse_temp_read(void);

/**
 * @brief Tests extracting a valid device name from a command string.
 */
void test_extract_device_name_valid(void);

/**
 * @brief Tests device name extraction when no delimiter is present.
 */
void test_extract_device_name_no_delimiter(void);

/**
 * @brief Tests command parsing with a NULL input command.
 */
void test_parse_null_input(void);

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_parse_led_on);
    RUN_TEST(test_parse_motor_start_with_rpm);
    RUN_TEST(test_parse_temp_read);
    RUN_TEST(test_extract_device_name_valid);
    RUN_TEST(test_extract_device_name_no_delimiter);
    RUN_TEST(test_parse_null_input);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_parse_led_on() {
    char response[64];
    int8_t result = parse_command("LED:ON", response, sizeof(response));

    TEST_ASSERT_EQUAL_INT8(PARSE_SUCCESS, result);
    TEST_ASSERT_EQUAL_STRING("[OK] LED set to ON", response);
}

void test_parse_motor_start_with_rpm() {
    char response[64];
    int8_t result = parse_command("MOTOR:START:500", response, sizeof(response));

    TEST_ASSERT_EQUAL_INT8(PARSE_SUCCESS, result);
    TEST_ASSERT_EQUAL_STRING("[OK] MOTOR started at 500 RPM", response);
}

void test_parse_temp_read() {
    char response[64];
    int8_t result = parse_command("TEMP:READ", response, sizeof(response));

    TEST_ASSERT_EQUAL_INT8(PARSE_SUCCESS, result);
    TEST_ASSERT_EQUAL_STRING("[OK] TEMP read requested", response);
}

void test_extract_device_name_valid() {
    char device[64];
    int8_t result = extract_device_name("MOTOR:STOP", device, sizeof(device));

    TEST_ASSERT_EQUAL_INT8(FIND_DELIMITER_SUCCESS, result);
    TEST_ASSERT_EQUAL_STRING("MOTOR", device);
}

void test_extract_device_name_no_delimiter() {
    char device[64];
    int8_t result = extract_device_name("LED ON", device, sizeof(device));

    TEST_ASSERT_EQUAL_INT8(FIND_DELIMITER_FAILED, result);
}

void test_parse_null_input() {
    char device[64];
    int8_t result = extract_device_name("", device, sizeof(device));

    TEST_ASSERT_EQUAL_INT8(ERROR, result);
}