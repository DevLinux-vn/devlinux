#include "ring_buffer.h"
#include "unity.h"

static ring_buffer_t g_rb;

/**
 * @brief Sets up the test environment before each test.
 */
void setUp(void);

/**
 * @brief Cleans up the test environment after each test.
 */
void tearDown(void);

/**
 * @brief Tests that a newly initialized ring buffer is empty.
 */
void test_ring_buffer_init_empty(void);

/**
 * @brief Tests push and pop operations using FIFO order.
 */
void test_ring_buffer_push_pop_fifo(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ring_buffer_init_empty);
  RUN_TEST(test_ring_buffer_push_pop_fifo);

  return UNITY_END();
}

void setUp(void) { ring_init(&g_rb); }

void tearDown(void) {}

void test_ring_buffer_init_empty(void) {
  TEST_ASSERT_TRUE(ring_is_empty(&g_rb));
  TEST_ASSERT_FALSE(ring_is_full(&g_rb));
  TEST_ASSERT_EQUAL_UINT32(0U, ring_count(&g_rb));
}

void test_ring_buffer_push_pop_fifo(void) {
  ring_push(&g_rb, 0x11U);
  ring_push(&g_rb, 0x22U);

  TEST_ASSERT_EQUAL_UINT32(2U, ring_count(&g_rb));
  TEST_ASSERT_EQUAL_UINT8(0x11U, ring_pop(&g_rb));
  TEST_ASSERT_EQUAL_UINT8(0x22U, ring_pop(&g_rb));
  TEST_ASSERT_TRUE(ring_is_empty(&g_rb));
}