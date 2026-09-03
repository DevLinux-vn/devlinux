#include "unity.h"
#include "moving_average.h"

ma_filter_t filter;

void setUp(void)
{
    ma_init(&filter);
}

void tearDown(void) {}

void test_startup_phase_no_zero_bias(void)
{
    TEST_ASSERT_EQUAL_INT16(100, ma_process(&filter, 100));
    TEST_ASSERT_EQUAL_INT16(150, ma_process(&filter, 200));
    TEST_ASSERT_EQUAL_INT16(200, ma_process(&filter, 300));
}

void test_constant_signal_returns_same_value(void)
{
    for(int i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQUAL_INT16(250, ma_process(&filter, 250));
    }
}

void test_step_response_sliding_window(void)
{
    for(int i = 0; i < 4; i++) { ma_process(&filter, 100); }
    TEST_ASSERT_EQUAL_INT16(125, ma_process(&filter, 200)); 
    TEST_ASSERT_EQUAL_INT16(150, ma_process(&filter, 200)); 
}

void test_spike_smoothing_and_eviction(void)
{
    for(int i = 0; i < 4; i++) { ma_process(&filter, 100); }
    TEST_ASSERT_EQUAL_INT16(125, ma_process(&filter, 200)); 
    ma_process(&filter, 100);
    ma_process(&filter, 100);
    ma_process(&filter, 100);
    TEST_ASSERT_EQUAL_INT16(100, ma_process(&filter, 100)); 
}

void test_reset_clears_history(void)
{
    ma_process(&filter, 100);
    ma_reset(&filter);
    TEST_ASSERT_EQUAL_INT16(50, ma_process(&filter, 50));
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