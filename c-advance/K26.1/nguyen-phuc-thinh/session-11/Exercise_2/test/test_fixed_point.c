/**
 * @file test_fixed_point.c
 * @brief Unity unit tests for the fixed-point math module.
 */
#include "unity.h"
#include "fixed_point.h"

void setUp(void) {}
void tearDown(void) {}

void test_float_to_q8_8_positive(void)
{
    TEST_ASSERT_EQUAL_INT16(448, float_to_q8_8(1.75f));
}

void test_float_to_q8_8_negative(void)
{
    TEST_ASSERT_EQUAL_INT16(-384, float_to_q8_8(-1.5f));
}

void test_float_to_q8_8_zero(void)
{
    TEST_ASSERT_EQUAL_INT16(0, float_to_q8_8(0.0f));
}

void test_q8_8_to_float_roundtrip(void)
{
    float original = 2.25f;
    int16_t q_val = float_to_q8_8(original);
    float result = q8_8_to_float(q_val);
    /* Float comparison using threshold */
    TEST_ASSERT_FLOAT_WITHIN(0.001f, original, result);
}

void test_q8_8_multiply_positive(void)
{
    int16_t a = float_to_q8_8(2.0f);  /* 512 */
    int16_t b = float_to_q8_8(1.5f);  /* 384 */
    int16_t expected = float_to_q8_8(3.0f); /* 768 */
    TEST_ASSERT_EQUAL_INT16(expected, q8_8_multiply(a, b));
}

void test_q8_8_multiply_negative(void)
{
    int16_t a = float_to_q8_8(2.0f);   /* 512 */
    int16_t b = float_to_q8_8(-1.5f);  /* -384 */
    int16_t expected = float_to_q8_8(-3.0f); /* -768 */
    TEST_ASSERT_EQUAL_INT16(expected, q8_8_multiply(a, b));
}

void test_q8_8_exceeds_threshold_true(void)
{
    int16_t sample = float_to_q8_8(5.0f);
    int16_t threshold = float_to_q8_8(4.0f);
    TEST_ASSERT_TRUE(q8_8_exceeds_threshold(sample, threshold));
}

void test_q8_8_exceeds_threshold_false(void)
{
    int16_t sample = float_to_q8_8(3.0f);
    int16_t threshold = float_to_q8_8(4.0f);
    TEST_ASSERT_FALSE(q8_8_exceeds_threshold(sample, threshold));
}

int main(void)
{
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