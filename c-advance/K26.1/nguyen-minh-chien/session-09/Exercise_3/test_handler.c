/**
 * @file    test_handler.c
 * @brief   Unity + FFF test suite for command_handler.c.
 */
#include "unity.h"
#include "fake_uart.h"

/* Instantiate FFF's global bookkeeping state (call counts, history...). */
DEFINE_FFF_GLOBALS

/* Generate the actual fake function bodies declared in fake_uart.h. */
DEFINE_FAKE_VALUE_FUNC(uint32_t, uart_hal_receive, uint8_t *, uint32_t)
DEFINE_FAKE_VALUE_FUNC(bool, uart_hal_transmit, const uint8_t *, uint32_t)

/**
 * @brief   Function under test, declared here since it has no header
 *          of its own in this exercise.
 *
 * @return  true if data was received and echoed back, false otherwise.
 */
bool cmd_process_echo(void);

/**
 * @brief   Unity setup hook: reset all fakes before every test so
 *          tests do not leak state into one another.
 */
void setUp(void)
{
    RESET_FAKE(uart_hal_receive);
    RESET_FAKE(uart_hal_transmit);
    FFF_RESET_HISTORY();
}

/**
 * @brief   Unity teardown hook, run after each test.
 */
void tearDown(void)
{
    /* Nothing to tear down. */
}

/**
 * @brief   When the fake receiver reports 5 bytes available, the
 *          handler must transmit exactly once.
 */
void test_echo_success(void)
{
    uart_hal_receive_fake.return_val = 5U;

    (void)cmd_process_echo();

    TEST_ASSERT_EQUAL_UINT32(1U, uart_hal_transmit_fake.call_count);
}

/**
 * @brief   When the fake receiver reports 0 bytes available, the
 *          handler must not call transmit at all.
 */
void test_echo_no_data(void)
{
    uart_hal_receive_fake.return_val = 0U;

    (void)cmd_process_echo();

    TEST_ASSERT_EQUAL_UINT32(0U, uart_hal_transmit_fake.call_count);
}

/**
 * @brief   Test runner entry point.
 *
 * @return  Number of failed tests (0 on success).
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_echo_success);
    RUN_TEST(test_echo_no_data);

    return UNITY_END();
}