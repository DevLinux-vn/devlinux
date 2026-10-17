/**
 * @file test_float_inspector.c
 * @brief Unity unit tests for the IEEE-754 float inspector.
 */
#include "unity.h"
#include "float_inspector.h"
#include <math.h>

void setUp(void) {}
void tearDown(void) {}

void test_classify_positive_infinity(void)
{
    TEST_ASSERT_EQUAL_STRING("+Infinity", classify_float(1.0f / 0.0f));
}

void test_classify_negative_infinity(void)
{
    TEST_ASSERT_EQUAL_STRING("-Infinity", classify_float(-1.0f / 0.0f));
}

void test_classify_nan(void)
{
    TEST_ASSERT_EQUAL_STRING("NaN", classify_float(sqrtf(-1.0f)));
}

void test_classify_negative_zero(void)
{
    TEST_ASSERT_EQUAL_STRING("Negative Zero", classify_float(-0.0f));
}

void test_classify_normal(void)
{
    TEST_ASSERT_EQUAL_STRING("Normal", classify_float(42.0f));
}

void test_float_equal_within_epsilon(void)
{
    TEST_ASSERT_TRUE(float_equal(0.1f + 0.2f, 0.3f, 1e-6f));
}

void test_float_equal_outside_epsilon(void)
{
    TEST_ASSERT_FALSE(float_equal(0.1f + 0.2f, 0.4f, 1e-6f));
}

void test_inspect_sign_bit(void)
{
    float_inspector_t pos = { .f = 1.0f };
    float_inspector_t neg = { .f = -1.0f };
    TEST_ASSERT_EQUAL_UINT32(0, pos.bits.sign);
    TEST_ASSERT_EQUAL_UINT32(1, neg.bits.sign);
}

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