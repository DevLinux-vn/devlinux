#include "common_types.h"
#include "data_proc.h"
#include "event_log.h"
#include "ring_buffer.h"
#include "unity.h"
#include <stdint.h>
#include <unity_internals.h>

/**
 * @brief Set up the test fixture before each test case.
 */
void setUp(void);

/**
 * @brief Tear down the test fixture after each test case.
 */
void tearDown(void);

/**
 * @brief Verify that a newly initialized ring buffer is empty.
 */
void test_ring_buffer_init_empty(void);

/**
 * @brief Verify FIFO behavior when pushing and popping items.
 */
void test_ring_buffer_push_pop_fifo(void);

/**
 * @brief Verify ring buffer behavior when the read/write positions wrap around.
 */
void test_ring_buffer_wrap_around(void);

/**
 * @brief Verify that the ring buffer correctly detects a full state.
 */
void test_ring_buffer_full_detection(void);

/**
 * @brief Verify data processing behavior during the startup phase.
 */
void test_data_proc_startup_phase(void);

/**
 * @brief Verify data processing behavior during the steady-state phase.
 */
void test_data_proc_steady_state(void);

/**
 * @brief Verify that spike smoothing reduces the effect of abnormal input
 * values.
 */
void test_data_proc_spike_smoothing(void);

/**
 * @brief Verify that event log entries are inserted in sorted order.
 */
void test_event_log_sorted_insert(void);

/**
 * @brief Verify that the event log respects its bounded capacity.
 */
void test_event_log_bounded_capacity(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ring_buffer_init_empty);
  RUN_TEST(test_ring_buffer_push_pop_fifo);
  RUN_TEST(test_ring_buffer_wrap_around);
  RUN_TEST(test_ring_buffer_full_detection);
  RUN_TEST(test_data_proc_startup_phase);
  RUN_TEST(test_data_proc_steady_state);
  RUN_TEST(test_data_proc_spike_smoothing);
  RUN_TEST(test_event_log_sorted_insert);
  RUN_TEST(test_event_log_bounded_capacity);

  return UNITY_END();
}

void setUp(void) {}

void tearDown(void) {}

void test_ring_buffer_init_empty(void) {
  ring_buffer_t rb;

  ring_init(&rb);

  TEST_ASSERT_TRUE(ring_is_empty(&rb));
  TEST_ASSERT_FALSE(ring_is_full(&rb));
  TEST_ASSERT_EQUAL_UINT32(0U, ring_count(&rb));
}

void test_ring_buffer_push_pop_fifo(void) {
  ring_buffer_t rb;
  int16_t value;

  ring_init(&rb);

  ring_push(&rb, 100);
  ring_push(&rb, 200);
  ring_push(&rb, 300);

  TEST_ASSERT_EQUAL_UINT32(3U, ring_count(&rb));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, ring_pop(&rb, &value));
  TEST_ASSERT_EQUAL_INT16(100, value);

  TEST_ASSERT_EQUAL_INT(STATUS_OK, ring_pop(&rb, &value));
  TEST_ASSERT_EQUAL_INT16(200, value);

  TEST_ASSERT_EQUAL_INT(STATUS_OK, ring_pop(&rb, &value));
  TEST_ASSERT_EQUAL_INT16(300, value);

  TEST_ASSERT_TRUE(ring_is_empty(&rb));
}

void test_ring_buffer_wrap_around(void) {
  ring_buffer_t rb;
  int16_t value;
  uint32_t index;

  ring_init(&rb);

  for (index = 0U; index < RING_BUFFER_CAPACITY; index++) {
    ring_push(&rb, (int16_t)index);
  }

  for (index = 0U; index < 8U; index++) {
    TEST_ASSERT_EQUAL_INT(STATUS_OK, ring_pop(&rb, &value));
    TEST_ASSERT_EQUAL_INT16((int16_t)index, value);
  }

  for (index = 16U; index < 24U; index++) {
    ring_push(&rb, (int16_t)index);
  }

  for (index = 8U; index < 24U; index++) {
    TEST_ASSERT_EQUAL_INT(STATUS_OK, ring_pop(&rb, &value));
    TEST_ASSERT_EQUAL_INT16((int16_t)index, value);
  }

  TEST_ASSERT_TRUE(ring_is_empty(&rb));
}

void test_ring_buffer_full_detection(void) {
  ring_buffer_t rb;
  uint32_t index;

  ring_init(&rb);

  for (index = 0U; index < RING_BUFFER_CAPACITY; index++) {
    ring_push(&rb, (int16_t)index);
  }

  TEST_ASSERT_TRUE(ring_is_full(&rb));
  TEST_ASSERT_FALSE(ring_is_empty(&rb));
  TEST_ASSERT_EQUAL_UINT32(RING_BUFFER_CAPACITY, ring_count(&rb));
}

void test_data_proc_startup_phase(void) {
  data_proc_t proc;

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_init(&proc, 4U));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 100));
  TEST_ASSERT_EQUAL_INT16(100, data_proc_get_average(&proc));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 200));
  TEST_ASSERT_EQUAL_INT16(150, data_proc_get_average(&proc));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 300));
  TEST_ASSERT_EQUAL_INT16(200, data_proc_get_average(&proc));
}

void test_data_proc_steady_state(void) {
  data_proc_t proc;

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_init(&proc, 4U));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 100));
  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 200));
  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 300));
  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 400));

  TEST_ASSERT_EQUAL_INT16(250, data_proc_get_average(&proc));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 500));

  TEST_ASSERT_EQUAL_INT16(350, data_proc_get_average(&proc));
}

void test_data_proc_spike_smoothing(void) {
  data_proc_t proc;

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_init(&proc, 4U));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 100));
  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 100));
  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 100));
  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 100));

  TEST_ASSERT_EQUAL_INT16(100, data_proc_get_average(&proc));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, data_proc_add_sample(&proc, 500));

  TEST_ASSERT_EQUAL_INT16(200, data_proc_get_average(&proc));
}

void test_event_log_sorted_insert(void) {
  event_log_init();

  TEST_ASSERT_EQUAL_INT(STATUS_OK, event_log_add(EVENT_SYSTEM_RESET, 10U, 0));

  TEST_ASSERT_EQUAL_INT(STATUS_OK, event_log_add(EVENT_SENSOR_ERROR, 50U, 0));

  TEST_ASSERT_EQUAL_INT(STATUS_OK,
                        event_log_add(EVENT_ALERT_TEMP_HIGH, 100U, 351));

  TEST_ASSERT_EQUAL_UINT32(3U, event_log_get_count());
}

void test_event_log_bounded_capacity(void) {
  uint32_t index;

  event_log_init();

  for (index = 0U; index < EVENT_LOG_MAX_ENTRIES; index++) {
    TEST_ASSERT_EQUAL_INT(
        STATUS_OK, event_log_add(EVENT_SENSOR_ERROR, index, (int16_t)index));
  }

  TEST_ASSERT_EQUAL_UINT32(EVENT_LOG_MAX_ENTRIES, event_log_get_count());

  TEST_ASSERT_EQUAL_INT(
      STATUS_OK,
      event_log_add(EVENT_ALERT_TEMP_HIGH, EVENT_LOG_MAX_ENTRIES + 100U, 999));

  TEST_ASSERT_EQUAL_UINT32(EVENT_LOG_MAX_ENTRIES, event_log_get_count());
}