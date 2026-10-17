#include "moving_average.h"
#include "unity.h"
#include <stdint.h>

/**
 * @brief Sets up the test environment before each test case.
 */
void setUp();

/**
 * @brief Cleans up the test environment after each test case.
 */
void tearDown();

/**
 * @brief Verifies that the filter startup phase does not produce a zero bias.
 *
 * Ensures that the moving average filter produces the expected output
 * while the sliding window is being filled.
 */
void test_startup_phase_no_zero_bias();

/**
 * @brief Verifies that a constant input signal produces the same output value.
 *
 * Ensures that the moving average converges to and maintains the
 * constant input value.
 */
void test_constant_signal_returns_same_value();

/**
 * @brief Verifies the step response of the sliding window.
 *
 * Ensures that the moving average changes correctly as new samples
 * enter the window and older samples are evicted.
 */
void test_step_response_sliding_window();

/**
 * @brief Verifies spike smoothing and sample eviction.
 *
 * Ensures that a transient spike is smoothed by the moving average
 * and is eventually removed from the window.
 */
void test_spike_smoothing_and_eviction();

/**
 * @brief Verifies that resetting the filter clears its sample history.
 *
 * Ensures that all previously stored samples are discarded and
 * the filter returns to its initial state.
 */
void test_reset_clears_history();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_startup_phase_no_zero_bias);
    RUN_TEST(test_constant_signal_returns_same_value);
    RUN_TEST(test_step_response_sliding_window);
    RUN_TEST(test_spike_smoothing_and_eviction);
    RUN_TEST(test_reset_clears_history);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_startup_phase_no_zero_bias() {
    ma_filter_t filt;
    ma_init(&filt);

    int16_t filtered_result = 0U;

    ma_process(&filt, 10U);
    filtered_result = ma_process(&filt, 20U);

    TEST_ASSERT_EQUAL_UINT32(2U, filt.count);
    TEST_ASSERT_EQUAL_INT32(15U, filtered_result);
}

void test_constant_signal_returns_same_value() {
    ma_filter_t filt;
    ma_init(&filt);

    int16_t filtered_result = 0U;

    ma_process(&filt, 10U);
    ma_process(&filt, 10U);
    filtered_result = ma_process(&filt, 10U);

    TEST_ASSERT_EQUAL_UINT32(3U, filt.count);
    TEST_ASSERT_EQUAL_INT16(10U, filtered_result);
}

void test_step_response_sliding_window() {
    ma_filter_t filt;
    ma_init(&filt);

    int16_t filtered_result = 0U;

    ma_process(&filt, 0U);
    ma_process(&filt, 0U);
    ma_process(&filt, 0U);
    ma_process(&filt, 0U);

    ma_process(&filt, 100U);
    ma_process(&filt, 200U);
    ma_process(&filt, 300U);
    filtered_result = ma_process(&filt, 400U);
    (void)filtered_result;

    for (size_t i = 0U; i < MA_WINDOW_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT16((i + 1U) * 100U, filt.buffer[i]);
    }
}

void test_spike_smoothing_and_eviction() {
    ma_filter_t filt;
    ma_init(&filt);

    int16_t filtered_result = 0U;

    ma_process(&filt, 10U);
    ma_process(&filt, 100U);
    ma_process(&filt, 10U);
    filtered_result = ma_process(&filt, 10U);

    TEST_ASSERT_EQUAL_INT32(32U, filtered_result);

    ma_process(&filt, 10U);
    filtered_result = ma_process(&filt, 10U);

    TEST_ASSERT_EQUAL_INT32(10U, filtered_result);

    for (size_t i = 0U; i < MA_WINDOW_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT16(10U, filt.buffer[i]);
    }
}

void test_reset_clears_history() {
    ma_filter_t filt;
    ma_init(&filt);

    int16_t filtered_result = 0U;

    ma_process(&filt, 10U);
    ma_process(&filt, 100U);
    filtered_result = ma_process(&filt, 30U);
    (void)filtered_result;

    ma_reset(&filt);

    TEST_ASSERT_EQUAL_UINT32(0U, filt.head);
    TEST_ASSERT_EQUAL_INT32(0, filt.running_sum);
    TEST_ASSERT_EQUAL_UINT32(0U, filt.count);

    filtered_result = ma_process(&filt, 1000);
    (void)filtered_result;

    TEST_ASSERT_EQUAL_INT16(1000, filt.buffer[0]);
}