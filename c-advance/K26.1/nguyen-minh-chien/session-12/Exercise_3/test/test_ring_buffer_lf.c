/**
 * @file test_ring_buffer_lf.c
 * @brief Unity tests for the SPSC lock-free ring buffer.
 */

#include <stdint.h>

#include "ring_buffer_lf.h"
#include "unity.h"

static ring_buf_lf_t g_rb;

void setUp(void)
{
    rb_init(&g_rb);
}

void tearDown(void)
{
}

static void test_initial_state_empty(void)
{
    TEST_ASSERT_TRUE(rb_is_empty(&g_rb));
    TEST_ASSERT_FALSE(rb_is_full(&g_rb));
    TEST_ASSERT_EQUAL_UINT32(0U, rb_available(&g_rb));
}

static void test_push_until_full_waste_one_slot(void)
{
    uint32_t index;

    for (index = 0U; index < (RB_CAPACITY - 1U); ++index)
    {
        TEST_ASSERT_TRUE(rb_push(&g_rb, (int16_t)(index + 1U)));
    }

    TEST_ASSERT_TRUE(rb_is_full(&g_rb));
    TEST_ASSERT_EQUAL_UINT32(RB_CAPACITY - 1U, rb_available(&g_rb));
}

static void test_push_full_returns_false(void)
{
    uint32_t index;

    for (index = 0U; index < (RB_CAPACITY - 1U); ++index)
    {
        TEST_ASSERT_TRUE(rb_push(&g_rb, (int16_t)(index + 10U)));
    }

    TEST_ASSERT_FALSE(rb_push(&g_rb, 1234));
    TEST_ASSERT_EQUAL_UINT32(RB_CAPACITY - 1U, rb_available(&g_rb));
}

static void test_pop_empty_returns_false(void)
{
    int16_t data = 0;

    TEST_ASSERT_FALSE(rb_pop(&g_rb, &data));
}

static void test_wrap_around_pointer_math(void)
{
    int16_t data;
    const int16_t expected_values[7] = {40, 50, 60, 70, 80, 90, 100};
    uint32_t index;

    TEST_ASSERT_TRUE(rb_push(&g_rb, 10));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 20));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 30));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 40));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 50));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 60));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 70));

    TEST_ASSERT_TRUE(rb_pop(&g_rb, &data));
    TEST_ASSERT_EQUAL_INT16(10, data);
    TEST_ASSERT_TRUE(rb_pop(&g_rb, &data));
    TEST_ASSERT_EQUAL_INT16(20, data);
    TEST_ASSERT_TRUE(rb_pop(&g_rb, &data));
    TEST_ASSERT_EQUAL_INT16(30, data);

    TEST_ASSERT_TRUE(rb_push(&g_rb, 80));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 90));
    TEST_ASSERT_TRUE(rb_push(&g_rb, 100));

    for (index = 0U; index < 7U; ++index)
    {
        TEST_ASSERT_TRUE(rb_pop(&g_rb, &data));
        TEST_ASSERT_EQUAL_INT16(expected_values[index], data);
    }

    TEST_ASSERT_TRUE(rb_is_empty(&g_rb));
}

static void test_peek_does_not_advance_tail(void)
{
    int16_t data;
    uint32_t tail_before;

    TEST_ASSERT_TRUE(rb_push(&g_rb, 55));
    tail_before = g_rb.tail;

    TEST_ASSERT_TRUE(rb_peek(&g_rb, &data));
    TEST_ASSERT_EQUAL_INT16(55, data);
    TEST_ASSERT_EQUAL_UINT32(tail_before, g_rb.tail);
    TEST_ASSERT_EQUAL_UINT32(1U, rb_available(&g_rb));

    TEST_ASSERT_TRUE(rb_pop(&g_rb, &data));
    TEST_ASSERT_EQUAL_INT16(55, data);
    TEST_ASSERT_TRUE(rb_is_empty(&g_rb));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_initial_state_empty);
    RUN_TEST(test_push_until_full_waste_one_slot);
    RUN_TEST(test_push_full_returns_false);
    RUN_TEST(test_pop_empty_returns_false);
    RUN_TEST(test_wrap_around_pointer_math);
    RUN_TEST(test_peek_does_not_advance_tail);

    return UNITY_END();
}
