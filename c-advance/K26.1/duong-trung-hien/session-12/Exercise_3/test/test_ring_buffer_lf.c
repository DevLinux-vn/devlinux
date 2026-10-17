#include "ring_buffer_lf.h"
#include "unity.h"
#include <stdint.h>

/**
 * @brief Sets up the test fixture before each test.
 */
void setUp(void);

/**
 * @brief Cleans up the test fixture after each test.
 */
void tearDown(void);

/**
 * @brief Tests that a newly initialized ring buffer is empty.
 */
void test_initial_state_empty(void);

/**
 * @brief Tests that the ring buffer stores at most RB_CAPACITY - 1 elements.
 */
void test_push_until_full_waste_one_slot(void);

/**
 * @brief Tests that pushing to a full ring buffer returns false.
 */
void test_push_full_returns_false(void);

/**
 * @brief Tests that popping from an empty ring buffer returns false.
 */
void test_pop_empty_returns_false(void);

/**
 * @brief Tests that the head pointer wraps around correctly.
 */
void test_wrap_around_pointer_math(void);

/**
 * @brief Tests that peeking at the oldest element does not advance the tail pointer.
 */
void test_peek_does_not_advance_tail(void);

int32_t main() {
    UNITY_BEGIN();

    RUN_TEST(test_initial_state_empty);
    RUN_TEST(test_push_until_full_waste_one_slot);
    RUN_TEST(test_push_full_returns_false);
    RUN_TEST(test_pop_empty_returns_false);
    RUN_TEST(test_wrap_around_pointer_math);
    RUN_TEST(test_peek_does_not_advance_tail);

    return UNITY_END();
}

void setUp() {

}

void tearDown() {

}

void test_initial_state_empty() {
    ring_buf_lf_t data;

    rb_init(&data);

    TEST_ASSERT_EQUAL_UINT(data.head, data.tail);
    TEST_ASSERT_TRUE(rb_is_empty(&data));
}

void test_push_until_full_waste_one_slot() {
    ring_buf_lf_t data;

    rb_init(&data);

    for (size_t i = 0U; i < (RB_CAPACITY - 1U); i++) {
        TEST_ASSERT_TRUE(rb_push(&data, i * 10U));
    }

    TEST_ASSERT_TRUE(rb_is_full(&data));
    TEST_ASSERT_EQUAL_UINT(RB_CAPACITY - 1U, rb_available(&data));

}

void test_push_full_returns_false() {
    ring_buf_lf_t data;

    rb_init(&data);

    for (size_t i = 0U; i < (RB_CAPACITY - 1U); i++) {
        TEST_ASSERT_TRUE(rb_push(&data, i * 10U));
    }

    TEST_ASSERT_TRUE(rb_is_full(&data));
    TEST_ASSERT_FALSE(rb_push(&data, 80U));
}

void test_pop_empty_returns_false() {
    ring_buf_lf_t data;
    int16_t restored;

    rb_init(&data);

    TEST_ASSERT_TRUE(rb_is_empty(&data));
    TEST_ASSERT_FALSE(rb_pop(&data, &restored));

}

void test_wrap_around_pointer_math() {
    ring_buf_lf_t data;
    int16_t restored;

    rb_init(&data);

    for (size_t i = 0U; i < (RB_CAPACITY - 1U); i++) {
        TEST_ASSERT_TRUE(rb_push(&data, i * 10U));
    }

    TEST_ASSERT_EQUAL_UINT(RB_CAPACITY - 1U, data.head);

    TEST_ASSERT_TRUE(rb_pop(&data, &restored));
    TEST_ASSERT_TRUE(rb_push(&data, 80U));
    TEST_ASSERT_EQUAL_UINT(0U, data.head);
}

void test_peek_does_not_advance_tail() {
    ring_buf_lf_t data;
    int16_t restored;

    rb_init(&data);

    for (size_t i = 0U; i < (RB_CAPACITY - 1U); i++) {
        TEST_ASSERT_TRUE(rb_push(&data, i * 10U));
    }

    TEST_ASSERT_TRUE(rb_peek(&data, &restored));
    TEST_ASSERT_EQUAL_UINT(0U, data.tail);
    
}