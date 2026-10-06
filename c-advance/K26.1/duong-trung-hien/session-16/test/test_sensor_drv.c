#include "common_types.h"
#include "hal_sim.h"
#include "hal_timer.h"
#include "humi_drv.h"
#include "sensor_intf.h"
#include "sensor_mgr.h"
#include "temp_drv.h"
#include "unity.h"
#include <stddef.h>
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
 * @brief Verify that the temperature driver initializes successfully.
 */
void test_temp_drv_init(void);

/**
 * @brief Verify that the temperature driver returns a valid reading.
 */
void test_temp_drv_read_returns_valid(void);

/**
 * @brief Verify that the humidity driver initializes successfully.
 */
void test_humi_drv_init(void);

/**
 * @brief Verify that the humidity driver returns a valid reading.
 */
void test_humi_drv_read_returns_valid(void);

/**
 * @brief Verify that the sensor manager reads all supported sensors
 * successfully.
 */
void test_sensor_mgr_read_all(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_temp_drv_init);
  RUN_TEST(test_temp_drv_read_returns_valid);
  RUN_TEST(test_humi_drv_init);
  RUN_TEST(test_humi_drv_read_returns_valid);
  RUN_TEST(test_sensor_mgr_read_all);

  return UNITY_END();
}

void setUp(void) {
  hal_sim_init(42U);
  hal_timer_init();
  hal_sim_update();

  (void)sensor_mgr_init();
}

void tearDown(void) {}

void test_temp_drv_init(void) {
  const sensor_intf_t *p_interface;
  status_t status;

  p_interface = temp_drv_get_interface();

  TEST_ASSERT_NOT_NULL(p_interface);

  if (NULL == p_interface) {
    return;
  }

  TEST_ASSERT_NOT_NULL(p_interface->init);

  if (NULL == p_interface->init) {
    return;
  }

  status = p_interface->init();

  TEST_ASSERT_EQUAL(STATUS_OK, status);
}

void test_temp_drv_read_returns_valid(void) {
  const sensor_intf_t *p_interface;
  int16_t value;
  status_t status;

  p_interface = temp_drv_get_interface();

  TEST_ASSERT_NOT_NULL(p_interface);

  if (NULL == p_interface) {
    return;
  }

  (void)p_interface->init();

  value = 0;

  status = p_interface->read(&value);

  TEST_ASSERT_EQUAL(STATUS_OK, status);
  TEST_ASSERT_TRUE(value >= 0);
}

void test_humi_drv_init(void) {
  const sensor_intf_t *p_interface;
  status_t status;

  p_interface = humi_drv_get_interface();

  TEST_ASSERT_NOT_NULL(p_interface);

  if (NULL == p_interface) {
    return;
  }

  TEST_ASSERT_NOT_NULL(p_interface->init);

  if (NULL == p_interface->init) {
    return;
  }

  status = p_interface->init();

  TEST_ASSERT_EQUAL(STATUS_OK, status);
}

void test_humi_drv_read_returns_valid(void) {
  const sensor_intf_t *p_interface;
  int16_t value;
  status_t status;

  p_interface = humi_drv_get_interface();

  TEST_ASSERT_NOT_NULL(p_interface);

  if (NULL == p_interface) {
    return;
  }

  (void)p_interface->init();

  value = 0;

  status = p_interface->read(&value);

  TEST_ASSERT_EQUAL(STATUS_OK, status);
  TEST_ASSERT_TRUE(value >= 0);
}

void test_sensor_mgr_read_all(void) {
  sensor_data_t data;
  status_t status;

  status = sensor_mgr_read_all(&data);

  TEST_ASSERT_EQUAL(STATUS_OK, status);
  TEST_ASSERT_TRUE(data.temperature >= 0);
  TEST_ASSERT_TRUE(data.humidity >= 0);
  TEST_ASSERT_EQUAL_UINT32(0U, data.timestamp);
}