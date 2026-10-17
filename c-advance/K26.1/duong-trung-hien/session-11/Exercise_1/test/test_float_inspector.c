#include "float_inspector.h"
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
 * @brief Test classification of positive infinity.
 *
 * Verifies that a positive infinity floating-point value is correctly
 * classified.
 */
void test_classify_positive_infinity();

/**
 * @brief Test classification of negative infinity.
 *
 * Verifies that a negative infinity floating-point value is correctly
 * classified.
 */
void test_classify_negative_infinity();

/**
 * @brief Test classification of NaN.
 *
 * Verifies that a NaN (Not a Number) floating-point value is correctly
 * classified.
 */
void test_classify_nan();

/**
 * @brief Test classification of negative zero.
 *
 * Verifies that a negative zero floating-point value is correctly
 * classified.
 */
void test_classify_negative_zero();

/**
 * @brief Test classification of a normal floating-point value.
 *
 * Verifies that a normal finite floating-point value is correctly
 * classified.
 */
void test_classify_normal();

/**
 * @brief Test floating-point comparison within epsilon.
 *
 * Verifies that two floating-point values whose absolute difference is
 * within the specified epsilon are considered equal.
 */
void test_float_equal_within_epsilon();

/**
 * @brief Test floating-point comparison outside epsilon.
 *
 * Verifies that two floating-point values whose absolute difference exceeds
 * the specified epsilon are not considered equal.
 */
void test_float_equal_outside_epsilon();

/**
 * @brief Test extraction of the floating-point sign bit.
 *
 * Verifies that the sign bit is correctly extracted from a floating-point
 * value by the inspection function.
 */
void test_inspect_sign_bit();


int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_classify_positive_infinity);
    RUN_TEST(test_classify_negative_infinity);
    RUN_TEST(test_classify_nan);
    RUN_TEST(test_classify_negative_zero);
    RUN_TEST(test_classify_normal);
    RUN_TEST(test_float_equal_within_epsilon);
    RUN_TEST(test_float_equal_outside_epsilon);
    RUN_TEST(test_inspect_sign_bit);

    return UNITY_END();
}

void setUp() {
    
}

void tearDown() {

}

void test_classify_positive_infinity() {
    float_inspector_t data;

    TEST_ASSERT_EQUAL_STRING("+Infinity", classify_float((1.0f / 0.0f), &data));
}

void test_classify_negative_infinity() {
    float_inspector_t data;

    TEST_ASSERT_EQUAL_STRING("-Infinity", classify_float((-1.0f / 0.0f), &data));
}

void test_classify_nan() {
    float_inspector_t data;

    TEST_ASSERT_EQUAL_STRING("NaN", classify_float((0.0f / 0.0f), &data));
}

void test_classify_negative_zero() {
    float_inspector_t data;

    TEST_ASSERT_EQUAL_STRING("Negative Zero", classify_float((-0.0f), &data));
}

void test_classify_normal() {
    float_inspector_t data;

    TEST_ASSERT_EQUAL_STRING("Normal", classify_float((42.0f), &data));
}

void test_float_equal_within_epsilon() {
    float a = 0.1f + 0.2f;
    float b = 0.3f;
    float epsilon = 1e-6;

    TEST_ASSERT_TRUE(float_equal(a, b, epsilon));
}

void test_float_equal_outside_epsilon() {
    float a = 0.1f + 0.2f;
    float b = 0.31f;
    float epsilon = 1e-6;

    TEST_ASSERT_FALSE(float_equal(a, b, epsilon));
}

void test_inspect_sign_bit() {
    float_inspector_t data;
    
    inspect_float(1.0f, &data);
    TEST_ASSERT_EQUAL_UINT8(0U, data.bits.sign);

    inspect_float(-1.0f, &data);
    TEST_ASSERT_EQUAL_UINT8(1U, data.bits.sign);
}