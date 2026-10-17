#include "sensor.h"
#include "unity.h"
#include <stdint.h>

/**
 * @brief Set up the test environment before each test.
 */
void setUp(void);

/**
 * @brief Clean up the test environment after each test.
 */
void tearDown(void);

/**
 * @brief Test sensor reading boundary handling.
 */
void test_sensor_process_readings_bounds(void);

/**
 * @brief Test safe accumulation of sensor values without overflow.
 */
void test_sensor_accumulate_overflow_safe(void);

/**
 * @brief Test that the sensor calibration value is valid.
 */
void test_sensor_get_calibration_valid(void);

int32_t main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_sensor_process_readings_bounds);
  RUN_TEST(test_sensor_accumulate_overflow_safe);
  RUN_TEST(test_sensor_get_calibration_valid);
  return UNITY_END();
}

void setUp(void) {}

void tearDown(void) {}

void test_sensor_process_readings_bounds(void) {
  uint8_t samples[SENSOR_SAMPLE_COUNT] = {1, 2, 3, 4, 5, 6, 7, 8};
  sensor_process_readings(samples, SENSOR_SAMPLE_COUNT);
  TEST_ASSERT_EQUAL_UINT8(2U, samples[0]);
  TEST_ASSERT_EQUAL_UINT8(16U, samples[7]);
}

void test_sensor_accumulate_overflow_safe(void) {
  int32_t base = 2147483640;
  int32_t sample = 100;
  int32_t res = sensor_accumulate_data(base, sample);
  /* Expect safe saturation or clamped result */
  TEST_ASSERT_TRUE(res > 0);
}

void test_sensor_get_calibration_valid(void) {
  uint32_t cal = sensor_get_calibration();
  TEST_ASSERT_EQUAL_HEX32(0xDEADBEEFU, cal);
}