#include "command_handler.h"
#include "fake_uart.h"
#include "uart_hal.h"
#include "fff.h"
#include "unity.h"

/**
 * @brief Defines the global state used by the FFF framework.
 *
 * Provides the global fake function state required by the Fake Function
 * Framework (FFF).
 */
DEFINE_FFF_GLOBALS;

/**
 * @brief Defines the fake UART receive function.
 *
 * Creates a fake implementation of the UART receive function for use
 * in unit tests.
 */
DEFINE_FAKE_VALUE_FUNC(uint32_t, uart_hal_receive, uint8_t*, uint32_t);

/**
 * @brief Defines the fake UART transmit function.
 *
 * Creates a fake implementation of the UART transmit function for use
 * in unit tests.
 */
DEFINE_FAKE_VALUE_FUNC(bool, uart_hal_transmit, const uint8_t*, uint32_t);

/**
 * @brief Sets up the test environment before each test case.
 *
 * Initializes or resets the fake functions and test-related state
 * required by the test cases.
 */
void setUp(void);

/**
 * @brief Cleans up the test environment after each test case.
 *
 * Releases or resets any test-related state after a test case has
 * completed.
 */
void tearDown(void);

/**
 * @brief Tests successful echo processing.
 *
 * Verifies that received UART data is transmitted back successfully
 * and that the echo command reports a successful result.
 */
void test_echo_success(void);

/**
 * @brief Tests echo processing when no data is received.
 *
 * Verifies that the echo command returns false when the UART receive
 * function reports that no data is available.
 */
void test_echo_no_data(void);

int32_t main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_echo_success);
    RUN_TEST(test_echo_no_data);

    return UNITY_END();
}

void setUp(void) {
    RESET_FAKE(uart_hal_receive);
    RESET_FAKE(uart_hal_transmit);
}

void tearDown(void) {

}

void test_echo_success(void) {
    uart_hal_receive_fake.return_val = 5;
    cmd_process_echo();
    TEST_ASSERT_EQUAL_UINT32(1U, uart_hal_transmit_fake.call_count);
}

void test_echo_no_data(void) {
    uart_hal_receive_fake.return_val = 0;
    cmd_process_echo();
    TEST_ASSERT_EQUAL_UINT32(0U, uart_hal_transmit_fake.call_count);
}
