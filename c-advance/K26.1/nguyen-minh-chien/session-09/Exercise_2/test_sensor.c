/**
 * @file    test_sensor.c
 * @brief   Unity test suite for the sensor driver module.
 */
#include "unity.h"
#include "sensor.h"

/**
 * @brief   Unity setup hook, run before each test.
 */
void setUp(void)
{
    /* Nothing to set up for these stateless functions. */
}

/**
 * @brief   Unity teardown hook, run after each test.
 */
void tearDown(void)
{
    /* Nothing to tear down for these stateless functions. */
}

/**
 * @brief   ADC value of 0 must convert to 0 mV.
 */
void test_adc_zero(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, sensor_adc_to_mv(0U));
}

/**
 * @brief   Max valid ADC value (4095) must convert to full-scale mV.
 */
void test_adc_max(void)
{
    TEST_ASSERT_EQUAL_UINT32(3300U, sensor_adc_to_mv(4095U));
}

/**
 * @brief   Out-of-range ADC value (4096) must safely return 0.
 */
void test_adc_out_of_range(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, sensor_adc_to_mv(4096U));
}

/**
 * @brief   Moving average of a typical 4-sample input.
 */
void test_moving_average_valid(void)
{
    const uint32_t samples[4] = {1000U, 2000U, 3000U, 4000U};

    TEST_ASSERT_EQUAL_UINT32(2500U, sensor_moving_average(samples, 4U));
}

/**
 * @brief   Moving average with count=0 must return 0, not divide by zero.
 */
void test_moving_average_zero_count(void)
{
    const uint32_t samples[1] = {1234U};

    TEST_ASSERT_EQUAL_UINT32(0U, sensor_moving_average(samples, 0U));
}

/**
 * @brief   Test runner entry point.
 *
 * @return  Number of failed tests (0 on success).
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_adc_zero);
    RUN_TEST(test_adc_max);
    RUN_TEST(test_adc_out_of_range);
    RUN_TEST(test_moving_average_valid);
    RUN_TEST(test_moving_average_zero_count);

    return UNITY_END();
}