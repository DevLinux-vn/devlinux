#include "hal_sim.h"
#include "hal_timer.h"
#include <stdint.h>
#include <stdlib.h>
#include <unity.h>
#include <unity_internals.h>

/**
 * @brief Sets up the test environment before each test case.
 */
void setUp(void);

/**
 * @brief Cleans up the test environment after each test case.
 */
void tearDown(void);

/**
 * @brief Verifies that HAL simulation initialization sets the random seed.
 */
void test_hal_sim_init_sets_seed(void);

/**
 * @brief Verifies reading from and writing to simulated registers.
 */
void test_hal_sim_read_write_register(void);

/**
 * @brief Verifies that the HAL simulation update changes register values.
 */
void test_hal_sim_update_changes_values(void);

/**
 * @brief Verifies that timer initialization resets the timer tick.
 */
void test_hal_timer_init_resets(void);

/**
 * @brief Verifies that each timer tick increments the timer counter.
 */
void test_hal_timer_tick_increments(void);

int32_t main() {
  UNITY_BEGIN();

  RUN_TEST(test_hal_sim_init_sets_seed);
  RUN_TEST(test_hal_sim_read_write_register);
  RUN_TEST(test_hal_sim_update_changes_values);
  RUN_TEST(test_hal_timer_init_resets);
  RUN_TEST(test_hal_timer_tick_increments);

  return UNITY_END();
}

void setUp() { hal_sim_init(50U); }

void tearDown() {}

void test_hal_sim_init_sets_seed() {
  hal_sim_init(100U);
  int32_t expected = rand(); // NOLINT

  hal_sim_init(100U);
  int32_t actual = rand(); // NOLINT

  TEST_ASSERT_EQUAL_INT32(expected, actual);
}

void test_hal_sim_read_write_register() {
  uint16_t reg_addr = HAL_REG_TEMP_RAW;
  uint16_t value = 0x0A3CU;

  hal_sim_write_register(reg_addr, value);

  TEST_ASSERT_EQUAL_UINT16(value, hal_sim_read_register(reg_addr));
}

void test_hal_sim_update_changes_values() {
  hal_sim_update();
  uint16_t temp_before = hal_sim_read_register(HAL_REG_TEMP_RAW);
  uint16_t humi_before = hal_sim_read_register(HAL_REG_HUMI_RAW);

  hal_sim_update();
  uint16_t temp_after = hal_sim_read_register(HAL_REG_TEMP_RAW);
  uint16_t humi_after = hal_sim_read_register(HAL_REG_HUMI_RAW);

  TEST_ASSERT_TRUE((temp_before != temp_after) || (humi_before != humi_after));
}

void test_hal_timer_init_resets() {
  hal_timer_init();

  hal_timer_tick();
  hal_timer_tick();

  hal_timer_init();

  TEST_ASSERT_EQUAL_UINT32(0U, hal_timer_get_tick());
}

void test_hal_timer_tick_increments() {
  hal_timer_init();
  TEST_ASSERT_EQUAL_UINT32(0U, hal_timer_get_tick());

  hal_timer_tick();
  TEST_ASSERT_EQUAL_UINT32(1U, hal_timer_get_tick());

  hal_timer_tick();
  TEST_ASSERT_EQUAL_UINT32(2U, hal_timer_get_tick());
}
