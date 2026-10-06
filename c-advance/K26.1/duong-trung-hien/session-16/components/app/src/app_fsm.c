#include "app_fsm.h"
#include "common_types.h"
#include "logger.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Handle SYS_INIT state.
 *
 * @param[in] event Event received while in SYS_INIT.
 *
 * @return Next system state.
 */
static system_state_t state_init(system_event_t event);

/**
 * @brief Handle SYS_IDLE state.
 *
 * @param[in] event Event received while in SYS_IDLE.
 *
 * @return Next system state.
 */
static system_state_t state_idle(system_event_t event);

/**
 * @brief Handle SYS_MONITORING state.
 *
 * @param[in] event Event received while in SYS_MONITORING.
 *
 * @return Next system state.
 */
static system_state_t state_monitoring(system_event_t event);

/**
 * @brief Handle SYS_ALERT state.
 *
 * @param[in] event Event received while in SYS_ALERT.
 *
 * @return Next system state.
 */
static system_state_t state_alert(system_event_t event);

/**
 * @brief Handle SYS_ERROR state.
 *
 * @param[in] event Event received while in SYS_ERROR.
 *
 * @return Next system state.
 */
static system_state_t state_error(system_event_t event);

/**
 * @brief Convert a system state to its string representation.
 *
 * @param[in] state System state.
 *
 * @return State name.
 */
static const char *state_to_string(system_state_t state);

/**
 * @brief Convert a system event to its string representation.
 *
 * @param[in] event System event.
 *
 * @return Event name.
 */
static const char *event_to_string(system_event_t event);

/**
 * @brief FSM state handler dispatch table.
 */
static const state_handler_t s_state_table[SYS_NUM_STATES] = {
    [SYS_INIT] = state_init,
    [SYS_IDLE] = state_idle,
    [SYS_MONITORING] = state_monitoring,
    [SYS_ALERT] = state_alert,
    [SYS_ERROR] = state_error,
};

system_state_t fsm_run(system_state_t current_state, system_event_t event) {
  if ((current_state >= SYS_NUM_STATES) || (event >= EVT_NUM_EVENTS)) {
    LOG_ERROR("Invalid FSM state/event: state=%d event=%d", (int)current_state,
              (int)event);
    return current_state;
  }

  const state_handler_t handler = s_state_table[current_state];

  if (NULL == handler) {
    LOG_ERROR("NULL FSM handler for state=%d", (int)current_state);
    return current_state;
  }

  const system_state_t next_state = handler(event);

  if (next_state != current_state) {
    LOG_INFO("FSM: %s -> %s (%s)", state_to_string(current_state),
             state_to_string(next_state), event_to_string(event));
  }

  return next_state;
}

static system_state_t state_init(system_event_t event) {
  system_state_t next_state = SYS_INIT;

  if (EVT_INIT_OK == event) {
    LOG_INFO("%s", "Init complete");
    next_state = SYS_IDLE;
  } else if (EVT_INIT_FAIL == event) {
    LOG_ERROR("%s", "Init failed");
    next_state = SYS_ERROR;
  }

  return next_state;
}

static system_state_t state_idle(system_event_t event) {
  system_state_t next_state = SYS_IDLE;

  if (EVT_CMD_START == event) {
    LOG_INFO("%s", "Monitoring started");
    next_state = SYS_MONITORING;
  } else if (EVT_CMD_QUIT == event) {
    LOG_INFO("%s", "Shutting down...");
  }

  return next_state;
}

static system_state_t state_monitoring(system_event_t event) {
  system_state_t next_state = SYS_MONITORING;

  if (EVT_ALERT_TRIGGERED == event) {
    LOG_WARN("%s", "Alert triggered");
    next_state = SYS_ALERT;
  } else if (EVT_SENSOR_FAIL == event) {
    LOG_ERROR("%s", "Sensor failure");
    next_state = SYS_ERROR;
  } else if (EVT_CMD_STOP == event) {
    LOG_INFO("%s", "Monitoring stopped");
    next_state = SYS_IDLE;
  }

  return next_state;
}

static system_state_t state_alert(system_event_t event) {
  system_state_t next_state = SYS_ALERT;

  if (EVT_ALERT_ACK == event) {
    LOG_INFO("%s", "Alert acknowledged");
    next_state = SYS_MONITORING;
  } else if (EVT_SENSOR_FAIL == event) {
    LOG_ERROR("%s", "Sensor failure in alert");
    next_state = SYS_ERROR;
  }

  return next_state;
}

static system_state_t state_error(system_event_t event) {
  system_state_t next_state = SYS_ERROR;

  if (EVT_CMD_RESET == event) {
    LOG_INFO("%s", "System reset");
    next_state = SYS_INIT;
  }

  return next_state;
}

static const char *state_to_string(system_state_t state) {
  static const char *const state_names[SYS_NUM_STATES] = {
      [SYS_INIT] = "SYS_INIT",
      [SYS_IDLE] = "SYS_IDLE",
      [SYS_MONITORING] = "SYS_MONITORING",
      [SYS_ALERT] = "SYS_ALERT",
      [SYS_ERROR] = "SYS_ERROR",
  };

  if (state >= SYS_NUM_STATES) {
    return "SYS_INVALID";
  }

  return state_names[state];
}

static const char *event_to_string(system_event_t event) {
  static const char *const event_names[EVT_NUM_EVENTS] = {
      [EVT_INIT_OK] = "EVT_INIT_OK",
      [EVT_INIT_FAIL] = "EVT_INIT_FAIL",
      [EVT_CMD_START] = "EVT_CMD_START",
      [EVT_CMD_STOP] = "EVT_CMD_STOP",
      [EVT_CMD_RESET] = "EVT_CMD_RESET",
      [EVT_CMD_QUIT] = "EVT_CMD_QUIT",
      [EVT_ALERT_TRIGGERED] = "EVT_ALERT_TRIGGERED",
      [EVT_ALERT_ACK] = "EVT_ALERT_ACK",
      [EVT_SENSOR_FAIL] = "EVT_SENSOR_FAIL",
  };

  if (event >= EVT_NUM_EVENTS) {
    return "EVT_INVALID";
  }

  return event_names[event];
}