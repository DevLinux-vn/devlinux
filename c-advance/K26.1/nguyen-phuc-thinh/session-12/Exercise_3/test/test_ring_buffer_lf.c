#include "unity.h"
#include "ring_buffer_lf.h"

ring_buf_lf_t rb;

void setUp(void)
{
    rb_init(&rb);
}

void tearDown(void) {}

void test_initial_state_empty(void)
{
    TEST_ASSERT_TRUE(rb_is_empty(&rb));
    TEST_ASSERT_FALSE(rb_is_full(&rb));
    TEST_ASSERT_EQUAL_UINT32(0, rb_available(&rb));
}

void test_push_until_full_waste_one_slot(void)
{
    for (int16_t i = 1; i <= 7; i++)
    {
        TEST_ASSERT_TRUE(rb_push(&rb, i));
    }
    TEST_ASSERT_TRUE(rb_is_full(&rb));
    TEST_ASSERT_EQUAL_UINT32(7, rb_available(&rb));
}

void test_push_full_returns_false(void)
{
    for (int16_t i = 1; i <= 7; i++)
    {
        rb_push(&rb, i);
    }
    TEST_ASSERT_FALSE(rb_push(&rb, 8));
}

void test_pop_empty_returns_false(void)
{
    int16_t val;
    TEST_ASSERT_FALSE(rb_pop(&rb, &val));
}

void test_wrap_around_pointer_math(void)
{
    int16_t val;
    for (int16_t i = 1; i <= 7; i++)
    {
        rb_push(&rb, i);
    }
    for (int i = 0; i < 3; i++)
    {
        rb_pop(&rb, &val);
    }
    
    TEST_ASSERT_TRUE(rb_push(&rb, 8));
    TEST_ASSERT_TRUE(rb_push(&rb, 9));
    TEST_ASSERT_TRUE(rb_push(&rb, 10));
    
    TEST_ASSERT_TRUE(rb_is_full(&rb));
    
    rb_pop(&rb, &val);
    TEST_ASSERT_EQUAL_INT16(4, val); 
}

void test_peek_does_not_advance_tail(void)
{
    int16_t val;
    rb_push(&rb, 42);
    TEST_ASSERT_TRUE(rb_peek(&rb, &val));
    TEST_ASSERT_EQUAL_INT16(42, val);
    TEST_ASSERT_EQUAL_UINT32(1, rb_available(&rb)); 
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