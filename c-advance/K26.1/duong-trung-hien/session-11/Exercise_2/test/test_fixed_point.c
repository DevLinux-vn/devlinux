#include "fixed_point.h"
#include "unity.h"

/**
 * @brief Set up the test environment before each test case.
 *
 * This function is called by the Unity test framework before executing
 * each test function.
 */
void setUp();

/**
 * @brief Clean up the test environment after each test case.
 *
 * This function is called by the Unity test framework after executing
 * each test function.
 */
void tearDown();

/**
 * @brief Test conversion of a positive floating-point value to Q8.8 format.
 *
 * Verifies that a positive floating-point value is correctly converted to
 * the corresponding Q8.8 fixed-point representation.
 */
void test_float_to_q8_8_positive();

/**
 * @brief Test conversion of a negative floating-point value to Q8.8 format.
 *
 * Verifies that a negative floating-point value is correctly converted to
 * the corresponding Q8.8 fixed-point representation.
 */
void test_float_to_q8_8_negative();

/**
 * @brief Test conversion of zero to Q8.8 format.
 *
 * Verifies that a zero floating-point value is correctly converted to zero
 * in Q8.8 fixed-point representation.
 */
void test_float_to_q8_8_zero();

/**
 * @brief Test round-trip conversion between floating-point and Q8.8 formats.
 *
 * Verifies that converting a floating-point value to Q8.8 format and then
 * converting it back produces the expected floating-point value.
 */
void test_q8_8_to_float_roundtrip();

/**
 * @brief Test multiplication of two positive Q8.8 fixed-point values.
 *
 * Verifies that multiplying two positive Q8.8 values produces the expected
 * Q8.8 fixed-point result.
 */
void test_q8_8_multiply_positive();

/**
 * @brief Test multiplication involving a negative Q8.8 fixed-point value.
 *
 * Verifies that multiplication with a negative Q8.8 operand produces the
 * expected signed result.
 */
void test_q8_8_multiply_negative();

/**
 * @brief Test threshold comparison when the sample exceeds the threshold.
 *
 * Verifies that the comparison function returns true when the sample value
 * is greater than the specified threshold.
 */
void test_q8_8_exceeds_threshold_true();

/**
 * @brief Test threshold comparison when the sample does not exceed the threshold.
 *
 * Verifies that the comparison function returns false when the sample value
 * is less than or equal to the specified threshold.
 */
void test_q8_8_exceeds_threshold_false();

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_float_to_q8_8_positive);
    RUN_TEST(test_float_to_q8_8_negative);
    RUN_TEST(test_float_to_q8_8_zero);
    RUN_TEST(test_q8_8_to_float_roundtrip);
    RUN_TEST(test_q8_8_multiply_positive);
    RUN_TEST(test_q8_8_multiply_negative);
    RUN_TEST(test_q8_8_exceeds_threshold_true);
    RUN_TEST(test_q8_8_exceeds_threshold_false);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_float_to_q8_8_positive() {
    int16_t result = float_to_q8_8(1.5f);

    TEST_ASSERT_EQUAL_INT16(384, result);
}

void test_float_to_q8_8_negative() {
    int16_t result = float_to_q8_8(-1.5f);

    TEST_ASSERT_EQUAL_INT16(-384, result);
}

void test_float_to_q8_8_zero() {
    int16_t result = float_to_q8_8(0.0f);

    TEST_ASSERT_EQUAL_INT16(0, result);
}

void test_q8_8_to_float_roundtrip() {
    float result = q8_8_to_float(512);
    (void)result;

    TEST_ASSERT_EQUAL_FLOAT(2.0f, result);
}

void test_q8_8_multiply_positive() {
    int16_t data1 = float_to_q8_8(2.0f);
    int16_t data2 = float_to_q8_8(1.5f);
    int16_t result = q8_8_multiply(data1, data2);

    TEST_ASSERT_EQUAL_INT16(768, result);
}

void test_q8_8_multiply_negative() {
    int16_t data1 = float_to_q8_8(-2.0f);
    int16_t data2 = float_to_q8_8(1.5f);
    int16_t result = q8_8_multiply(data1, data2);

    TEST_ASSERT_EQUAL_INT16(-768, result);
}

void test_q8_8_exceeds_threshold_true() {
    int16_t sample = 400;
    int16_t threshold = 300;
    bool result = q8_8_exceeds_threshold(sample, threshold);

    TEST_ASSERT_TRUE(result);
}

void test_q8_8_exceeds_threshold_false() {
    int16_t sample = 200;
    int16_t threshold = 300;
    bool result = q8_8_exceeds_threshold(sample, threshold);

    TEST_ASSERT_FALSE(result);
}