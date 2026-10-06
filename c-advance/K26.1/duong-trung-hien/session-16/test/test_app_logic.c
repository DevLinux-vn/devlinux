#include "alert.h"
#include "app_fsm.h"
#include "cli_cmd.h"
#include "common_types.h"
#include "unity.h"
#include <stdint.h>
#include <unity_internals.h>

/** @brief Set up test fixtures before each test. */
void setUp(void);

/** @brief Clean up test fixtures after each test. */
void tearDown(void);

/** @brief Verify the FSM transitions from INIT to IDLE. */
void test_fsm_init_to_idle(void);

/** @brief Verify the FSM transitions from IDLE to MONITORING. */
void test_fsm_idle_to_monitoring(void);

/** @brief Verify the FSM transitions from MONITORING to ALERT. */
void test_fsm_monitoring_to_alert(void);

/** @brief Verify acknowledging an alert returns the FSM to the expected state.
 */
void test_fsm_alert_acknowledge(void);

/** @brief Verify resetting the FSM from ERROR restores normal operation. */
void test_fsm_error_reset(void);

/** @brief Verify an invalid FSM state returns an error. */
void test_fsm_invalid_state_returns_error(void);

/** @brief Verify the CLI dispatches a known command correctly. */
void test_cli_dispatch_known_command(void);

/** @brief Verify the CLI rejects an unknown command. */
void test_cli_dispatch_unknown_command(void);

/** @brief Verify alert thresholds are detected correctly. */
void test_alert_threshold_detection(void);

/** @brief Verify acknowledging an alert clears the active alert condition. */
void test_alert_acknowledge_clears(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_fsm_init_to_idle);
  RUN_TEST(test_fsm_idle_to_monitoring);
  RUN_TEST(test_fsm_monitoring_to_alert);
  RUN_TEST(test_fsm_alert_acknowledge);
  RUN_TEST(test_fsm_error_reset);
  RUN_TEST(test_fsm_invalid_state_returns_error);
  RUN_TEST(test_cli_dispatch_known_command);
  RUN_TEST(test_cli_dispatch_unknown_command);
  RUN_TEST(test_alert_threshold_detection);
  RUN_TEST(test_alert_acknowledge_clears);

  return UNITY_END();
}

void setUp(void) {}

void tearDown(void) {}

/**
 * @brief Test INIT -> IDLE transition.
 */
void test_fsm_init_to_idle(void) {
  const system_state_t next_state = fsm_run(SYS_INIT, EVT_INIT_OK);

  TEST_ASSERT_EQUAL(SYS_IDLE, next_state);
}

/**
 * @brief Test IDLE -> MONITORING transition.
 */
void test_fsm_idle_to_monitoring(void) {
  const system_state_t next_state = fsm_run(SYS_IDLE, EVT_CMD_START);

  TEST_ASSERT_EQUAL(SYS_MONITORING, next_state);
}

/**
 * @brief Test MONITORING -> ALERT transition.
 */
void test_fsm_monitoring_to_alert(void) {
  const system_state_t next_state =
      fsm_run(SYS_MONITORING, EVT_ALERT_TRIGGERED);

  TEST_ASSERT_EQUAL(SYS_ALERT, next_state);
}

/**
 * @brief Test ALERT -> MONITORING transition.
 */
void test_fsm_alert_acknowledge(void) {
  const system_state_t next_state = fsm_run(SYS_ALERT, EVT_ALERT_ACK);

  TEST_ASSERT_EQUAL(SYS_MONITORING, next_state);
}

/**
 * @brief Test ERROR -> INIT transition.
 */
void test_fsm_error_reset(void) {
  const system_state_t next_state = fsm_run(SYS_ERROR, EVT_CMD_RESET);

  TEST_ASSERT_EQUAL(SYS_INIT, next_state);
}

/**
 * @brief Test invalid FSM state handling.
 */
void test_fsm_invalid_state_returns_error(void) {
  const system_state_t invalid_state = SYS_NUM_STATES;
  const system_state_t next_state = fsm_run(invalid_state, EVT_INIT_OK);

  TEST_ASSERT_EQUAL(invalid_state, next_state);
}

/**
 * @brief Test dispatching a known CLI command.
 */
void test_cli_dispatch_known_command(void) {
  system_state_t state = SYS_IDLE;

  cli_context_t context = {
      .p_state = &state,
  };

  const status_t status = cli_dispatch("start\n", &context);

  TEST_ASSERT_EQUAL(STATUS_OK, status);
  TEST_ASSERT_EQUAL(SYS_MONITORING, state);
}

/**
 * @brief Test dispatching an unknown CLI command.
 */
void test_cli_dispatch_unknown_command(void) {
  system_state_t state = SYS_IDLE;

  cli_context_t context = {
      .p_state = &state,
  };

  const status_t status = cli_dispatch("invalid\n", &context);

  TEST_ASSERT_EQUAL(STATUS_ERR_INVALID_PARAM, status);
  TEST_ASSERT_EQUAL(SYS_IDLE, state);
}

/**
 * @brief Test alert threshold detection.
 */
void test_alert_threshold_detection(void) {
  const alert_config_t config = {
      .temp_max = 350,
      .temp_min = 100,
      .humi_max = 800,
      .humi_min = 200,
  };

  const sensor_data_t data = {
      .temperature = 360,
      .humidity = 600,
      .timestamp = 100U,
  };

  alert_init(&config);

  TEST_ASSERT_FALSE(alert_is_active());
  TEST_ASSERT_TRUE(alert_check(&data));
  TEST_ASSERT_TRUE(alert_is_active());
}

/**
 * @brief Test alert acknowledgement clears the active state.
 */
void test_alert_acknowledge_clears(void) {
  const alert_config_t config = {
      .temp_max = 350,
      .temp_min = 100,
      .humi_max = 800,
      .humi_min = 200,
  };

  const sensor_data_t data = {
      .temperature = 360,
      .humidity = 600,
      .timestamp = 100U,
  };

  alert_init(&config);
  (void)alert_check(&data);

  TEST_ASSERT_TRUE(alert_is_active());

  alert_acknowledge();

  TEST_ASSERT_FALSE(alert_is_active());
}