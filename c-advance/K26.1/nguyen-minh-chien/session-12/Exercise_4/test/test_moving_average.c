/**
 * @file test_moving_average.c
 * @brief Unity unit tests for the moving average filter.
 */

#include <stdint.h>

#include "moving_average.h"
#include "unity.h"

static ma_filter_t g_filter;

void setUp(void)
{
    ma_init(&g_filter);
}

void tearDown(void)
{
}

static void test_startup_phase_no_zero_bias(void)
{
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(256, ma_process(&g_filter, 262));
    TEST_ASSERT_EQUAL_INT16(253, ma_process(&g_filter, 248));
    TEST_ASSERT_EQUAL_INT16(256, ma_process(&g_filter, 265));
}

static void test_constant_signal_returns_same_value(void)
{
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
}

static void test_step_response_sliding_window(void)
{
    TEST_ASSERT_EQUAL_INT16(100, ma_process(&g_filter, 100));
    TEST_ASSERT_EQUAL_INT16(100, ma_process(&g_filter, 100));
    TEST_ASSERT_EQUAL_INT16(100, ma_process(&g_filter, 100));
    TEST_ASSERT_EQUAL_INT16(100, ma_process(&g_filter, 100));

    TEST_ASSERT_EQUAL_INT16(125, ma_process(&g_filter, 200));
    TEST_ASSERT_EQUAL_INT16(150, ma_process(&g_filter, 200));
    TEST_ASSERT_EQUAL_INT16(175, ma_process(&g_filter, 200));
    TEST_ASSERT_EQUAL_INT16(200, ma_process(&g_filter, 200));
}

static void test_spike_smoothing_and_eviction(void)
{
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));

    TEST_ASSERT_EQUAL_INT16(262, ma_process(&g_filter, 300));
    TEST_ASSERT_EQUAL_INT16(262, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(262, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(262, ma_process(&g_filter, 250));
    TEST_ASSERT_EQUAL_INT16(250, ma_process(&g_filter, 250));
}

static void test_reset_clears_history(void)
{
    uint32_t index;

    TEST_ASSERT_EQUAL_INT16(100, ma_process(&g_filter, 100));
    TEST_ASSERT_EQUAL_INT16(150, ma_process(&g_filter, 200));

    ma_reset(&g_filter);

    TEST_ASSERT_EQUAL_UINT32(0U, g_filter.head);
    TEST_ASSERT_EQUAL_UINT32(0U, g_filter.count);
    TEST_ASSERT_EQUAL_INT32(0, g_filter.running_sum);

    for (index = 0U; index < MA_WINDOW_SIZE; ++index)
    {
        TEST_ASSERT_EQUAL_INT16(0, g_filter.buffer[index]);
    }

    TEST_ASSERT_EQUAL_INT16(300, ma_process(&g_filter, 300));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_startup_phase_no_zero_bias);
    RUN_TEST(test_constant_signal_returns_same_value);
    RUN_TEST(test_step_response_sliding_window);
    RUN_TEST(test_spike_smoothing_and_eviction);
    RUN_TEST(test_reset_clears_history);

    return UNITY_END();
}
