#include "unity.h"
#include "sensor.h"

#define INIT_VAL            (0U)
#define MAX_VAL             (4095U)
#define OUT_OF_RANGE_VAL    (4096U)

/**
 * @brief Set up the test environment before each test case.
 *
 * This function is automatically called by Unity before each test
 * case to initialize or reset the required test state.
 */
void setUp(void);

/**
 * @brief Clean up the test environment after each test case.
 *
 * This function is automatically called by Unity after each test
 * case to release resources or restore the test state.
 */
void tearDown(void);

/**
 * @brief Test ADC conversion with a zero input value.
 *
 * Verifies that an ADC value of zero is converted to the expected
 * output voltage.
 */
void test_adc_zero(void);

/**
 * @brief Test ADC conversion with the maximum input value.
 *
 * Verifies that the maximum supported ADC value is converted to
 * the expected output voltage.
 */
void test_adc_max(void);

/**
 * @brief Test ADC conversion with an out-of-range input value.
 *
 * Verifies that the ADC conversion function handles an input value
 * outside the supported ADC range correctly.
 */
void test_adc_out_of_range(void);

/**
 * @brief Test moving average calculation with valid samples.
 *
 * Verifies that the moving average function returns the expected
 * average when provided with a valid set of samples and count.
 */
void test_moving_average_valid(void);

/**
 * @brief Test moving average calculation with zero sample count.
 *
 * Verifies that the moving average function handles a zero sample
 * count correctly without performing an invalid calculation.
 */
void test_moving_average_zero_count(void);

int32_t main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_adc_zero);
    RUN_TEST(test_adc_max);
    RUN_TEST(test_adc_out_of_range);
    RUN_TEST(test_moving_average_valid);
    RUN_TEST(test_moving_average_zero_count);

    return UNITY_END();
}

void setUp(void) {
    
}

void tearDown(void) {

}

void test_adc_zero(void) {
    TEST_ASSERT_EQUAL_INT32(0, sensor_adc_to_mv(INIT_VAL));
}

void test_adc_max(void) {
    TEST_ASSERT_EQUAL_INT32(5000, sensor_adc_to_mv(MAX_VAL));
}

void test_adc_out_of_range(void) {
    TEST_ASSERT_EQUAL_INT32(0, sensor_adc_to_mv(OUT_OF_RANGE_VAL));
}

void test_moving_average_valid(void) {
    const uint32_t samples[4] = {100U, 200U, 300U, 400U};
    TEST_ASSERT_EQUAL_INT32(250, sensor_moving_average(samples, sizeof(samples) / sizeof(samples[0])));
}

void test_moving_average_zero_count(void) {
    const uint32_t samples[1] = {100U};
    TEST_ASSERT_EQUAL_INT32(0, sensor_moving_average(samples, 0U));
}