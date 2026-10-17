/**
 * @file    test_float_inspector.c
 * @brief   Unity unit tests for the IEEE-754 Float Inspector module.
 */
#include "unity.h"
#include "float_inspector.h"

#include <math.h>

/**
 * @brief Unity setup hook, run before each test. No shared state to
 *        reset for this module, so it is intentionally empty.
 *
 * @note Called by Unity's RUN_TEST() macro before every test case;
 *       static analyzers that do not parse the Unity framework
 *       source cannot see that call site, hence the suppression.
 */
// cppcheck-suppress unusedFunction
void setUp(void)
{
    /* No per-test setup required. */
}

/**
 * @brief Unity teardown hook, run after each test. No shared state to
 *        release for this module, so it is intentionally empty.
 *
 * @note Called by Unity's RUN_TEST() macro after every test case;
 *       see the note on setUp() above.
 */
// cppcheck-suppress unusedFunction
void tearDown(void)
{
    /* No per-test teardown required. */
}

/**
 * @brief classify_float() must report positive infinity correctly.
 */
void test_classify_positive_infinity(void)
{
    TEST_ASSERT_EQUAL_STRING("+Infinity", classify_float(1.0f / 0.0f));
}

/**
 * @brief classify_float() must report negative infinity correctly.
 */
void test_classify_negative_infinity(void)
{
    TEST_ASSERT_EQUAL_STRING("-Infinity", classify_float(-1.0f / 0.0f));
}

/**
 * @brief classify_float() must report NaN correctly, for both a
 *        0.0f/0.0f NaN and a sqrtf(-1.0f) NaN.
 */
void test_classify_nan(void)
{
    /* Both expressions below are intentionally out-of-domain /
     * self-dividing to produce NaN, exercising classify_float()'s
     * NaN branch -- not mistakes. */
    // cppcheck-suppress duplicateExpression
    TEST_ASSERT_EQUAL_STRING("NaN", classify_float(0.0f / 0.0f));
    // cppcheck-suppress invalidFunctionArg
    TEST_ASSERT_EQUAL_STRING("NaN", classify_float(sqrtf(-1.0f)));
}

/**
 * @brief classify_float() must distinguish -0.0f from +0.0f as
 *        "Negative Zero", even though they compare equal with ==.
 */
void test_classify_negative_zero(void)
{
    TEST_ASSERT_EQUAL_STRING("Negative Zero", classify_float(-0.0f));
    TEST_ASSERT_EQUAL_STRING("Normal", classify_float(0.0f));
}

/**
 * @brief classify_float() must report an ordinary finite value as
 *        "Normal".
 */
void test_classify_normal(void)
{
    TEST_ASSERT_EQUAL_STRING("Normal", classify_float(42.0f));
    TEST_ASSERT_EQUAL_STRING("Normal", classify_float(-2.5f));
}

/**
 * @brief float_equal() must return true when the difference is
 *        smaller than the given epsilon (e.g. 0.1f + 0.2f vs 0.3f).
 */
void test_float_equal_within_epsilon(void)
{
    volatile float a = 1.1f;
    volatile float b = 2.2f;
    volatile float sum = a + b;

    TEST_ASSERT_TRUE(float_equal(sum, 3.3f, 1e-6f));
}

/**
 * @brief float_equal() must return false when the difference is
 *        larger than or equal to the given epsilon.
 */
void test_float_equal_outside_epsilon(void)
{
    TEST_ASSERT_FALSE(float_equal(1.0f, 2.0f, 1e-6f));
}

/**
 * @brief The float_inspector_t union must correctly expose the sign
 *        bit for both a positive and a negative value.
 */
void test_inspect_sign_bit(void)
{
    float_inspector_t positive;
    float_inspector_t negative;

    positive.f = 1.0f;
    negative.f = -2.5f;

    TEST_ASSERT_EQUAL_UINT32(0U, positive.bits.sign);
    TEST_ASSERT_EQUAL_UINT32(1U, negative.bits.sign);
}

/**
 * @brief Unity test runner entry point.
 *
 * @return Number of failed tests (0 on full success), as required by
 *         Unity's RUN_TEST/UNITY_END convention.
 */
int main(void)
{
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