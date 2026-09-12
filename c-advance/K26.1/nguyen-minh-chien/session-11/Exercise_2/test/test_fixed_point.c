/**
 * @file    test_fixed_point.c
 * @brief   Unity unit tests for the Q8.8 fixed-point module.
 */
#include "unity.h"
#include "fixed_point.h"

#include <math.h>

/**
 * @brief Unity setup hook. No shared state to reset.
 */
// cppcheck-suppress unusedFunction
void setUp(void)
{
    /* No per-test setup required. */
}

/**
 * @brief Unity teardown hook. No shared state to release.
 */
// cppcheck-suppress unusedFunction
void tearDown(void)
{
    /* No per-test teardown required. */
}

/**
 * @brief float_to_q8_8() must scale a positive value by 256.
 */
void test_float_to_q8_8_positive(void)
{
    /* 1.75 * 256 = 448 */
    TEST_ASSERT_EQUAL_INT16(448, float_to_q8_8(1.75f));
}

/**
 * @brief float_to_q8_8() must scale a negative value by 256.
 */
void test_float_to_q8_8_negative(void)
{
    /* -2.5 * 256 = -640 */
    TEST_ASSERT_EQUAL_INT16(-640, float_to_q8_8(-2.5f));
}

/**
 * @brief float_to_q8_8() must map 0.0f to 0.
 */
void test_float_to_q8_8_zero(void)
{
    TEST_ASSERT_EQUAL_INT16(0, float_to_q8_8(0.0f));
}

/**
 * @brief Converting float -> Q8.8 -> float should round-trip to
 *        within one Q8.8 "step" (1/256 = 0.00390625f) of the
 *        original value, since Q8.8 has finite fractional precision.
 */
void test_q8_8_to_float_roundtrip(void)
{
    const float original = 3.5f;
    const int16_t q = float_to_q8_8(original);
    const float back = q8_8_to_float(q);

    /* Q8.8 has a fixed resolution of 1/256; any round-trip error must
     * be smaller than one full step. */
    TEST_ASSERT_TRUE(fabsf(back - original) < (1.0f / (float)Q8_8_SCALE));
}

/**
 * @brief q8_8_multiply() must correctly multiply two positive Q8.8
 *        values via the 32-bit intermediate.
 */
void test_q8_8_multiply_positive(void)
{
    /* Deliberately small operands: 10.0 * 1.75 = 17.5, which stays
     * well inside Q8.8's +/-127.996 range, so this test exercises
     * the multiplication math itself rather than the saturation
     * path (that is covered separately, see the pipeline demo's
     * documented saturation behavior for larger values). */
    const int16_t q_10 = float_to_q8_8(10.0f);   /* 2560 */
    const int16_t q_1_75 = float_to_q8_8(1.75f); /* 448  */

    /* 10 * 1.75 = 17.5 -> Q8.8 = 4480 */
    TEST_ASSERT_EQUAL_INT16(4480, q8_8_multiply(q_10, q_1_75));
}

/**
 * @brief q8_8_multiply() must correctly multiply a negative Q8.8
 *        value via the 32-bit intermediate.
 */
void test_q8_8_multiply_negative(void)
{
    const int16_t q_neg_10 = float_to_q8_8(-10.0f); /* -2560 */
    const int16_t q_1_75 = float_to_q8_8(1.75f);    /* 448   */

    /* -10 * 1.75 = -17.5 -> Q8.8 = -4480 */
    TEST_ASSERT_EQUAL_INT16(-4480, q8_8_multiply(q_neg_10, q_1_75));
}

/**
 * @brief q8_8_multiply() must saturate (not wrap) when the true
 *        product's magnitude exceeds what Q8.8/int16_t can hold.
 */
void test_q8_8_multiply_saturates_on_overflow(void)
{
    const int16_t q_100 = float_to_q8_8(100.0f); /* 25600 */
    const int16_t q_1_75 = float_to_q8_8(1.75f); /* 448   */

    /* True result is 175.0 (Q8.8 = 44800), which does not fit in
     * int16_t (max 32767) -- must saturate, not silently wrap to a
     * negative value. */
    TEST_ASSERT_EQUAL_INT16(INT16_MAX, q8_8_multiply(q_100, q_1_75));
}

/**
 * @brief q8_8_exceeds_threshold() must return true when the sample
 *        is strictly greater than the threshold.
 */
void test_q8_8_exceeds_threshold_true(void)
{
    const int16_t sample = float_to_q8_8(50.0f);
    const int16_t threshold = float_to_q8_8(10.0f);

    TEST_ASSERT_TRUE(q8_8_exceeds_threshold(sample, threshold));
}

/**
 * @brief q8_8_exceeds_threshold() must return false when the sample
 *        does not exceed the threshold.
 */
void test_q8_8_exceeds_threshold_false(void)
{
    const int16_t sample = float_to_q8_8(5.0f);
    const int16_t threshold = float_to_q8_8(10.0f);

    TEST_ASSERT_FALSE(q8_8_exceeds_threshold(sample, threshold));
}

/**
 * @brief sample_to_q8_8() must succeed for a raw sample within the
 *        representable Q8.8 integer range ([-128, 127]).
 */
void test_sample_to_q8_8_in_range(void)
{
    int16_t out = 0;

    TEST_ASSERT_TRUE(sample_to_q8_8((int16_t)100, &out));
    TEST_ASSERT_EQUAL_INT16(25600, out);
}

/**
 * @brief sample_to_q8_8() must detect overflow for a raw sample
 *        outside the representable Q8.8 integer range, such as
 *        INT16_MAX or INT16_MIN, and leave the output untouched.
 */
void test_sample_to_q8_8_overflow(void)
{
    int16_t out = 0;

    TEST_ASSERT_FALSE(sample_to_q8_8(INT16_MAX, &out));
    TEST_ASSERT_FALSE(sample_to_q8_8(INT16_MIN, &out));
}

/**
 * @brief Unity test runner entry point.
 * @return Number of failed tests (0 on full success).
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_float_to_q8_8_positive);
    RUN_TEST(test_float_to_q8_8_negative);
    RUN_TEST(test_float_to_q8_8_zero);
    RUN_TEST(test_q8_8_to_float_roundtrip);
    RUN_TEST(test_q8_8_multiply_positive);
    RUN_TEST(test_q8_8_multiply_negative);
    RUN_TEST(test_q8_8_multiply_saturates_on_overflow);
    RUN_TEST(test_q8_8_exceeds_threshold_true);
    RUN_TEST(test_q8_8_exceeds_threshold_false);
    RUN_TEST(test_sample_to_q8_8_in_range);
    RUN_TEST(test_sample_to_q8_8_overflow);

    return UNITY_END();
}